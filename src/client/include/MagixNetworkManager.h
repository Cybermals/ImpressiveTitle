#ifndef __MagixNetworkManager_h_
#define __MagixNetworkManager_h_

enum NETWORKMESSAGE
{
	NETWORK_NOTHING,
	NETWORK_CONNECTED,
	NETWORK_CONNECTIONFAILED,
	NETWORK_LOSTCONNECTION,
	NETWORK_LOGONSUCCESS,
	NETWORK_LOGONFAIL,
	NETWORK_CREATEACCOUNTSUCCESS,
	NETWORK_CREATEACCOUNTFAIL,
	NETWORK_EDITACCOUNTSUCCESS,
	NETWORK_EDITACCOUNTFAIL,
	NETWORK_LOADCHARSUCCESS,
	NETWORK_LOADCHARFAIL,
	NETWORK_CREATECHARSUCCESS,
	NETWORK_CREATECHARFAIL,
	NETWORK_EDITCHARSUCCESS,
	NETWORK_EDITCHARFAIL,
	NETWORK_LOGONINUSE,
	NETWORK_PARTYREQUEST,
	NETWORK_BLOCKNAME
};
struct NetworkMessage
{
	unsigned short message;
	String param;
	NetworkMessage(const unsigned short &m=NETWORK_NOTHING, const String &p="")
	{
		message = m;
		param = p;
	}
};

#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "StringCompressor.h"
#include "GameConfig.h"
#include "MagixNetworkDefines.h"
#include "MagixChatManager.h"
#include "MagixAlertBox.h"
#include "MagixCharScreenManager.h"

struct PlayerData
{
	String name;
	String mapName;

	PlayerData(const String &n="", const String &m="")
	{
		name = n;
		mapName = m;
	}
};

struct PositionInfo
{
	Vector3 position;
	bool hasOwnGroundHeight;
	Real groundHeight;
	PositionInfo(const Vector3 &p,bool hOGH=false,const Real &gH=0)
	{
		position = p;
		hasOwnGroundHeight = hOGH;
		groundHeight = gH;
	}
};

class MagixNetworkManager
{
protected:
	MagixExternalDefinitions *mDef;
	MagixGameStateManager *mGameStateManager;
	MagixItemManager *mItemManager;
	MagixCritterManager *mCritterManager;
	MagixUnitManager *mUnitManager;
	MagixWorld *mWorld;
	MagixSkyManager *mSkyManager;
	MagixChatManager *mChatManager;
	MagixAlertBox *mAlertBox;
	MagixCharScreenManager *mCharScreenManager;
	RakPeerInterface *mPeer;
	bool acceptedFlag;
	Real updateCount;
	Real moveUpdateCount;
	vector<const PlayerData>::type playerList;
	bool playerListUpdated;
	String targetBioUpdated;
	NetworkMessage networkMessage;
	bool isMainServerConnection;
	unsigned short countsIntoGame;
	vector<const std::pair<unsigned char,SystemAddress>>::type serverAdd;
	bool sentPlayerUpdate;
	vector<const std::pair<OwnerToken,PositionInfo>>::type playerPositionQueue;
	std::pair<Vector3,Real> lastMovePosition;
	Degree lastFaceDirection;
	std::pair<bool,Quaternion> lastLookQuaternion;
	unsigned char lastFlags;
	unsigned char lastPrimaryFlags;
	unsigned char lastSecondaryFlags;
public:
	MagixNetworkManager()
	{
		mDef = 0;
		mGameStateManager = 0;
		mItemManager = 0;
		mCritterManager = 0;
		mUnitManager = 0;
		mWorld = 0;
		mSkyManager = 0;
		mChatManager = 0;
		mAlertBox = 0;
		mCharScreenManager = 0;
		mPeer = 0;
		acceptedFlag = false;
		updateCount = 0;
		moveUpdateCount = 0;
		playerList.clear();
		playerListUpdated = false;
		targetBioUpdated = "";
		networkMessage = NetworkMessage();
		isMainServerConnection = false;
		countsIntoGame = 0;
		serverAdd.clear();
		sentPlayerUpdate = false;
		playerPositionQueue.clear();
		lastMovePosition = std::pair<Vector3,Real>(Vector3::ZERO,0);
		lastFaceDirection = Degree(0);
		lastLookQuaternion = std::pair<bool,Quaternion>(false,Quaternion::IDENTITY);
		lastFlags = 0;
		lastPrimaryFlags = 0;
		lastSecondaryFlags = 0;
	}
	~MagixNetworkManager()
	{
		if(mPeer)
		{
			if(mPeer->IsActive())disconnect();
			RakNetworkFactory::DestroyRakPeerInterface(mPeer);
		}
	}
	void initialize(MagixExternalDefinitions *def, MagixGameStateManager *gamestateMgr, MagixItemManager *itemMgr, MagixCritterManager *critterMgr, MagixUnitManager *unitMgr, MagixWorld *world, MagixSkyManager *skyMgr, MagixChatManager *chatMgr, MagixAlertBox *alertBox, MagixCharScreenManager *charscreenMgr)
	{
		mDef = def;
		mGameStateManager = gamestateMgr;
		mItemManager = itemMgr;
		mCritterManager = critterMgr;
		mUnitManager = unitMgr;
		mWorld = world;
		mSkyManager = skyMgr;
		mChatManager = chatMgr;
		mAlertBox = alertBox;
		mCharScreenManager = charscreenMgr;
		mPeer = RakNetworkFactory::GetRakPeerInterface();
	}
	bool connect()
	{
		if(serverAdd.size()==0)
		{
			mAlertBox->showAlert("No servers to connect to :U",0.5,0.5,10);
			mGameStateManager->setGameState(GAMESTATE_RETURNSTARTSCREEN);
			disconnect();
			return false;
		}
		
		mPeer->Startup(1,30,&SocketDescriptor(), 1);

		const String tLocalIP = mDef->loadLocalIP("localIP.txt");
		if(tLocalIP!="")
		{
			mPeer->Connect(tLocalIP.c_str(), SERVER_PORT, SERVER_PASSWORD, (int)strlen(SERVER_PASSWORD));
			mChatManager->message("Connecting to Server "+tLocalIP);
		}
		else
		{
			unsigned char tServerID = Math::RangeRandom(0,(int)serverAdd.size());
			if(tServerID>=(int)serverAdd.size())tServerID = (int)serverAdd.size()-1;
			mPeer->Connect(serverAdd[tServerID].second.ToString(false), serverAdd[tServerID].second.port, SERVER_PASSWORD, (int)strlen(SERVER_PASSWORD));
			mChatManager->message("Connecting to Server "+StringConverter::toString(serverAdd[tServerID].first+1));
		}
		mPeer->SetUnreliableTimeout(30);
		isMainServerConnection = false;
		return true;
	}
	void connectMainServer()
	{
		unsigned short tNumDays = 0;
		if(mDef->loadBanFile(tNumDays))
		{
			String tMessage = "You have been banned";
			if(tNumDays>0)
			{
				tMessage += " (" + StringConverter::toString(tNumDays);
				tMessage += tNumDays>1?" days left)":" day left)";
			}
			mAlertBox->showAlert(tMessage,0.5,0.5,10);
			return;
		}
		serverAdd.clear();
		mPeer->Startup(1,30,&SocketDescriptor(), 1);
		const String tLocalIP = mDef->loadLocalIP("localIP.txt");
		if(tLocalIP!="")
		{
			mPeer->Connect(tLocalIP.c_str(), MAIN_SERVER_PORT, SERVER_PASSWORD, (int)strlen(SERVER_PASSWORD));
		}
		else
		{
			mPeer->Connect(MAIN_SERVER_IP, MAIN_SERVER_PORT, SERVER_PASSWORD, (int)strlen(SERVER_PASSWORD));
		}
		isMainServerConnection = true;
	}
	void disconnect()
	{
		mPeer->Shutdown(300);
		isMainServerConnection = false;
		acceptedFlag = false;
		playerList.clear();
		playerListUpdated = true;
		sentPlayerUpdate = false;
		reset();
	}
	void reset()
	{
		playerPositionQueue.clear();
		lastMovePosition = std::pair<Vector3,Real>(Vector3::ZERO,0);
		lastFaceDirection = Degree(0);
		lastLookQuaternion = std::pair<bool,Quaternion>(false,Quaternion::IDENTITY);
		lastFlags = 0;
		lastPrimaryFlags = 0;
		lastSecondaryFlags = 0;
	}
	void update(const FrameEvent evt)
	{
		if(acceptedFlag)
		{
			updateCount -= evt.timeSinceLastFrame;
			moveUpdateCount -= evt.timeSinceLastFrame;
			if(moveUpdateCount<=0)
			{
				moveUpdateCount += (mUnitManager->getPlayer()->getIsCrouching()?0.75:(mUnitManager->getPlayer()->getIsWalking()?0.5:0.25));
				sendMovePosition();
				sendFaceDirection();
			}
			if(updateCount<=0)
			{
				updateCount += 1;
				sendLookQuaternion();
				sendActionFlags();

				/*if(countsIntoGame<5)countsIntoGame++;
				if(countsIntoGame==5)
				{
					countsIntoGame = 31;
					if(mGameStateManager->getDimension()==0 &&
						(mWorld->getWorldName()=="Default" || mWorld->getWorldName()=="First Cave" || mWorld->getWorldName()=="Ice Cave" || mWorld->getWorldName()=="Lava Cave" || mWorld->getWorldName()=="Waterfall Cave")&&
						Math::RangeRandom(0,50)>0 && mItemManager->getNumItems()<(unsigned short)playerList.size())
					{
						const String tItems[] = {"Egg","Egg","Egg","Egg","Egg2","Egg2","Egg2","Egg2","Egg3","Egg3","Egg3","Egg3","Egg4","Egg5","Egg4"};
						short tRand = (short)Math::RangeRandom(-1,15);
						if(tRand<0)tRand = 0;
						if(tRand>14)tRand = 14;
						const Vector2 tSize = mWorld->getWorldSize();
						sendItemDrop(tItems[tRand],Math::RangeRandom(110,tSize.x-110),Math::RangeRandom(110,tSize.y-110));
					}
				}*/
			}
		}

		Packet *packet = mPeer->Receive();
		bool tCont = true;
		while(packet && tCont)
		{
			switch (getPacketIdentifier(packet))
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				if(isMainServerConnection)
					networkMessage = NETWORK_CONNECTED;
				else
				{
					mChatManager->message("Connected.");
					sendPlayerTokenRequest();
				}
				break;
			case ID_TOKENCONNECTED:
				processPlayerToken(packet);
				sendValidateName();
				//sendHPUpdate(true);
				//sendSkillInitialize();
				//sendPetUpdate();
				break;
			case ID_WORLDINFO:
				//Game starts here if this is first connection
				processWorldInfo(packet);
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				mAlertBox->showAlert("Our connection request has failed :c",0.5,0.5,10);
				if(isMainServerConnection)
					networkMessage = NETWORK_CONNECTIONFAILED;
				else
					mGameStateManager->setGameState(GAMESTATE_RETURNSTARTSCREEN);
				disconnect();
				tCont = false;
				break;
			case ID_INVALID_PASSWORD:
				mAlertBox->showAlert("Please download the latest version c:",0.5,0.5,10);
				if(isMainServerConnection)
					networkMessage = NETWORK_CONNECTIONFAILED;
				else
					mGameStateManager->setGameState(GAMESTATE_RETURNSTARTSCREEN);
				disconnect();
				tCont = false;
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				mAlertBox->showAlert("This server is temporarily unavailable :B",0.5,0.5,10);
				if(isMainServerConnection)
					networkMessage = NETWORK_CONNECTIONFAILED;
				else
					mGameStateManager->setGameState(GAMESTATE_RETURNSTARTSCREEN);
				disconnect();
				tCont = false;
				break;
			case ID_CONNECTION_BANNED:
				mAlertBox->showAlert("You have been banned",0.5,0.5,10);
				disconnect();
				tCont = false;
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if(isMainServerConnection)
				{
					mAlertBox->showAlert("We have been disconnected :c",0.5,0.5,10);
					networkMessage = NETWORK_LOSTCONNECTION;
				}
				else
				{
					//Remove disconnected server from server list
					unsigned char tServerID = MAX_SERVERS;
					for(vector<const std::pair<unsigned char,SystemAddress>>::type::iterator it=serverAdd.begin();it!=serverAdd.end();it++)
					{
						const std::pair<unsigned char,SystemAddress> tServer = *it;
						if(tServer.second==packet->systemAddress)
						{
							tServerID = tServer.first;
							if(serverAdd.size()>1)serverAdd.erase(it);
							break;
						}
					}
					mAlertBox->showAlert("Disconnected, reconnecting...",0.5,0.5,2.5);
					mChatManager->message("Disconnected from Server "+StringConverter::toString(tServerID+1)+".");
					disconnect();
					mItemManager->reset();
					mCritterManager->reset();
					mUnitManager->reset(false,false);
					mUnitManager->clearPartyMembers();
					mUnitManager->getPlayer()->setIndex(0);
					connect();
					tCont = false;
				}
				break;
			case ID_CONNECTION_LOST:
				if(isMainServerConnection)
				{
					mAlertBox->showAlert("Connection lost :c",0.5,0.5,10);
					networkMessage = NETWORK_LOSTCONNECTION;
				}
				else
				{
					//Remove disconnected server from server list
					unsigned char tServerID = MAX_SERVERS;
					for(vector<const std::pair<unsigned char,SystemAddress>>::type::iterator it=serverAdd.begin();it!=serverAdd.end();it++)
					{
						const std::pair<unsigned char,SystemAddress> tServer = *it;
						if(tServer.second==packet->systemAddress)
						{
							tServerID = tServer.first;
							if(serverAdd.size()>1)serverAdd.erase(it);
							break;
						}
					}
					mAlertBox->showAlert("Connection lost, reconnecting...",0.5,0.5,2.5);
					mChatManager->message("Connection lost from Server "+StringConverter::toString(tServerID+1)+", reconnecting...");
					disconnect();
					mItemManager->reset();
					mCritterManager->reset();
					mUnitManager->reset(false,false);
					mUnitManager->clearPartyMembers();
					mUnitManager->getPlayer()->setIndex(0);
					connect();
					tCont = false;
				}
				break;
			case ID_CHAT: processChat(packet); break;
			case ID_NEWPLAYER: processNewPlayer(packet); break;
			//case ID_NEWPLAYERCOLOURS: processNewPlayerColours(packet); break;
			case ID_POSITION: processPosition(packet); break;
			case ID_NEWPLAYERUPDATE: processAndSendNewPlayerUpdate(packet); break;
			case ID_REQUESTMAPINFO: processAndSendMapInfo(packet); break;
			case ID_PLAYERDISCONNECTED: processPlayerDisconnected(packet); break;
			case ID_MOVEPOSITION: processMovePosition(packet); break;
			case ID_LOOKQUATERNION: processLookQuaternion(packet); break;
			case ID_FACEDIRECTION: processFaceDirection(packet); break;
			case ID_ACTIONFLAGS: processActionFlags(packet); break;
			case ID_EMOTE: processEmote(packet); break;
			case ID_ATTACK: processAttack(packet); break;
			case ID_PLAYERMAPCHANGE: processMapChange(packet); break;
			//case ID_VALIDATENAME: processValidateName(packet); break;
			case ID_PLAYERDATA: processPlayerData(packet); break;
			case ID_BIO: processBio(packet); break;
			case ID_LOGON: processLogon(packet); break;
			case ID_CREATEACCOUNT: processCreateAccount(packet); break;
			case ID_LOADCHAR: processLoadChar(packet); break;
			case ID_CREATECHAR: processCreateChar(packet); break;
			case ID_EDITCHAR: processEditChar(packet); break;
			case ID_EDITACCOUNT: processEditAccount(packet); break;
			case ID_KICK: tCont = processKick(packet); break;
			case ID_ITEMDROP: processItemDrop(packet); break;
			case ID_ITEMPICKUP: processItemPickup(packet); break;
			case ID_ITEMEQUIP: processItemEquip(packet); break;
			case ID_ITEMUNEQUIP: processItemUnequip(packet); break;
			case ID_GODSPEAK: processGodSpeak(packet); break;
			case ID_FORCELOGOUT: tCont = processForceLogout(packet); break;
			case ID_EVENT: processEvent(packet); break;
			case ID_INFO: processInfo(packet); break;
			//case ID_DENIED: processDenied(packet); break;
			case ID_CRITTEROWNAGE: processCritterOwnage(packet); break;
			case ID_CRITTERINFO:
			case ID_CRITTERINFOONEWAY:
				processCritterInfo(packet);
				break;
			case ID_CRITTERTARGETEDINFO: processCritterTargetedInfo(packet); break;
			//case ID_OBJECTOWNAGE: processObjectOwnage(packet); break;
			//case ID_OBJECTINFO: processObjectInfo(packet); break;
			//case ID_OBJECTTARGETEDINFO: processObjectTargetedInfo(packet); break;
			//case ID_UNITOWNAGE: processUnitOwnage(packet); break;
			//case ID_UNITINFO: processUnitInfo(packet); break;
			//case ID_UNITTARGETEDINFO: processUnitTargetedInfo(packet); break;
			case ID_PLAYERINFO:
			case ID_PLAYERTARGETEDINFO:
				processPlayerInfo(packet);
				break;
			case ID_ITEMSTASH: processItemStash(packet); break;
			case ID_FINDPLAYER: processFindPlayer(packet); break;
			case ID_SERVERLIST: processServerList(packet); break;
			case ID_SERVERCONNECTED: processServerConnected(packet); break;
			case ID_MAINTENANCE: processMaintenance(packet); break;
			default:
				//mDebugText = "Message with identifier " + StringConverter::toString(int(getPacketIdentifier(packet))) + " has arrived.";
				break;
			}

			mPeer->DeallocatePacket(packet);
			if(tCont)packet = mPeer->Receive();
		}
	}
	const MessageID getPacketIdentifier(Packet *p)
	{
		if (p==0)
			return 255;

		if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		{
			assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
			return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
		}
		else
			return (unsigned char) p->data[0];
	}
	const vector<const String>::type getPlayersOnline()
	{
		vector<const String>::type tList;
		vector<const String>::type tMapList;
		vector<const unsigned short>::type tCountList;
		tList.clear();
		tMapList.clear();
		tCountList.clear();

		//Count players in each map
		for(vector<const PlayerData>::type::iterator it=playerList.begin(); it!=playerList.end(); it++)
		{
			const PlayerData *tData = &*it;
			bool tHasMap = false;
			const String tMapName = mDef->getMapName(tData->mapName);

			for(int i=0;i<(int)tMapList.size();i++)
			{
				//Map exists in list, increment player count
				if(tMapName == tMapList[i])
				{
					tHasMap = true;
					tCountList[i] += 1;
					break;
				}
			}

			//New map, add to list
			if(!tHasMap)
			{
				tMapList.push_back(tMapName);
				tCountList.push_back(1);
			}
		}

		//Write to output string
		for(int i=0;i<(int)tMapList.size();i++)
		{
			const String tLine = tMapList[i] + ": " + StringConverter::toString(tCountList[i]);
			tList.push_back(tLine);
		}

		return tList;
	}
	bool isPlayerOnline(String name)
	{
		StringUtil::toLowerCase(name);
		for(vector<const PlayerData>::type::iterator it=playerList.begin(); it!=playerList.end(); it++)
		{
			const PlayerData *tData = &*it;
			String tName = tData->name;
			StringUtil::toLowerCase(tName);
			if(tName == name)return true;
		}
		return false;
	}
	const String getPlayerMap(String name)
	{
		StringUtil::toLowerCase(name);
		for(vector<const PlayerData>::type::iterator it=playerList.begin(); it!=playerList.end(); it++)
		{
			const PlayerData *tData = &*it;
			String tName = tData->name;
			StringUtil::toLowerCase(tName);
			if(tName == name)return mDef->getMapName(tData->mapName);
		}
		return "";
	}
	void processPlayerToken(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;
		unsigned short tDayTime = (unsigned short)mSkyManager->getDayTime();
		unsigned short tWeatherTime = (unsigned short)mSkyManager->getWeatherTime();

		tBitStream.Read(tID);
		tBitStream.Read(tToken);
		tBitStream.Read(tDayTime);
		tBitStream.Read(tWeatherTime);

		mUnitManager->getPlayer()->setIndex(tToken);
		mSkyManager->setDayTime(tDayTime);
		mSkyManager->setWeatherTime(tWeatherTime);
	}
	void sendPlayerTokenRequest()
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_TOKENCONNECTED));
		stringCompressor->EncodeString(mUnitManager->getPlayer()->getUser().c_str(),16,&tBitStream);
		tBitStream.Write((unsigned short)mCharScreenManager->getCharID());

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processChat(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;
		char tCaption[256];

		tBitStream.Read(tID);
		tBitStream.Read(tToken);
		stringCompressor->DecodeString(tCaption,256,&tBitStream);

		if(tToken==SERVER_ID)
		{
			bool tIsPM = false;
			bool tIsParty = false;
			tBitStream.Read(tIsPM);
			if(tIsPM)
			{
				char tName[16]="";
				stringCompressor->DecodeString(tName,16,&tBitStream);
				if(String(tName)!="")
				{
					if(mDef->isInBlockedList(tName))return;
				}
			}
			else tBitStream.Read(tIsParty);
			mChatManager->message(String(tCaption),tIsParty?CHAT_PARTY:0);
		}
		else
		{
			MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
			if(!tUnit)return;

			unsigned char tType=CHAT_GENERAL;
			bool tLipSync=false;
			tBitStream.Read(tType);
			tBitStream.Read(tLipSync);

			//Return if sayer is blocked
			if(tUnit->isBlocked)return;

			if(tLipSync||tType==CHAT_LIPSYNC)tUnit->doLipSync(tCaption);
			if(tType!=CHAT_LIPSYNC)mChatManager->say(mUnitManager,tUnit,String(tCaption),tType);
		}
	}
	void sendChat(const String &caption, const unsigned char &type, const String &pmTarget="", bool autoLipSync=false)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CHAT));
		tBitStream.Write(OwnerToken(TOKEN_NULL));
		
		//Private message
		if(type==CHAT_PRIVATE &&pmTarget.length()>0)
		{
			if(pmTarget==mUnitManager->getPlayer()->getUser())
			{
				mChatManager->message("Cannot send Private Message to yourself.");
				return;
			}
			else
			{
				tBitStream.Write(true);
				stringCompressor->EncodeString(pmTarget.c_str(),16,&tBitStream);
				mChatManager->message("Sent Private Message to " + pmTarget + ".");
			}
		}
		else tBitStream.Write(false);

		stringCompressor->EncodeString(caption.c_str(),256,&tBitStream);
		tBitStream.Write(type);
		tBitStream.Write(autoLipSync);

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendPartyChat(const OwnerToken &target, const String &caption, const unsigned char &type, bool autoLipSync=false)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CHAT));
		tBitStream.Write(target);
		
		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(target);
		//Is on different map, server will process and redirect chat
		if(!tUnit)tBitStream.Write(true);
		//Normal chat send
		else tBitStream.Write(false);

		stringCompressor->EncodeString(caption.c_str(),256,&tBitStream);
		tBitStream.Write(type);
		tBitStream.Write(autoLipSync);

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processNewPlayer(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;
		char tUser[16];
		char tName[32];
		char tMeshString[128];
		char tMatName[32];
		char tMeshString2[32];
		bool tDefaultScale;
		bool tBadData = false;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);
		stringCompressor->DecodeString(tUser,16,&tBitStream);
		stringCompressor->DecodeString(tName,32,&tBitStream);
		stringCompressor->DecodeString(tMeshString,128,&tBitStream);
		stringCompressor->DecodeString(tMatName,32,&tBitStream);
		stringCompressor->DecodeString(tMeshString2,32,&tBitStream);
		tBitStream.Read(tDefaultScale);

		const vector<String>::type tParam = StringUtil::split(String(tMeshString),";",5);
		if(tParam.size()!=5)tBadData = true;
		const vector<String>::type tParam2 = StringUtil::split(String(tMeshString2),";",1);
		if(tParam2.size()!=1)tBadData = true;

		//Unit already exists, don't create
		if(mUnitManager->getUnitByIndex(tToken))return;

		MagixIndexedUnit *tUnit = 
			tBadData?mUnitManager->createUnit(tToken,"Body","Head1","Maneless","Tail1","Wingless",tMatName,"Tuftless")
			:mUnitManager->createUnit(tToken,tParam[0],tParam[1],mDef->hasManeMesh(tParam[2])?tParam[2]:"Maneless",(mDef->hasTailMesh(tParam[3])?tParam[3]:"Tail1"),(mDef->hasWingMesh(tParam[4])?tParam[4]:"Wingless"),tMatName,(mDef->hasTuftMesh(tParam2[0])?tParam2[0]:"Tuftless"));
		tUnit->setHP(500);
		mUnitManager->createNameTag(tUnit,tName);
		mUnitManager->createUserTag(tUnit,tUser);
		if(!tDefaultScale)
		{
			float tScale;
			tBitStream.Read(tScale);
			tUnit->getObjectNode()->setScale(tScale,tScale,tScale);
		}
		if(String(tMatName)=="CustomMat")
		{
			short tBodyMark, tHeadMark, tTailMark;
			char tColours[256];
			tBitStream.Read(tBodyMark);
			tBitStream.Read(tHeadMark);
			tBitStream.Read(tTailMark);
			tUnit->setMarkings((mDef->hasBodyMark(tBodyMark)?tBodyMark:0),(mDef->hasHeadMark(tHeadMark)?tHeadMark:0),(mDef->hasTailMark(tTailMark)?tTailMark:0));
			stringCompressor->DecodeString(tColours,256,&tBitStream);
			tUnit->setColours(String(tColours));
		}

		tUnit->setAlliance(ALLIANCE_FRIEND);
		tUnit->isFriend = mDef->isInFriendList(tUser);
		tUnit->isBlocked = mDef->isInBlockedList(tUser);
		
		char tAdminToken[8] = "";
		stringCompressor->DecodeString(tAdminToken,8,&tBitStream);
		tUnit->isAdmin = (String(tAdminToken)=="ok.");
		tUnit->isMod = (String(tAdminToken)=="mod");

		String tAddMessage = "";
		if(tUnit->isFriend)tAddMessage = " (Friend)";
		if(tUnit->isBlocked)
		{
			tAddMessage = " (Blocked)";
			tUnit->isHidden = true;
			sendInfo(INFO_YOURBLOCKED,tToken);
		}
		if(tUnit->isAdmin)
		{
			tAddMessage = " (" USER_ADMIN_TEXT ")";
			tUnit->isHidden = false;
			tUnit->isBlocked = false;
			char tAdminColour[16];
			stringCompressor->DecodeString(tAdminColour,16,&tBitStream);
			mUnitManager->createAdminAura(tUnit,StringConverter::parseColourValue(String(tAdminColour)));
		}
		if(tUnit->isMod)
		{
			tUnit->isHidden = false;
			tUnit->isBlocked = false;
			/*bool tIsModOn = false;
			tBitStream.Read(tIsModOn);
			if(tIsModOn)mUnitManager->createParticleOnUnit(tUnit,"RisingShards",-1,true);*/
		}

		const String tAddName = (tUnit->getUser()==""?tUnit->getName():tUnit->getUser());
		mChatManager->message(tAddName + tAddMessage + " has arrived.",(mUnitManager->isPartyMember(tUnit)?0:(mChatManager->getChannel()==0?CHAT_LOCAL:CHAT_GENERAL)));
		if(tBadData)mChatManager->message("Error notice:" + tAddName + "has sent erroneous data.");

		//Process early position packets
		for(vector<const std::pair<OwnerToken,PositionInfo>>::type::iterator it=playerPositionQueue.begin();it!=playerPositionQueue.end();it++)
		{
			const std::pair<OwnerToken,PositionInfo> tPos = *it;
			if(tPos.first==tToken)
			{
				tUnit->setPosition(tPos.second.position);
				tUnit->resetTarget();
				tUnit->allocFlags.hasOwnGroundHeight = tPos.second.hasOwnGroundHeight;
				if(tPos.second.hasOwnGroundHeight)tUnit->setGroundHeight(tPos.second.groundHeight);
				else mUnitManager->clampUnitToTerrain(tUnit,false);
				playerPositionQueue.erase(it);
				break;
			}
		}
	}
	void sendNewPlayer(const OwnerToken &target=OwnerToken(TOKEN_NULL))
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_NEWPLAYER));
		tBitStream.Write(target);
		stringCompressor->EncodeString(mUnitManager->getPlayer()->getUser().c_str(),16,&tBitStream);
		stringCompressor->EncodeString(mUnitManager->getPlayer()->getName().c_str(),32,&tBitStream);
		stringCompressor->EncodeString(mUnitManager->getPlayer()->getMeshString().c_str(),128,&tBitStream);
		stringCompressor->EncodeString(mUnitManager->getPlayer()->getMatGroupName().c_str(),32,&tBitStream);
		stringCompressor->EncodeString(mUnitManager->getPlayer()->getMeshString2().c_str(),32,&tBitStream);
		const bool tDefaultScale = (mUnitManager->getPlayer()->getObjectNode()->getScale().x == 1);
		tBitStream.Write(tDefaultScale);
		if(!tDefaultScale)tBitStream.Write(mUnitManager->getPlayer()->getObjectNode()->getScale().x);
		if(mUnitManager->getPlayer()->getMatGroupName()=="CustomMat")
		{
			short tBodyMark=0, tHeadMark=0, tTailMark=0;
			mUnitManager->getPlayer()->getMarkings(tBodyMark,tHeadMark,tTailMark);
			tBitStream.Write(tBodyMark);
			tBitStream.Write(tHeadMark);
			tBitStream.Write(tTailMark);
			stringCompressor->EncodeString(mUnitManager->getPlayer()->getColourString().c_str(),256,&tBitStream);
		}
		stringCompressor->EncodeString(mDef->isAdmin?"ok.":(mDef->isMod?"mod":"nil"),8,&tBitStream);
		if(mDef->isAdmin)stringCompressor->EncodeString(StringConverter::toString(mDef->adminAuraColour).c_str(),16,&tBitStream);
		//if(mDef->isMod)tBitStream.Write(mDef->isModOn);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processPosition(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;
		Vector3 tPosition;
		bool tHasGroundHeight = false;
		Real tGroundHeight = 0;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);
		tBitStream.Read(tPosition);
		tBitStream.Read(tHasGroundHeight);
		if(tHasGroundHeight)tBitStream.Read(tGroundHeight);

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)
		{
			playerPositionQueue.push_back(std::pair<OwnerToken,PositionInfo>(tToken,PositionInfo(tPosition,tHasGroundHeight,tGroundHeight)));
			return;
		}
		tUnit->setPosition(tPosition);
		tUnit->resetTarget();
		tUnit->allocFlags.hasOwnGroundHeight = tHasGroundHeight;
		if(tHasGroundHeight)tUnit->setGroundHeight(tGroundHeight);
		else mUnitManager->clampUnitToTerrain(tUnit,false);
	}
	void sendPosition(const OwnerToken &target=OwnerToken(TOKEN_NULL))
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_POSITION));
		tBitStream.Write(target);
		tBitStream.Write(mUnitManager->getPlayer()->getPosition());
		bool tOwnGroundHeight = mUnitManager->getPlayer()->allocFlags.hasOwnGroundHeight;
		tBitStream.Write(tOwnGroundHeight);
		if(tOwnGroundHeight)tBitStream.Write(mUnitManager->getPlayer()->getGroundHeight());

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processMovePosition(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)return;

		Vector3 tPosition;
		bool tOwnGroundHeight;
		tBitStream.Read(tPosition);
		tBitStream.Read(tOwnGroundHeight);

		tUnit->allocFlags.hasOwnGroundHeight = tOwnGroundHeight;
		if(tOwnGroundHeight)
		{
			Real tGroundHeight;
			tBitStream.Read(tGroundHeight);
			tUnit->setGroundHeight(tGroundHeight);
		}

		tUnit->setTarget(tPosition);
	}
	void sendMovePosition()
	{
		const Vector3 tPos = mUnitManager->getPlayer()->getPosition();
		const Real tHeight = mUnitManager->getPlayer()->getGroundHeight();

		if(lastMovePosition.first==tPos && lastMovePosition.second==tHeight)return;
		lastMovePosition.first = tPos;
		lastMovePosition.second = tHeight;

		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_MOVEPOSITION));
		tBitStream.Write(OwnerToken(TOKEN_NULL));
		tBitStream.Write(tPos);
		bool tOwnGroundHeight = mUnitManager->getPlayer()->allocFlags.hasOwnGroundHeight;
		tBitStream.Write(tOwnGroundHeight);
		if(tOwnGroundHeight)tBitStream.Write(tHeight);

		mPeer->Send(&tBitStream, LOW_PRIORITY, UNRELIABLE_SEQUENCED, 5, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processLookQuaternion(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)return;

		bool tFreeLook;
		tBitStream.Read(tFreeLook);
		tUnit->setFreeLook(tFreeLook);
		if(!tFreeLook)return;

		Quaternion tDirection;
		tBitStream.Read(tDirection);

		tUnit->setLookDirection(tDirection);
	}
	void sendLookQuaternion(const OwnerToken &target=OwnerToken(TOKEN_NULL))
	{
		const bool tFreeLook = mUnitManager->getPlayer()->getFreeLook();
		if(target==TOKEN_NULL)
		{
			if(!tFreeLook && !lastLookQuaternion.first)return;
			lastLookQuaternion.first = tFreeLook;
		}

		const Quaternion tDir = mUnitManager->getPlayer()->getLookDirection();
		if(lastLookQuaternion.second==tDir)return;
		lastLookQuaternion.second = tDir;

		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_LOOKQUATERNION));
		tBitStream.Write(target);
		tBitStream.Write(tFreeLook);
		if(tFreeLook)tBitStream.Write(tDir);

		mPeer->Send(&tBitStream, LOW_PRIORITY, UNRELIABLE_SEQUENCED, 5, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processFaceDirection(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)return;
		//So that fainted units don't rotate on the floor
		if(tUnit->isDisabled() && tUnit->getHP()<=0)return;

		Degree tYaw;
		tBitStream.Read(tYaw);

		tUnit->setYaw(tYaw);
	}
	void sendFaceDirection(const OwnerToken &target=OwnerToken(TOKEN_NULL))
	{
		const Degree tYaw = mUnitManager->getPlayer()->getYaw();
		if(target==TOKEN_NULL)
		{
			if(lastFaceDirection==tYaw)return;
			lastFaceDirection = tYaw;
		}

		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_FACEDIRECTION));
		tBitStream.Write(target);
		tBitStream.Write(tYaw);

		mPeer->Send(&tBitStream, LOW_PRIORITY, UNRELIABLE_SEQUENCED, 5, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processActionFlags(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)return;

		bool tHasFlags;
		tBitStream.Read(tHasFlags);
		if(tHasFlags)
		{
			std::bitset<3> tData;
			for(int i=0;i<(int)tData.size();i++)
			{
				bool tBool;
				tBitStream.Read(tBool);
				tData[i] = tBool;
			}
			unsigned char tFlags = tData.to_ulong();

			tUnit->setAntiGravity(tFlags==1);
			if(tFlags==2)tUnit->doDoubleJump();
			tUnit->setJump(tFlags==3);
			if((tFlags==4)!=tUnit->getIsCrouching())tUnit->doCrouch(tFlags==4);
			tUnit->setIsWalking(tFlags==5);
			if((tFlags==6)!=tUnit->getIsStance())tUnit->toggleStance(mDef);
		}
		else
		{
			tUnit->setAntiGravity(false);
			tUnit->setJump(false);
			if(tUnit->getIsCrouching())tUnit->doCrouch(false);
			tUnit->setIsWalking(false);
			if(tUnit->getIsStance())tUnit->toggleStance(mDef);
		}
		bool tHasPrimary;
		tBitStream.Read(tHasPrimary);
		if(tHasPrimary)
		{
			std::bitset<5> tData;
			for(int i=0;i<(int)tData.size();i++)
			{
				bool tBool;
				tBitStream.Read(tBool);
				tData[i] = tBool;
			}
			unsigned char tPrimary = tData.to_ulong();

			if(tPrimary==1)
			{
				if(!tUnit->isSitting())tUnit->doSit();
			}
			else if(tPrimary==2)
			{
				if(!tUnit->isLaying())
				{
					if(!tUnit->isSitting())tUnit->doSit(true);
					tUnit->doSit();
				}
			}
			else if(tPrimary==3)
			{
				if(!tUnit->isPlopped())
				{
					if(!tUnit->isLaying())
					{
						if(!tUnit->isSitting())tUnit->doSit(true);
						tUnit->doSit(true);
					}
					tUnit->doSit();
				}
			}
			else if(tPrimary==4)
			{
				if(!tUnit->isSideLaying())
				{
					if(!tUnit->isSitting())tUnit->doSit(true);
					tUnit->doSideLay();
				}
			}
			else if(tPrimary==5)
			{
				if(!tUnit->isLeaning())
				{
					if(!tUnit->isSideLaying())
					{
						if(!tUnit->isSitting())tUnit->doSit(true);
						tUnit->doSideLay(true);
					}
					tUnit->doSideLay();
				}
			}
			else if(tPrimary==6)
			{
				if(!tUnit->isRolledOver())
				{
					if(!tUnit->isLeaning())
					{
						if(!tUnit->isSideLaying())
						{
							if(!tUnit->isSitting())tUnit->doSit(true);
							tUnit->doSideLay(true);
						}
						tUnit->doSideLay(true);
					}
					tUnit->doSideLay();
				}
			}
			else if(tPrimary==7)tUnit->doHeadswing();
			else if(tPrimary==8)tUnit->doHeadbang();
			else if(tPrimary==9)tUnit->doButtswing();
			else if(tPrimary==10)tUnit->doWingwave();
			else if(tPrimary==11)tUnit->doMoonwalk();
			else if(tPrimary==12)tUnit->doThriller();
			else if(tPrimary==13)tUnit->doPoint();
			else if(tPrimary==14)tUnit->doStretch();
			else if(tPrimary==15)tUnit->doRoar();
			else if(tPrimary==16)tUnit->doRofl();
			else if(tPrimary==17)tUnit->doCurl();
			else if(tPrimary==18)tUnit->doFaint();
		}
		bool tHasSecondary;
		tBitStream.Read(tHasSecondary);
		if(tHasSecondary)
		{
			std::bitset<4> tData;
			for(int i=0;i<(int)tData.size();i++)
			{
				bool tBool;
				tBitStream.Read(tBool);
				tData[i] = tBool;
			}
			unsigned char tSecondary = tData.to_ulong();

			if(tSecondary==1)tUnit->doNodHead();
			else if(tSecondary==2)tUnit->doShakeHead();
			else if(tSecondary==3)tUnit->doNodHeadSlow();
			else if(tSecondary==4)tUnit->doShakeHeadSlow();
			else if(tSecondary==5)tUnit->doHeadTilt();
			else if(tSecondary==8)tUnit->doSniff();
			else if(tSecondary==9)tUnit->doTailFlick();
			else if(tSecondary==10)tUnit->doLaugh();
			else if(tSecondary==11)tUnit->doChuckle();
			if((tSecondary==6)!=tUnit->isLick())tUnit->toggleLick();
			if((tSecondary==7)!=tUnit->isNuzzle())tUnit->toggleNuzzle();
		}
		else
		{
			if(tUnit->isLick())tUnit->toggleLick();
			if(tUnit->isNuzzle())tUnit->toggleNuzzle();
		}
	}
	void sendActionFlags(const OwnerToken &target=OwnerToken(TOKEN_NULL))
	{
		//Flags = 6
		unsigned char tFlags = 0;
		if(mUnitManager->getPlayer()->isAntiGravity())tFlags = 1;
		else if(mUnitManager->getPlayer()->getHasDoubleJumped())tFlags = 2;
		else if(mUnitManager->getPlayer()->getJump())tFlags = 3;
		else if(mUnitManager->getPlayer()->getIsCrouching())tFlags = 4;
		else if(mUnitManager->getPlayer()->getIsWalking())tFlags = 5;
		else if(mUnitManager->getPlayer()->getIsStance())tFlags = 6;
		//Primary actions = 18
		unsigned char tPrimary = 0;
		if(mUnitManager->getPlayer()->isSitting())tPrimary = 1;
		else if(mUnitManager->getPlayer()->isLaying())tPrimary = 2;
		else if(mUnitManager->getPlayer()->isPlopped())tPrimary = 3;
		else if(mUnitManager->getPlayer()->isSideLaying())tPrimary = 4;
		else if(mUnitManager->getPlayer()->isLeaning())tPrimary = 5;
		else if(mUnitManager->getPlayer()->isRolledOver())tPrimary = 6;
		else if(mUnitManager->getPlayer()->isHeadswinging())tPrimary = 7;
		else if(mUnitManager->getPlayer()->isHeadbanging())tPrimary = 8;
		else if(mUnitManager->getPlayer()->isButtswinging())tPrimary = 9;
		else if(mUnitManager->getPlayer()->isWingwaving())tPrimary = 10;
		else if(mUnitManager->getPlayer()->isMoonwalking())tPrimary = 11;
		else if(mUnitManager->getPlayer()->isThrillering())tPrimary = 12;
		else if(mUnitManager->getPlayer()->isPoint())tPrimary = 13;
		else if(mUnitManager->getPlayer()->isStretch())tPrimary = 14;
		else if(mUnitManager->getPlayer()->isRoaring())tPrimary = 15;
		else if(mUnitManager->getPlayer()->isRofling())tPrimary = 16;
		else if(mUnitManager->getPlayer()->isCurled())tPrimary = 17;
		else if(mUnitManager->getPlayer()->isFainting())tPrimary = 18;
		//Secondary actions = 11
		unsigned char tSecondary = 0;
		if(mUnitManager->getPlayer()->isNodHead())tSecondary = 1;
		else if(mUnitManager->getPlayer()->isShakeHead())tSecondary = 2;
		else if(mUnitManager->getPlayer()->isNodHeadSlow())tSecondary = 3;
		else if(mUnitManager->getPlayer()->isShakeHeadSlow())tSecondary = 4;
		else if(mUnitManager->getPlayer()->isHeadTilt())tSecondary = 5;
		else if(mUnitManager->getPlayer()->isLick())tSecondary = 6;
		else if(mUnitManager->getPlayer()->isNuzzle())tSecondary = 7;
		else if(mUnitManager->getPlayer()->isSniff())tSecondary = 8;
		else if(mUnitManager->getPlayer()->isTailFlick())tSecondary = 9;
		else if(mUnitManager->getPlayer()->isLaugh())tSecondary = 10;
		else if(mUnitManager->getPlayer()->isChuckle())tSecondary = 11;

		if(target==TOKEN_NULL)
		{
			if(tFlags==lastFlags && tPrimary==lastPrimaryFlags && tSecondary==lastSecondaryFlags)return;
			lastFlags = tFlags;
			lastPrimaryFlags = tPrimary;
			lastSecondaryFlags = tSecondary;
		}

		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_ACTIONFLAGS));
		tBitStream.Write(target);

		//Flags
		const bool tHasFlags = (tFlags>0);
		tBitStream.Write(tHasFlags);
		if(tHasFlags)
		{
			std::bitset<3> tData(tFlags);
			for(int i=0;i<(int)tData.size();i++)tBitStream.Write((bool)tData[i]);
		}
		//Primary actions
		const bool tHasPrimary = (tPrimary>0);
		tBitStream.Write(tHasPrimary);
		if(tHasPrimary)
		{
			std::bitset<5> tData(tPrimary);
			for(int i=0;i<(int)tData.size();i++)tBitStream.Write((bool)tData[i]);
		}
		//Secondary actions
		const bool tHasSecondary = (tSecondary>0);
		tBitStream.Write(tHasSecondary);
		if(tHasSecondary)
		{
			std::bitset<4> tData(tSecondary);
			for(int i=0;i<(int)tData.size();i++)tBitStream.Write((bool)tData[i]);
		}

		mPeer->Send(&tBitStream, LOW_PRIORITY, UNRELIABLE_SEQUENCED, 5, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processEmote(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)return;

		char tEmote[32];
		stringCompressor->DecodeString(tEmote,32,&tBitStream);

		tUnit->setEmote(String(tEmote),mDef);
	}
	void sendEmote(const String &emote)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_EMOTE));
		tBitStream.Write(OwnerToken(TOKEN_NULL));
		stringCompressor->EncodeString(emote.c_str(),32,&tBitStream);

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, UNRELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processAndSendNewPlayerUpdate(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);

		sendNewPlayerProcedure(tToken);
		sendMyCritters(tToken);
		sendLookQuaternion(tToken);
		sendFaceDirection(tToken);
		sendActionFlags(tToken);
	}
	void sendNewPlayerProcedure(const OwnerToken &target=OwnerToken(TOKEN_NULL))
	{
		sendNewPlayer(target);
		sendPosition(target);
		for(int i=0;i<MAX_EQUIP;i++)
			if(mUnitManager->getPlayer()->getEquip(i)!="")sendItemEquip(mUnitManager->getPlayer()->getEquip(i),i,target,true);
		sendPlayerUpdate(target);
	}
	void sendNewPlayerAloneProcedure()
	{
		for(int i=0;i<MAX_EQUIP;i++)
			if(mUnitManager->getPlayer()->getEquip(i)!="")sendItemEquip(mUnitManager->getPlayer()->getEquip(i),i,true);
	}
	void processPlayerDisconnected(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);

		disownCritters(tToken);

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)return;

		bool tIsProper;
		tBitStream.Read(tIsProper);

		const String tName = (tUnit->getUser()==""?tUnit->getName():tUnit->getUser());
		mChatManager->message(tName + (tIsProper?" has disconnected.":" timed out."),(mUnitManager->isPartyMember(tUnit)?0:(mChatManager->getChannel()==0?CHAT_LOCAL:CHAT_GENERAL)));
		mUnitManager->deleteUnit(tUnit);
	}
	void processWorldInfo(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;
		bool tHasOtherPlayers;
		//float tDayTime = mSkyManager->getDayTime();
		//float tWeatherTime = mSkyManager->getWeatherTime();

		tBitStream.Read(tID);
		tBitStream.Read(tToken);
		tBitStream.Read(tHasOtherPlayers);
		/*if(tHasOtherPlayers)
		{
			tBitStream.Read(tDayTime);
			tBitStream.Read(tWeatherTime);

			mSkyManager->setDayTime(tDayTime);
			mSkyManager->setWeatherTime(tWeatherTime);
		}*/
		//Stop here if we've already sent update
		if(sentPlayerUpdate)return;

		if(tHasOtherPlayers)
		{
			mUnitManager->reclampAllUnitsToTerrain();
			sendNewPlayerProcedure();
		}
		else
		{
			sendNewPlayerAloneProcedure();
		}
		if(mUnitManager->getPlayer()->hasPet())
		{
			const Critter tC = mDef->getCritter(mUnitManager->getPlayer()->getPet());
			const Vector3 tPosition = mUnitManager->getPlayer()->getPosition() + Vector3(Math::RangeRandom(-20,20),tC.flying?15:0,-25);
			sendCritterCreate(tC.type,tPosition,true);
		}
		sendImReady();
		if(!acceptedFlag)mGameStateManager->setGameState(GAMESTATE_STARTGAME);
		acceptedFlag = true;
		sentPlayerUpdate = true;
		updateCount = 1;

		countsIntoGame = 0;
	}
	void processAndSendMapInfo(Packet *p)
	{
		RakNet::BitStream tReceiveBit(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;

		tReceiveBit.Read(tID);
		tReceiveBit.Read(tToken);

		/*RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_WORLDINFO));
		tBitStream.Write(tToken);
		tBitStream.Write(true);
		tBitStream.Write(float(mSkyManager->getDayTime()));
		tBitStream.Write(float(mSkyManager->getWeatherTime()));

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS, true);*/

		sendAllItems(tToken);
	}
	void processAttack(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)return;

		char tAttack[32];
		stringCompressor->DecodeString(tAttack,32,&tBitStream);

		tUnit->forceAttack(tAttack);
	}
	void sendAttack(const String &attack)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_ATTACK));
		tBitStream.Write(OwnerToken(TOKEN_NULL));
		stringCompressor->EncodeString(attack.c_str(),32,&tBitStream);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, UNRELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processMapChange(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);

		disownCritters(tToken);

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)return;

		const String tName = (tUnit->getUser()==""?tUnit->getName():tUnit->getUser());
		mChatManager->message(tName + " has left.",(mUnitManager->isPartyMember(tUnit)?0:(mChatManager->getChannel()==0?CHAT_LOCAL:CHAT_GENERAL)));
		mUnitManager->deleteUnit(tUnit);
	}
	void sendMapChange(const String &mapName, const unsigned char &dimension)
	{
		sentPlayerUpdate = false;
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_PLAYERMAPCHANGE));
		tBitStream.Write(OwnerToken(TOKEN_NULL));
		stringCompressor->EncodeString(mapName.c_str(),32,&tBitStream);
		tBitStream.Write(dimension);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	/*void processValidateName(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;
		char tNewName[32];

		tBitStream.Read(tID);
		tBitStream.Read(tToken);
		stringCompressor->DecodeString(tNewName,32,&tBitStream);

		mUnitManager->createNameTag(mUnitManager->getPlayer(),tNewName);
		mChatManager->message("Name in use. Changed name to " + mUnitManager->getPlayer()->getName());
	}*/
	void sendValidateName()
	{
		//EASTER EGGS LOL
		for(int i=0;i<MAX_EQUIP;i++)
		{
			if(mUnitManager->getPlayer()->getEquip(i)=="Egg")
			{
				mUnitManager->unequipItem(mUnitManager->getPlayer(),i);
				mUnitManager->equipItem(mUnitManager->getPlayer(),"collar3",i);
				mChatManager->message("Your Red Easter Egg has hatched into a Spiked Collar!");
				sendItemEquip("collar3",i);
			}
			else if(mUnitManager->getPlayer()->getEquip(i)=="Egg2")
			{
				mUnitManager->unequipItem(mUnitManager->getPlayer(),i);
				mUnitManager->equipItem(mUnitManager->getPlayer(),"bracelet1",i);
				mChatManager->message("Your Blue Easter Egg has hatched into a Checkered Bracelet!");
				sendItemEquip("bracelet1",i);
			}
			else if(mUnitManager->getPlayer()->getEquip(i)=="Egg3")
			{
				mUnitManager->unequipItem(mUnitManager->getPlayer(),i);
				mUnitManager->equipItem(mUnitManager->getPlayer(),"fangs",i);
				mChatManager->message("Your Green Easter Egg has hatched into a pair of Fangs!");
				sendItemEquip("fangs",i);
			}
			else if(mUnitManager->getPlayer()->getEquip(i)=="Egg4")
			{
				mUnitManager->unequipItem(mUnitManager->getPlayer(),i);
				mUnitManager->equipItem(mUnitManager->getPlayer(),"clawsL",i);
				const short tNextSlot = mUnitManager->getNextEmptyItemSlot(mUnitManager->getPlayer());
				if(tNextSlot>0)mUnitManager->equipItem(mUnitManager->getPlayer(),"clawsR",tNextSlot);
				mChatManager->message("Your Silver Easter Egg has hatched into two sets of claws!");
				sendItemEquip("clawsL",i);
				if(tNextSlot>0)sendItemEquip("clawsR",i-1);
			}
			else if(mUnitManager->getPlayer()->getEquip(i)=="Egg5")
			{
				mUnitManager->unequipItem(mUnitManager->getPlayer(),i);
				mUnitManager->equipItem(mUnitManager->getPlayer(),"note",i);
				mChatManager->message("Your Gold Easter Egg has hatched into...a piece of note?");
				sendItemEquip("note",i);
			}
		}

		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_VALIDATENAME));
		tBitStream.Write(OwnerToken(mUnitManager->getPlayer()->getIndex()));
		stringCompressor->EncodeString(mUnitManager->getPlayer()->getUser().c_str(),16,&tBitStream);
		stringCompressor->EncodeString(mWorld->getWorldName().c_str(),32,&tBitStream);
		//tBitStream.Write((unsigned short)mCharScreenManager->getCharID());
		tBitStream.Write(mGameStateManager->getDimension());

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processPlayerData(Packet *p)
	{
		playerListUpdated = true;

		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		bool tArrive;
		char tName[16];

		tBitStream.Read(tID);
		tBitStream.Read(tArrive);
		stringCompressor->DecodeString(tName,16,&tBitStream);
		if(tArrive)
		{
			char tMapName[32];
			stringCompressor->DecodeString(tMapName,32,&tBitStream);

			for(vector<const PlayerData>::type::iterator it=playerList.begin(); it!=playerList.end(); it++)
			{
				PlayerData *tData = &*it;
				if(tData->name == tName)
				{
					tData->mapName = tMapName;
					return;
				}
			}
			playerList.push_back(PlayerData(tName,tMapName));
			return;
		}
		else
		{
			for(vector<const PlayerData>::type::iterator it=playerList.begin(); it!=playerList.end(); it++)
			{
				PlayerData *tData = &*it;
				if(tData->name == tName)
				{
					playerList.erase(it);
					break;
				}
			}
			if(mUnitManager->removePartyMember(tName))mChatManager->message("Party Member "+String(tName)+" has disconnected.",CHAT_PARTY);
		}
	}
	bool popPlayerListUpdated()
	{
		const bool tFlag = playerListUpdated;
		playerListUpdated = false;
		return tFlag;
	}
	void processBio(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;
		bool tIsRequest;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);
		tBitStream.Read(tIsRequest);

		if(tIsRequest)
		{
			RakNet::BitStream tSendStream;

			tSendStream.Write(MessageID(ID_BIO));
			tSendStream.Write(tToken);
			tSendStream.Write(false);
			stringCompressor->EncodeString(mDef->loadBio().c_str(),512,&tSendStream);

			mPeer->Send(&tSendStream, LOW_PRIORITY, UNRELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
		}
		else
		{
			char tBio[512];
			stringCompressor->DecodeString(tBio,512,&tBitStream);

			targetBioUpdated = tBio;
			if(targetBioUpdated=="")targetBioUpdated = "None";
		}
	}
	void sendBioRequest(const String &name)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_BIO));
		tBitStream.Write(OwnerToken(TOKEN_NULL));
		tBitStream.Write(true);
		stringCompressor->EncodeString(name.c_str(),32,&tBitStream);

		mPeer->Send(&tBitStream, LOW_PRIORITY, UNRELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	const String popTargetBioUpdated()
	{
		const String tBio = targetBioUpdated;
		targetBioUpdated = "";
		return tBio;
	}
	void sendCreateAccount(const String &arg1, const String &arg2, const String &arg3, const String &arg4, const String &arg5)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CREATEACCOUNT));
		stringCompressor->EncodeString(arg1.c_str(),16,&tBitStream);
		stringCompressor->EncodeString(arg2.c_str(),16,&tBitStream);
		stringCompressor->EncodeString(arg3.c_str(),64,&tBitStream);
		stringCompressor->EncodeString(arg4.c_str(),128,&tBitStream);
		stringCompressor->EncodeString(arg5.c_str(),128,&tBitStream);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processCreateAccount(Packet *p)
	{
		if(!isMainServerConnection)return;
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		bool tSuccess;

		tBitStream.Read(tID);
		tBitStream.Read(tSuccess);
		networkMessage = NetworkMessage(tSuccess?NETWORK_CREATEACCOUNTSUCCESS:NETWORK_CREATEACCOUNTFAIL);
	}
	void sendEditAccount(const String &arg1, const String &arg2, const String &arg3)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_EDITACCOUNT));
		stringCompressor->EncodeString(arg1.c_str(),16,&tBitStream);
		stringCompressor->EncodeString(arg2.c_str(),16,&tBitStream);
		stringCompressor->EncodeString(arg3.c_str(),16,&tBitStream);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processEditAccount(Packet *p)
	{
		if(!isMainServerConnection)return;
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		bool tSuccess;

		tBitStream.Read(tID);
		tBitStream.Read(tSuccess);
		networkMessage = NetworkMessage(tSuccess?NETWORK_EDITACCOUNTSUCCESS:NETWORK_EDITACCOUNTFAIL);
	}
	void sendLogon(const String &arg1, const String &arg2)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_LOGON));
		stringCompressor->EncodeString(arg1.c_str(),16,&tBitStream);
		stringCompressor->EncodeString(arg2.c_str(),16,&tBitStream);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processLogon(Packet *p)
	{
		if(!isMainServerConnection)return;
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		bool tSuccess;

		tBitStream.Read(tID);
		tBitStream.Read(tSuccess);
		if(tSuccess)
		{
			short tNumChars = 0;
			tBitStream.Read(tNumChars);
			mCharScreenManager->setNumChars(tNumChars);
		}
		networkMessage = NetworkMessage(tSuccess?NETWORK_LOGONSUCCESS:NETWORK_LOGONFAIL);
	}
	const NetworkMessage popNetworkMessage()
	{
		const NetworkMessage tMessage = networkMessage;
		networkMessage = NetworkMessage();
		return tMessage;
	}
	void processLoadChar(Packet *p)
	{
		if(!isMainServerConnection)return;
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		short tIndex;
		bool tSuccess;

		tBitStream.Read(tID);
		tBitStream.Read(tIndex);
		if(tIndex!=mCharScreenManager->getCharID())return;

		tBitStream.Read(tSuccess);
		if(tSuccess)
		{
			char tBuffer[512] = "";
			bool tIsAdmin = false;
			bool tIsMod = false;
			stringCompressor->DecodeString(tBuffer,512,&tBitStream);
			tBitStream.Read(tIsAdmin);
			tBitStream.Read(tIsMod);
			for(int i=0;i<MAX_EQUIP;i++)mUnitManager->unequipItem(mUnitManager->getPlayer(),i);
			mCharScreenManager->updateChar(String(tBuffer));
			mDef->isAdmin = tIsAdmin;
			mDef->isMod = tIsMod;
			if(tIsAdmin)
			{
				char tAdminColour[16];
				stringCompressor->DecodeString(tAdminColour,16,&tBitStream);
				mDef->adminAuraColour = StringConverter::parseColourValue(String(tAdminColour));
				mUnitManager->createAdminAura(mUnitManager->getPlayer(),mDef->adminAuraColour);
			}
			if(tIsMod)mDef->isModOn = false;
			
			//Load Items
			for(int j=0;j<MAX_EQUIP;j++)
			{
				bool tHasItem = false;
				tBitStream.Read(tHasItem);
				if(tHasItem)
				{
					char tItem[16];
					stringCompressor->DecodeString(tItem,16,&tBitStream);
					mUnitManager->equipItem(mUnitManager->getPlayer(),String(tItem),j);
				}
			}

			//Load HP
			bool tHasHP = false;
			tBitStream.Read(tHasHP);
			if(tHasHP)
			{
				char tHP[16];
				stringCompressor->DecodeString(tHP,16,&tBitStream);
				const vector<String>::type tPart = StringUtil::split(tHP,";\n");
				const unsigned short tMaxHP = StringConverter::parseInt(tPart[1]);
				if(tPart.size()>1)mUnitManager->getPlayer()->setHP(tMaxHP==0?500:tMaxHP,0);
				if(tPart.size()>0)mUnitManager->getPlayer()->addHP(StringConverter::parseInt(tPart[0]));
			}
			else mUnitManager->getPlayer()->setHP(500);

			//Load Skills
			bool tHasSkills = false;
			tBitStream.Read(tHasSkills);
			if(tHasSkills)
			{
				char tSkills[512];
				stringCompressor->DecodeString(tSkills,512,&tBitStream);
				const vector<String>::type tLine = StringUtil::split(tSkills,"|\n");
				for(int i=0;i<(int)tLine.size();i++)
				{
					const vector<String>::type tPart = StringUtil::split(tLine[i],";");
					if(tPart.size()==2)
					{
						mUnitManager->getPlayer()->addSkill(tPart[0],StringConverter::parseInt(tPart[1]));
					}
				}
			}

			//Load Pet
			bool tHasPet = false;
			tBitStream.Read(tHasPet);
			if(tHasPet)
			{
				char tPet[512];
				stringCompressor->DecodeString(tPet,32,&tBitStream);
				mUnitManager->getPlayer()->setPet(tPet,mDef);
			}

			mUnitManager->getPlayer()->resetChanges();
		}
		else mCharScreenManager->updateChar("");
		mCharScreenManager->refreshPage();
		networkMessage = (tSuccess?NETWORK_LOADCHARSUCCESS:NETWORK_LOADCHARFAIL);
	}
	void sendLoadChar(const String &username, const short &charID)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_LOADCHAR));
		stringCompressor->EncodeString(username.c_str(),16,&tBitStream);
		tBitStream.Write(charID);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processCreateChar(Packet *p)
	{
		if(!isMainServerConnection)return;
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		bool tSuccess;

		tBitStream.Read(tID);
		tBitStream.Read(tSuccess);
		networkMessage = (tSuccess?NETWORK_CREATECHARSUCCESS:NETWORK_CREATECHARFAIL);
	}
	void sendCreateChar(const String &username, const String &name, const String &data)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CREATECHAR));
		stringCompressor->EncodeString(username.c_str(),16,&tBitStream);
		stringCompressor->EncodeString(name.c_str(),32,&tBitStream);
		stringCompressor->EncodeString(data.c_str(),512,&tBitStream);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendDeleteChar(const String &username, const short &charID)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_DELETECHAR));
		stringCompressor->EncodeString(username.c_str(),16,&tBitStream);
		tBitStream.Write(charID);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processEditChar(Packet *p)
	{
		if(!isMainServerConnection)return;
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		bool tSuccess;

		tBitStream.Read(tID);
		tBitStream.Read(tSuccess);
		networkMessage = (tSuccess?NETWORK_EDITCHARSUCCESS:NETWORK_EDITCHARFAIL);
	}
	void sendEditChar(const String &username, const short &charID, const String &data)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_EDITCHAR));
		stringCompressor->EncodeString(username.c_str(),16,&tBitStream);
		tBitStream.Write(charID);
		stringCompressor->EncodeString(data.c_str(),512,&tBitStream);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	bool processKick(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		char tName[16];
		bool tIsBanned = false;
		unsigned short tNumDays = 0;

		tBitStream.Read(tID);
		stringCompressor->DecodeString(tName,16,&tBitStream);
		tBitStream.Read(tIsBanned);
		if(tIsBanned)tBitStream.Read(tNumDays);

		String tPlayerName = mUnitManager->getPlayer()->getUser();
		StringUtil::toLowerCase(tPlayerName);
		String tCompareName = String(tName);
		StringUtil::toLowerCase(tCompareName);

		if(tPlayerName==tCompareName)
		{
			disconnect();
			mAlertBox->showAlert("Disconnected from server (Kicked by Admin)",0.5,0.5,10);
			mChatManager->message("Disconnected from server (Kicked by Admin)");
			if(tIsBanned)mDef->saveBanFile(true,tNumDays);
			return false;
		}
		return true;
	}
	void sendKick(const String &name, bool isBan=false, const unsigned short numDays=0)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_KICK));
		stringCompressor->EncodeString(name.c_str(),16,&tBitStream);
		if(isBan)
		{
			tBitStream.Write(isBan);
			tBitStream.Write(numDays);
		}

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, UNRELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processItemDrop(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		unsigned short tItemID;
		char tMesh[16];
		Real tX;
		Real tZ;
		bool tImTheSender = false;

		tBitStream.Read(tID);
		tBitStream.Read(tItemID);
		stringCompressor->DecodeString(tMesh,16,&tBitStream);
		tBitStream.Read(tX);
		tBitStream.Read(tZ);
		/*tBitStream.Read(tImTheSender);

		//i sent this, i'll tell server to unreserve this now
		if(tImTheSender)
		{
			sendAcknowledge(ACK_ITEMDROPPED,tItemID);
			//check that i'm still in the place where i sent this
			char tMap[32];
			unsigned char tDimension;
			stringCompressor->DecodeString(tMap,32,&tBitStream);
			tBitStream.Read(tDimension);
			if(String(tMap)!=mWorld->getWorldName() || tDimension!=mGameStateManager->getDimension())return;
		}*/
		mUnitManager->createItem(tItemID,String(tMesh),tX,tZ);
	}
	void sendItemDrop(const String &meshName, const Real &x, const Real &z)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_ITEMDROP));
		tBitStream.Write(OwnerToken(TOKEN_NULL));
		tBitStream.Write(mItemManager->getNextEmptyID(mUnitManager->getPlayer()->getIndex()*100));
		stringCompressor->EncodeString(meshName.c_str(),16,&tBitStream);
		tBitStream.Write(x);
		tBitStream.Write(z);

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 2, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendAllItems(const OwnerToken &token)
	{
		const vector<MagixItem*>::type tList = mItemManager->getItemList();
		for(int i=0;i<(int)tList.size();i++)
		{
			MagixItem *tItem = tList[i];
			RakNet::BitStream tBitStream;

			tBitStream.Write(MessageID(ID_ITEMDROP));
			tBitStream.Write(token);
			tBitStream.Write(tItem->getID());
			stringCompressor->EncodeString(tItem->getMesh().c_str(),16,&tBitStream);
			const Vector3 tPos = tItem->getPosition();
			tBitStream.Write(tPos.x);
			tBitStream.Write(tPos.z);

			mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 2, UNASSIGNED_SYSTEM_ADDRESS, true);
		}
	}
	void processItemPickup(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		unsigned short tItemID;
		bool tSuccess;

		tBitStream.Read(tID);
		tBitStream.Read(tItemID);
		tBitStream.Read(tSuccess);

		MagixItem *tItem = mItemManager->getByID(tItemID);
		if(!tItem)return;
		if(tSuccess)
		{
			//sendAcknowledge(ACK_ITEMPICKED,tItemID);
			const String tMeshName = tItem->getMesh();
			short tSlot = mUnitManager->getNextEmptyItemSlot(mUnitManager->getPlayer());
			if(tSlot!=-1)
			{
				mChatManager->message("Equipped "+mDef->getItemName(tMeshName)+".");
				mUnitManager->equipItem(mUnitManager->getPlayer(),tMeshName,tSlot);
				sendItemEquip(tMeshName,tSlot);
			}
		}
		mUnitManager->deleteItem(tItem);
	}
	void sendItemPickup(const unsigned short &iID)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_ITEMPICKUP));
		tBitStream.Write(iID);

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 2, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processItemEquip(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)return;

		char tMesh[16];
		unsigned short tSlotID;
		stringCompressor->DecodeString(tMesh,16,&tBitStream);
		tBitStream.Read(tSlotID);

		mUnitManager->equipItem(tUnit,String(tMesh),tSlotID);
	}
	void sendItemEquip(const String &meshName, const unsigned short &slotID, const OwnerToken &target=OwnerToken(TOKEN_NULL), bool dontTellMainServer=false)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_ITEMEQUIP));
		tBitStream.Write(target);
		stringCompressor->EncodeString(meshName.c_str(),16,&tBitStream);
		tBitStream.Write(slotID);
		tBitStream.Write(dontTellMainServer);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processItemUnequip(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;
		unsigned short tSlotID;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);
		tBitStream.Read(tSlotID);

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)return;

		mUnitManager->unequipItem(tUnit,tSlotID);
	}
	void sendItemUnequip(const unsigned short &slotID)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_ITEMUNEQUIP));
		tBitStream.Write(OwnerToken(TOKEN_NULL));
		tBitStream.Write(slotID);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 2, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendImReady()
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_IMREADY));

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processGodSpeak(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		char tCaption[512];
		
		tBitStream.Read(tID);
		stringCompressor->DecodeString(tCaption,512,&tBitStream);

		mChatManager->message(tCaption);
	}
	void sendGodSpeak(const String &caption)
	{
		const String tCaption = "<<(" USER_ADMIN_TEXT ")"+mUnitManager->getPlayer()->getUser()+">>"+caption;
		mChatManager->message(tCaption);

		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_GODSPEAK));
		stringCompressor->EncodeString(tCaption.c_str(),512,&tBitStream);

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	bool processForceLogout(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		char tName[16];

		tBitStream.Read(tID);
		stringCompressor->DecodeString(tName,16,&tBitStream);

		String tPlayerName = mUnitManager->getPlayer()->getUser();
		StringUtil::toLowerCase(tPlayerName);
		String tCompareName = String(tName);
		StringUtil::toLowerCase(tCompareName);

		if(tPlayerName==tCompareName)
		{
			disconnect();
			mAlertBox->showAlert("Disconnected by another user",0.5,0.5,10);
			mChatManager->message("Disconnected from server (Another user logged on)");
			return false;
		}
		return true;
	}
	void processEvent(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		unsigned char type;
		
		tBitStream.Read(tID);
		tBitStream.Read(type);

		if(type==EVENT_EARTHQUAKE)mSkyManager->setWeatherEffect("Earthquake");
		else if(type==EVENT_MASSBLOCK)
		{
			OwnerToken tToken;
			tBitStream.Read(tToken);
			MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
			if(!tUnit)return;
			networkMessage = NetworkMessage(NETWORK_BLOCKNAME,tUnit->getUser());
		}
	}
	void sendEvent(const unsigned char &type)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_EVENT));
		tBitStream.Write(type);

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendMassBlock(const OwnerToken &target)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_EVENT));
		tBitStream.Write(unsigned char(EVENT_MASSBLOCK));
		tBitStream.Write(target);

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processInfo(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		unsigned char type;
		OwnerToken tToken;
		
		tBitStream.Read(tID);
		tBitStream.Read(type);
		tBitStream.Read(tToken);

		//Fella says "Can i join party"
		if(type==INFO_PARTYREQUEST)
		{
			mUnitManager->setPartyInviter(tToken);
			networkMessage = NETWORK_PARTYREQUEST;
			return;
		}
		//Fella says "Wanna join party"
		else if(type==INFO_PARTYINVITE)
		{
			//Already have one, thanks
			if(mUnitManager->hasParty())
			{
				sendInfo(INFO_PARTYALREADY,tToken);
				return;
			}
			mUnitManager->setPartyInviter(tToken);
			networkMessage = NETWORK_PARTYREQUEST;
			return;
		}
		//Fella says "Sure i'll join party"
		else if(type==INFO_PARTYINVITEACCEPTED)
		{
			//Already full, too bad
			if(mUnitManager->isPartyFull())return;
			//Remember that this fella asked to join party
			mUnitManager->setPartyInviter(tToken);
			sendPartyAccepted(tToken);
			return;
		}
		else if(type==INFO_PARTYACCEPTED)
		{
			//Avoid double partying
			if(mUnitManager->hasParty())return;
			//Get party list
			for(int i=0;i<MAX_PARTYMEMBERS;i++)
			{
				bool tHasMore = false;
				OwnerToken tMemberToken;
				char tMemberName[16];
				tBitStream.Read(tHasMore);
				if(!tHasMore)break;
				tBitStream.Read(tMemberToken);
				stringCompressor->DecodeString(tMemberName,16,&tBitStream);
				mUnitManager->addPartyMember(tMemberToken,tMemberName);
				mChatManager->message(String(tMemberName)+" has joined the party.",CHAT_PARTY);
				//Tell this fella i joined the party
				sendPartyJoined(tMemberToken);
				//This fella invited me
				if(tMemberToken==mUnitManager->getPartyInviter())mUnitManager->setPartyInviter(0);
			}
			return;
		}
		else if(type==INFO_PARTYJOINED)
		{
			//Check if member already exists in party
			const std::pair<OwnerToken,String> tCheck = mUnitManager->getPartyMember(tToken);
			if(tCheck.first==tToken)return;
			//Allow new party member to join
			char tMemberName[16];
			stringCompressor->DecodeString(tMemberName,16,&tBitStream);

			bool tPartyOverload = mUnitManager->isPartyFull();
			mUnitManager->addPartyMember(tToken,tMemberName);
			mChatManager->message(String(tMemberName)+" has joined the party.",CHAT_PARTY);
			//This fella requested from me
			if(tToken==mUnitManager->getPartyInviter())
			{
				mUnitManager->setPartyInviter(0);
				//Party overloaded, kick new fella
				if(tPartyOverload)sendInfo(INFO_PARTYKICK,tToken);
			}
			return;
		}
		else if(type==INFO_PARTYKICK)
		{
			sendPartyLeave();
			return;
		}
		else if(type==INFO_PARTYLEAVE)
		{
			const std::pair<OwnerToken,String> tMember = mUnitManager->getPartyMember(tToken);
			if(tMember.first!=tToken)return;
			mUnitManager->removePartyMember(tMember.second);
			mChatManager->message(tMember.second+" has left the party.",CHAT_PARTY);
			return;
		}

		MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
		if(!tUnit)return;

		/*if(type==INFO_MODON)mUnitManager->createParticleOnUnit(tUnit,"RisingShards",-1,true);
		else if(type==INFO_MODOFF)mUnitManager->destroyParticleOnUnit(tUnit,"RisingShards");
		else*/ if(type==INFO_YOURBLOCKED)
		{
			tUnit->isBlocked = true;
			tUnit->isHidden = true;
		}
		else if(type==INFO_YOURUNBLOCKED)
		{
			if(mDef->isInBlockedList(tUnit->getUser()))
			{
				sendInfo(INFO_YOURBLOCKED,tToken);
			}
			else
			{
				tUnit->isBlocked = false;
				tUnit->isHidden = false;
			}
		}
		else if(type==INFO_PARTYALREADY)
		{
			mChatManager->message(tUnit->getUser()+" is already in a party.",CHAT_PARTY);
		}
	}
	void sendInfo(const unsigned char &type, const OwnerToken &target=OwnerToken(TOKEN_NULL))
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_INFO));
		tBitStream.Write(type);
		tBitStream.Write(target);

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendPartyAccepted(const OwnerToken &target)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_INFO));
		tBitStream.Write(unsigned char(INFO_PARTYACCEPTED));
		tBitStream.Write(target);
		const vector<const std::pair<OwnerToken,String>>::type tParty = mUnitManager->getPartyMembers();
		for(int i=0;i<(int)tParty.size();i++)
		{
			tBitStream.Write(true);
			tBitStream.Write(OwnerToken(tParty[i].first));
			stringCompressor->EncodeString(tParty[i].second.c_str(),16,&tBitStream);
		}
		tBitStream.Write(true);
		tBitStream.Write(OwnerToken(mUnitManager->getPlayer()->getIndex()));
		stringCompressor->EncodeString(mUnitManager->getPlayer()->getUser().c_str(),16,&tBitStream);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendPartyJoined(const OwnerToken &target, bool requestPartyList=false)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_INFO));
		tBitStream.Write(unsigned char(INFO_PARTYJOINED));
		tBitStream.Write(target);
		stringCompressor->EncodeString(mUnitManager->getPlayer()->getUser().c_str(),16,&tBitStream);
		tBitStream.Write(requestPartyList);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendPartyLeave()
	{
		const vector<const std::pair<OwnerToken,String>>::type tParty = mUnitManager->getPartyMembers();
		for(int i=0;i<(int)tParty.size();i++)sendInfo(INFO_PARTYLEAVE,tParty[i].first);
		mUnitManager->clearPartyMembers();
		mChatManager->message("You have left the party.");
	}
	/*void processDenied(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		unsigned char type;
		
		tBitStream.Read(tID);
		tBitStream.Read(type);
		
		//Resend itemdropped
		if(type==ACK_ITEMDROPPED)
		{
			char tMesh[16];
			Real tX;
			Real tZ;
			stringCompressor->DecodeString(tMesh,16,&tBitStream);
			tBitStream.Read(tX);
			tBitStream.Read(tZ);
			sendItemDrop(tMesh,tX,tZ);
			return;
		}
		//Resend create critter
		if(type==ID_CRITTEROWNAGE)
		{
			bool tIsCreate;
			tBitStream.Read(tIsCreate);
			if(tIsCreate)
			{
				OwnerToken tToken;
				bool tIsWorldCritter;
				unsigned char tTypeID;
				char tType[32];
				Vector3 tPosition;
				tBitStream.Read(tToken);
				tBitStream.Read(tIsWorldCritter);
				if(tIsWorldCritter)tBitStream.Read(tTypeID);
				else stringCompressor->DecodeString(tType,32,&tBitStream);
				tBitStream.Read(tPosition);
				if(tIsWorldCritter)sendCritterCreate(tTypeID,tPosition,true);
				else sendCritterCreate(tType,tPosition,true);
			}
			return;
		}
	}*/
	void sendAcknowledge(const unsigned char &type, const unsigned short &iID)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_ACKNOWLEDGE));
		tBitStream.Write(type);
		tBitStream.Write(iID);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	/*void sendSkillInitialize()
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_UPDATESKILLS));
		tBitStream.Write(true);
		const vector<Skill>::type tSkill = mUnitManager->getPlayer()->getSkills();
		for(int i=0;i<(int)tSkill.size();i++)
		{
			tBitStream.Write(true);
			stringCompressor->EncodeString(tSkill[i].name.c_str(),32,&tBitStream);
			tBitStream.Write((unsigned char)tSkill[i].stock);
		}

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}*/
	void sendSkillUpdate(const String &type, const unsigned char &stock)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_UPDATESKILLS));
		//tBitStream.Write(false);
		stringCompressor->EncodeString(type.c_str(),32,&tBitStream);
		tBitStream.Write(stock);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendHPUpdate(bool sendMaxHP=false)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_UPDATEHP));
		tBitStream.Write(unsigned short(mUnitManager->getPlayer()->getIsWounded()?0:mUnitManager->getPlayer()->getHP()));
		if(sendMaxHP)
		{
			tBitStream.Write(true);
			tBitStream.Write((unsigned short)mUnitManager->getPlayer()->getMaxHP());
		}
		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendPetUpdate()
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_UPDATEPET));
		stringCompressor->EncodeString(mUnitManager->getPlayer()->getPet().c_str(),32,&tBitStream);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendCritterCreate(const String &type, const Vector3 &position, bool isPet=false)
	{
		const unsigned short tID = mCritterManager->getNextEmptyID((mUnitManager->getPlayer()->getIndex()-1)*200);
		mCritterManager->pushSentID(tID);

		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTEROWNAGE));
		tBitStream.Write(tID);
		tBitStream.Write(true);
		tBitStream.Write((OwnerToken)mUnitManager->getPlayer()->getIndex());
		tBitStream.Write(false);
		stringCompressor->EncodeString(type.c_str(),32,&tBitStream);
		tBitStream.Write(position);
		//stringCompressor->EncodeString(mWorld->getWorldName().c_str(),32,&tBitStream);
		//tBitStream.Write(mGameStateManager->getDimension());
		if(isPet)tBitStream.Write(isPet);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendCritterCreate(const unsigned char &typeID, const Vector3 &position)
	{
		const unsigned short tID = mCritterManager->getNextEmptyID((mUnitManager->getPlayer()->getIndex()-1)*200);
		mCritterManager->pushSentID(tID);

		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTEROWNAGE));
		tBitStream.Write(tID);
		tBitStream.Write(true);
		tBitStream.Write((OwnerToken)mUnitManager->getPlayer()->getIndex());
		tBitStream.Write(true);
		tBitStream.Write(typeID);
		tBitStream.Write(position);
		//stringCompressor->EncodeString(mWorld->getWorldName().c_str(),32,&tBitStream);
		//tBitStream.Write(mGameStateManager->getDimension());

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void disownCritters(const OwnerToken &target)
	{
		const vector<MagixCritter*>::type tList = mCritterManager->getCritterList();
		const Vector3 tPlayerPos = mUnitManager->getPlayer()->getPosition();
		for(int i=0;i<(int)tList.size();i++)
		{
			MagixCritter *tC = tList[i];
			if(tC->getOwnerIndex()==target && !tC->getIsDead())
			{
				if(tC->getIsPet())mUnitManager->deleteCritter(tC->getID());
				else
				{
					//Check if player can be suiter
					const Real tDistance = tC->getPosition().squaredDistance(tPlayerPos);
					bool tOwn = false;
					//Fight for ownership
					if(tDistance<4000000)
					{
						tC->setOwnerIndex(-1);
						sendCritterTakeover(tC->getID(),OwnerToken(TOKEN_NULL),true);
					}
					else mUnitManager->deleteCritter(tC->getID());
				}
			}
		}
	}
	void sendCritterTakeover(const unsigned short &iID, const OwnerToken &token=OwnerToken(TOKEN_NULL), bool fastestFirst=false)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTEROWNAGE));
		tBitStream.Write(iID);
		tBitStream.Write(false);
		tBitStream.Write(true);
		tBitStream.Write( (token==TOKEN_NULL?(OwnerToken)mUnitManager->getPlayer()->getIndex():token) );
		tBitStream.Write(fastestFirst);
		if(token!=TOKEN_NULL)tBitStream.Write((OwnerToken)mUnitManager->getPlayer()->getIndex());

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processCritterOwnage(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		unsigned short tCritterID;
		bool tIsCreate = false;
		bool tIsTakeover = false;

		tBitStream.Read(tID);
		tBitStream.Read(tCritterID);

		//Create
		tBitStream.Read(tIsCreate);
		if(tIsCreate)
		{
			OwnerToken tToken;
			bool tIsWorldCritter = true;
			unsigned char tTypeID;
			char tType[32];
			Vector3 tPosition;
			//char tMap[32];
			//unsigned char tDimension;
			bool tIsPet = false;

			tBitStream.Read(tToken);

			if(tToken!=mUnitManager->getPlayer()->getIndex())
			{
				MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
				//No such owner
				if(!tUnit)return;
			}

			tBitStream.Read(tIsWorldCritter);
			if(tIsWorldCritter)tBitStream.Read(tTypeID);
			else stringCompressor->DecodeString(tType,32,&tBitStream);
			tBitStream.Read(tPosition);
			//stringCompressor->DecodeString(tMap,32,&tBitStream);
			//tBitStream.Read(tDimension);
			tBitStream.Read(tIsPet);
			
			//i sent this, i'll tell server to unreserve this now
			//if(tToken==mUnitManager->getPlayer()->getIndex())sendAcknowledge(ACK_CRITTERCREATED,tCritterID);
			if(tToken==mUnitManager->getPlayer()->getIndex())mCritterManager->popSentID(tCritterID);

			//if(String(tMap)!=mWorld->getWorldName() || tDimension!=mGameStateManager->getDimension())return;
			MagixCritter *tC = 0;
			if(tIsWorldCritter)tC = mUnitManager->createCritter(tCritterID,tTypeID,tPosition,(tToken==mUnitManager->getPlayer()->getIndex()?0:tToken));
			else tC = mUnitManager->createCritter(tCritterID,tType,tPosition,(tToken==mUnitManager->getPlayer()->getIndex()?0:tToken));
			if(tIsPet)mCritterManager->setIsPet(tC);

			//This is my pet
			if(tC && tIsPet && tC->imTheOwner())
			{
				PetFlags *tFlags = mUnitManager->getPlayer()->getPetFlags();
				if(tFlags->shrink)sendCritterShrink(tC->getID(),true);
				if(tFlags->evolve)sendCritterShrink(tC->getID(),true,true);
			}
			return;
		}
		//Takeover
		tBitStream.Read(tIsTakeover);
		if(tIsTakeover)
		{
			OwnerToken tToken;
			bool tFastestFirst = false;
			tBitStream.Read(tToken);
			tBitStream.Read(tFastestFirst);
			
			if(tToken!=mUnitManager->getPlayer()->getIndex())
			{
				MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tToken);
				//No such owner
				if(!tUnit)return;
			}
			
			//i sent this, i'll tell server to unreserve this now
			//if(tToken==mUnitManager->getPlayer()->getIndex())sendAcknowledge(ACK_CRITTERCREATED,tCritterID);

			MagixCritter *tC = mCritterManager->getByID(tCritterID);
			if(!tC)
			{
				OwnerToken tSender = tToken;
				tBitStream.Read(tSender);
				//That critter doesn't exist for me, please update me on it
				sendCritterUpdateRequest(tSender,tCritterID);
				return;
			}

			//Critter must be unowned for fastest first
			if(tFastestFirst && tC->getOwnerIndex()>=0)return;

			//No taking over of pets
			if(tC->getIsPet())return;

			if(tToken==mUnitManager->getPlayer()->getIndex())
			{
				tC->setOwnerIndex(0);
				mCritterManager->removeFromMyCritters(tC);
				mCritterManager->pushMyCritters(tC);
			}
			else
			{
				tC->setOwnerIndex(tToken);
				mCritterManager->removeFromMyCritters(tC);
			}
			return;
		}
	}
	void sendCritterHit(const unsigned short &iID, const Real &damage, const Vector3 &force)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTERINFOONEWAY));
		tBitStream.Write(iID);
		tBitStream.Write(true);
		tBitStream.Write(OwnerToken(mUnitManager->getPlayer()->getIndex()));
		tBitStream.Write(damage);
		const bool tNoForce = (force==Vector3::ZERO);
		tBitStream.Write(tNoForce);
		if(!tNoForce)tBitStream.Write(force);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendCritterDecision(const unsigned short &iID, const Vector3 &target)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTERINFOONEWAY));
		tBitStream.Write(iID);
		tBitStream.Write(false);
		tBitStream.Write(true);
		tBitStream.Write(false);
		tBitStream.Write(target);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendCritterAttackDecision(const unsigned short &iID, const unsigned short &targetID, const unsigned char &attackID, bool targetIsCritter=false)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTERINFOONEWAY));
		tBitStream.Write(iID);
		tBitStream.Write(false);
		tBitStream.Write(true);
		tBitStream.Write(true);
		tBitStream.Write(targetIsCritter);
		if(targetIsCritter)tBitStream.Write(targetID);
		else tBitStream.Write((OwnerToken)targetID);
		tBitStream.Write(attackID);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendCritterKilled(const unsigned short &iID, const short &killer)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTERINFO));
		tBitStream.Write(iID);
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(true);
		const OwnerToken tToken = killer==-1?TOKEN_NULL: (killer==0?mUnitManager->getPlayer()->getIndex():killer);
		tBitStream.Write(tToken);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendCritterTamed(const unsigned short &iID)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTERINFO));
		tBitStream.Write(iID);
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(true);
		tBitStream.Write((OwnerToken)mUnitManager->getPlayer()->getIndex());

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendCritterShrink(const unsigned short &iID, bool flag, bool isEvolve=false)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTERINFO));
		tBitStream.Write(iID);
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(true);
		tBitStream.Write(flag);
		if(flag && isEvolve)tBitStream.Write(true);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	/*void sendCritterDisown(const unsigned short &iID)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTERINFO));
		tBitStream.Write(iID);
		tBitStream.Write(OwnerToken(mUnitManager->getPlayer()->getIndex()));
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(true);

		mPeer->Send(&tBitStream, MEDIUM_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}*/
	void processCritterInfo(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		unsigned short tCritterID;
		bool tIsHit = false;
		bool tIsDecision = false;
		bool tIsKilled = false;
		bool tIsTamed = false;
		bool tIsShrunk = false;

		tBitStream.Read(tID);
		tBitStream.Read(tCritterID);

		//Hit
		tBitStream.Read(tIsHit);
		if(tIsHit)
		{
			OwnerToken tToken;
			Real tDamage;
			bool tNoForce = true;
			Vector3 tForce = Vector3::ZERO;

			tBitStream.Read(tToken);
			tBitStream.Read(tDamage);
			tBitStream.Read(tNoForce);
			if(!tNoForce)tBitStream.Read(tForce);

			mCritterManager->hitCritter(tCritterID,tDamage,tForce,tToken);
			return;
		}
		//Decision
		tBitStream.Read(tIsDecision);
		if(tIsDecision)
		{
			MagixCritter *tCritter = mCritterManager->getByID(tCritterID);
			if(!tCritter)return;

			bool tIsAttack = false;
			tBitStream.Read(tIsAttack);
			//It's an attack!
			if(tIsAttack)
			{
				bool tTargetIsCritter;
				tBitStream.Read(tTargetIsCritter);
				MagixLiving *tTarget = 0;
				if(tTargetIsCritter)
				{
					unsigned short tTargetID;
					tBitStream.Read(tTargetID);
					tTarget = mCritterManager->getByID(tTargetID);
				}
				else
				{
					OwnerToken tTargetID;
					tBitStream.Read(tTargetID);
					if(tTargetID==mUnitManager->getPlayer()->getIndex())tTarget = mUnitManager->getPlayer();
					else tTarget = mUnitManager->getUnitByIndex(tTargetID);
				}
				if(!tTarget)return;
				unsigned char tAttackID;
				tBitStream.Read(tAttackID);

				tCritter->setTarget(tTarget,tAttackID);
			}
			//Just roaming
			else
			{
				Vector3 tTarget;
				tBitStream.Read(tTarget);

				tCritter->setTarget(tTarget);
			}
			return;
		}
		//Killed
		tBitStream.Read(tIsKilled);
		if(tIsKilled)
		{
			OwnerToken tToken;
			tBitStream.Read(tToken);

			MagixCritter *tCritter = mCritterManager->getByID(tCritterID);
			if(!tCritter)return;
			mUnitManager->killAndRewardCritter(tCritter,(tToken==mUnitManager->getPlayer()->getIndex()));
			return;
		}
		//Tamed
		tBitStream.Read(tIsTamed);
		if(tIsTamed)
		{
			MagixCritter *tCritter = mCritterManager->getByID(tCritterID);
			if(!tCritter)return;
			OwnerToken tToken;
			tBitStream.Read(tToken);
			const bool tImTheOwner = tToken==mUnitManager->getPlayer()->getIndex();
			
			if(tCritter->getIsPet())return;
			mCritterManager->setIsPet(tCritter);
			tCritter->setOwnerIndex(tImTheOwner?0:tToken);
			tCritter->setLastHitIndex(-1);
			tCritter->setDecisionTimer(0);
			mCritterManager->removeFromMyCritters(tCritter);
			if(tImTheOwner)
			{
				mCritterManager->pushMyCritters(tCritter);
				const String tType = tCritter->getCritterType();
				const bool tVowel = (tType.length()>0 && (tType[0]=='A'||tType[0]=='E'||tType[0]=='I'||tType[0]=='O'||tType[0]=='U'));
				mChatManager->message("You have tamed "+(tVowel?String("an "):String("a "))+tType+".");
				if(mUnitManager->getPlayer()->getPet()!="")mChatManager->message("You have tamed more than one pet, only the newest one will follow you.");
				mUnitManager->getPlayer()->setPet(tType,mDef);
				sendPetUpdate();
			}
			return;
		}
		//Shrink
		tBitStream.Read(tIsShrunk);
		if(tIsShrunk)
		{
			bool tIsTrue;
			tBitStream.Read(tIsTrue);
			bool tIsEvolve = false;
			tBitStream.Read(tIsEvolve);

			MagixCritter *tCritter = mCritterManager->getByID(tCritterID);
			if(!tCritter)return;
			if(tIsTrue)
			{
				if(tIsEvolve)tCritter->getObjectNode()->setScale(15,15,15);
				else
				{
					const Real tScale = tCritter->getLength()>50?0.1: (1 - tCritter->getLength()*0.018);
					tCritter->getObjectNode()->setScale(tScale,tScale,tScale);
				}
			}
			else
			{
				const Critter tC = mDef->getCritter(tCritter->getCritterType());
				const Real tScale = (tCritter->getIsPet()&&tC.scale>1.5)?1.5:tC.scale;
				tCritter->getObjectNode()->setScale(tScale,tScale,tScale);
			}
			return;
		}
		/*//Disown
		tBitStream.Read(tIsDisown);
		if(tIsDisown)
		{
			MagixCritter *tCritter = mCritterManager->getByID(tCritterID);
			if(!tCritter)return;
			tCritter->setOwnerIndex(-1);
			if(tToken==mUnitManager->getPlayer()->getIndex())mCritterManager->removeFromMyCritters(tCritter);
			return;
		}*/
	}
	void sendMyCritters(const OwnerToken &target)
	{
		const vector<MagixCritter*>::type tList = mCritterManager->getMyCritters();
		MagixCritter *tPet = 0;
		for(int i=0;i<(int)tList.size();i++)
		{
			MagixCritter *tC = tList[i];
			if(!tC->getIsDead())
			{
				if(tC->getIsWorldCritter())sendCritterUpdate(target,tC->getID(),tC->getWorldCritterID(),tC->getPosition(),tC->getHPRatio());
				else
				{
					sendCritterUpdate(target,tC->getID(),tC->getCritterType(),tC->getPosition(),tC->getHPRatio(),tC->getIsPet());
					if(tC->getIsPet())tPet = tC;
				}
			}
		}
		if(tPet)
		{
			PetFlags *tFlags = mUnitManager->getPlayer()->getPetFlags();
			if(tFlags->shrink)sendCritterShrinkTargeted(target,tPet->getID());
			if(tFlags->evolve)sendCritterShrinkTargeted(target,tPet->getID(),true);
		}
	}
	void sendCritterUpdate(const OwnerToken &target, const unsigned short &iID, const unsigned char &typeID, const Vector3 &position, const Real &hpRatio, const OwnerToken &token=OwnerToken(TOKEN_NULL))
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTERTARGETEDINFO));
		tBitStream.Write(iID);
		tBitStream.Write(target);
		tBitStream.Write(true);
		tBitStream.Write(true);
		tBitStream.Write(typeID);
		tBitStream.Write(position);
		tBitStream.Write(hpRatio);
		if(token!=TOKEN_NULL)tBitStream.Write(token);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendCritterUpdate(const OwnerToken &target, const unsigned short &iID, const String &type, const Vector3 &position, const Real &hpRatio, bool isPet=false, const OwnerToken &token=OwnerToken(TOKEN_NULL))
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTERTARGETEDINFO));
		tBitStream.Write(iID);
		tBitStream.Write(target);
		tBitStream.Write(true);
		tBitStream.Write(false);
		stringCompressor->EncodeString(type.c_str(),32,&tBitStream);
		tBitStream.Write(position);
		tBitStream.Write(hpRatio);
		tBitStream.Write(isPet);
		if(token!=TOKEN_NULL)tBitStream.Write(token);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendCritterUpdateRequest(const OwnerToken &target, const unsigned short &iID)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTERTARGETEDINFO));
		tBitStream.Write(iID);
		tBitStream.Write(target);
		tBitStream.Write(false);
		tBitStream.Write(true);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendCritterShrinkTargeted(const OwnerToken &target, const unsigned short &iID, bool isEvolve=false)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_CRITTERTARGETEDINFO));
		tBitStream.Write(iID);
		tBitStream.Write(target);
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(true);
		if(isEvolve)tBitStream.Write(true);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 4, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processCritterTargetedInfo(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		unsigned short tCritterID;
		OwnerToken tToken;
		bool tIsUpdate = false;
		bool tIsUpdateRequest = false;
		bool tIsShrunk = false;

		tBitStream.Read(tID);
		tBitStream.Read(tCritterID);
		tBitStream.Read(tToken);

		//Update
		tBitStream.Read(tIsUpdate);
		if(tIsUpdate)
		{
			bool tIsWorldCritter = true;
			unsigned char tTypeID;
			char tType[32];
			Vector3 tPosition;
			Real tHPRatio;
			bool tIsPet = false;
			OwnerToken tOwner = 0;

			tBitStream.Read(tIsWorldCritter);
			if(tIsWorldCritter)tBitStream.Read(tTypeID);
			else stringCompressor->DecodeString(tType,32,&tBitStream);
			tBitStream.Read(tPosition);
			tBitStream.Read(tHPRatio);

			MagixCritter *tCritter = 0;
			if(tIsWorldCritter)tCritter = mUnitManager->createCritter(tCritterID,tTypeID,tPosition,tToken);
			else
			{
				tCritter = mUnitManager->createCritter(tCritterID,tType,tPosition,tToken);
				tBitStream.Read(tIsPet);
			}
			tBitStream.Read(tOwner);
			if(tCritter)
			{
				tCritter->setHPRatio(tHPRatio);
				if(tIsPet)mCritterManager->setIsPet(tCritter);
				if(tOwner!=0)
				{
					if(tOwner==mUnitManager->getPlayer()->getIndex())
					{
						tCritter->setOwnerIndex(0);
						mCritterManager->pushMyCritters(tCritter);
					}
					else tCritter->setOwnerIndex(tOwner);
				}
			}
			return;
		}
		//Update request
		tBitStream.Read(tIsUpdateRequest);
		if(tIsUpdateRequest)
		{
			MagixCritter *tCritter = mCritterManager->getByID(tCritterID);
			if(!tCritter)return;
			const short tOwner = tCritter->getOwnerIndex();
			if(tCritter->getIsWorldCritter())
				sendCritterUpdate(tToken,tCritterID,tCritter->getWorldCritterID(),tCritter->getPosition(),tCritter->getHPRatio(),tOwner>0?tOwner:OwnerToken(TOKEN_NULL));
			else
				sendCritterUpdate(tToken,tCritterID,tCritter->getCritterType(),tCritter->getPosition(),tCritter->getHPRatio(),tCritter->getIsPet(),tOwner>0?tOwner:OwnerToken(TOKEN_NULL));
			return;
		}
		//Shrink
		tBitStream.Read(tIsShrunk);
		if(tIsShrunk)
		{
			bool tIsEvolve = false;
			tBitStream.Read(tIsEvolve);

			MagixCritter *tCritter = mCritterManager->getByID(tCritterID);
			if(!tCritter)return;
			if(tIsEvolve)tCritter->getObjectNode()->setScale(15,15,15);
			else tCritter->getObjectNode()->setScale(0.1,0.1,0.1);
			return;
		}
	}
	void sendPlayerUpdate(const OwnerToken &target=OwnerToken(TOKEN_NULL))
	{
		RakNet::BitStream tBitStream;

		if(target==OwnerToken(TOKEN_NULL))
		{
			tBitStream.Write(MessageID(ID_PLAYERINFO));
			tBitStream.Write((OwnerToken)mUnitManager->getPlayer()->getIndex());
		}
		else
		{
			tBitStream.Write(MessageID(ID_PLAYERTARGETEDINFO));
			tBitStream.Write(target);
		}
		tBitStream.Write(true);
		tBitStream.Write((unsigned short)mUnitManager->getPlayer()->getMaxHP());
		const bool tWounded = mUnitManager->getPlayer()->getIsWounded();
		tBitStream.Write(tWounded);
		if(!tWounded)tBitStream.Write((unsigned short)mUnitManager->getPlayer()->getHP());

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendPlayerHit(const OwnerToken &unitID, const Real &damage, const Vector3 &force)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_PLAYERINFO));
		tBitStream.Write((OwnerToken)mUnitManager->getPlayer()->getIndex());
		tBitStream.Write(false);
		tBitStream.Write(true);
		tBitStream.Write(unitID);
		tBitStream.Write(damage);
		const bool tNoForce = force==Vector3::ZERO;
		tBitStream.Write(tNoForce);
		if(!tNoForce)tBitStream.Write(force);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendPlayerWounded()
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_PLAYERINFO));
		tBitStream.Write((OwnerToken)mUnitManager->getPlayer()->getIndex());
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(true);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendPlayerUnwounded()
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_PLAYERINFO));
		tBitStream.Write((OwnerToken)mUnitManager->getPlayer()->getIndex());
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(false);
		tBitStream.Write(true);

		mPeer->Send(&tBitStream, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processPlayerInfo(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		OwnerToken tToken;
		bool tIsUpdate = false;
		bool tIsHit = false;
		bool tIsWounded = false;
		bool tIsUnwounded = false;

		tBitStream.Read(tID);
		tBitStream.Read(tToken);

		//Update
		tBitStream.Read(tIsUpdate);
		if(tIsUpdate)
		{
			MagixIndexedUnit *unit = mUnitManager->getUnitByIndex(tToken);
			if(!unit)return;

			unsigned short tMaxHP;
			bool tIsWounded = false;
			unsigned short tHP;

			tBitStream.Read(tMaxHP);
			unit->setHP(tMaxHP,0);

			tBitStream.Read(tIsWounded);
			if(!tIsWounded)
			{
				tBitStream.Read(tHP);
				unit->addHP(tHP);
			}
			else
			{
				unit->isWounded = true;
			}
			return;
		}
		//Hit
		tBitStream.Read(tIsHit);
		if(tIsHit)
		{
			if(tToken==mUnitManager->getPlayer()->getIndex())return;
			OwnerToken tUnitID;
			Real tDamage;
			bool tNoForce = true;
			Vector3 tForce = Vector3::ZERO;

			tBitStream.Read(tUnitID);
			tBitStream.Read(tDamage);
			tBitStream.Read(tNoForce);
			if(!tNoForce)tBitStream.Read(tForce);

			mUnitManager->hitUnit(tUnitID,tDamage,tForce);
			return;
		}
		//Wounded
		tBitStream.Read(tIsWounded);
		if(tIsWounded)
		{
			MagixIndexedUnit *unit = mUnitManager->getUnitByIndex(tToken);
			if(!unit)return;
			unit->isWounded = true;
			unit->setHP(0);
			unit->cancelAction(5.5,"Faint");
			if(mUnitManager->isPartyMember(unit))mChatManager->message(unit->getName()+" is wounded!",CHAT_PARTY);
		}
		//Unwounded
		tBitStream.Read(tIsUnwounded);
		if(tIsUnwounded)
		{
			MagixIndexedUnit *unit = mUnitManager->getUnitByIndex(tToken);
			if(!unit)return;
			unit->isWounded = false;
			unit->setHPRatio(0.05);
			if(mUnitManager->isPartyMember(unit))mChatManager->message(unit->getName()+" recovered from the wound.",CHAT_PARTY);
		}
	}
	void sendItemStash(const unsigned short &slot)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_ITEMSTASH));
		tBitStream.Write(true);
		tBitStream.Write(slot);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 6, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendItemUnstash(const unsigned short &line, const unsigned short &slot)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_ITEMSTASH));
		tBitStream.Write(false);
		tBitStream.Write(line);
		tBitStream.Write(slot);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 6, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processItemStash(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		bool tIsStash = false;

		tBitStream.Read(tID);
		tBitStream.Read(tIsStash);

		if(tIsStash)
		{
			bool tIsInit = false;
			tBitStream.Read(tIsInit);
			if(tIsInit)
			{
				mItemManager->clearStash();

				bool tHasMore = false;
				tBitStream.Read(tHasMore);
				while(tHasMore)
				{
					tHasMore = false;
					char tMesh[16];

					stringCompressor->DecodeString(tMesh,16,&tBitStream);
					mItemManager->pushStash(tMesh);

					tBitStream.Read(tHasMore);
				}
			}
			else
			{
				unsigned short tSlot;
				tBitStream.Read(tSlot);

				const String tEquip = mUnitManager->unequipItem(mUnitManager->getPlayer(),tSlot);
				mItemManager->pushStash(tEquip);
				mChatManager->message("Moved "+mDef->getItemName(tEquip)+" into stash.");
			}
		}
		//Unstash
		else
		{
			unsigned short tLine;
			unsigned short tSlot;
			tBitStream.Read(tLine);
			tBitStream.Read(tSlot);

			const String tEquip = mItemManager->popStash(tLine);

			if(tEquip!="" && mUnitManager->getPlayer()->getEquip(tSlot)=="")
			{
				mChatManager->message("Equipped "+mDef->getItemName(tEquip)+".");
				mUnitManager->equipItem(mUnitManager->getPlayer(),tEquip,tSlot);
			}
		}
	}
	void processFindPlayer(Packet *p)
	{
		RakNet::BitStream tBitStream(p->data, p->length, false);
		MessageID tID;
		char tName[16];
		bool tOnline = false;
		
		tBitStream.Read(tID);
		stringCompressor->DecodeString(tName,16,&tBitStream);
		tBitStream.Read(tOnline);
		if(tOnline)
		{
			
			OwnerToken tToken;
			char tMap[32];
			unsigned char tDimension;

			tBitStream.Read(tToken);
			stringCompressor->DecodeString(tMap,32,&tBitStream);
			tBitStream.Read(tDimension);
			
			mChatManager->message(String(tName)+"["+StringConverter::toString(tToken)+"] is in "+String(tMap)+" dimension "+StringConverter::toString(tDimension)+".");
		}
		else mChatManager->message(String(tName)+" is not online.");
	}
	void sendFindPlayer(const String &name)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_FINDPLAYER));
		tBitStream.Write(false);
		stringCompressor->EncodeString(name.c_str(),16,&tBitStream);

		mPeer->Send(&tBitStream, LOW_PRIORITY, UNRELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void sendFindToken(const OwnerToken &token)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_FINDPLAYER));
		tBitStream.Write(true);
		tBitStream.Write(token);

		mPeer->Send(&tBitStream, LOW_PRIORITY, UNRELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	void processServerList(Packet *p)
	{
		RakNet::BitStream tReceiveBit(p->data, p->length, false);
		MessageID tMessage;
		
		tReceiveBit.Read(tMessage);

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

			if(!tFull)serverAdd.push_back(std::pair<unsigned char,SystemAddress>(tID,tAdd));
			tReceiveBit.Read(tHasMore);
		}
	}
	void processServerConnected(Packet *p)
	{
		RakNet::BitStream tReceiveBit(p->data, p->length, false);
		MessageID tMessage;
		bool tIsConnect;
		unsigned char tID;
		SystemAddress tAdd;
		tReceiveBit.Read(tMessage);
		tReceiveBit.Read(tIsConnect);
		if(tIsConnect)tReceiveBit.Read(tID);
		tReceiveBit.Read(tAdd);

		if(tIsConnect)
		{
			serverAdd.push_back(std::pair<unsigned char,SystemAddress>(tID,tAdd));
		}
		else
		{
			for(vector<const std::pair<unsigned char,SystemAddress>>::type::iterator it=serverAdd.begin();it!=serverAdd.end();it++)
			{
				const std::pair<unsigned char,SystemAddress> tServer = *it;
				if(tServer.second==tAdd)
				{
					serverAdd.erase(it);
					break;
				}
			}
		}
	}
	void processMaintenance(Packet *p)
	{
		RakNet::BitStream tReceiveBit(p->data, p->length, false);
		MessageID tMessage;
		unsigned char tCount;
		tReceiveBit.Read(tMessage);
		tReceiveBit.Read(tCount);
		mChatManager->message("Servers are restarting in "+StringConverter::toString((int)tCount)+" seconds. Please log out to avoid data loss. Thank you C:");
	}
	void sendViolation(const String &info)
	{
		RakNet::BitStream tBitStream;

		tBitStream.Write(MessageID(ID_VIOLATION));
		stringCompressor->EncodeString(info.c_str(),32,&tBitStream);

		mPeer->Send(&tBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}

};
#endif