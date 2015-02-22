#include "Ogre.h"

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
	Ogre::String param;
	NetworkMessage(const unsigned short &m = NETWORK_NOTHING, const Ogre::String &p = "")
	{
		message = m;
		param = p;
	}
	/*NetworkMessage(unsigned short m = NETWORK_NOTHING, const Ogre::String p = "")
	{
		message = m;
		param = p;
	}*/
	/*NetworkMessage(NETWORKMESSAGE m = NETWORK_NOTHING, const Ogre::String p = "")
	{
		message = m;
		param = p;
	}
	NetworkMessage()
	{
		message = NETWORK_NOTHING;
		param = "";
	}*/
};

#pragma once

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "StringCompressor.h"
#include "GameConfig.h"
#include "MagixNetworkDefines.h"
#include "MagixCritterManager.h"
#include "MagixChatManager.h"
#include "MagixAlertBox.h"
#include "MagixCharScreenManager.h"

using namespace RakNet;
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
	MagixNetworkManager();
	~MagixNetworkManager();
	void initialize(MagixExternalDefinitions *def, MagixGameStateManager *gamestateMgr, MagixItemManager *itemMgr, MagixCritterManager *critterMgr, 
		MagixUnitManager *unitMgr, MagixWorld *world, MagixSkyManager *skyMgr, MagixChatManager *chatMgr, MagixAlertBox *alertBox, MagixCharScreenManager *charscreenMgr);
	bool connect();
	void connectMainServer();
	void disconnect();
	void reset();
	void update(const FrameEvent evt);
	const MessageID getPacketIdentifier(Packet *p);
	const vector<const String>::type getPlayersOnline();
	bool isPlayerOnline(String name);
	const String getPlayerMap(String name);
	void processPlayerToken(Packet *p);
	void sendPlayerTokenRequest();
	void processChat(Packet *p);
	void sendChat(const String &caption, const unsigned char &type, const String &pmTarget = "", bool autoLipSync = false);
	void sendPartyChat(const OwnerToken &target, const String &caption, const unsigned char &type, bool autoLipSync = false);
	void processNewPlayer(Packet *p);
	void sendNewPlayer(const OwnerToken &target = OwnerToken(TOKEN_NULL));
	void processPosition(Packet *p);
	void sendPosition(const OwnerToken &target = OwnerToken(TOKEN_NULL));
	void processMovePosition(Packet *p);
	void sendMovePosition();
	void processLookQuaternion(Packet *p);
	void sendLookQuaternion(const OwnerToken &target = OwnerToken(TOKEN_NULL));
	void processFaceDirection(Packet *p);
	void sendFaceDirection(const OwnerToken &target = OwnerToken(TOKEN_NULL));
	void processActionFlags(Packet *p);
	void sendActionFlags(const OwnerToken &target = OwnerToken(TOKEN_NULL));
	void processEmote(Packet *p);
	void sendEmote(const String &emote);
	void processAndSendNewPlayerUpdate(Packet *p);
	void sendNewPlayerProcedure(const OwnerToken &target = OwnerToken(TOKEN_NULL));
	void sendNewPlayerAloneProcedure();
	void processPlayerDisconnected(Packet *p);
	void processWorldInfo(Packet *p);
	void processAndSendMapInfo(Packet *p);
	void processAttack(Packet *p);
	void sendAttack(const String &attack);
	void processMapChange(Packet *p);
	void sendMapChange(const String &mapName, const unsigned char &dimension);
	void sendValidateName();
	void processPlayerData(Packet *p);
	bool popPlayerListUpdated();
	void processBio(Packet *p);
	void sendBioRequest(const String &name);
	const String popTargetBioUpdated();
	void sendCreateAccount(const String &arg1, const String &arg2, const String &arg3, const String &arg4, const String &arg5);
	void processCreateAccount(Packet *p);
	void sendEditAccount(const String &arg1, const String &arg2, const String &arg3);
	void processEditAccount(Packet *p);
	void sendLogon(const String &arg1, const String &arg2);
	void processLogon(Packet *p);
	const NetworkMessage popNetworkMessage();
	void processLoadChar(Packet *p);
	void sendLoadChar(const String &username, const short &charID);
	void processCreateChar(Packet *p);
	void sendCreateChar(const String &username, const String &name, const String &data);
	void sendDeleteChar(const String &username, const short &charID);
	void processEditChar(Packet *p);
	void sendEditChar(const String &username, const short &charID, const String &data);
	bool processKick(Packet *p);
	void sendKick(const String &name, bool isBan = false, const unsigned short numDays = 0);
	void processItemDrop(Packet *p);
	void sendItemDrop(const String &meshName, const Real &x, const Real &z);
	void sendAllItems(const OwnerToken &token);
	void processItemPickup(Packet *p);
	void sendItemPickup(const unsigned short &iID);
	void processItemEquip(Packet *p);
	void sendItemEquip(const String &meshName, const unsigned short &slotID, const OwnerToken &target = OwnerToken(TOKEN_NULL), bool dontTellMainServer = false);
	void processItemUnequip(Packet *p);
	void sendItemUnequip(const unsigned short &slotID);
	void sendImReady();
	void processGodSpeak(Packet *p);
	void sendGodSpeak(const String &caption);
	bool processForceLogout(Packet *p);
	void processEvent(Packet *p);
	void sendEvent(const unsigned char &type);
	void sendMassBlock(const OwnerToken &target);
	void processInfo(Packet *p);
	void sendInfo(const unsigned char &type, const OwnerToken &target = OwnerToken(TOKEN_NULL));
	void sendPartyAccepted(const OwnerToken &target);
	void sendPartyJoined(const OwnerToken &target, bool requestPartyList = false);
	void sendPartyLeave();
	void sendAcknowledge(const unsigned char &type, const unsigned short &iID);
	void sendSkillUpdate(const String &type, const unsigned char &stock);
	void sendHPUpdate(bool sendMaxHP = false);
	void sendPetUpdate();
	void sendCritterCreate(const String &type, const Vector3 &position, bool isPet = false);
	void sendCritterCreate(const unsigned char &typeID, const Vector3 &position);
	void disownCritters(const OwnerToken &target);
	void sendCritterTakeover(const unsigned short &iID, const OwnerToken &token = OwnerToken(TOKEN_NULL), bool fastestFirst = false);
	void processCritterOwnage(Packet *p);
	void sendCritterHit(const unsigned short &iID, const Real &damage, const Vector3 &force);
	void sendCritterDecision(const unsigned short &iID, const Vector3 &target);
	void sendCritterAttackDecision(const unsigned short &iID, const unsigned short &targetID, const unsigned char &attackID, bool targetIsCritter = false);
	void sendCritterKilled(const unsigned short &iID, const short &killer);
	void sendCritterTamed(const unsigned short &iID);
	void sendCritterShrink(const unsigned short &iID, bool flag, bool isEvolve = false);
	void processCritterInfo(Packet *p);
	void sendMyCritters(const OwnerToken &target);
	void sendCritterUpdate(const OwnerToken &target, const unsigned short &iID, const unsigned char &typeID, const Vector3 &position, const Real &hpRatio, const OwnerToken &token = OwnerToken(TOKEN_NULL));
	void sendCritterUpdate(const OwnerToken &target, const unsigned short &iID, const String &type, const Vector3 &position, const Real &hpRatio, bool isPet = false, const OwnerToken &token = OwnerToken(TOKEN_NULL));
	void sendCritterUpdateRequest(const OwnerToken &target, const unsigned short &iID);
	void sendCritterShrinkTargeted(const OwnerToken &target, const unsigned short &iID, bool isEvolve = false);
	void processCritterTargetedInfo(Packet *p);
	void sendPlayerUpdate(const OwnerToken &target = OwnerToken(TOKEN_NULL));
	void sendPlayerHit(const OwnerToken &unitID, const Real &damage, const Vector3 &force);
	void sendPlayerWounded();
	void sendPlayerUnwounded();
	void processPlayerInfo(Packet *p);
	void sendItemStash(const unsigned short &slot);
	void sendItemUnstash(const unsigned short &line, const unsigned short &slot);
	void processItemStash(Packet *p);
	void processFindPlayer(Packet *p);
	void sendFindPlayer(const String &name);
	void sendFindToken(const OwnerToken &token);
	void processServerList(Packet *p);
	void processServerConnected(Packet *p);
	void processMaintenance(Packet *p);
	void sendViolation(const String &info);
};
