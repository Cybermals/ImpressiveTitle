#ifdef __MINGW__
    #include <thread>
#endif

#include "main.h"
//#include "RakNetworkFactory.h" //obsolete
#include "RakPeerInterface.h"
#include "RakPeer.h"
#include "MagixNetworkDefines.h"

#if defined(_CONSOLE_2)
#include "Console2SampleIncludes.h"
_CONSOLE_2_SetSystemProcessParams
#endif

using namespace std;
using namespace RakNet;


//Forward Declarations
//================================================================================
unsigned char GetPacketIdentifier(Packet *p);


//Utility Functions
//================================================================================
// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const string currentDateTime()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://www.cplusplus.com/reference/clibrary/ctime/strftime/
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}


// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
MessageID GetPacketIdentifier(Packet *p)
{
	if(p == 0)
    {
        return 255;
    }

	if((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
    {
		return (unsigned char)p->data[0];
    }
}


//Server Manager Functions
//================================================================================
ServerManager::ServerManager()
{
	server = new RakPeer();
	server->SetIncomingPassword(SERVER_PASSWORD, (int)strlen(SERVER_PASSWORD));

	numClients = 0;
	numServers = 0;
	showTraffic = false;

	for(int i = 0; i < MAX_SERVERS; i++)
	{
		serverAdd[i] = UNASSIGNED_SYSTEM_ADDRESS;
		serverTunnelAdd[i] = UNASSIGNED_SYSTEM_ADDRESS;
		serverFull[i] = false;
	}

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		playerToken[i] = PlayerToken();
	}

	banlist.clear();
	loginSession.clear();
	alive = true;
}


ServerManager::~ServerManager()
{
	delete server;
}


void ServerManager::startThread()
{
	bool inited = false;

	while(!inited)
	{
		inited = initialize();

		if(!inited)
		{
			RakSleep(5000);
		}
	}

	runLoop();
	shutdown();
}


bool ServerManager::initialize()
{
	if(!alive)
	{
		return false;
	}

	cout << "Starting server..." << endl;
	SocketDescriptor socketDescriptor(MAIN_SERVER_PORT, 0);
	bool b = server->Startup(MAX_CLIENTS, &socketDescriptor, 1, 30);
	server->SetMaximumIncomingConnections(MAX_CLIENTS);

	//For some reason, the first server startup call fails :(
    if(!b)
    {
        b = server->Startup(MAX_CLIENTS, &socketDescriptor, 1, 30);
    }

	if(b)
	{
		cout << "Server started, waiting for connections." << endl;
	}
	else
	{
		cout << "Server failed to start.  Terminating." << endl;
		return false;
	}

	server->SetOccasionalPing(true);
	cout << "Max players allowed: " << MAX_CLIENTS << endl;

	dayTime = 500;
	weatherTime = 0;
	maintenanceTime = 0;
	time(&prevTime);
	loadPingRange();
	loadBanlist();

	return true;
}


void ServerManager::runLoop()
{
	//char message[512]="";
	string message = "";

	// Loop for input
	while(alive)
	{

		// This sleep keeps RakNet responsive
		RakSleep(30);

		updateTimer();
		updateServers();

		// Get a packet from either the server or the client

		Packet *p = server->Receive();

		if(!alive)
		{
			break;
		}

		if(!server->IsActive())
		{
			cout << "-------------------------" << endl;
			cout << "Network threads have died" << endl;
			cout << "    Restarting Raknet    " << endl;
			cout << "-------------------------" << endl;
			shutdown();
			initialize();
		}

		/*if(p==0)
			continue; // Didn't get any packets*/

		while(p)
		{
			updateTimer();
			updateServers();

			// We got a packet, get the identifier with our handy function
			const unsigned char packetIdentifier = GetPacketIdentifier(p);

			// Check if this is a network message packet
			switch(packetIdentifier)
			{
			case ID_NEW_INCOMING_CONNECTION:
				cout << "ID_NEW_INCOMING_CONNECTION from " << p->systemAddress.ToString() << endl;
				//sendServerList(p->systemAddress);
				break;

			case ID_DISCONNECTION_NOTIFICATION:
				cout << "ID_DISCONNECTION_NOTIFICATION from " << p->systemAddress.ToString() << endl;
				unregisterServer(p);
				endSession(p);
				break;

			case ID_CONNECTION_LOST:
				cout << "ID_CONNECTION_LOST from " << p->systemAddress.ToString() << endl;
				unregisterServer(p);
				endSession(p);
				break;

			case ID_TOKENCONNECTED:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				SystemAddress tAdd;
				char tUsername[16] = "";
				unsigned short tCharIndex = 0;

				tReceiveBit.Read(tMessage);
				tReceiveBit.Read(tAdd);
				StringCompressor::Instance()->DecodeString(tUsername, 16, &tReceiveBit);
				tReceiveBit.Read(tCharIndex);

				const OwnerToken tToken = assignToken(tAdd, getServerID(p->systemAddress), tUsername,
                    tCharIndex);

				if(tToken == 0)
				{
					cout << "Failed to assign token for " << tAdd.ToString() << "!\n";
					ofstream outFile("./exceptions.log", ios::app);

					if(outFile.good())
					{
						outFile << "Failed to assign token for " << tAdd.ToString() << endl;
					}

					outFile.close();
					break;
				}

				RakNet::BitStream tBitStream;
				tBitStream.Write(MessageID(ID_TOKENCONNECTED));
				tBitStream.Write(tToken);
				tBitStream.Write(tAdd);
				tBitStream.Write((unsigned short)dayTime);
				tBitStream.Write((unsigned short)weatherTime);

				server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, p->systemAddress, false);
				break;
			}

			case ID_TOKENDISCONNECTED:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				bool tIsToken;
				OwnerToken tToken;
				SystemAddress tAdd;
				tReceiveBit.Read(tMessage);
				tReceiveBit.Read(tIsToken);

				if(tIsToken)
				{
					tReceiveBit.Read(tToken);
					unassignToken(tToken);
				}
				else
				{
					tReceiveBit.Read(tAdd);
					unassignToken(tAdd);
				}

				break;
			}

			/*case ID_MODIFIED_PACKET:
				{
				cout << "ID_MODIFIED_PACKET from " <<
				p->systemAddress.ToString() << endl;
				}
				break;*/

			case ID_LOGON:
			{
				cout << "ID_LOGON from " << p->systemAddress.ToString() << endl;

				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				char tUsername[16] = "";
				char tPassword[16] = "";
				tReceiveBit.Read(tMessage);
				StringCompressor::Instance()->DecodeString(tUsername, 16, &tReceiveBit);
				StringCompressor::Instance()->DecodeString(tPassword, 16, &tReceiveBit);

				bool tLogonSuccess = false;

				if(strlen(tUsername) > 0 && strlen(tPassword) > 0)
				{
					bool tRenameFile = false, tEncryptPass = false,
						tFileExists = false, tResaveFile = false,
						tReEncryptPass = false;
					char tPassword2[16] = "";
					char tEmail[64] = "";
					char tQuestion[128] = "";
					char tAnswer[128] = "";
					ifstream inFile(getFilename(tUsername, ".user"), ios::binary);

					if(!inFile.good())
					{
						inFile.clear();
						inFile.open(getFilename(tUsername, ".user", false), ios::binary);

						if(inFile.good())
						{
							tRenameFile = true;
						}
					}

					if(inFile.good())
					{
						tFileExists = true;
						inFile.getline(tPassword2, 16);
						const string tPassStr = tPassword;
						const string tPassStr2 = XOR7(tPassword2);

						if(tPassStr == tPassStr2)
						{
							tLogonSuccess = true;
						}
					}

					inFile.close();

					if(tRenameFile)
					{
						rename(getFilename(tUsername, ".user", false).c_str(),
                            getFilename(tUsername, ".user").c_str());
					}

					//Read file as non binary and try again
					if(tFileExists && !tLogonSuccess)
					{
						inFile.clear();
						fstream inFile(getFilename(tUsername, ".user"));

						if(inFile.good())
						{
							inFile.getline(tPassword2, 16);
							const string tPassStr = tPassword;
							const string tPassStr2 = XOR7(tPassword2);

							if(tPassStr == tPassStr2)
							{
								tLogonSuccess = true;
								tResaveFile = true;
								inFile.getline(tEmail, 64);
								inFile.getline(tQuestion, 128);
								inFile.getline(tAnswer, 128);
							}
							else if(tPassStr == string(tPassword2))
							{
								tLogonSuccess = true;
								tResaveFile = true;
								tEncryptPass = true;
								inFile.getline(tEmail, 64);
								inFile.getline(tQuestion, 128);
								inFile.getline(tAnswer, 128);
							}
							else if(tPassStr == XOR7OLD(tPassword2))
							{
								tLogonSuccess = true;
								tResaveFile = true;
								tReEncryptPass = true;
								inFile.getline(tEmail, 64);
								inFile.getline(tQuestion, 128);
								inFile.getline(tAnswer, 128);
							}
						}

						inFile.close();
					}

					//Read file as OLDXOR7 and try again
					if(tFileExists && !tLogonSuccess)
					{
						inFile.clear();
						ifstream inFile(getFilename(tUsername, ".user"), ios::binary);

						if(inFile.good())
						{
							inFile.getline(tPassword2, 16);
							const string tPassStr = tPassword;
							const string tPassStr2 = XOR7OLD(tPassword2);

							if(tPassStr == tPassStr2)
							{
								tLogonSuccess = true;
								tResaveFile = true;
								tReEncryptPass = true;
								inFile.getline(tEmail, 64);
								inFile.getline(tQuestion, 128);
								inFile.getline(tAnswer, 128);
							}
						}

						inFile.close();
					}

					if(tResaveFile)
					{
						ofstream outFile(getFilename(tUsername, ".user"), ios::binary);

						if(outFile.good())
						{
							string tBuffer = "";

							if(tEncryptPass)
							{
								tBuffer = XOR7(tPassword2);
							}
							else if(tReEncryptPass)
							{
								const string tOrigPass = XOR7OLD(tPassword2);
								tBuffer = XOR7(tOrigPass);
							}
							else
							{
								tBuffer = string(tPassword2);
							}

							tBuffer += "\n";
							outFile.write(tBuffer.c_str(), tBuffer.length());
							tBuffer = XOR7(tEmail);
							tBuffer += "\n";
							outFile.write(tBuffer.c_str(), tBuffer.length());
							tBuffer = XOR7(tQuestion);
							tBuffer += "\n";
							outFile.write(tBuffer.c_str(), tBuffer.length());
							tBuffer = XOR7(tAnswer);
							tBuffer += "\n";
							outFile.write(tBuffer.c_str(), tBuffer.length());
						}

						outFile.close();
					}
				}

				short tCount = 0;
				bool tDoInstantBoot = false;

				if(tLogonSuccess)
				{
					//Check if username is in banlist
					const string tUsernameStr = toLowerCase(string(tUsername));

					for(int i = 0; i < (int)banlist.size(); i++)
					{
						//Ban this IP
						if(toLowerCase(banlist[i].name) == tUsernameStr)
						{
							const string tIP = p->systemAddress.ToString(false);
							banlist[i].IPList.push_back(tIP);
							server->AddToBanList(tIP.c_str());
							tLogonSuccess = false;
							break;
						}
					}

					{
						bool tRenameFile = false;
						ifstream inFile(getFilename(tUsername, ".charlist"));

						if(!inFile.good())
						{
							inFile.clear();
							inFile.open(getFilename(
								tUsername, ".charlist", false).c_str());

							if(inFile.good())
							{
								tRenameFile = true;
							}
						}

						if(inFile.good())
						{
							while(!inFile.eof())
							{
								char tBuffer[32] = "";
								inFile.getline(tBuffer, 32);

								if(strlen(tBuffer) > 0)
								{
									tCount++;
								}
							}
						}

						inFile.close();

						if(tRenameFile)
						{
							rename(
								getFilename(tUsername, ".charlist", false).c_str(),
								getFilename(tUsername, ".charlist").c_str());
						}
					}

					//add to logfile
					ofstream outFile("security.log", ios::app);

					if(outFile.good())
					{
						outFile << currentDateTime() << " - LOGON - " << p->systemAddress.ToString() << "," <<
						    tUsername << endl;
					}

					outFile.close();

					//Save logged in data
					for(int i = 0; i < MAX_CLIENTS; i++)
					{
						if(toLowerCase(playerToken[i].name) == tUsernameStr)
						{
							savePlayerData(i + 1);
							tDoInstantBoot = true;
							break;
						}
					}

					//Restrict one login per username
					for(vector<pair<string, SystemAddress> >::const_iterator it = loginSession.begin();
						it != loginSession.end(); it++)
					{
						pair<string, SystemAddress> tSession = *it;

						if(tSession.first == tUsernameStr)
						{
							server->CloseConnection(tSession.second, true);
							break;
						}
					}

					loginSession.push_back(
						pair<string, SystemAddress>(tUsernameStr, p->systemAddress));

					//Notify servers to boot logged in username
					serversListMutex.Lock();

					for(int i = 0; i < MAX_SERVERS; i++)
					{
						if(serverTunnelAdd[i] != UNASSIGNED_SYSTEM_ADDRESS)
						{
							RakNet::BitStream tBitStream;

							tBitStream.Write(MessageID(ID_FORCELOGOUT));
							tBitStream.Write(true);
							StringCompressor::Instance()->EncodeString(
								tUsernameStr.c_str(), 16, &tBitStream);

							server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE,
								0, serverTunnelAdd[i], false);
						}
					}

					serversListMutex.Unlock();
				}

				RakNet::BitStream tBitStream;

				tBitStream.Write(MessageID(ID_LOGON));
				tBitStream.Write(tLogonSuccess);

				if(tLogonSuccess)
				{
					tBitStream.Write(tCount);
				}

				server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 1, p->systemAddress, false);

				if(tLogonSuccess)
				{
					sendServerList(p->systemAddress, true);
				}

				if(showTraffic)
				{
					if(tLogonSuccess)
					{
						cout << tUsername << " logon success.\n";
					}
					else
					{
						cout << tUsername << " logon failed.\n";
					}
				}

				break;
			}

			case ID_CREATEACCOUNT:
			{
				cout << "ID_CREATEACCOUNT from " << p->systemAddress.ToString() << endl;

				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				char tUsername[16] = "";
				char tPassword[16] = "";
				char tEmail[64] = "";
				char tQuestion[128] = "";
				char tAnswer[128] = "";
				tReceiveBit.Read(tMessage);
				StringCompressor::Instance()->DecodeString(tUsername, 16, &tReceiveBit);
				StringCompressor::Instance()->DecodeString(tPassword, 16, &tReceiveBit);
				StringCompressor::Instance()->DecodeString(tEmail, 64, &tReceiveBit);
				StringCompressor::Instance()->DecodeString(tQuestion, 128, &tReceiveBit);
				StringCompressor::Instance()->DecodeString(tAnswer, 128, &tReceiveBit);
				bool tCreateSuccess = false;

				if(strlen(tUsername) > 0 && strlen(tPassword) > 0)
				{
					//Check if account (with underscores) exists
					ifstream inFile(getFilename(tUsername, ".user"));

					if(!inFile.good())
					{
						inFile.clear();

						//Check if account (with spaces) exists
						inFile.open(
							getFilename(tUsername, ".user", false).c_str());

						if(!inFile.good())
						{
							inFile.clear();

							//Check if account (underscores replaced with spaces) exists
							inFile.open(getFilename(tUsername, ".user", false, true));

							if(!inFile.good())
							{
								tCreateSuccess = true;
							}
						}
					}

					inFile.close();

					if(tCreateSuccess)
					{
						ofstream outFile(getFilename(tUsername, ".user"), ios::binary);

						if(outFile.good())
						{
							string tBuffer = XOR7(tPassword);
							tBuffer += "\n";
							outFile.write(tBuffer.c_str(), tBuffer.length());
							tBuffer = XOR7(tEmail);
							tBuffer += "\n";
							outFile.write(tBuffer.c_str(), tBuffer.length());
							tBuffer = XOR7(tQuestion);
							tBuffer += "\n";
							outFile.write(tBuffer.c_str(), tBuffer.length());
							tBuffer = XOR7(tAnswer);
							tBuffer += "\n";
							outFile.write(tBuffer.c_str(), tBuffer.length());
						}
						else
						{
							tCreateSuccess = false;
						}

						outFile.close();
					}
				}

				RakNet::BitStream tBitStream;

				tBitStream.Write(MessageID(ID_CREATEACCOUNT));
				tBitStream.Write(tCreateSuccess);

				server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 1, p->systemAddress, false);

				if(showTraffic)
				{
					if(tCreateSuccess)
					{
						cout << tUsername << " create account success.\n";
					}
					else
					{
						cout << tUsername << " create account failed.\n";
					}
				}

				break;
			}

			case ID_EDITACCOUNT:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				char tUsername[16] = "";
				char tPassword[16] = "";
				char tNewPassword[16] = "";
				tReceiveBit.Read(tMessage);
				StringCompressor::Instance()->DecodeString(tUsername, 16, &tReceiveBit);
				StringCompressor::Instance()->DecodeString(tPassword, 16, &tReceiveBit);
				StringCompressor::Instance()->DecodeString(tNewPassword, 16, &tReceiveBit);

				bool tSuccess = false;
				char tEmail[64] = "";
				char tQuestion[128] = "";
				char tAnswer[128] = "";

				if(strlen(tUsername) > 0 && strlen(tPassword) > 0)
				{
					bool renameFile = false;
					ifstream inFile(getFilename(tUsername, ".user"), ios::binary);

					if(!inFile.good())
					{
						inFile.clear();
						inFile.open(getFilename(tUsername, ".user", false), ios::binary);

						if(inFile.good())
						{
							renameFile = true;
						}
					}

					if(inFile.good())
					{
						char tPassword2[32] = "";
						inFile.getline(tPassword2, 32);
						const string tPassStr = tPassword;
						const string tPassStr2 = XOR7(tPassword2);

						if(tPassStr == tPassStr2 || tPassStr == string(tPassword2))
						{
							tSuccess = true;
							inFile.getline(tEmail, 64);
							inFile.getline(tQuestion, 128);
							inFile.getline(tAnswer, 128);
						}
					}

					inFile.close();

					if(renameFile)
					{
						rename(getFilename(tUsername, ".user", false).c_str(),
                            getFilename(tUsername, ".user").c_str());
					}
				}

				if(tSuccess)
				{
					ofstream outFile(getFilename(tUsername, ".user"), ios::binary);

					if(outFile.good())
					{
						string tBuffer = XOR7(tNewPassword);
						tBuffer += "\n";
						outFile.write(tBuffer.c_str(), tBuffer.length());
						tBuffer = tEmail;
						tBuffer += "\n";
						outFile.write(tBuffer.c_str(), tBuffer.length());
						tBuffer = tQuestion;
						tBuffer += "\n";
						outFile.write(tBuffer.c_str(), tBuffer.length());
						tBuffer = tAnswer;
						tBuffer += "\n";
						outFile.write(tBuffer.c_str(), tBuffer.length());
					}
					else
					{
						tSuccess = false;
					}

					outFile.close();
				}

				RakNet::BitStream tBitStream;

				tBitStream.Write(MessageID(ID_EDITACCOUNT));
				tBitStream.Write(tSuccess);

				server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 1, p->systemAddress, false);

				if(showTraffic)
				{
					if(tSuccess)
					{
						cout << tUsername << " edit account success.\n";
					}
					else
					{
						cout << tUsername << " edit account failed.\n";
					}
				}

				break;
			}

			case ID_LOADCHAR:
			{
				cout << "ID_LOADCHAR from " << p->systemAddress.ToString() << endl;

				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				char tUsername[16] = "";
				short tIndex = 0;
				tReceiveBit.Read(tMessage);
				StringCompressor::Instance()->DecodeString(tUsername, 16, &tReceiveBit);
				tReceiveBit.Read(tIndex);

				bool tLoadSuccess = false;
				char tName[32] = "";
				bool renameFile = false;
				ifstream inFile(getFilename(tUsername, ".charlist"));

				if(!inFile.good())
				{
					inFile.clear();
					inFile.open(getFilename(tUsername, ".charlist", false));

					if(inFile.good())
					{
						renameFile = true;
					}
				}

				if(inFile.good())
				{
					short tCount = 0;

					while(!inFile.eof() && inFile.good())
					{
						inFile.getline(tName, 32);

						if(tIndex == tCount)
						{
							tLoadSuccess = true;
							break;
						}

						tCount++;
					}
				}

				inFile.close();

				if(renameFile)
				{
					rename(getFilename(tUsername, ".charlist", false).c_str(),
						getFilename(tUsername, ".charlist").c_str());
				}

				RakNet::BitStream tBitStream;

				tBitStream.Write(MessageID(ID_LOADCHAR));
				tBitStream.Write(tIndex);
				tBitStream.Write(tLoadSuccess);

				if(tLoadSuccess)
				{
					string tFilename = tUsername;
					tFilename += "_";
					tFilename += tName;

					char tData[512] = "";
					char tAdminToken[16] = "";
					char tItem[MAX_EQUIP][16];

					for(int j = 0; j < MAX_EQUIP; j++)
					{
						strcpy(tItem[j], "");
					}

					char tHP[16] = "";
					char tSkills[512] = "";
					char tPet[32] = "";

					//Load char data
					bool tRenameFile = false;
					inFile.open(getFilename(tFilename.c_str(), ".character"));

					if(!inFile.good())
					{
						inFile.clear();
						inFile.open(getFilename(tFilename.c_str(), ".character", false));

						if(inFile.good())
						{
							tRenameFile = true;
						}
					}

					if(inFile.good())
					{
						inFile.getline(tData, 512);
						inFile.getline(tAdminToken, 16);
					}
					else
					{
						tLoadSuccess = false;
					}

					inFile.close();

					if(tRenameFile)
					{
						rename(getFilename(tFilename.c_str(), ".character", false).c_str(),
							getFilename(tFilename.c_str(), ".character").c_str());
					}

					//Load item, HP, skills and pet
					bool tRenameItemFile = false;
					inFile.open(getFilename(tFilename.c_str(), ".item"));

					if(!inFile.good())
					{
						inFile.clear();
						inFile.open(getFilename(tFilename.c_str(), ".item", false));

						if(inFile.good())
						{
							tRenameItemFile = true;
						}
					}

					if(inFile.good())
					{
						for(int j = 0; j < MAX_EQUIP; j++)
						{
							if(inFile.good())
                            {
                                inFile.getline(tItem[j], 16);
                            }
						}

						if(inFile.good())
						{
							inFile.getline(tHP, 16);
						}

						if(inFile.good())
						{
							inFile.getline(tSkills, 512);
						}

						if(inFile.good())
						{
							inFile.getline(tPet, 32);
						}
					}

					inFile.close();

					if(tRenameItemFile)
					{
						rename(getFilename(tFilename.c_str(), ".item", false).c_str(),
							getFilename(tFilename.c_str(), ".item").c_str());
					}

					//Write player data
					StringCompressor::Instance()->EncodeString(tData, 512, &tBitStream);
					const bool tIsAdmin = (strlen(tAdminToken) >= 3 && string(tAdminToken).erase(2) == "ok");
					const bool tIsMod = (strlen(tAdminToken) >= 3 && string(tAdminToken) == "mod");
					tBitStream.Write(tIsAdmin);
					tBitStream.Write(tIsMod);

					if(tIsAdmin)
					{
						StringCompressor::Instance()->EncodeString(string(tAdminToken).erase(0, 3).c_str(), 16,
							&tBitStream);
					}

					//Write items
					for(int j = 0; j < MAX_EQUIP; j++)
					{
						const bool tHasItem = (strlen(tItem[j])>0);
						tBitStream.Write(tHasItem);

						if(tHasItem)
                        {
                            StringCompressor::Instance()->EncodeString(tItem[j], 16, &tBitStream);
                        }
					}

					//Write HP
					const bool tHasHP = (strlen(tHP) > 0);
					tBitStream.Write(tHasHP);

					if(tHasHP)
                    {
                        StringCompressor::Instance()->EncodeString(tHP, 16, &tBitStream);
                    }

					//Write skills
					const bool tHasSkills = (strlen(tSkills) > 0);
					tBitStream.Write(tHasSkills);

					if(tHasSkills)
                    {
                        StringCompressor::Instance()->EncodeString(tSkills, 512, &tBitStream);
                    }

					//Write Pet
					const bool tHasPet = (strlen(tPet) > 0);
					tBitStream.Write(tHasPet);

					if(tHasPet)
                    {
                        StringCompressor::Instance()->EncodeString(tPet, 32, &tBitStream);
                    }
				}

				server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE_SEQUENCED, 1, p->systemAddress, false);

				if(showTraffic)
				{
					if(tLoadSuccess)
					{
						cout << tUsername << " load char success.\n";
					}
					else
					{
						cout << tUsername << " load char failed.\n";
					}
				}

				break;
			}

			case ID_CREATECHAR:
			{
				cout << "ID_CREATECHAR from " << p->systemAddress.ToString() << endl;

				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				char tUsername[16] = "";
				char tName[32] = "";
				char tData[512] = "";
				tReceiveBit.Read(tMessage);
				StringCompressor::Instance()->DecodeString(tUsername, 16, &tReceiveBit);
				StringCompressor::Instance()->DecodeString(tName, 32, &tReceiveBit);
				StringCompressor::Instance()->DecodeString(tData, 512, &tReceiveBit);

				bool tCreateSuccess = false;

				bool tNameUsed = true;

				while(tNameUsed)
				{
					tNameUsed = false;
					bool tRenameFile = false;
					ifstream inFile(getFilename(tUsername, ".charlist"));

					if(!inFile.good())
					{
						inFile.clear();
						inFile.open(getFilename(tUsername, ".charlist", false));

						if(inFile.good())
						{
							tRenameFile = true;
						}
					}

					if(inFile.good())
					{
						while(!inFile.eof() && inFile.good())
						{
							char tBuffer[32] = "";
							inFile.getline(tBuffer, 32);

							if(strcmp(tName, tBuffer) == 0)
							{
								tNameUsed = true;
								strcat(tName, "2");
								break;
							}
						}
					}

					inFile.close();

					if(tRenameFile)
					{
						rename(getFilename(tUsername, ".charlist", false).c_str(),
							getFilename(tUsername, ".charlist").c_str());
					}
				}

				//Write charlist data
				ofstream outFile(getFilename(tUsername, ".charlist"), ios::app);
				string tBuffer = tName;
				tBuffer += "\n";
				outFile.write(tBuffer.c_str(), tBuffer.length());
				outFile.close();

				string tFilename = tUsername;
				tFilename += "_";
				tFilename += tName;

				//Write char data
				outFile.open(getFilename(tFilename.c_str(), ".character"));
				tBuffer = tData;
				tBuffer += "\n";
				tBuffer += "false\n";
				outFile.write(tBuffer.c_str(), tBuffer.length());
				outFile.close();

				//Write item file
				outFile.open(getFilename(tFilename.c_str(), ".item"));

				for(int i = 0; i < MAX_EQUIP; i++)
                {
                    outFile << endl;
                }

				outFile << "500;500" << endl;

				for(int i = 0; i < 2; i++)
                {
                    outFile << endl;
                }

				outFile.close();

				//Confirm file creation success
				ifstream inFile(getFilename(tFilename.c_str(), ".character"));
				tCreateSuccess = inFile.good();
				inFile.close();

				RakNet::BitStream tBitStream;

				tBitStream.Write(MessageID(ID_CREATECHAR));
				tBitStream.Write(tCreateSuccess);

				server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 1, p->systemAddress, false);

				if(showTraffic)
				{
					if(tCreateSuccess)
					{
						cout << tUsername << " create char success.\n";
					}
					else
					{
						cout << tUsername << " create char failed.\n";
					}
				}

				break;
			}

			case ID_DELETECHAR:
			{
				cout << "ID_DELETECHAR from " << p->systemAddress.ToString() << endl;

				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				char tUsername[16] = "";
				short tIndex = 0;
				tReceiveBit.Read(tMessage);
				StringCompressor::Instance()->DecodeString(tUsername, 16, &tReceiveBit);
				tReceiveBit.Read(tIndex);

				string tName = "";
				string tBuffer = "";
				bool tSuccess = false;
				ifstream inFile(getFilename(tUsername, ".charlist"));

				if(inFile.good())
				{
					short tCount = 0;

					while(!inFile.eof() && inFile.good())
					{
						char tLine[32] = "";
						inFile.getline(tLine, 32);

						if(tIndex == tCount)
						{
							tName = tLine;
							tSuccess = true;
						}
						else if(strlen(tLine) > 0)
						{
							tBuffer += tLine;
							tBuffer += "\n";
						}

						tCount++;
					}
				}

				inFile.close();

				if(tSuccess)
				{
					//Edit charlist
					ofstream outFile(getFilename(tUsername, ".charlist"));
					outFile.write(tBuffer.c_str(), tBuffer.length());
					outFile.close();

					string tFilename = tUsername;
					tFilename += "_";
					tFilename += tName;

					//Delete character file
					unlink(getFilename(tFilename.c_str(), ".character").c_str());

					//Delete item file
					unlink(getFilename(tFilename.c_str(), ".item").c_str());
				}

				if(showTraffic)
				{
					if(tSuccess)
					{
						cout << tUsername << " delete char success.\n";
					}
					else
					{
						cout << tUsername << " delete char failed.\n";
					}
				}

				break;
			}

			case ID_EDITCHAR:
			{
				cout << "ID_EDITCHAR from " << p->systemAddress.ToString() << endl;

				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				char tUsername[16] = "";
				short tIndex = 0;
				char tData[512] = "";
				tReceiveBit.Read(tMessage);
				StringCompressor::Instance()->DecodeString(tUsername, 16, &tReceiveBit);
				tReceiveBit.Read(tIndex);
				StringCompressor::Instance()->DecodeString(tData, 512, &tReceiveBit);

				string tName = "";
				bool tSuccess = false;
				ifstream inFile(getFilename(tUsername, ".charlist"));

				if(inFile.good())
				{
					short tCount = 0;

					while(!inFile.eof() && inFile.good())
					{
						char tLine[32] = "";
						inFile.getline(tLine, 32);

						if(tIndex == tCount)
						{
							tName = tLine;
							tSuccess = true;
							break;
						}

						tCount++;
					}
				}

				inFile.close();

				if(tSuccess)
				{
					string tFilename = tUsername;
					tFilename += "_";
					tFilename += tName;

					string tBuffer = tData;
					tBuffer += "\n";

					//Read old character file
					ifstream inFile(getFilename(tFilename.c_str(), ".character"));

					if(inFile.good())
					{
						tSuccess = true;
						char tUnusedData[512];
						inFile.getline(tUnusedData, 512);	//read char data
						char tLine[16] = "";
						inFile.getline(tLine, 16);	//read admin data
						tBuffer += tLine;
						tBuffer += "\n";
					}

					inFile.close();

					//Write new character file
					if(tSuccess)
					{
						ofstream outFile(getFilename(tFilename.c_str(), ".character"));
						outFile.write(tBuffer.c_str(), tBuffer.length());
						outFile.close();

						//Confirm edit success
						inFile.open(getFilename(tFilename.c_str(), ".character").c_str());
						tSuccess = inFile.good();
						inFile.close();
					}
				}

				RakNet::BitStream tBitStream;

				tBitStream.Write(MessageID(ID_EDITCHAR));
				tBitStream.Write(tSuccess);

				server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 1, p->systemAddress, false);

				if(showTraffic)
				{
					if(tSuccess)
					{
						cout << tUsername << " edit char success.\n";
					}
					else
					{
						cout << tUsername << " edit char failed.\n";
					}
				}

				break;
			}

			/*case ID_PLAYERDATA:
				{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				bool tEnter;
				char tUsername[16] = "";
				tReceiveBit.Read(tMessage);
				tReceiveBit.Read(tEnter);
				StringCompressor::Instance()->DecodeString(tUsername,16,&tReceiveBit);

				short tServerIndex = -1;
				for(int i=0; i<MAX_SERVERS; i++)
				if(serverAdd[i]==p->systemAddress)
				{
				tServerIndex = i;
				break;
				}
				if(tServerIndex==-1)break;

				if(tEnter)
				{
				bool tIsUsed = false;
				const string tUsernameStr = toLowerCase(string(tUsername));
				for(int i=0; i<MAX_SERVERS; i++)
				{
				for(vector<string>::iterator it=loggedOnUsernames[i].begin(); it!=loggedOnUsernames[i].end(); it++)
				{
				if(*it==tUsernameStr)
				{
				tIsUsed = true;
				break;
				}
				}
				if(tIsUsed)break;
				}
				if(!tIsUsed)
				{
				loggedOnUsernames[tServerIndex].push_back(toLowerCase(string(tUsername)));
				if(showTraffic)cout << tUsername << " entered game.\n" << endl;
				}
				}
				else
				{
				const string tUsernameStr = toLowerCase(string(tUsername));
				bool tFound = false;
				for(int i=0; i<MAX_SERVERS; i++)
				{
				for(vector<string>::iterator it=loggedOnUsernames[i].begin(); it!=loggedOnUsernames[i].end(); it++)
				{
				if(*it==tUsernameStr)
				{
				loggedOnUsernames[i].erase(it);
				tFound = true;
				break;
				}
				}
				if(tFound)break;
				}
				if(showTraffic)cout << tUsername << " left game.\n";
				}
				}
				break;*/

			case ID_IMASERVER:
				sendServerList(p->systemAddress, false);
				registerServer(p);
				break;

				/*case ID_SAVEITEM:
					{
					RakNet::BitStream tReceiveBit(p->data, p->length, false);
					MessageID tMessage;
					char tUsername[16] = "";
					unsigned short tIndex = 0;
					char tItem[MAX_EQUIP][16];
					char tHP[16] = "";
					char tSkills[512] = "";
					char tPet[32] = "";

					tReceiveBit.Read(tMessage);
					StringCompressor::Instance()->DecodeString(tUsername,16,&tReceiveBit);
					tReceiveBit.Read(tIndex);
					for(int j=0;j<MAX_EQUIP;j++)StringCompressor::Instance()->DecodeString(tItem[j],16,&tReceiveBit);
					StringCompressor::Instance()->DecodeString(tHP,16,&tReceiveBit);
					StringCompressor::Instance()->DecodeString(tSkills,512,&tReceiveBit);
					StringCompressor::Instance()->DecodeString(tPet,32,&tReceiveBit);

					string tName = "";
					bool tSuccess = false;
					ifstream inFile(getFilename(tUsername,".charlist").c_str());
					if(inFile.good())
					{
					unsigned short tCount = 0;
					while(!inFile.eof() && inFile.good())
					{
					char tLine[32] = "";
					inFile.getline(tLine,32);
					if(tIndex==tCount)
					{
					tName = tLine;
					tSuccess = true;
					break;
					}
					tCount++;
					}
					}
					inFile.close();

					if(tSuccess)
					{
					//Write new item file
					string tFilename = tUsername;
					tFilename += "_";
					tFilename += tName;

					//Items
					string tBuffer = "";
					for(int j=0;j<MAX_EQUIP;j++)
					{
					tBuffer += tItem[j];
					tBuffer += "\n";
					}
					//HP
					tBuffer += tHP;
					tBuffer += "\n";
					//Skills
					tBuffer += tSkills;
					tBuffer += "\n";
					//Pet
					tBuffer += tPet;
					tBuffer += "\n";

					ofstream outFile(getFilename(tFilename.c_str(),".item").c_str());
					outFile.write(tBuffer.c_str(),tBuffer.length());
					outFile.close();
					}

					if(showTraffic)
					if(tSuccess)cout << tUsername << "[" << tIndex << "] save item success.\n" << endl;
					else cout << tUsername << "[" << tIndex << "] save item failed.\n" << endl;
					}
					break;*/

			case ID_GODSPEAK:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				server->Send(&tReceiveBit, MEDIUM_PRIORITY, RELIABLE, 4, p->systemAddress, true);
				break;
			}

			case ID_ITEMSTASH:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				OwnerToken tToken;
				bool tIsRequest;

				tReceiveBit.Read(tMessage);
				tReceiveBit.Read(tToken);
				tReceiveBit.Read(tIsRequest);

				if(tToken > MAX_CLIENTS || tToken <= 0 ||
                    playerToken[tToken - 1].add == UNASSIGNED_SYSTEM_ADDRESS)
				{
					break;
				}

				const char *tUsername = playerToken[tToken - 1].name.c_str();

				//Retrieve and send stash data
				if(tIsRequest)
				{
					RakNet::BitStream tBitStream;

					tBitStream.Write(MessageID(ID_ITEMSTASH));
					tBitStream.Write(tToken);

					ifstream inFile(getFilename(tUsername, ".stash"));

					if(inFile.good())
					{
						while(!inFile.eof() && inFile.good())
						{
							char tLine[16] = "";
							inFile.getline(tLine, 16);

							if(strlen(tLine) > 0)
							{
								tBitStream.Write(true);
								StringCompressor::Instance()->EncodeString(tLine, 16, &tBitStream);
							}
						}
					}

					inFile.close();
					server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 6, p->systemAddress, false);
				}
				//Update stash data
				else
				{
					bool tIsAdd = true;
					tReceiveBit.Read(tIsAdd);

					//Add item
					if(tIsAdd)
					{
						string tItem = "";
						unsigned short tSlot;
						tReceiveBit.Read(tSlot);

						//Unequip item from player
						if(tSlot >= 0 && tSlot < MAX_EQUIP)
						{
							tItem = playerToken[tToken - 1].item[tSlot];
							playerToken[tToken - 1].item[tSlot] = "";
						}

						//Save item stash
						if(tItem.length() > 0)
						{
							ofstream outFile(getFilename(tUsername, ".stash"), ios::app);
							const string tBuffer = tItem + "\n";
							outFile.write(tBuffer.c_str(), tBuffer.length());
							outFile.close();
						}
					}
					//Remove item
					else
					{
						unsigned short tLineID;
						tReceiveBit.Read(tLineID);
						string tBuffer = "";
						unsigned short tCount = 0;
						string tItem = "";

						//Extract item from stash
						ifstream inFile(getFilename(tUsername, ".stash"));

						if(inFile.good())
						{
							while(!inFile.eof() && inFile.good())
							{
								char tLine[16] = "";
								inFile.getline(tLine, 16);

								if(strlen(tLine) > 0)
								{
									if(tCount != tLineID)
									{
										tBuffer += tLine;
										tBuffer += "\n";
									}
									else
									{
										tItem = tLine;
									}
								}

								tCount++;
							}
						}

						inFile.close();
						ofstream outFile(getFilename(tUsername, ".stash"));

						if(tBuffer.length() > 0)
						{
							outFile.write(tBuffer.c_str(), tBuffer.length());
						}

						outFile.close();

						//Equip item on player
						unsigned short tSlot;
						tReceiveBit.Read(tSlot);

						if(tSlot >= 0 && tSlot < MAX_EQUIP)
						{
							playerToken[tToken - 1].item[tSlot] = tItem;
						}
					}
				}

				break;
			}

			case ID_ITEMEQUIP:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				OwnerToken tToken;

				tReceiveBit.Read(tMessage);
				tReceiveBit.Read(tToken);

				if(tToken > MAX_CLIENTS || tToken <= 0 ||
                    playerToken[tToken - 1].add == UNASSIGNED_SYSTEM_ADDRESS)
				{
					break;
				}

				char tItem[256];
				unsigned short tSlot;
				StringCompressor::Instance()->DecodeString(tItem, 16,
					&tReceiveBit);
				tReceiveBit.Read(tSlot);

				if(tSlot >= 0 && tSlot < MAX_EQUIP)
				{
					playerToken[tToken - 1].item[tSlot] = tItem;
				}

				break;
			}

			case ID_ITEMUNEQUIP:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				OwnerToken tToken;

				tReceiveBit.Read(tMessage);
				tReceiveBit.Read(tToken);

				if(tToken > MAX_CLIENTS || tToken <= 0 ||
					playerToken[tToken - 1].add == UNASSIGNED_SYSTEM_ADDRESS)
				{
					break;
				}

				unsigned short tSlot;
				tReceiveBit.Read(tSlot);

				if(tSlot >= 0 && tSlot < MAX_EQUIP)
				{
					playerToken[tToken - 1].item[tSlot] = "";
				}

				break;
			}

			case ID_UPDATEHP:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				OwnerToken tToken;

				tReceiveBit.Read(tMessage);
				tReceiveBit.Read(tToken);

				if(tToken > MAX_CLIENTS || tToken <= 0 ||
					playerToken[tToken - 1].add == UNASSIGNED_SYSTEM_ADDRESS)
				{
					break;
				}

				int tHP = 500;
				bool tHasMaxHP = false;
				tReceiveBit.Read(tHP);
				playerToken[tToken - 1].hp.first = (unsigned short)tHP;
				tReceiveBit.Read(tHasMaxHP);

				if(tHasMaxHP)
				{
					int tMaxHP = 500;
					tReceiveBit.Read(tMaxHP);
					playerToken[tToken - 1].hp.second = (unsigned short)tMaxHP;
				}

				break;
			}

			case ID_UPDATESKILLS:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				OwnerToken tToken;

				tReceiveBit.Read(tMessage);
				tReceiveBit.Read(tToken);

				if(tToken > MAX_CLIENTS || tToken <= 0 ||
					playerToken[tToken - 1].add == UNASSIGNED_SYSTEM_ADDRESS)
				{
					break;
				}

				char tSkill[32] = "";
				unsigned char tStock = 0;
				StringCompressor::Instance()->DecodeString(tSkill, 32, &tReceiveBit);
				tReceiveBit.Read(tStock);

				if(strlen(tSkill) > 0)
				{
					bool tFound = false;
					for(vector<pair<string, unsigned char> >::iterator it = playerToken[tToken - 1].skill.begin();
						it != playerToken[tToken - 1].skill.end(); it++)
					{
						pair<string, unsigned char> *tPair = &*it;

						if(tPair->first == string(tSkill))
						{
							tFound = true;

							if(tStock > 0)
							{
								tPair->second = tStock;
							}
							else
							{
								playerToken[tToken - 1].skill.erase(it);
							}

							break;
						}
					}

					if(!tFound && tStock > 0)
					{
						playerToken[tToken - 1].skill.push_back(
							pair<string, unsigned char>(tSkill, tStock));
					}
				}

				break;
			}

			case ID_UPDATEPET:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				OwnerToken tToken;

				tReceiveBit.Read(tMessage);
				tReceiveBit.Read(tToken);

				if(tToken > MAX_CLIENTS || tToken <= 0 ||
					playerToken[tToken - 1].add == UNASSIGNED_SYSTEM_ADDRESS)
				{
					break;
				}

				char tPet[32] = "";
				StringCompressor::Instance()->DecodeString(tPet, 32,
					&tReceiveBit);
				playerToken[tToken - 1].pet = tPet;

				break;
			}

			case ID_VIOLATION:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				SystemAddress tAdd;
				char tInfo[32] = "";
				ostringstream sin;

				tReceiveBit.Read(tMessage);
				tReceiveBit.Read(tAdd);
				StringCompressor::Instance()->DecodeString(tInfo, 32, &tReceiveBit);

				const OwnerToken tToken = getTokenByAdd(tAdd);

				ofstream outFile("./violations.log", ios::app);

				if(outFile.good())
				{
					outFile << tInfo << " -> " << tAdd.ToString();

					if(tToken > 0 && tToken <= MAX_CLIENTS)
					{
						outFile << " -> " << playerToken[tToken - 1].name <<
							" [" << playerToken[tToken - 1].charID << "]";
					}

					outFile << " -> " << currentDateTime() << endl;
				}

				outFile.close();

				break;
			}

			case ID_KICK:
			{
				RakNet::BitStream tReceiveBit(p->data, p->length, false);
				MessageID tMessage;
				char tName[16];
				bool tIsBanned = false;
				unsigned short tNumDays = 0;

				tReceiveBit.Read(tMessage);
				StringCompressor::Instance()->DecodeString(tName, 16, &tReceiveBit);
				tReceiveBit.Read(tIsBanned);

				if(tIsBanned)
				{
					tReceiveBit.Read(tNumDays);

					time_t rawtime;
					struct tm * timeinfo;
					time(&rawtime);
					timeinfo = localtime(&rawtime);
					int tYear = timeinfo->tm_year;
					int tDay = timeinfo->tm_yday + tNumDays;

					while(tDay > 365)
					{
						tYear += 1;
						tDay -= 365;
					}

					BanInfo tInfo(tName, tYear, tDay);
					const OwnerToken tToken = getTokenByName(tName);

					if(tToken > 0)
					{
						tInfo.IPList.push_back(playerToken[tToken - 1].add.ToString(false));
						banlist.push_back(tInfo);
						server->AddToBanList(playerToken[tToken - 1].add.ToString(false));
						updateBanlist();
						saveBanlist();
					}
				}

				break;
			}

			default:
				break;
				/*cout << (int)packetIdentifier << ": " << endl;
				cout << (char*)p->data) << endl;
				*/
			}

			server->DeallocatePacket(p);
			p = server->Receive();
		}
	}
}


const string ServerManager::getFilename(const char *name,
	const char *fileExtension, bool replaceSpaces, bool replaceUnderscores)
{
	//Get first character to determine folder
	string firstChar = "0";
	string cname;

	if(strlen(name) > 0 && name[0] >= 'A' && name[0] <= 'z')
	{
		firstChar[0] = name[0];
		if(name[0] >= 'a')
			firstChar[0] = name[0] - 'a' + 'A';
	}

	string filename = "data/" + firstChar + "/";
	cname = toLowerCase(name);
	filename += cname;

	//Convert all spaces to underscores
	if(replaceSpaces)
	{
		for(int i = 0; i < (int)filename.length(); i++)
		{
			if(filename[i] == ' ')
			{
				filename[i] = '_';
			}
		}
	}
	else if(replaceUnderscores)
	{
		for(int i = 0; i < (int)filename.length(); i++)
		{
			if(filename[i] == '_')
			{
				filename[i] = ' ';
			}
		}
	}

	filename += fileExtension;
	return filename;
}


void ServerManager::shutdown()
{
	if(server)
	{
		server->Shutdown(300);
	}

	savePingRange();
	saveBanlist();
}


void ServerManager::quit()
{
	alive = false;
}


const string ServerManager::toLowerCase(string text)
{
	const char tDiff = char('A') - char('a');

	for(int i = 0; i < (int)text.length(); i++)
	{
		if(text[i] >= char('A') && text[i] <= char('Z'))
		{
			text[i] -= tDiff;
		}
	}

	return text;
}


const unsigned char ServerManager::registerServer(Packet *p)
{
	serversListMutex.Lock();

	for(int i = 0; i < MAX_SERVERS; i++)
	{
		if(serverTunnelAdd[i] == UNASSIGNED_SYSTEM_ADDRESS)
		{
			RakNet::BitStream tReceiveBit(p->data, p->length, false);
			MessageID tMessage;
			char buffer[64] = "";
			tReceiveBit.Read(tMessage);
			StringCompressor::Instance()->DecodeString(buffer, 64, &tReceiveBit);

			SystemAddress broadcastAdd("", p->systemAddress.GetPort());
			broadcastAdd.SetBinaryAddress(buffer);
			serverAdd[i] = broadcastAdd;

			serverTunnelAdd[i] = p->systemAddress;
			serverFull[i] = false;
			numServers++;
			cout << "Server " << i + 1 << " connected, Tunnel: " << p->systemAddress.ToString() <<
			    ", IP: " << broadcastAdd.ToString() << endl;
			broadcastServerConnected(i, p->systemAddress);
			notifyServerID(i, p->systemAddress);
			serversListMutex.Unlock();
			return i;
		}
	}

	serversListMutex.Unlock();
	return 0;
}


const unsigned char ServerManager::getServerID(const SystemAddress &add)
{
	serversListMutex.Lock();

	for(int i = 0; i < MAX_SERVERS; i++)
	{
		if(serverTunnelAdd[i] == add)
		{
			serversListMutex.Unlock();
			return i;
		}
	}

	serversListMutex.Unlock();
	return MAX_SERVERS;
}


bool ServerManager::unregisterServer(Packet *p)
{
	serversListMutex.Lock();

	for(int i = 0; i < MAX_SERVERS; i++)
	{
		if(serverTunnelAdd[i] == p->systemAddress)
		{
			//loggedOnUsernames[i].clear();
			unassignTokens(p->systemAddress);
			cout << "Server " << i + 1 << " disconnected, Tunnel: " << p->systemAddress.ToString() <<
			    ", IP: " << serverAdd[i].ToString() << endl;
			broadcastServerDisconnected(i, p->systemAddress);
			serverAdd[i] = UNASSIGNED_SYSTEM_ADDRESS;
			serverTunnelAdd[i] = UNASSIGNED_SYSTEM_ADDRESS;
			serverFull[i] = false;
			numServers--;
			serversListMutex.Unlock();
			return true;
		}
	}

	serversListMutex.Unlock();
	return false;
}


void ServerManager::sendServerList(const SystemAddress &target,
	const bool &isClient)
{
	RakNet::BitStream tBitStream;

	tBitStream.Write(MessageID(ID_SERVERLIST));
	serversListMutex.Lock();

	for(int i = 0; i < MAX_SERVERS; i++)
	{
		if(serverTunnelAdd[i] != UNASSIGNED_SYSTEM_ADDRESS)
		{
			tBitStream.Write(true);
			tBitStream.Write((unsigned char)i);

			if(isClient)
			{
				tBitStream.Write(SystemAddress(serverAdd[i]));
			}
			else
			{
				tBitStream.Write(SystemAddress(serverTunnelAdd[i]));
			}

			tBitStream.Write(serverFull[i]);
		}
	}

	serversListMutex.Unlock();
	server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, target, false);
}


void ServerManager::notifyServerID(const unsigned char &iID,
	const SystemAddress &add)
{
	RakNet::BitStream tBitStream;

	tBitStream.Write(MessageID(ID_SERVERCONNECTED));
	tBitStream.Write(true);
	tBitStream.Write(iID);
	tBitStream.Write(SystemAddress(serverAdd[iID]));
	tBitStream.Write(SystemAddress(serverTunnelAdd[iID]));
	tBitStream.Write(true);

	server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, add, false);
}


void ServerManager::broadcastServerConnected(const unsigned char &iID,
	const SystemAddress &exceptionAdd)
{
	RakNet::BitStream tBitStream;

	tBitStream.Write(MessageID(ID_SERVERCONNECTED));
	tBitStream.Write(true);
	tBitStream.Write(iID);
	tBitStream.Write(SystemAddress(serverAdd[iID]));
	tBitStream.Write(SystemAddress(serverTunnelAdd[iID]));

	server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, exceptionAdd, true);
}


void ServerManager::broadcastServerDisconnected(const unsigned char &iID,
	const SystemAddress &exceptionAdd)
{
	RakNet::BitStream tBitStream;

	tBitStream.Write(MessageID(ID_SERVERCONNECTED));
	tBitStream.Write(false);
	tBitStream.Write(SystemAddress(serverAdd[iID]));
	tBitStream.Write(SystemAddress(serverTunnelAdd[iID]));

	server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, exceptionAdd, true);
}


void ServerManager::broadcastMaintenance(const unsigned char &time)
{
	RakNet::BitStream tBitStream;

	tBitStream.Write(MessageID(ID_MAINTENANCE));
	tBitStream.Write(time);

	server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
}


const OwnerToken ServerManager::assignToken(const SystemAddress &add, const unsigned char &serverID,
    const string &name, const unsigned short &charID)
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(playerToken[i].add == UNASSIGNED_SYSTEM_ADDRESS)
		{
			playerToken[i].add = add;
			playerToken[i].serverID = serverID;
			playerToken[i].name = name;
			playerToken[i].charID = charID;
			loadPlayerData(i + 1);
			numClients++;
			return (i + 1);
		}
	}

	return 0;
}


void ServerManager::unassignToken(const OwnerToken &token)
{
	if(token > 0 && token <= MAX_CLIENTS)
	{
		savePlayerData(token);
		playerToken[token - 1] = PlayerToken();
		numClients--;
	}
}


void ServerManager::unassignToken(const SystemAddress &add)
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(playerToken[i].add == add)
		{
			unassignToken(i + 1);
			return;
		}
	}
}


const OwnerToken ServerManager::getTokenByAdd(const SystemAddress &add)
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(playerToken[i].add == add)
		{
			return (i + 1);
		}
	}

	return 0;
}


const OwnerToken ServerManager::getTokenByName(const string &name)
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(toLowerCase(playerToken[i].name) == toLowerCase(name))
		{
			return (i + 1);
		}
	}

	return 0;
}


void ServerManager::unassignTokens(const SystemAddress &serverAdd)
{
	const unsigned char tServerID = getServerID(serverAdd);

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(playerToken[i].add != UNASSIGNED_SYSTEM_ADDRESS &&
			playerToken[i].serverID == tServerID)
		{
			unassignToken(i + 1);
		}
	}
}


void ServerManager::broadcastServerUpdate()
{
	serversListMutex.Lock();

	for(int i = 0; i < MAX_SERVERS; i++)
	{
		if(serverTunnelAdd[i] != UNASSIGNED_SYSTEM_ADDRESS)
		{
			RakNet::BitStream tBitStream;

			tBitStream.Write(MessageID(ID_SERVERUPDATE));

			server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, serverTunnelAdd[i], false);
		}
	}

	serversListMutex.Unlock();
}


void ServerManager::loadPlayerData(const OwnerToken &token)
{
	if(token <= 0 || token > MAX_CLIENTS)
	{
		return;
	}

	string tName = "";
	bool tSuccess = false;
	ifstream inFile(getFilename(playerToken[token - 1].name.c_str(), ".charlist"));

	if(inFile.good())
	{
		unsigned short tCount = 0;

		while(!inFile.eof() && inFile.good())
		{
			char tLine[32] = "";
			inFile.getline(tLine, 32);

			if(playerToken[token - 1].charID == tCount)
			{
				tName = tLine;
				tSuccess = true;
				break;
			}

			tCount++;
		}
	}

	inFile.close();

	if(!tSuccess)
	{
		return;
	}

	string tFilename = playerToken[token - 1].name;
	tFilename += "_";
	tFilename += tName;

	//Load everything
	char tItem[MAX_EQUIP][16];

	for(int j = 0; j < MAX_EQUIP; j++)
	{
		strcpy(tItem[j], "");
	}

	char tHP[16] = "";
	char tSkills[512] = "";
	char tPet[32] = "";
	inFile.open(getFilename(tFilename.c_str(), ".item"));

	if(inFile.good())
	{
		for(int j = 0; j < MAX_EQUIP; j++)
		{
			if(inFile.good())
			{
				inFile.getline(tItem[j], 16);
			}
		}

		if(inFile.good())
		{
			inFile.getline(tHP, 16);
		}

		if(inFile.good())
		{
			inFile.getline(tSkills, 512);
		}

		if(inFile.good())
		{
			inFile.getline(tPet, 32);
		}
	}

	inFile.close();

	for(int j = 0; j < MAX_EQUIP; j++)
	{
		playerToken[token - 1].item[j] = tItem[j];
	}

	const vector<string> tHPPart = tokenize(string(tHP), ";\n");

	if(tHPPart.size() == 2)
	{
	    playerToken[token - 1].hp.first = atoi(tHPPart[0].c_str());
		playerToken[token - 1].hp.second = atoi(tHPPart[1].c_str());
	}
	else
	{
		playerToken[token - 1].hp.first = 500;
		playerToken[token - 1].hp.second = 500;
	}

	const vector<string> tSkillLine = tokenize(string(tSkills), "|\n");

	for(int i = 0; i < (int)tSkillLine.size(); i++)
	{
		const vector<string> tSkillPart = tokenize(tSkillLine[i], ";");

		if(tSkillPart.size() == 2)
		{
			playerToken[token - 1].skill.push_back(
				pair<string, unsigned char>(tSkillPart[0], atoi(tSkillPart[1].c_str())));
		}
	}

	playerToken[token - 1].pet = tPet;
}


void ServerManager::savePlayerData(const OwnerToken &token)
{
    if(token <= 0 || token > MAX_CLIENTS)
	{
		return;
	}

	string tName = "";
	bool tSuccess = false;
	ostringstream sin;
	ifstream inFile(getFilename(playerToken[token - 1].name.c_str(), ".charlist"));

	if(inFile.good())
	{
		unsigned short tCount = 0;

		while(!inFile.eof() && inFile.good())
		{
			char tLine[32] = "";
			inFile.getline(tLine, 32);

			if(playerToken[token - 1].charID == tCount)
			{
				tName = tLine;
				tSuccess = true;
				break;
			}

			tCount++;
		}
	}

	inFile.close();

	if(!tSuccess)
	{
		return;
	}

	//Write new item file
	string tFilename = playerToken[token - 1].name;
	tFilename += "_";
	tFilename += tName;

	//Items
	string tBuffer = "";

	for(int j = 0; j < MAX_EQUIP; j++)
	{
		tBuffer += playerToken[token - 1].item[j];
		tBuffer += "\n";
	}

	//HP
	stringstream ssHP;
	stringstream ssMaxHP;

	ssHP << (int)playerToken[token - 1].hp.first;
    ssMaxHP << (int)(playerToken[token - 1].hp.second == 0 ? 500 : playerToken[token - 1].hp.second);

	string tHPStr = ssHP.str();
	tHPStr += ";";
	tHPStr += ssMaxHP.str();

	tBuffer += tHPStr;
	tBuffer += "\n";

	//Skills
	string tSkill = "";

	for(int j = 0; j < (int)playerToken[token - 1].skill.size(); j++)
	{
		tSkill += playerToken[token - 1].skill[j].first;
		tSkill += ";";

		stringstream ssStock;
		ssStock << (int)playerToken[token - 1].skill[j].second;

		tSkill += ssStock.str();
		tSkill += "|";
	}

	if(tSkill.length() >= 512)
	{
		cout << "(" << (int)token << ")" << playerToken[token - 1].name.c_str() <<
		    " skill length longer than 512\n";
		ofstream outFile("./exceptions.log", ios::app);
		char tBuffer[128] = "";
		sin << (int)token;
		string val = sin.str();
		strcpy(tBuffer, val.c_str());
		//itoa((int)token,tBuffer,10);
		outFile.write(tBuffer, strlen(tBuffer));
		strcpy(tBuffer, playerToken[token - 1].name.c_str());
		outFile.write(tBuffer, strlen(tBuffer));
		strcpy(tBuffer, " skill length longer than 512\n");
		outFile.write(tBuffer, strlen(tBuffer));
		outFile.close();
	}

	tBuffer += tSkill;
	tBuffer += "\n";

	//Pet
	tBuffer += playerToken[token - 1].pet;
	tBuffer += "\n";

	ofstream outFile(getFilename(tFilename.c_str(), ".item"));
	outFile << tBuffer;
	outFile.close();
}


const vector<string> ServerManager::tokenize(const string& str,
	const string& delimiters)
{
	vector<string> tokens;

	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);

	// Find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while(string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));

		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);

		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}


void ServerManager::updateTimer()
{
	time_t currTime;
	time(&currTime);
	const float timeSinceLastUpdate = (float)difftime(currTime, prevTime);
	prevTime = currTime;
	dayTime += timeSinceLastUpdate;

	if(dayTime >= 2400)
	{
		dayTime = 0;
	}

	weatherTime += timeSinceLastUpdate;

	if(weatherTime >= 7000)
	{
		weatherTime = 0;
	}

	//4 hourly maintenance
	const unsigned short MAINTENANCE_PERIOD = 14400;
	/*unsigned char tPrepareMark=0;
	if(maintenanceTime<MAINTENANCE_PERIOD-60)tPrepareMark = 60;
	else if(maintenanceTime<MAINTENANCE_PERIOD-30)tPrepareMark = 30;
	else if(maintenanceTime<MAINTENANCE_PERIOD-15)tPrepareMark = 15;*/
	maintenanceTime += timeSinceLastUpdate;

	//Check if timer crossed the mark
	//if(tPrepareMark!=0 && maintenanceTime>=MAINTENANCE_PERIOD-tPrepareMark)broadcastMaintenance(tPrepareMark);
	if(maintenanceTime >= MAINTENANCE_PERIOD)
	{
		updateBanlist();
		saveBanlist();
		//broadcastServerUpdate();
		maintenanceTime = 0;
	}
}


void ServerManager::updateServers(bool forceUpdate)
{
	if(!forceUpdate && (int(dayTime) % 10 != 0))
	{
		return;
	}

	serversListMutex.Lock();

	for(int i = 0; i < MAX_SERVERS; i++)
	{
		if(serverTunnelAdd[i] != UNASSIGNED_SYSTEM_ADDRESS)
		{
			const int tPing = server->GetAveragePing(serverTunnelAdd[i]);

			if(tPing >= highPing)
			{
				serverFull[i] = true;
			}
			else if(tPing <= lowPing)
			{
				serverFull[i] = false;
			}
		}
	}

	serversListMutex.Unlock();
}


const unsigned short ServerManager::getNumClientsInServer(
	const unsigned char &serverID)
{
	unsigned short tCount = 0;

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(playerToken[i].serverID == serverID)
		{
			tCount++;
		}
	}

	return tCount;
}


const string ServerManager::XOR7(const string &input)
{
	string output = "";

	for(int i = 0; i < (int)input.length(); i++)
	{
		char tC = input[i];
		tC ^= 7 * (i % 7 + 1);

		//Replace illegal characters with original character
		if(tC > char(126) || tC<char(32))
		{
			tC = input[i];
		}

		output += tC;
	}

	return output;
}


const string ServerManager::XOR7OLD(const string &input)
{
	string output = "";

	for(int i = 0; i < (int)input.length(); i++)
	{
		char tC = input[i];
		tC ^= 7 * (i % 7 + 1);
		output += tC;
	}

	return output;
}


void ServerManager::saveBanlist()
{
	ostringstream sin;
	ofstream outFile("banlist.txt");
	string tBuffer;

	for(int i = 0; i < (int)banlist.size(); i++)
	{
		char tYear[6] = "";
		char tDay[6] = "";
		sin << banlist[i].year;
		string val = sin.str();
		strcpy(tYear, val.c_str());
		sin << banlist[i].yDay;
		val = sin.str();
		strcpy(tDay, val.c_str());
		//itoa(banlist[i].year,tYear,6,10);
		//itoa(banlist[i].yDay,tDay,6,10);
		tBuffer = banlist[i].name + ";" + tYear + ";" + tDay + "|";
		outFile.write(tBuffer.c_str(), tBuffer.length());

		for(int j = 0; j < (int)banlist[i].IPList.size(); j++)
		{
			tBuffer = banlist[i].IPList[j] + ";";
			outFile.write(tBuffer.c_str(), tBuffer.length());
		}

		tBuffer = "\n";
		outFile.write(tBuffer.c_str(), tBuffer.length());
	}

	outFile.close();
}


void ServerManager::loadBanlist()
{
	ifstream inFile("banlist.txt");

	while(inFile.good() && !inFile.eof())
	{
		char tBuffer[1024] = "";
		inFile.getline(tBuffer, 1024);
		const vector<string> tPart = tokenize(tBuffer, "|");

		if(tPart.size() > 1)
		{
			const vector<string> tInfo = tokenize(tPart[0], ";");
			if(tInfo.size() > 2)
			{
				BanInfo tBanInfo(tInfo[0], atoi(tInfo[1].c_str()),
					atoi(tInfo[2].c_str()));
				const vector<string> tIPs = tokenize(tPart[1], ";");

				for(int i = 0; i < (int)tIPs.size(); i++)
				{
					tBanInfo.IPList.push_back(tIPs[i]);
					server->AddToBanList(tIPs[i].c_str());
				}

				banlist.push_back(tBanInfo);
			}
		}
	}

	inFile.close();
}


void ServerManager::updateBanlist()
{
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	const int tCurrentYear = timeinfo->tm_year;
	const int tCurrentDay = timeinfo->tm_yday;

	vector<BanInfo>::iterator it = banlist.begin();

	while(it != banlist.end())
	{
		BanInfo tInfo = *it;
		const int tYear = tInfo.year - tCurrentYear;
		const int tDay = tInfo.yDay - tCurrentDay;

		if(tYear < 0 || (tYear == 0 && tDay <= 0))
		{
			for(int i = 0; i < (int)tInfo.IPList.size(); i++)
			{
				server->RemoveFromBanList(tInfo.IPList[i].c_str());
			}

			tInfo.IPList.clear();
			it = banlist.erase(it);
			continue;
		}

		it++;
	}
}


void ServerManager::clearBanlist()
{
	vector<BanInfo>::iterator it = banlist.begin();

	while(it != banlist.end())
	{
		BanInfo tInfo = *it;
		for(int i = 0; i < (int)tInfo.IPList.size(); i++)
			server->RemoveFromBanList(tInfo.IPList[i].c_str());
		tInfo.IPList.clear();
		it = banlist.erase(it);
	}
}


void ServerManager::endSession(Packet *p)
{
	unsigned int k = 0;

	for(vector<pair<string, SystemAddress> >::iterator it = loginSession.begin();
		it != loginSession.end(); it++)
	{
		pair<string, SystemAddress> tSession = *it;

		if(tSession.second == p->systemAddress)
		{
			serversListMutex.Lock();

			for(int i = 0; i < MAX_SERVERS; i++)
			{
				if(serverTunnelAdd[i] != UNASSIGNED_SYSTEM_ADDRESS)
				{
					RakNet::BitStream tBitStream;

					tBitStream.Write(MessageID(ID_FORCELOGOUT));
					tBitStream.Write(false);
					StringCompressor::Instance()->EncodeString( tSession.first.c_str(), 16, &tBitStream);

					server->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, serverTunnelAdd[i], false);
				}
			}

			serversListMutex.Unlock();
			loginSession.erase(it);
			break;
		}

		k++;
	}
}


void ServerManager::loadPingRange()
{
	lowPing = 400;
	highPing = 500;
	char tBuffer[16] = "";
	ifstream inFile("pingrange.txt");

	if(inFile.good() && !inFile.eof())
	{
		inFile.getline(tBuffer, 16);
		const unsigned short tPing = atoi(tBuffer);

		if(tPing != 0)
		{
			lowPing = tPing;
		}
	}

	if(inFile.good() && !inFile.eof())
	{
		strcpy(tBuffer, "");
		inFile.getline(tBuffer, 16);
		const unsigned short tPing = atoi(tBuffer);

		if(tPing != 0)
		{
			highPing = tPing;
		}
	}

	cout << "Servers accept connections at ping below " << lowPing << endl;
	cout << "Servers reject connections at ping above " << highPing << endl;
	inFile.close();
}


void ServerManager::savePingRange()
{
	ofstream outFile("pingrange.txt");
	ostringstream sin;
	char tLowPing[16] = "", tHighPing[16] = "";
	sin << lowPing;
	string val = sin.str();
	strcpy(tLowPing, val.c_str());
	//itoa(lowPing,tLowPing,16,10);
	sin.clear();
	sin.str("");
	sin << highPing;
	val = sin.str();
	strcpy(tHighPing, val.c_str());
	//itoa(highPing,tHighPing,16,10);
	string tBuffer = tLowPing;
	tBuffer += "\n";
	tBuffer += tHighPing;
	tBuffer += "\n";
	outFile.write(tBuffer.c_str(), tBuffer.length());
	outFile.close();
}


RakNetStatistics* ServerManager::getStatistics()
{
	return server->GetStatistics(server->GetSystemAddressFromIndex(0));
}


int ServerManager::getAveragePing()
{
	return server->GetAveragePing(server->GetSystemAddressFromIndex(0));
}


void ServerManager::addToBanList(const char *input)
{
	server->AddToBanList(input);
}


bool ServerManager::ShowTraffic()
{
	showTraffic = !showTraffic;
	return showTraffic;
}


unsigned short ServerManager::getNumClients()
{
	return numClients;
}


unsigned short ServerManager::getNumServers()
{
	return numServers;
}


void ServerManager::printServers()
{
	bool printed = false;
	serversListMutex.Lock();

	for(int i = 0; i < MAX_SERVERS; i++)
	{
		if(serverTunnelAdd[i] != UNASSIGNED_SYSTEM_ADDRESS)
		{
			printed = true;
			cout << "Server " << i + 1 << ": " << serverAdd[i].ToString() << "  ";
			cout << "Tunnel IP " << i + 1 << ": " << serverTunnelAdd[i].ToString() << "  ";
			cout << "Ping: " << server->GetAveragePing(serverTunnelAdd[i]) << " ";
			cout << "Clients: " << (int)getNumClientsInServer(i);

			if(serverFull[i])
			{
				cout << "(ping overload)\n";
			}
			else
			{
				cout << endl;
			}
		}
	}

	serversListMutex.Unlock();

	if(!printed)
	{
		cout << "Nothing to print\n";
	}
}


void ServerManager::printTimeWeather()
{
	cout << "Day time: " << (int)dayTime << "\nWeather time: " <<
		(int)weatherTime << endl;
}


void ServerManager::enterPingRange()
{
	char input[512] = "";
	cout << "Enter Ping range with space between (low high): ";
	fgets(input, 512, stdin);
	const vector<string> tPart = tokenize(string(input), " \n");


	if(tPart.size() > 0)
	{
		lowPing = atoi(tPart[0].c_str());
	}

	if(tPart.size() > 1)
	{
		highPing = atoi(tPart[1].c_str());
	}


	cout << "Servers accept connections at ping below " << (int)lowPing << endl;
	cout << "Servers reject connections at ping above " << (int)highPing << endl;
	updateServers(true);
}


void ServerManager::enterServerPing()
{
	char input[512] = "";
	cout << "Enter ServerID to Ping: ";
	fgets(input, 512, stdin);
	const string tInput = input;
	const int tID = atoi(tInput.c_str());
	serversListMutex.Lock();

	if(tID > 0 && tID <= MAX_SERVERS &&
		serverTunnelAdd[tID - 1] != UNASSIGNED_SYSTEM_ADDRESS)
	{
		cout << "Server " << tID << " Ping: " <<
			server->GetAveragePing(serverTunnelAdd[tID - 1]) << endl;
	}
	else
	{
		cout << "No such server: " << tID << endl;
	}

	serversListMutex.Unlock();
}


void ServerManager::reloadBanList()
{
	clearBanlist();
	loadBanlist();
}


//Entry Point
//================================================================================
int main(void)
{
	ServerManager mServerMgr;

    #ifdef __MINGW__
	thread loginServerThread(&ServerManager::startThread, &mServerMgr);

	bool wekilled = false;

	while(loginServerThread.joinable())
	{
		RakSleep(200);

        #ifdef _WIN32
		if(_kbhit())
		{
			char c = _getch();

			if(c == 'Q')
			{
				cout << "Quitting." << endl;
				mServerMgr.quit();
				wekilled = true;
				break;
			}
			else if(c == 'S')
			{
				char temp[2048] = "";
				RakNetStatistics* rss = mServerMgr.getStatistics();
				StatisticsToString(rss, temp, 2);
				cout << temp;
				cout << "Ping " << mServerMgr.getAveragePing() << endl;
			}
			else if(c == 'B')
			{
				cout << "Enter IP to ban.  You can use * as a wildcard\n";
				char input[512] = "";
				fgets(input, 512, stdin);
				mServerMgr.addToBanList(input);

				cout << "IP " << input << " added to ban list.\n";
			}
			else if(c == 'T')
			{
				bool res = mServerMgr.ShowTraffic();

				if(res)
				{
					cout << "ShowTraffic ON\n";
				}
				else
				{
					cout << "ShowTraffic OFF\n";
				}
			}
			else if(c == 'C')
			{
				cout << "Current number of connected clients: " <<
					mServerMgr.getNumClients() << endl;
			}
			else if(c == 'N')
			{
				cout << "Current number of servers: " <<
					mServerMgr.getNumServers() << endl;
			}
			else if(c == 'M')
			{
				mServerMgr.printServers();
			}
			else if(c == 'U')
			{
				mServerMgr.broadcastServerUpdate();
				cout << "Sent broadcast\n";
			}
			else if(c == 'Y')
			{
				mServerMgr.printTimeWeather();
			}
			else if(c == 'P')
			{
				mServerMgr.enterPingRange();
			}
			else if(c == 'O')
			{
				mServerMgr.enterServerPing();
			}
			else if(c == 'R')
			{
				mServerMgr.reloadBanList();
				cout << "Reloaded the banlist\n";
			}
			else
			{
				cout << "Possible commands:\n Q - Quit\n S - Stats\n B - Ban IP\n T - Show/hide traffic\n C - Numbers of Client\n N - Number of Servers\n M - Print connected servers\n"
					" U - Broadcast Server Update\n Y - Print Time and Weather\n P - Enter new ping range\n O - Ping a server\n R - Reload ban list\n";
			}
		}
        #endif
	}

	if(!wekilled)
	{
		mServerMgr.shutdown();
	}

	loginServerThread.join(); // wait for the server to exit
	RakSleep(1500);
    #else
	mServerMgr.startThread();
    #endif

	return 0;
}

