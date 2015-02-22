#include "GameServer.h"
#include "MagixNetworkDefines.h"
#include "RakNetworkFactory.h"
#include <thread>

using namespace RakNet;

unsigned char GetPacketIdentifier(Packet *p);

#ifdef _CONSOLE_2
_CONSOLE_2_SetSystemProcessParams
#endif

using namespace RakNet;
using namespace std;

#ifdef __linux__
void changemode(int dir)
{
	static struct termios oldt, newt;

	if ( dir == 1 )
	{
		tcgetattr( STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~( ICANON | ECHO );
		tcsetattr( STDIN_FILENO, TCSANOW, &newt);
	}
	else
		tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

int kbhit (void)
{
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET (STDIN_FILENO, &rdfs);

	select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &rdfs);
}
#endif

string intToString(int number)
{
	stringstream ss;
	ss << number;
	return ss.str();
}


ServerManager::ServerManager()
{
	server = RakNetworkFactory::GetRakPeerInterface();
	server->SetIncomingPassword(SERVER_PASSWORD, (int)strlen(SERVER_PASSWORD));

	for(int i=0;i<MAX_CLIENTS;i++)
	{
		clientIsMine[i] = false;
		clientAdd[i] = UNASSIGNED_SYSTEM_ADDRESS;
		clientMap[i] = "";
		clientName[i] = "";
		clientIsReady[i] = false;
		clientIsKicked[i] = false;
		clientDimension[i] = 0;
		clientHasTime[i] = false;
	}
	numClients = 0;
	showTraffic = false;
	showChat = false;
	showMapChange = false;
	printlog = false;
	hideText = false;
	mainServerAdd = UNASSIGNED_SYSTEM_ADDRESS;
	for(int i=0;i<MAX_SERVERS;i++)
	{
		serverAdd[i] = UNASSIGNED_SYSTEM_ADDRESS;
		serverConnected[i] = false;
	}
	serverID = MAX_SERVERS;
	hasBegun = false;
	requestedPlayerData = false;
	connectAttempts = 0;
	doUpdate = false;
	bootlist.clear();
	alive = true;

	//if(!readIPs())
	//{
		broadcastAdd = MAIN_SERVER_IP;
		mainServerInitAdd = "127.0.0.1";//MAIN_SERVER_IP;
	//}
}
ServerManager::~ServerManager()
{
}
void ServerManager::startThread()
{
	bool inited = false;
	while (!inited)
	{
		inited = initialize();
		if (!inited)RakSleep(5000);
	}
	runLoop();
	shutdown();
}
bool ServerManager::initialize(const unsigned int &sleepTime)
{
	if (!alive)
		return false;
	hasBegun = false;
	mainServerAdd = UNASSIGNED_SYSTEM_ADDRESS;
	if(sleepTime>0)RakSleep(sleepTime);
	if(!hideText)puts("Starting server");
	SocketDescriptor socketDescriptor(SERVER_PORT,0);
	if (!server->Startup(MAX_CLIENTS, 30, &socketDescriptor, 1))
	{
		if (!hideText)puts("Server failed to start.  Terminating.");
		return false;
	}
	server->SetMaximumIncomingConnections(0);
	server->SetOccasionalPing(true);
	if(!hideText)puts("Connecting to main server.");

	server->Connect(mainServerInitAdd.c_str(), MAIN_SERVER_PORT, SERVER_PASSWORD, (int)strlen(SERVER_PASSWORD));

	return true;
}
void ServerManager::begin()
{
	hasBegun = true;
	server->SetMaximumIncomingConnections(MAX_CLIENTS);
	if(!hideText)puts("Connected to main server. Starting server.");
	if(!hideText)printf("Max players allowed: %i\n", MAX_CLIENTS);

	maintenanceTime = 0;
	time(&prevTime);

	//resetData();
	//logNumPlayers();
}
void ServerManager::runLoop()
{
	//string message="";

	// Loop for input
	while (alive)
	{
		// This sleep keeps RakNet responsive
		RakSleep(30);

		//updateMemCheck(doUpdate);
		//if(doUpdate)break;

		// Get a packet from either the server or the client

		Packet *p = server->Receive();
		if (!alive)
			break;
		if (!server->IsActive())
		{
			printf("-------------------------\n");
			printf("network threads have died\n");
			printf("    Restarting Raknet    \n");
			printf("-------------------------\n");
			shutdown();
			initialize();
		}
		bool tCont = true;

		/*if (p==0)
			continue; // Didn't get any packets
		*/
		while(p && tCont)
		{
			/*updateMemCheck(doUpdate);
			if(doUpdate)
			{
				tCont = false;
				break;
			}*/

			// We got a packet, get the identifier with our handy function
			const unsigned char packetIdentifier = GetPacketIdentifier(p);
			//printf("Packet: %u\n", packetIdentifier);


			PacketPriority tPacketPriority = NUMBER_OF_PRIORITIES;
			PacketReliability tPacketReliability = NUMBER_OF_RELIABILITIES;
			char tOrderingChannel = 9;
			// Check if this is a network message packet
			switch (packetIdentifier)
			{
				case ID_CONNECTION_REQUEST_ACCEPTED:
					{
						RakNet::BitStream tBitStream;
						tBitStream.Write(MessageID(ID_IMASERVER));
						StringCompressor::Instance()->EncodeString(broadcastAdd.c_str(), 64, &tBitStream);
						server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, p->systemAddress, false);

						if(!hasBegun)
						{
							mainServerAdd = p->systemAddress;
							begin();
						}
						else
						{
							if(!requestedPlayerData)
							{
								requestedPlayerData = true;
								RakNet::BitStream tBitStream2;
								tBitStream2.Write(MessageID(ID_REQUESTPLAYERDATA));
								server->Send(&tBitStream2, HIGH_PRIORITY, RELIABLE, 0, p->systemAddress, false);
							}
							registerServer(p);
						}
					}
					break;
				case ID_IMASERVER:
					registerServer(p);
					break;
				case ID_REQUESTPLAYERDATA:
					{
						//Send entire player data list to new server
						for(int i=0;i<MAX_CLIENTS;i++)
						{
							if(clientName[i]!="")
							{
								RakNet::BitStream tBitStream;

								tBitStream.Write(MessageID(ID_PLAYERDATA));
								tBitStream.Write(OwnerToken(i+1));
								StringCompressor::Instance()->EncodeString(clientName[i].c_str(), 16, &tBitStream);
								StringCompressor::Instance()->EncodeString(clientMap[i].c_str(), 32, &tBitStream);
								tBitStream.Write(clientDimension[i]);

								server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, p->systemAddress, false);
							}
						}
					}
					break;
				case ID_PLAYERDATA:
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tToken;
						char tName[16];
						char tMapName[32];
						unsigned char tDimension;

						tReceiveBit.Read(tMessage);
						tReceiveBit.Read(tToken);
						StringCompressor::Instance()->DecodeString(tName, 16, &tReceiveBit);
						StringCompressor::Instance()->DecodeString(tMapName, 32, &tReceiveBit);
						tReceiveBit.Read(tDimension);

						assignPlayerData(tToken,false,p,tName,tMapName,tDimension/*,0*/);
					}
					break;
				case ID_CONNECTION_ATTEMPT_FAILED:
				case ID_NO_FREE_INCOMING_CONNECTIONS:
				case ID_ALREADY_CONNECTED:
				case ID_CONNECTION_BANNED:
					if(!hasBegun)
					{
						if(!hideText)printf("Failed to connect to main server with packet ID %i\n",(int)packetIdentifier);
						shutdown();
						connectAttempts++;
						if(connectAttempts>=MAX_SERVERS)connectAttempts = 0;
						initialize(5000);
						tCont = false;
					}
					else
					{
						if(!hideText)printf("Failed to connect to %s with packet ID %i\n",p->systemAddress.ToString(),(int)packetIdentifier);
						if(isServerAdd(p->systemAddress))
							server->Connect(p->systemAddress.ToString(false), p->systemAddress.port, SERVER_PASSWORD, (int)strlen(SERVER_PASSWORD));
					}
					break;
				case ID_INVALID_PASSWORD:
				case ID_SERVERUPDATE:
					doUpdate = true;
					tCont = false;
					break;
				case ID_NEW_INCOMING_CONNECTION:
					{
						if(!hideText)printf("ID_NEW_INCOMING_CONNECTION from %s\n", p->systemAddress.ToString());
					}
					break;

				case ID_DISCONNECTION_NOTIFICATION:
					{
						if(!hideText)printf("ID_DISCONNECTION_NOTIFICATION from %s\n", p->systemAddress.ToString());
						//Disconnected from main server
						if(p->systemAddress==mainServerAdd)
						{
							if(!hideText)puts("Disconnected from main server.");
							shutdown();
							initialize(5000);
							tCont = false;
							break;
						}
						if(unregisterServer(p))break;
						sendDisconnectProcedure(p,true);
						logNumPlayers();
					}
					break;
				
				case ID_CONNECTION_LOST:
					{
						//Disconnected from main server
						if(p->systemAddress==mainServerAdd)
						{
							if(!hideText)puts("Connection lost from main server.");
							shutdown();
							initialize(5000);
							tCont = false;
							break;
						}
						if(unregisterServer(p))break;
						sendDisconnectProcedure(p,false);
						logNumPlayers();
					}
					break;
				case ID_TOKENCONNECTED:
					//Forward to player
					if(p->systemAddress==mainServerAdd)
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tToken;
						SystemAddress tAdd;
						
						tReceiveBit.Read(tMessage);
						tReceiveBit.Read(tToken);
						tReceiveBit.Read(tAdd);

						RakNet::BitStream tBitStream;
						tBitStream.Write(MessageID(tMessage));
						tBitStream.Write(tToken);
						tBitStream.Write(&tReceiveBit);

						server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, tAdd, false);
					}
					//Forward to server
					else
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						tReceiveBit.Read(tMessage);

						RakNet::BitStream tBitStream;
						tBitStream.Write(tMessage);
						tBitStream.Write(p->systemAddress);
						tBitStream.Write(&tReceiveBit);

						server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, false);
					}
					break;
				case ID_FORCELOGOUT:
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						bool tAdd;
						char tName[16];

						tReceiveBit.Read(tMessage);
						tReceiveBit.Read(tAdd);
						StringCompressor::Instance()->DecodeString(tName, 16, &tReceiveBit);

						if(tAdd)
						{
							bootPlayer(tName);
							bootlist.push_back(tName);
						}
						else
						{
							for(vector<string>::iterator it=bootlist.begin();it!=bootlist.end();it++)
							{
								const string tBootee = *it;
								if(tBootee==tName)
								{
									bootlist.erase(it);
									break;
								}
							}
						}
					}
					break;
				case ID_PLAYERDISCONNECTED:
					{
						sendRemoteDisconnectProcedure(p);
						logNumPlayers();
					}
					break;

				/*case ID_MODIFIED_PACKET: // Packet type unavailable in newer RakNet
					{
						string tMapName;
						const OwnerToken tPlayerToken = getOwnerToken(p,tMapName);
						if(!hideText)printf("ID_MODIFIED_PACKET from %s ", p->systemAddress.ToString());
						if(!hideText)printf("(Player token %i)\n", tPlayerToken);
					}
					break;*/

				case ID_SERVERLIST:
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						
						tReceiveBit.Read(tMessage);

						vector<pair<unsigned char,SystemAddress> > tServerList;
						bool tHasMore = false;
						tReceiveBit.Read(tHasMore);
						while(tHasMore)
						{
							tHasMore = false;
							unsigned char tID;
							SystemAddress tAdd;
							bool tFull = false;

							tReceiveBit.Read(tID);
							tReceiveBit.Read(tAdd);
							tReceiveBit.Read(tFull);
							tServerList.push_back(pair<unsigned char,SystemAddress>(tID,tAdd));

							tReceiveBit.Read(tHasMore);
						}
						if(!hideText)printf("Received ServerList of size %i\n",(int)tServerList.size());
						for(int i=0;i<(int)tServerList.size();i++)
						{
							if(tServerList[i].first>=0 && tServerList[i].first<MAX_SERVERS)
							{
								serverAdd[tServerList[i].first] = tServerList[i].second;
								server->Connect(tServerList[i].second.ToString(false), tServerList[i].second.port, SERVER_PASSWORD, (int)strlen(SERVER_PASSWORD));
							}
						}
						tServerList.clear();
					}
					break;
				case ID_SERVERCONNECTED:
					{
						//Only receive from main server
						if(p->systemAddress!=mainServerAdd)break;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						bool tConnect;
						unsigned char tID;
						SystemAddress tAdd;
						SystemAddress tTunnelAdd;
						bool tThisIsMyID = false;

						tReceiveBit.Read(tMessage);
						tReceiveBit.Read(tConnect);
						if(tConnect)tReceiveBit.Read(tID);
						tReceiveBit.Read(tAdd);
						tReceiveBit.Read(tTunnelAdd);
						tReceiveBit.Read(tThisIsMyID);

						if(tThisIsMyID)
						{
							serverID = tID;
							break;
						}

						if(tConnect)
						{
							if(tID>=0 && tID<MAX_SERVERS)serverAdd[tID] = tTunnelAdd;
						}
						else unregisterServer(tTunnelAdd);

						//Relay to players
						RakNet::BitStream tBitStream;
						tBitStream.Write(tMessage);
						tBitStream.Write(tConnect);
						if(tConnect)tBitStream.Write(tID);
						tBitStream.Write(tAdd);
						server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, true);
					}
					break;
				case ID_MAINTENANCE:
					{
						//Only receive from main server
						if(p->systemAddress!=mainServerAdd)break;
						//Relay to players
						RakNet::BitStream tBitStream(p->data, p->length, false);
						server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, true);
					}
					break;
				case ID_VIOLATION:
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;

						tReceiveBit.Read(tMessage);
						
						RakNet::BitStream tBitStream;
						tBitStream.Write(tMessage);
						tBitStream.Write((SystemAddress)p->systemAddress);
						tBitStream.Write(&tReceiveBit);

						server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, false);
					}
					break;

				/*case ID_REQUESTMAPINFO:
					//Send map info to new player
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tToken;
						tReceiveBit.Read(tMessage);
						tReceiveBit.Read(tToken);
						if(tToken<=0 || tToken>MAX_CLIENTS)break;
						
						string mapName = clientMap[tToken-1];
						unsigned char dimension = clientDimension[tToken-1];
						if(playersInMap(mapName,dimension)<=0)break;

						//request map time from a player
						pair<int,SystemAddress> tLowestPing;
						tLowestPing.second = UNASSIGNED_SYSTEM_ADDRESS;
						for(int i=0;i<MAX_CLIENTS;i++)
						{
							if(clientIsMine[i] && clientHasTime[i] && i!=(tToken-1) && !clientIsKicked[i] && clientMap[i]==mapName && clientDimension[i]==dimension)
							{
								const int tPing = server->GetAveragePing(clientAdd[i]);
								if(tLowestPing.second==UNASSIGNED_SYSTEM_ADDRESS || (tLowestPing.first>tPing&&tPing!=-1))
								{
									tLowestPing.first = tPing;
									tLowestPing.second = clientAdd[i];
								}
							}
						}
						RakNet::BitStream tBitStream;

						tBitStream.Write(MessageID(ID_REQUESTMAPINFO));
						tBitStream.Write(tToken);
						if(tLowestPing.second!=UNASSIGNED_SYSTEM_ADDRESS)
						{
							server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 1, tLowestPing.second, false);
							if(showMapChange)printf("Requested a player to send %s data to player %i.\n",mapName.c_str(),tToken);
						}
						else sendNextServer(&tBitStream, HIGH_PRIORITY, RELIABLE, 1);
					}
					break;*/

				case ID_PLAYERMAPCHANGE:
					{
						bool tReadSenderToken = false;
						if(imTheOwner(p))
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							tReceiveBit.Read(tMessage);

							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(getOwnerToken(p));
							tBitStream.Write(&tReceiveBit);
							
							sendAllServers(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS);
						}
						else tReadSenderToken = true;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tSender;
						OwnerToken tToken;
						char tMapName[32];
						unsigned char tDimension;

						tReceiveBit.Read(tMessage);
						if(tReadSenderToken)
						{
							tReceiveBit.Read(tSender);
							if(tSender<=0 || tSender>MAX_CLIENTS)break;
						}
						tReceiveBit.Read(tToken);
						StringCompressor::Instance()->DecodeString(tMapName, 32, &tReceiveBit);
						tReceiveBit.Read(tDimension);

						//Broadcast player's map change to other players in old map
						RakNet::BitStream tBitStream;
						string tOldMapName;
						unsigned char tOldDimension;
						if(tReadSenderToken)
						{
							tOldMapName = clientMap[tSender-1];
							tOldDimension = clientDimension[tSender-1];
						}
						else tSender = getOwnerToken(p,tOldMapName,tOldDimension);

						tBitStream.Write(MessageID(ID_PLAYERMAPCHANGE));
						tBitStream.Write(tSender);

						broadcastInMap(tOldMapName, tOldDimension, server, &tBitStream, LOW_PRIORITY, RELIABLE, 0, (tReadSenderToken?clientAdd[tSender-1]:p->systemAddress));

						//Change player's map and dimension
						if(tSender>0 && tSender<=MAX_CLIENTS)
						{
							clientMap[tSender-1] = tMapName;
							clientDimension[tSender-1] = tDimension;

							if(clientIsMine[tSender-1])
							{
								//Broadcast new players' arrival to other players in new map
								sendNewPlayerProcedure(tSender,tMapName,tDimension);
							}
							else
							{
								//Ping all old players to send newplayer info
								RakNet::BitStream tBitStream2;

								tBitStream2.Write(MessageID(ID_NEWPLAYERUPDATE));
								tBitStream2.Write(tSender);

								broadcastInMap(tMapName, tDimension, server, &tBitStream2, HIGH_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS);
								if(showMapChange)printf("Broadcasted ID_NEWPLAYERUPDATE in %s for player %i.\n",tMapName,tSender);

								//Broadcast player data
								RakNet::BitStream tBitStream;

								tBitStream.Write(MessageID(ID_PLAYERDATA));
								tBitStream.Write(true);
								StringCompressor::Instance()->EncodeString(clientName[tSender - 1].c_str(), 16, &tBitStream);
								StringCompressor::Instance()->EncodeString(clientMap[tSender - 1].c_str(), 32, &tBitStream);

								sendMyPlayers(server, &tBitStream, LOW_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS);
							}
						}
					}
					break;

				case ID_VALIDATENAME:
					{
						//relayPacket(p,HIGH_PRIORITY,RELIABLE,1);

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tToken;
						char tName[16];
						char tMapName[32] = "";
						unsigned short tCharID = 0;
						unsigned char tDimension = 0;
						bool tIsNotMine = false;

						tReceiveBit.Read(tMessage);
						tReceiveBit.Read(tToken);
						StringCompressor::Instance()->DecodeString(tName, 16, &tReceiveBit);
						StringCompressor::Instance()->DecodeString(tMapName, 32, &tReceiveBit);
						tReceiveBit.Read(tDimension);
						tReceiveBit.Read(tIsNotMine);

						if(!tIsNotMine)
						{
							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(tToken);
							StringCompressor::Instance()->EncodeString(tName, 16, &tBitStream);
							StringCompressor::Instance()->EncodeString(tMapName, 32, &tBitStream);
							tBitStream.Write(tDimension);
							tBitStream.Write(true);
							
							sendAllServers(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS);
						}

						//Check if user already logged in
						bootPlayer(tName);

						if(printlog)logNumPlayers();

						assignPlayerData(tToken,(!tIsNotMine),p,tName,tMapName,tDimension/*,tCharID*/);

						//Boot player if logged in elsewhere
						bool tIsBooted = false;
						for(int i=0;i<(int)bootlist.size();i++)
						{
							if(bootlist[i]==toLowerCase(tName))
							{
								bootPlayer(tName);
								tIsBooted = true;
								break;
							}
						}
						if(tIsBooted)break;

						if(!tIsNotMine)
						{
							//Send entire player data list to new player
							for(int i=0;i<MAX_CLIENTS;i++)
							{
								if(clientName[i]!="")
								{
									RakNet::BitStream tBitStream;

									tBitStream.Write(MessageID(ID_PLAYERDATA));
									tBitStream.Write(true);
									StringCompressor::Instance()->EncodeString(clientName[i].c_str(), 16, &tBitStream);
									StringCompressor::Instance()->EncodeString(clientMap[i].c_str(), 32, &tBitStream);

									server->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 0, p->systemAddress, false);
								}
							}
							//Request item stash from Main Server
							RakNet::BitStream tBitStream;

							tBitStream.Write(MessageID(ID_ITEMSTASH));
							tBitStream.Write(tToken);
							tBitStream.Write(true);

							server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, false);

							//Proceed to allow new player to start game
							sendNewPlayerProcedure(tToken,tMapName,tDimension);
						}
						else
						{
							//Ping all old players to send newplayer info
							RakNet::BitStream tBitStream2;

							tBitStream2.Write(MessageID(ID_NEWPLAYERUPDATE));
							tBitStream2.Write(tToken);

							broadcastInMap(tMapName, tDimension, server, &tBitStream2, HIGH_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS);
							if(showMapChange)printf("Broadcasted ID_NEWPLAYERUPDATE in %s for player %i.\n",tMapName,tToken);

							//Broadcast player data
							RakNet::BitStream tBitStream;

							tBitStream.Write(MessageID(ID_PLAYERDATA));
							tBitStream.Write(true);
							StringCompressor::Instance()->EncodeString(tName, 16, &tBitStream);
							StringCompressor::Instance()->EncodeString(tMapName, 32, &tBitStream);

							sendMyPlayers(server, &tBitStream, LOW_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS);
						}
					}
					break;
				case ID_BIO:
					//check bio type
					{
						bool tReadSenderToken = false;
						if(imTheOwner(p))
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							tReceiveBit.Read(tMessage);

							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(getOwnerToken(p));
							tBitStream.Write(&tReceiveBit);
							
							sendAllServers(&tBitStream, LOW_PRIORITY, UNRELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS);
						}
						else tReadSenderToken = true;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tSender;
						OwnerToken tToken;
						bool tIsRequest = false;

						tReceiveBit.Read(tMessage);
						if(tReadSenderToken)
						{
							tReceiveBit.Read(tSender);
							if(tSender<=0 || tSender>MAX_CLIENTS)break;
						}
						tReceiveBit.Read(tToken);
						tReceiveBit.Read(tIsRequest);

						if(tIsRequest)
						{
							char tTarget[32];
							StringCompressor::Instance()->DecodeString(tTarget, 32, &tReceiveBit);
							tToken = getTokenByName(tTarget);
							if(tToken==0)break;
							if(!clientIsMine[tToken-1])break;

							RakNet::BitStream tBitStream;
							tBitStream.Write(MessageID(ID_BIO));
							tBitStream.Write(tReadSenderToken?tSender:getOwnerToken(p));
							tBitStream.Write(true);

							server->Send(&tBitStream, LOW_PRIORITY, UNRELIABLE, 0, clientAdd[tToken-1], false);
						}
						else
						{
							if(tToken<=MAX_CLIENTS && tToken>0)
							if(clientAdd[tToken-1]!= UNASSIGNED_SYSTEM_ADDRESS)
							{
								if(!clientIsMine[tToken-1])break;
								RakNet::BitStream tBitStream;
								tBitStream.Write(MessageID(ID_BIO));
								tBitStream.Write(tToken);
								tBitStream.Write(false);
								tBitStream.Write(&tReceiveBit);

								
								server->Send(&tBitStream, LOW_PRIORITY, UNRELIABLE, 0, clientAdd[tToken-1], false);
							}
						}
					}
					break;
				case ID_KICK:
					{
						relayPacket(p, MEDIUM_PRIORITY, RELIABLE, 0);
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						char tName[16];

						tReceiveBit.Read(tMessage);
						StringCompressor::Instance()->DecodeString(tName, 16, &tReceiveBit);

						OwnerToken tToken = getTokenByName(tName);
						if(tToken==0)break;
						clientIsKicked[tToken-1] = true;

						if(clientIsMine[tToken-1])
						{
							bool tIsBanned = false;
							unsigned short tNumDays = 0;
							tReceiveBit.Read(tIsBanned);
							if(tIsBanned)tReceiveBit.Read(tNumDays);

							RakNet::BitStream tBitStream;
							tBitStream.Write(MessageID(ID_KICK));
							StringCompressor::Instance()->EncodeString(tName, 16, &tBitStream);
							if(tIsBanned)
							{
								tBitStream.Write(tIsBanned);
								tBitStream.Write(tNumDays);

								server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, false);
							}

							server->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 0, clientAdd[tToken-1], false);
						}
					}
					break;
				case ID_GODSPEAK:
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						if(p->systemAddress==mainServerAdd)
							sendMyPlayers(server,&tReceiveBit, MEDIUM_PRIORITY, RELIABLE, 0, p->systemAddress);
						else
						{
							server->Send(&tReceiveBit, MEDIUM_PRIORITY, RELIABLE, 0, mainServerAdd, false);
							sendMyPlayers(server,&tReceiveBit, MEDIUM_PRIORITY, RELIABLE, 0, p->systemAddress);
						}
					}
					break;
				case ID_ITEMDROP:
					{
						bool tReadSenderToken = false;
						if(imTheOwner(p))
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							tReceiveBit.Read(tMessage);

							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(getOwnerToken(p));
							tBitStream.Write(&tReceiveBit);
							
							sendAllServers(&tBitStream,MEDIUM_PRIORITY, RELIABLE, 2,UNASSIGNED_SYSTEM_ADDRESS);
						}
						else tReadSenderToken = true;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tSender;
						OwnerToken tToken;

						//Receive Item data
						tReceiveBit.Read(tMessage);
						if(tReadSenderToken)
						{
							tReceiveBit.Read(tSender);
							if(tSender<=0 || tSender>MAX_CLIENTS)break;
						}
						tReceiveBit.Read(tToken);

						//Just item data for a newly arrived player
						if(tToken!=TOKEN_NULL)
						{
							if(tToken<=MAX_CLIENTS && tToken>0)
							if(clientAdd[tToken-1]!= UNASSIGNED_SYSTEM_ADDRESS)
							{
								if(!clientIsMine[tToken-1])break;
								RakNet::BitStream tBitStream;
								tBitStream.Write(MessageID(ID_ITEMDROP));
								tBitStream.Write(&tReceiveBit);

								server->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 2, clientAdd[tToken-1], false);
							}
							break;
						}

						char tMesh[16];
						unsigned short tID;
						float tX;
						float tZ;

						tReceiveBit.Read(tID);
						StringCompressor::Instance()->DecodeString(tMesh, 16, &tReceiveBit);
						tReceiveBit.Read(tX);
						tReceiveBit.Read(tZ);

						string tMapName;
						unsigned char tDimension;
						if(!tReadSenderToken)tSender = getOwnerToken(p,tMapName,tDimension);
						else
						{
							tMapName = clientMap[tSender-1];
							tDimension = clientDimension[tSender-1];
						}

						//Send Item data
						RakNet::BitStream tBitStream;
						tBitStream.Write(MessageID(ID_ITEMDROP));
						tBitStream.Write(tID);
						StringCompressor::Instance()->EncodeString(tMesh, 16, &tBitStream);
						tBitStream.Write(tX);
						tBitStream.Write(tZ);

						broadcastInMap(tMapName, tDimension, server, &tBitStream, MEDIUM_PRIORITY, RELIABLE, 2, UNASSIGNED_SYSTEM_ADDRESS);
					}
					break;
				case ID_ITEMPICKUP:
					{
						bool tReadSenderToken = false;
						if(imTheOwner(p))
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							tReceiveBit.Read(tMessage);

							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(getOwnerToken(p));
							tBitStream.Write(&tReceiveBit);
							sendAllServers(&tBitStream,MEDIUM_PRIORITY, RELIABLE, 2,UNASSIGNED_SYSTEM_ADDRESS);
						}
						else tReadSenderToken = true;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tSender;
						unsigned short tID;				

						//Receive Item data
						tReceiveBit.Read(tMessage);
						if(tReadSenderToken)
						{
							tReceiveBit.Read(tSender);
							if(tSender<=0 || tSender>MAX_CLIENTS)break;
						}
						tReceiveBit.Read(tID);

						string tMapName;
						unsigned char tDimension;
						if(!tReadSenderToken)tSender = getOwnerToken(p,tMapName,tDimension);
						else
						{
							tMapName = clientMap[tSender-1];
							tDimension = clientDimension[tSender-1];
						}

						{
							//Send Item data
							RakNet::BitStream tBitStream;
							tBitStream.Write(MessageID(ID_ITEMPICKUP));
							tBitStream.Write(tID);
							tBitStream.Write(false);

							broadcastInMap(tMapName, tDimension, server, &tBitStream, MEDIUM_PRIORITY, RELIABLE, 2, (tReadSenderToken?clientAdd[tSender-1]:p->systemAddress));

							//Send player pickup success
							RakNet::BitStream tBitStream2;
							tBitStream2.Write(MessageID(ID_ITEMPICKUP));
							tBitStream2.Write(tID);
							tBitStream2.Write(true);

							server->Send(&tBitStream2, MEDIUM_PRIORITY, RELIABLE, 2, (tReadSenderToken?clientAdd[tSender-1]:p->systemAddress), false);
						}
					}
					break;
				case ID_IMREADY:
					{
						if(imTheOwner(p))
						{
							RakNet::BitStream tBitStream;

							tBitStream.Write(MessageID(ID_IMREADY));
							tBitStream.Write(getOwnerToken(p));
							
							sendAllServers(&tBitStream, LOW_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS);

							clientIsReady[getOwnerToken(p)-1] = true;
						}
						else
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							OwnerToken tSender;

							tReceiveBit.Read(tMessage);
							tReceiveBit.Read(tSender);
							if(tSender>0 && tSender<=MAX_CLIENTS)clientIsReady[tSender-1] = true;
						}
						
					}
					break;
				case ID_EVENT:
				case ID_INFO:
					{
						bool tReadSenderToken = false;
						if(imTheOwner(p))
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							tReceiveBit.Read(tMessage);

							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(getOwnerToken(p));
							tBitStream.Write(&tReceiveBit);
							sendAllServers(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS);
						}
						else tReadSenderToken = true;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tSender;
						unsigned char tType;
						OwnerToken tTarget;

						tReceiveBit.Read(tMessage);
						if(tReadSenderToken)
						{
							tReceiveBit.Read(tSender);
							if(tSender<=0 || tSender>MAX_CLIENTS)break;
						}
						tReceiveBit.Read(tType);
						if(packetIdentifier==ID_INFO)tReceiveBit.Read(tTarget);

						string tMapName;
						unsigned char tDimension;
						if(!tReadSenderToken)tSender = getOwnerToken(p,tMapName,tDimension);
						else
						{
							tMapName = clientMap[tSender-1];
							tDimension = clientDimension[tSender-1];
						}

						RakNet::BitStream tBitStream;

						tBitStream.Write(tMessage);
						tBitStream.Write(tType);
						if(packetIdentifier==ID_INFO)tBitStream.Write(tSender);
						tBitStream.Write(&tReceiveBit);

						//Send to everyone in the map/dimension
						if(packetIdentifier!=ID_INFO || tTarget==TOKEN_NULL)
							broadcastInMap(tMapName, tDimension, server, &tBitStream, MEDIUM_PRIORITY, RELIABLE, 0, (packetIdentifier==ID_EVENT?UNASSIGNED_SYSTEM_ADDRESS:(tReadSenderToken?clientAdd[tSender-1]:p->systemAddress)));
						//Send to specified target
						else
						{
							if(tTarget<=MAX_CLIENTS && tTarget>0)
							if(clientAdd[tTarget-1]!= UNASSIGNED_SYSTEM_ADDRESS)
							if(clientIsMine[tTarget-1])
							server->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 0, clientAdd[tTarget-1], false);
						}
					}
					break;
				case ID_CRITTEROWNAGE:
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						unsigned short tCritterID;
						bool tIsCreate = false;
						bool tIsTakeover = false;

						tReceiveBit.Read(tMessage);
						tReceiveBit.Read(tCritterID);
						tReceiveBit.Read(tIsCreate);
						if(tIsCreate)
						{
							OwnerToken tSender;
							bool tIsWorldCritter = true;
							unsigned char tTypeID;
							char tType[32];
							float tf;
							//Vector3 tPosition;
							bool tIsPet = false;
							tReceiveBit.Read(tSender);
							tReceiveBit.Read(tIsWorldCritter);
							if(!tIsWorldCritter)
							{
								StringCompressor::Instance()->DecodeString(tType, 32, &tReceiveBit);
								tReceiveBit.Read(tf); // x
								tReceiveBit.Read(tf); // y
								tReceiveBit.Read(tf); // z
								tReceiveBit.Read(tIsPet);
								if(!tIsPet)
								{
									string tMapName;
									unsigned char tDimension;
									tMapName = clientMap[tSender-1];
									tDimension = clientDimension[tSender-1];
									const string tCaption = "<<(SPAWN)>> "+clientName[tSender-1]+" SPAWNED "+tType;
									RakNet::BitStream wBitStream;
									wBitStream.Write(MessageID(ID_GODSPEAK));
									StringCompressor::Instance()->EncodeString(tCaption.c_str(), 512, &wBitStream);
									broadcastInMap(tMapName, tDimension, server, &wBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS);
								}
							}


						}
					}
				case ID_UNITOWNAGE:
				case ID_OBJECTOWNAGE:
					{
						bool tReadSenderToken = false;
						if(imTheOwner(p))
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							tReceiveBit.Read(tMessage);

							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(getOwnerToken(p));
							tBitStream.Write(&tReceiveBit);
							
							sendAllServers(&tBitStream, HIGH_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS);
						}
						else tReadSenderToken = true;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tSender;

						tReceiveBit.Read(tMessage);
						if(tReadSenderToken)
						{
							tReceiveBit.Read(tSender);
							if(tSender<=0 || tSender>MAX_CLIENTS)break;
						}

						string tMapName;
						unsigned char tDimension;
						if(!tReadSenderToken)tSender = getOwnerToken(p,tMapName,tDimension);
						else
						{
							tMapName = clientMap[tSender-1];
							tDimension = clientDimension[tSender-1];
						}
						//Complete packet relay
						RakNet::BitStream tBitStream;

						tBitStream.Write(tMessage);
						tBitStream.Write(&tReceiveBit);

						broadcastInMap(tMapName, tDimension, server, &tBitStream, HIGH_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS);
					}
					break;
				case ID_CRITTERTARGETEDINFO:
				case ID_UNITTARGETEDINFO:
				case ID_OBJECTTARGETEDINFO:
					{
						bool tReadSenderToken = false;
						if(imTheOwner(p))
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							tReceiveBit.Read(tMessage);

							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(getOwnerToken(p));
							tBitStream.Write(&tReceiveBit);
							sendAllServers(&tBitStream, HIGH_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS);
						}
						else tReadSenderToken = true;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tSender;
						unsigned short tID;
						OwnerToken tTarget;

						tReceiveBit.Read(tMessage);
						if(tReadSenderToken)
						{
							tReceiveBit.Read(tSender);
							if(tSender<=0 || tSender>MAX_CLIENTS)break;
						}
						else tSender = getOwnerToken(p);
						tReceiveBit.Read(tID);
						tReceiveBit.Read(tTarget);

						if(tTarget<=MAX_CLIENTS && tTarget>0)
						if(clientAdd[tTarget-1]!= UNASSIGNED_SYSTEM_ADDRESS)
						if(clientIsMine[tTarget-1])
						{
							//Relay remaining packet
							RakNet::BitStream tBitStream;
							tBitStream.Write(tMessage);
							tBitStream.Write(tID);
							tBitStream.Write(tSender);
							tBitStream.Write(&tReceiveBit);

							server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 4, clientAdd[tTarget-1], false);
						}
					}
					break;
				case ID_CRITTERINFO:
				case ID_CRITTERINFOONEWAY:
				case ID_UNITINFO:
				case ID_OBJECTINFO:
					{
						bool tReadSenderToken = false;
						if(imTheOwner(p))
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							tReceiveBit.Read(tMessage);

							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(getOwnerToken(p));
							tBitStream.Write(&tReceiveBit);
							sendAllServers(&tBitStream, LOW_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS);
						}
						else tReadSenderToken = true;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tSender;
						
						string tMapName;
						unsigned char tDimension;

						tReceiveBit.Read(tMessage);
						if(tReadSenderToken)
						{
							tReceiveBit.Read(tSender);
							if(tSender<=0 || tSender>MAX_CLIENTS)break;
							tMapName = clientMap[tSender-1];
							tDimension = clientDimension[tSender-1];
						}
						else getOwnerToken(p,tMapName,tDimension);

						//Complete packet relay
						RakNet::BitStream tBitStream;

						tBitStream.Write(tMessage);
						tBitStream.Write(&tReceiveBit);

						broadcastInMap(tMapName, tDimension, server, &tBitStream, LOW_PRIORITY, RELIABLE, 4, tMessage==ID_CRITTERINFOONEWAY?p->systemAddress:UNASSIGNED_SYSTEM_ADDRESS);
					}
					break;
				case ID_PLAYERTARGETEDINFO:
					{
						bool tReadSenderToken = false;
						if(imTheOwner(p))
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							tReceiveBit.Read(tMessage);

							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(getOwnerToken(p));
							tBitStream.Write(&tReceiveBit);
							sendAllServers(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS);
						}
						else tReadSenderToken = true;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tSender;
						OwnerToken tTarget;

						tReceiveBit.Read(tMessage);
						if(tReadSenderToken)
						{
							tReceiveBit.Read(tSender);
							if(tSender<=0 || tSender>MAX_CLIENTS)break;
						}
						else tSender = getOwnerToken(p);
						tReceiveBit.Read(tTarget);

						if(tTarget<=MAX_CLIENTS && tTarget>0)
						if(clientAdd[tTarget-1]!= UNASSIGNED_SYSTEM_ADDRESS)
						if(clientIsMine[tTarget-1])
						{
							//Relay remaining packet
							RakNet::BitStream tBitStream;
							tBitStream.Write(tMessage);
							tBitStream.Write(tSender);
							tBitStream.Write(&tReceiveBit);
							
							server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, clientAdd[tTarget-1], false);
						}
					}
					break;
				case ID_PLAYERINFO:
					{
						bool tReadSenderToken = false;
						if(imTheOwner(p))
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							tReceiveBit.Read(tMessage);

							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(getOwnerToken(p));
							tBitStream.Write(&tReceiveBit);
							sendAllServers(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS);
						}
						else tReadSenderToken = true;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tSender;
						
						string tMapName;
						unsigned char tDimension;
												bool tIsUpdate = false;
												bool tIsHit = false;

						tReceiveBit.Read(tMessage);
						BitSize_t offset;
						if(tReadSenderToken)
						{ //printf("RTok\n");
							tReceiveBit.Read(tSender);
							offset = tReceiveBit.GetReadOffset();
							if(tSender<=0 || tSender>MAX_CLIENTS)break;
							tMapName = clientMap[tSender-1];
							tDimension = clientDimension[tSender-1];
        											tReceiveBit.Read(tIsUpdate);
													tReceiveBit.Read(tIsHit);
						}
						else 
						{
							offset = tReceiveBit.GetReadOffset();
							getOwnerToken(p,tMapName,tDimension);
							tReceiveBit.Read(tSender);
														tReceiveBit.Read(tIsUpdate);
														tReceiveBit.Read(tIsHit);
						}

						if(tIsHit)
						{
							OwnerToken tUnitID;
							float tDamage = 0;

							tReceiveBit.Read(tUnitID);
							tReceiveBit.Read(tDamage);
							if(tDamage < 0 && tUnitID != tSender)
							{ // user attacked another user
								const string tCaption = "<<(SERVER WARNING)>> "+clientName[tSender-1]+" ATTACKED "+clientName[tUnitID-1]+" FOR "+intToString((int)tDamage);
								RakNet::BitStream wBitStream;
								wBitStream.Write(MessageID(ID_GODSPEAK));
								StringCompressor::Instance()->EncodeString(tCaption.c_str(), 512, &wBitStream);
								sendMyPlayers(server,&wBitStream, MEDIUM_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS);
							}
							tReceiveBit.SetReadOffset(offset);
						}
												//Complete packet relay
												RakNet::BitStream tBitStream;

												tBitStream.Write(tMessage);
												tBitStream.Write(&tReceiveBit);

						broadcastInMap(tMapName, tDimension, server, &tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS);
					}
					break;
				case ID_UPDATESKILLS:
				case ID_UPDATEPET:
				case ID_UPDATEHP:
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						tReceiveBit.Read(tMessage);
						const OwnerToken tToken = getOwnerToken(p);
						//if(tToken>MAX_CLIENTS || tToken<=0 || clientAdd[tToken-1]==UNASSIGNED_SYSTEM_ADDRESS)break;

						RakNet::BitStream tBitStream;

						tBitStream.Write(tMessage);
						tBitStream.Write(tToken);
						tBitStream.Write(&tReceiveBit);

						server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, false);
					}
					break;

				case ID_ITEMSTASH:
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						tReceiveBit.Read(tMessage);

						//Request from server to forward to player
						if(p->systemAddress==mainServerAdd)
						{
							OwnerToken tToken;
							tReceiveBit.Read(tToken);

							if(tToken>MAX_CLIENTS || tToken<=0 || clientAdd[tToken-1]==UNASSIGNED_SYSTEM_ADDRESS)break;

							//Relay remaining packet
							RakNet::BitStream tBitStream;
							tBitStream.Write(MessageID(ID_ITEMSTASH));
							tBitStream.Write(true);
							tBitStream.Write(true);
							tBitStream.Write(&tReceiveBit);

							server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 6, clientAdd[tToken-1], false);
						}
						else
						{
							//Data update to main server
							RakNet::BitStream tBitStream;
							tBitStream.Write(MessageID(ID_ITEMSTASH));
							tBitStream.Write((OwnerToken)getOwnerToken(p));
							tBitStream.Write(false);
							tBitStream.Write(&tReceiveBit);

							server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, false);

							//Return packet to player
							RakNet::BitStream tCopyBit(p->data, p->length, false);
							bool tIsStash;
							tCopyBit.Read(tMessage);
							tCopyBit.Read(tIsStash);

							RakNet::BitStream tBitStream2;
							tBitStream2.Write(MessageID(ID_ITEMSTASH));
							tBitStream2.Write(tIsStash);
							if(tIsStash)tBitStream2.Write(false);
							tBitStream2.Write(&tCopyBit);

							server->Send(&tBitStream2, MEDIUM_PRIORITY, RELIABLE, 0, p->systemAddress, false);
						}
					}
					break;
				case ID_FINDPLAYER:
					{
						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						bool tIsToken;
						OwnerToken tToken;
						string tName = "";
						string tMap = "";
						unsigned char tDimension = 0;

						tReceiveBit.Read(tMessage);
						tReceiveBit.Read(tIsToken);
						if(!tIsToken)
						{
							char tTarget[16];
							StringCompressor::Instance()->DecodeString(tTarget, 16, &tReceiveBit);
							tToken = getTokenByName(tTarget,false,tMap,tDimension);
							tName = tTarget;
						}
						else
						{
							tReceiveBit.Read(tToken);
							if(tToken>0&&tToken<=MAX_CLIENTS)
							{
								tName = clientName[tToken-1];
								tMap = clientMap[tToken-1];
								tDimension = clientDimension[tToken-1];
							}
							else
							{
								tToken = 0;
							}
						}

						RakNet::BitStream tBitStream;
						tBitStream.Write(tMessage);
						StringCompressor::Instance()->EncodeString(tName.c_str(), 16, &tBitStream);
						const bool tFound = (tToken!=0);
						tBitStream.Write(tFound);
						if(tFound)
						{
							tBitStream.Write(tToken);
							StringCompressor::Instance()->EncodeString(tMap.c_str(), 32, &tBitStream);
							tBitStream.Write(tDimension);
						}

						server->Send(&tBitStream, LOW_PRIORITY, UNRELIABLE, 0, p->systemAddress, false);
					}
					break;

				//High priority packets
				case ID_NEWPLAYER:
				//case ID_NEWPLAYERCOLOURS:
				case ID_POSITION:
					if(tOrderingChannel==9)tOrderingChannel = 0;
					if(tPacketPriority==NUMBER_OF_PRIORITIES)tPacketPriority = HIGH_PRIORITY;
					if(tPacketReliability==NUMBER_OF_RELIABILITIES)tPacketReliability = RELIABLE;
				case ID_ITEMEQUIP:
				case ID_ITEMUNEQUIP:
					if(tOrderingChannel==9)tOrderingChannel = 2;
					if(tPacketPriority==NUMBER_OF_PRIORITIES)tPacketPriority = LOW_PRIORITY;
					if(tPacketReliability==NUMBER_OF_RELIABILITIES)tPacketReliability = RELIABLE;
				//Medium/Reliable ordered packets
				case ID_CHAT:
					if(tPacketPriority==NUMBER_OF_PRIORITIES)tPacketPriority = HIGH_PRIORITY;
					if(tPacketReliability==NUMBER_OF_RELIABILITIES)tPacketReliability = RELIABLE;
				case ID_ATTACK:
					if(tPacketPriority==NUMBER_OF_PRIORITIES)tPacketPriority = HIGH_PRIORITY;
				case ID_EMOTE:
					if(tPacketPriority==NUMBER_OF_PRIORITIES)tPacketPriority = MEDIUM_PRIORITY;
					if(tPacketReliability==NUMBER_OF_RELIABILITIES)tPacketReliability = UNRELIABLE;
				//Low/Unreliable packets
				case ID_MOVEPOSITION:
				case ID_ACTIONFLAGS:
				case ID_LOOKQUATERNION:
				case ID_FACEDIRECTION:
					if(tOrderingChannel==9)tOrderingChannel = 5;
					if(tPacketPriority==NUMBER_OF_PRIORITIES)tPacketPriority = LOW_PRIORITY;
					if(tPacketReliability==NUMBER_OF_RELIABILITIES)tPacketReliability = UNRELIABLE_SEQUENCED;

					if(tPacketPriority!=NUMBER_OF_PRIORITIES && tPacketReliability!=NUMBER_OF_RELIABILITIES)
					{
						bool tReadSenderToken = false;
						if(imTheOwner(p))
						{
							RakNet::BitStream tReceiveBit(p->data, p->length, false);
							MessageID tMessage;
							tReceiveBit.Read(tMessage);

							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(getOwnerToken(p));
							tBitStream.Write(&tReceiveBit);
							sendAllServers(&tBitStream,tPacketPriority,tPacketReliability,tOrderingChannel,UNASSIGNED_SYSTEM_ADDRESS);
						}
						else tReadSenderToken = true;

						RakNet::BitStream tReceiveBit(p->data, p->length, false);
						MessageID tMessage;
						OwnerToken tSender;
						OwnerToken tToken;
						tReceiveBit.Read(tMessage);
						if(tReadSenderToken)
						{
							tReceiveBit.Read(tSender);
							if(tSender<=0 || tSender>MAX_CLIENTS)break;
						}
						tReceiveBit.Read(tToken);

						/*if(showTraffic)
						{
							printf("%i: ", tToken);
							printf("%i\n", tMessage);
						}*/
						//Scan for private messages
						if(packetIdentifier==ID_CHAT)
						{
							bool tIsPM = false;
							tReceiveBit.Read(tIsPM);

							//Private Message
							if(tIsPM && tToken==TOKEN_NULL)
							{
								char tPMTarget[16]="";
								StringCompressor::Instance()->DecodeString(tPMTarget, 16, &tReceiveBit);

								if(showChat)
								{
									if (tReadSenderToken)printChat(tSender, &tReceiveBit, tPMTarget);
									else printChat(p, &tReceiveBit, tPMTarget);
								}

								//Set target token to PMTarget (affects who to send to below)
								tToken = getTokenByName(tPMTarget,false);
								//Target doesn't exist
								if(tToken==0 && !tReadSenderToken)
								{
									RakNet::BitStream tBitStream;
									string tMessage = "Failed to send Private Message to ";
									tMessage += tPMTarget;
									tMessage += ".";

									tBitStream.Write(MessageID(ID_CHAT));
									tBitStream.Write(OwnerToken(SERVER_ID));
									StringCompressor::Instance()->EncodeString(tMessage.c_str(), 256, &tBitStream);
									tBitStream.Write(false);
								
									server->Send(&tBitStream, tPacketPriority, tPacketReliability, tOrderingChannel, p->systemAddress, false);
									break;
								}
								if(clientAdd[tToken-1]==UNASSIGNED_SYSTEM_ADDRESS || !clientIsMine[tToken-1])break;
								string tPlayerName = "", tMap;
								if(tReadSenderToken)tPlayerName = clientName[tSender-1];
								else getOwnerToken(p,tMap,tPlayerName);
								if(tPlayerName!="")
								{
									char tChat[256];
									StringCompressor::Instance()->DecodeString(tChat, 256, &tReceiveBit);

									RakNet::BitStream tBitStream;
									string tMessage = "((";
									tMessage += tPlayerName;
									tMessage += ")) ";
									tMessage += tChat;

									tBitStream.Write(MessageID(ID_CHAT));
									tBitStream.Write(OwnerToken(SERVER_ID));
									StringCompressor::Instance()->EncodeString(tMessage.c_str(), 256, &tBitStream);
									tBitStream.Write(true);
									StringCompressor::Instance()->EncodeString(tPlayerName.c_str(), 16, &tBitStream);
								
									server->Send(&tBitStream, tPacketPriority, tPacketReliability, tOrderingChannel, clientAdd[tToken-1], false);
									break;
								}
							}
							//Party/Targeted Message (insert sayer's name)
							else if(tIsPM && tToken!=TOKEN_NULL)
							{
								if(tToken<=MAX_CLIENTS && tToken>0)
								if(clientAdd[tToken-1]!= UNASSIGNED_SYSTEM_ADDRESS)
								if(clientIsMine[tToken-1])
								{
									if(showChat)
									{
										if (tReadSenderToken)printChat(tSender, &tReceiveBit);
										else printChat(p, &tReceiveBit);
									}

									char tChat[256];
									string tMapName,tSenderName;
									StringCompressor::Instance()->DecodeString(tChat, 256, &tReceiveBit);
									if(tReadSenderToken)
									{
										tMapName = clientMap[tSender-1];
										tSenderName = clientName[tSender-1];
									}
									else getOwnerToken(p,tMapName,tSenderName);

									RakNet::BitStream tBitStream;
									string tMessage = "(";
									tMessage += tSenderName;
									tMessage += ") ";
									tMessage += tChat;

									tBitStream.Write(MessageID(ID_CHAT));
									tBitStream.Write(OwnerToken(SERVER_ID));
									StringCompressor::Instance()->EncodeString(tMessage.c_str(), 256, &tBitStream);
									tBitStream.Write(false);
									tBitStream.Write(true);
								
									
									server->Send(&tBitStream, tPacketPriority, tPacketReliability, tOrderingChannel, clientAdd[tToken-1], false);
									break;
								}
							}

							if(showChat && !tIsPM) // dont show privates
							{
								if (tReadSenderToken)printChat(tSender, &tReceiveBit);
								else printChat(p, &tReceiveBit);
							}
						}
						//Save item equip
						if(packetIdentifier==ID_ITEMEQUIP && tToken==TOKEN_NULL && !tReadSenderToken)
						{
							RakNet::BitStream tCopyBit(p->data, p->length, false);
							tCopyBit.Read(tMessage);
							tCopyBit.Read(tToken);
							char tItem[256];
							unsigned short tSlot;
							bool tDontTellMainServer = false;
							StringCompressor::Instance()->DecodeString(tItem, 16, &tCopyBit);
							tCopyBit.Read(tSlot);
							tCopyBit.Read(tDontTellMainServer);
							
							if(!tDontTellMainServer)
							{
								const OwnerToken tOwner = getOwnerToken(p);
								RakNet::BitStream tBitStream;

								tBitStream.Write(tMessage);
								tBitStream.Write(tOwner);
								StringCompressor::Instance()->EncodeString(tItem, 16, &tBitStream);
								tBitStream.Write(tSlot);
								server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, false);
							}
						}
						//Save item unequip
						if(packetIdentifier==ID_ITEMUNEQUIP && !tReadSenderToken)
						{
							RakNet::BitStream tCopyBit(p->data, p->length, false);

							tCopyBit.Read(tMessage);
							tCopyBit.Read(tToken);
							unsigned short tSlot;
							tCopyBit.Read(tSlot);

							const OwnerToken tOwner = getOwnerToken(p);
							RakNet::BitStream tBitStream;

							tBitStream.Write(tMessage);
							tBitStream.Write(tOwner);
							tBitStream.Write(tSlot);
							server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, false);
						}

						//no one in particular, broadcast to all except sender
						if(tToken==TOKEN_NULL)
						{
							RakNet::BitStream tBitStream;
							string tMapName;
							unsigned char tDimension;

							tBitStream.Write(tMessage);
							tBitStream.Write(tReadSenderToken?tSender:getOwnerToken(p,tMapName,tDimension));
							tBitStream.Write(&tReceiveBit);

							if(tReadSenderToken)
								broadcastInMap(clientMap[tSender-1], clientDimension[tSender-1], server, &tBitStream, tPacketPriority, tPacketReliability, tOrderingChannel, UNASSIGNED_SYSTEM_ADDRESS);
							else
								broadcastInMap(tMapName, tDimension, server, &tBitStream, tPacketPriority, tPacketReliability, tOrderingChannel, p->systemAddress);
							//server->Send(&tBitStream, tPacketPriority, tPacketReliability, tOrderingChannel, p->systemAddress, true);
						}
						//target recepient specified, send only to target
						else
						{
							if(tToken<=MAX_CLIENTS && tToken>0)
							if(clientAdd[tToken-1]!= UNASSIGNED_SYSTEM_ADDRESS)
							{
								if(!clientIsMine[tToken-1])break;
								RakNet::BitStream tBitStream;

								tBitStream.Write(tMessage);
								tBitStream.Write(tReadSenderToken?tSender:getOwnerToken(p));
								tBitStream.Write(&tReceiveBit);

								server->Send(&tBitStream, tPacketPriority, tPacketReliability, tOrderingChannel, clientAdd[tToken-1], false);
							}
						}
					}
					break;

				default:
					break;
					/*if(!hideText)
					{
						std::ostringstream sin;
						printf("%i from %s\n",(int)packetIdentifier,p->systemAddress.ToString());
						//puts((char*)p->data);
						ofstream outFile("./exceptions.log",ios_base::app);
						char tBuffer[128] = "";
						sin << (int)packetIdentifier;
						std::string val = sin.str();
						strcpy(tBuffer,val.c_str());
						//_itoa_s((int)packetIdentifier,tBuffer,10);
						outFile.write(tBuffer,strlen(tBuffer));
						strcpy(tBuffer," from ");
						outFile.write(tBuffer,strlen(tBuffer));
						strcpy(tBuffer,p->systemAddress.ToString());
						outFile.write(tBuffer,strlen(tBuffer));
						strcpy(tBuffer,"\n");
						outFile.write(tBuffer,strlen(tBuffer));
						outFile.close();
					}*/
			}

			server->DeallocatePacket(p);
			if(tCont)p = server->Receive();
		}
	}
}
void ServerManager::printChat(Packet *p, RakNet::BitStream *bitStream, const char* pmTarget)
{
	char tChat[256];
	BitSize_t offset = bitStream->GetReadOffset();
	StringCompressor::Instance()->DecodeString(tChat, 256, bitStream);
	bitStream->SetReadOffset(offset);

	const OwnerToken tPlayerToken = getOwnerToken(p);
	printf("(%i)%s: ", tPlayerToken, clientName[tPlayerToken - 1].c_str());
	if (strcmp(pmTarget, "") != 0)printf("[%s]: ", pmTarget);
	printf("%s\n", tChat);
}
void ServerManager::printChat(const OwnerToken &sender, RakNet::BitStream *bitStream, const char* pmTarget)
{
	if (sender <= 0 || sender>MAX_CLIENTS)return;
	char tChat[256];
	BitSize_t offset = bitStream->GetReadOffset();
	StringCompressor::Instance()->DecodeString(tChat, 256, bitStream);
	bitStream->SetReadOffset(offset);

	printf("(%i)%s: ", sender, clientName[sender - 1].c_str());
	if (strcmp(pmTarget, "") != 0)printf("[%s]: ", pmTarget);
	printf("%s\n", tChat);
}
void ServerManager::sendNewPlayerProcedure(const OwnerToken &playerToken, const string &mapName, const unsigned char &dimension)
{
	if(playerToken==0)return;

	//allow player to start game first
	const bool tHasOtherPlayers = (playersInMap(mapName,dimension)>1);
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_WORLDINFO));
		tBitStream.Write(OwnerToken(SERVER_ID));
		tBitStream.Write(tHasOtherPlayers);

		server->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 1, clientAdd[playerToken-1], false);
		if(showMapChange)printf("Sent direct %s data to player %i.\n",mapName.c_str(),playerToken);
	}
	if(tHasOtherPlayers)
	{
		//Ping all old players to send newplayer info
		RakNet::BitStream tBitStream2;

		tBitStream2.Write(MessageID(ID_NEWPLAYERUPDATE));
		tBitStream2.Write(playerToken);

		broadcastInMap(mapName, dimension, server, &tBitStream2, HIGH_PRIORITY, RELIABLE, 1, clientAdd[playerToken-1]);
		if(showMapChange)printf("Broadcasted ID_NEWPLAYERUPDATE in %s for player %i.\n",mapName.c_str(),playerToken);
			
		/*//request map info from a player
		pair<int,SystemAddress> tLowestPing;
		tLowestPing.second = UNASSIGNED_SYSTEM_ADDRESS;
		for(int i=0;i<MAX_CLIENTS;i++)
		{
			if(clientIsMine[i] && clientHasTime[i] && i!=(playerToken-1) && !clientIsKicked[i] && clientMap[i]==mapName && clientDimension[i]==dimension)
			{
				const int tPing = server->GetAveragePing(clientAdd[i]);
				if(tLowestPing.second==UNASSIGNED_SYSTEM_ADDRESS || (tLowestPing.first>tPing&&tPing!=-1))
				{
					tLowestPing.first = tPing;
					tLowestPing.second = clientAdd[i];
				}
			}
		}
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_REQUESTMAPINFO));
		tBitStream.Write(playerToken);
		if(tLowestPing.second!=UNASSIGNED_SYSTEM_ADDRESS)
		{
			server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 1, tLowestPing.second, false);
			if(showMapChange)printf("Requested a player to send %s data to player %i.\n",mapName.c_str(),playerToken);
		}
		else sendNextServer(&tBitStream, HIGH_PRIORITY, RELIABLE, 1);*/
	}

	//Broadcast player data
	RakNet::BitStream tBitStream;

	tBitStream.Write(MessageID(ID_PLAYERDATA));
	tBitStream.Write(true);
	StringCompressor::Instance()->EncodeString(clientName[playerToken - 1].c_str(), 16, &tBitStream);
	StringCompressor::Instance()->EncodeString(clientMap[playerToken - 1].c_str(), 32, &tBitStream);

	sendMyPlayers(server,&tBitStream,HIGH_PRIORITY, RELIABLE, 1,UNASSIGNED_SYSTEM_ADDRESS);
}
void ServerManager::broadcastInMap(const string &mapName, const unsigned char &dimension, RakPeerInterface *peer, RakNet::BitStream *bitStream, const PacketPriority &priority, const PacketReliability &reliability, const char &orderingChannel, const SystemAddress &exceptionAddress)
{
	if(mapName=="" || !peer)return;
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(clientIsMine[i] && clientAdd[i]!=UNASSIGNED_SYSTEM_ADDRESS && clientDimension[i]==dimension && clientAdd[i]!=exceptionAddress && clientMap[i]==mapName)
			peer->Send(bitStream, priority, reliability, orderingChannel, clientAdd[i], false);
	}
}
void ServerManager::sendMyPlayers(RakPeerInterface *peer, RakNet::BitStream *bitStream, const PacketPriority &priority, const PacketReliability &reliability, const char &orderingChannel, const SystemAddress &exceptionAddress)
{
	if(!peer)return;
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(clientIsMine[i] && clientAdd[i]!=UNASSIGNED_SYSTEM_ADDRESS && clientAdd[i]!=exceptionAddress)
			peer->Send(bitStream, priority, reliability, orderingChannel, clientAdd[i], false);
	}
}
void ServerManager::relayPacket(Packet *p, const PacketPriority &priority, const PacketReliability &reliability, const char &orderingChannel)
{
	if(!imTheOwner(p))return;
	RakNet::BitStream tReceiveBit(p->data, p->length, false);
	sendAllServers(&tReceiveBit, priority, reliability, orderingChannel, UNASSIGNED_SYSTEM_ADDRESS);
}
void ServerManager::sendAllServers(RakNet::BitStream *bitStream, const PacketPriority &priority, const PacketReliability &reliability, const char &orderingChannel, const SystemAddress &exceptionAddress)
{
	for(int i=0;i<MAX_SERVERS;i++)
	{
		if(serverAdd[i]!=UNASSIGNED_SYSTEM_ADDRESS && serverAdd[i]!=exceptionAddress && serverConnected[i])
		{
			server->Send(bitStream, priority, reliability, orderingChannel, serverAdd[i], false);
		}
	}
}
void ServerManager::sendNextServer(RakNet::BitStream *bitStream, const PacketPriority &priority, const PacketReliability &reliability, const char &orderingChannel)
{
	for(int i=(serverID+1);i<MAX_SERVERS;i++)
	{
		if(serverAdd[i]!=UNASSIGNED_SYSTEM_ADDRESS && serverConnected[i])
		{
			server->Send(bitStream, priority, reliability, orderingChannel, serverAdd[i], false);
			return;
		}
	}
	for(int i=0;i<serverID;i++)
	{
		if(serverAdd[i]!=UNASSIGNED_SYSTEM_ADDRESS && serverConnected[i])
		{
			server->Send(bitStream, priority, reliability, orderingChannel, serverAdd[i], false);
			return;
		}
	}
}
const OwnerToken ServerManager::getOwnerToken(Packet *p, string &mapName, string &playerName)
{
	mapName = "";
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(p->systemAddress==clientAdd[i])
		{
			mapName = clientMap[i];
			playerName = clientName[i];
			return (i+1);
		}
	}
	return 0;
}
const OwnerToken ServerManager::getOwnerToken(Packet *p, string &mapName)
{
	mapName = "";
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(p->systemAddress==clientAdd[i])
		{
			mapName = clientMap[i];
			return (i+1);
		}
	}
	return 0;
}
const OwnerToken ServerManager::getOwnerToken(Packet *p, string &mapName, unsigned char &dimension)
{
	mapName = "";
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(p->systemAddress==clientAdd[i])
		{
			mapName = clientMap[i];
			dimension = clientDimension[i];
			return (i+1);
		}
	}
	return 0;
}
const OwnerToken ServerManager::getOwnerToken(Packet *p)
{
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(p->systemAddress==clientAdd[i])
		{
			return (i+1);
		}
	}
	return 0;
}
const OwnerToken ServerManager::getTokenByName(const string &playerName, bool caseSensitive)
{
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(caseSensitive)
		{
			if(clientName[i]==playerName)return (i+1);
		}
		else
		{
			if(toLowerCase(clientName[i])==toLowerCase(playerName))return (i+1);
		}
	}
	return 0;
}
const OwnerToken ServerManager::getTokenByName(const string &playerName, bool caseSensitive, string &map, unsigned char &dimension)
{
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(caseSensitive)
		{
			if(clientName[i]==playerName)
			{
				map = clientMap[i];
				dimension = clientDimension[i];
				return (i+1);
			}
		}
		else
		{
			if(toLowerCase(clientName[i])==toLowerCase(playerName))
			{
				map = clientMap[i];
				dimension = clientDimension[i];
				return (i+1);
			}
		}
	}
	return 0;
}
const string ServerManager::toLowerCase(string text)
{
	const char tDiff = char('A') - char('a');
	for(int i=0;i<(int)text.length();i++)
	{
		if(text[i]>=char('A')&&text[i]<=char('Z'))text[i] -= tDiff;
	}
	return text;
}
void ServerManager::shutdown()
{
	unregisterAllServers();
	requestedPlayerData = false;
	serverID = MAX_SERVERS;
	if (server)
		server->Shutdown(300);
	resetData();
	bootlist.clear();
}
void ServerManager::quit()
{
	alive = false;
}
void ServerManager::resetData()
{
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		unassignToken(i+1);
	}
	numClients = 0;
}
const OwnerToken ServerManager::unassignToken(Packet *p)
{
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(clientAdd[i] == p->systemAddress)
		{
			unassignToken(i+1);
			return (i+1);
		}
	}
	return 0;
}
void ServerManager::unassignToken(const OwnerToken &token)
{
	if(token<=0 || token>MAX_CLIENTS)return;
	clientIsMine[token-1] = false;
	clientAdd[token-1] = UNASSIGNED_SYSTEM_ADDRESS;
	clientMap[token-1] = "";
	clientName[token-1] = "";
	clientIsReady[token-1] = false;
	clientIsKicked[token-1] = false;
	clientHasTime[token-1] = false;
	numClients -= 1;
}
const unsigned short ServerManager::playersInMap(const string &mapName, const unsigned char &dimension)
{
	unsigned short tNum = 0;
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(clientMap[i]==mapName /*&& clientDimension[i]==dimension*/)
		{
			tNum += 1;
		}
	}
	return tNum;
}
void ServerManager::assignPlayerData(const OwnerToken &token, bool isMine, Packet *p, const string &playerName, const string &mapName, const unsigned char &dimension/*, const unsigned short &charID*/)
{
	if(token>0 && token<=MAX_CLIENTS)
	{
		clientIsMine[token-1] = isMine;
		clientAdd[token-1] = p->systemAddress;
		clientName[token-1] = playerName;
		clientMap[token-1] = mapName;
		clientDimension[token-1] = dimension;
	}
}
void ServerManager::sendTokenDisconnectProcedure(const OwnerToken &token)
{
	sendDisconnectProcedureToMyPlayers(token,true);
}
void ServerManager::sendDisconnectProcedure(Packet *p, bool normalDisconnect)
{
	const OwnerToken tPlayerToken = getOwnerToken(p);
	//Registered player
	if(tPlayerToken!=0)
	{
		RakNet::BitStream tReceiveBit(p->data, p->length, false);
		MessageID tMessage;
		tReceiveBit.Read(tMessage);

		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_PLAYERDISCONNECTED));
		tBitStream.Write(tPlayerToken);
		tBitStream.Write(normalDisconnect);
			
		sendAllServers(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS);
		sendTokenDisconnected(tPlayerToken);
		sendDisconnectProcedureToMyPlayers(tPlayerToken,normalDisconnect);
	}
	//Unregistered player
	else sendTokenDisconnected(p->systemAddress);
}
void ServerManager::sendRemoteDisconnectProcedure(Packet *p)
{
	RakNet::BitStream tReceiveBit(p->data, p->length, false);
	MessageID tMessage;
	OwnerToken tPlayerToken;
	bool tNormalDisconnect;

	tReceiveBit.Read(tMessage);
	tReceiveBit.Read(tPlayerToken);
	tReceiveBit.Read(tNormalDisconnect);
		
	sendDisconnectProcedureToMyPlayers(tPlayerToken,tNormalDisconnect);
}
void ServerManager::sendDisconnectProcedureToMyPlayers(const OwnerToken &token, bool normalDisconnect)
{
	string tPlayerName = "";
	string tMapName = "";
	unsigned char tDimension = 0;
	if(token>0 && token<=MAX_CLIENTS)
	{
		tPlayerName = clientName[token-1];
		tMapName = clientMap[token-1];
		tDimension = clientDimension[token-1];
	}
	else return;
	if(normalDisconnect)
	{
		if(!hideText)printf("ID_DISCONNECTION_NOTIFICATION from %s ", clientAdd[token-1].ToString());
		if(!hideText)printf("(Player token %i)\n", token);
	}
	else
	{
		if(!hideText)printf("ID_CONNECTION_LOST from %s ", clientAdd[token-1].ToString());
		if(!hideText)printf("(Player token %i)\n", token);
	}
	//Broadcast player disconnect in map
	RakNet::BitStream tBitStream;

	tBitStream.Write(MessageID(ID_PLAYERDISCONNECTED));
	tBitStream.Write(token);
	tBitStream.Write(normalDisconnect);
	broadcastInMap(tMapName, tDimension, server, &tBitStream, HIGH_PRIORITY, RELIABLE, 0, clientAdd[token-1]);

	//Broadcast delete player data
	if(tPlayerName!="")
	{
		RakNet::BitStream tBitStream2;

		tBitStream2.Write(MessageID(ID_PLAYERDATA));
		tBitStream2.Write(false);
		StringCompressor::Instance()->EncodeString(tPlayerName.c_str(), 16, &tBitStream2);

		sendMyPlayers(server, &tBitStream2, LOW_PRIORITY, RELIABLE, 1, clientAdd[token-1]);
	}
	unassignToken(token);
}
void ServerManager::logNumPlayers()
{
	std::ostringstream sin;
	if(!printlog)return;
	ofstream outFile("numplayers.log");
	char tBuffer[32] = "";
	sin << numClients;
	std::string val = sin.str();
	strcpy(tBuffer,val.c_str());
	//_itoa_s(numClients,tBuffer,10);
	outFile.write(tBuffer,strlen(tBuffer));
	outFile.close();
}
void ServerManager::registerServer(Packet *p)
{
	for(int i=0; i<MAX_SERVERS; i++)
	{
		if(serverAdd[i]==p->systemAddress)
		{
			serverConnected[i] = true;
			if(!hideText)printf("Server %i connected, IP: %s\n",i+1,p->systemAddress.ToString());
			break;
		}
	}
}
bool ServerManager::unregisterServer(const SystemAddress &add)
{
	for(int i=0; i<MAX_SERVERS; i++)
	{
		if(serverAdd[i]==add)
		{
			if(serverConnected[i])sendDisconnectProcedureOfAllTokensConnectedTo(add);
			serverAdd[i] = UNASSIGNED_SYSTEM_ADDRESS;
			serverConnected[i] = false;
			if(!hideText)printf("Server %i disconnected, IP: %s\n",i+1,add.ToString());
				
			return true;
		}
	}
	return false;
}
bool ServerManager::unregisterServer(Packet *p)
{
	return unregisterServer(p->systemAddress);
}
bool ServerManager::isServerAdd(const SystemAddress &add)
{
	for(int i=0; i<MAX_SERVERS; i++)
	{
		if(serverAdd[i]==add)return true;
	}
	return false;
}
bool ServerManager::imTheOwner(Packet *p)
{
	const SystemAddress tAdd = p->systemAddress;
	if(tAdd==mainServerAdd)return false;
	/*for(int i=0; i<MAX_SERVERS; i++)
	{
		if(serverAdd[i]==p->systemAddress)return false;
	}
	return true;*/
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(clientAdd[i]==tAdd)return clientIsMine[i];
	}
	return false;
}
void ServerManager::unregisterAllServers()
{
	for(int i=0; i<MAX_SERVERS; i++)
	{
		serverAdd[i] = UNASSIGNED_SYSTEM_ADDRESS;
		serverConnected[i] = false;
	}
}
void ServerManager::sendTokenDisconnected(const SystemAddress &add)
{
	RakNet::BitStream tBitStream;

	tBitStream.Write(MessageID(ID_TOKENDISCONNECTED));
	tBitStream.Write(false);
	tBitStream.Write(SystemAddress(add));

	server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, false);
}
void ServerManager::sendTokenDisconnected(const OwnerToken &tToken)
{
	if(tToken<=MAX_CLIENTS && tToken>0)
	if(clientAdd[tToken-1]!= UNASSIGNED_SYSTEM_ADDRESS)
	if(clientIsMine[tToken-1])
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_TOKENDISCONNECTED));
		tBitStream.Write(true);
		tBitStream.Write(OwnerToken(tToken));

		server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, mainServerAdd, false);
	}
}
void ServerManager::sendDisconnectProcedureOfAllTokensConnectedTo(const SystemAddress &add)
{
	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(clientAdd[i]==add)
		{
			sendTokenDisconnectProcedure(i+1);
		}
	}
}
bool ServerManager::needsUpdate()
{
	return doUpdate;
}
void ServerManager::bootPlayer(const char *tName)
{
	const OwnerToken tCurrentToken = getTokenByName(tName);
	if(tCurrentToken>0 && tCurrentToken<=MAX_CLIENTS)
	{
		if(clientIsMine[tCurrentToken-1])
		{
			//Boot current user out
			RakNet::BitStream tBitStream;
			tBitStream.Write(MessageID(ID_FORCELOGOUT));
			StringCompressor::Instance()->EncodeString(tName, 16, &tBitStream);
			server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 1, clientAdd[tCurrentToken-1], false);
		}
		clientIsKicked[tCurrentToken-1] = true;
	}
}
bool ServerManager::ShowChat()
{
	showChat = !showChat;
	return showChat;
}
bool ServerManager::HideText()
{
	hideText = !hideText;
	return hideText;
}
RakNetStatistics* ServerManager::getStatistics()
{
	return server->GetStatistics(server->GetSystemAddressFromIndex(0));
}
int ServerManager::getAveragePing()
{
	return server->GetAveragePing(server->GetSystemAddressFromIndex(0));
}
const bool ServerManager::readIPs()
{
	ifstream inFile("connection.txt");
	if(!inFile.good())return false;
	char buffer[128] = "";
	inFile.getline(buffer,128);
	mainServerInitAdd = buffer;
	inFile.getline(buffer,128);
	broadcastAdd = buffer;
	inFile.close();
	return true;
}

int main(void)
{
	ServerManager mServerMgr;

	std::thread gameServerThread(&ServerManager::startThread, &mServerMgr);
	
	bool wekilled = false;
	while (gameServerThread.joinable())
	{
		RakSleep(200);
#ifdef _WIN32
		if (_kbhit())
		{
			char c = _getch();
			if (c == 'Q')
			{
				puts("Quitting.");
				mServerMgr.quit();
				wekilled = true;
				break;
			}
			else if (c == 'S')
			{
				char temp[2048] = "";
				RakNetStatistics* rss = mServerMgr.getStatistics();
				StatisticsToString(rss, temp, 2);
				printf("%s", temp);
				printf("Ping %i\n", mServerMgr.getAveragePing());
			}
			else if (c == 'C')
			{
				bool res = mServerMgr.ShowChat();
				if (res)printf("Showchat ON\n");
				else printf("Showchat OFF\n");
			}
			else if (c == 'H')
			{
				bool res = mServerMgr.HideText();
				if (res)system("cls");
				else printf("Unhiding text\n");
			}
			else if (c == 'V')
			{
				printf("Version: Final KITO\n");
			}
			else
			{
				printf("Possible commands:\n Q - Quit\n S - Stats\n C - Show Chat\n H - Hide Text\n V - Version\n");
			}
		}
#endif
	}
	if (!wekilled)
		mServerMgr.shutdown();
	mServerMgr.logNumPlayers();

	gameServerThread.join(); // wait for the server to exit
	RakSleep(1500);
	return 0;
}



