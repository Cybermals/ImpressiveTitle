#pragma once

#include "GameConfig.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "StringCompressor.h"
#include <assert.h>
#include <stdio.h>
#ifdef __linux__
#include <termios.h>
#endif
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <sstream>
#ifdef _WIN32
#include <conio.h>
#endif
#include <fstream>
#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <time.h>

#ifdef __APPLE__
#include <mach/task.h>
#include <mach/mach_init.h>
#endif

#ifdef __linux__
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/time.h>
#endif

#ifdef _WIN32
#include <windows.h> // Sleep and CreateProcess
#include <psapi.h>
#else
#include <unistd.h> // usleep
#include <cstdio>
#include <sys/resource.h>
typedef char* PSTR, *LPSTR;
#endif

#if defined(_CONSOLE_2)
#include "Console2SampleIncludes.h"
#endif

#include "MagixNetworkDefines.h"

using namespace RakNet;
using namespace std;

// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
MessageID GetPacketIdentifier(Packet *p)
{
	if (p == 0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
		return (unsigned char)p->data[0];
}

class ServerManager
{
protected:
	bool alive;
	RakPeerInterface *server;
	bool clientIsMine[MAX_CLIENTS];
	SystemAddress clientAdd[MAX_CLIENTS];
	string clientMap[MAX_CLIENTS];
	string clientName[MAX_CLIENTS];
	bool clientIsReady[MAX_CLIENTS];
	bool clientIsKicked[MAX_CLIENTS];
	unsigned char clientDimension[MAX_CLIENTS];
	bool clientHasTime[MAX_CLIENTS];
	unsigned short numClients;
	bool showTraffic;
	bool showChat;
	bool showMapChange;
	bool printlog;
	bool hideText;
	SystemAddress mainServerAdd;
	SystemAddress serverAdd[MAX_SERVERS];
	unsigned char serverID;
	bool serverConnected[MAX_SERVERS];
	bool requestedPlayerData;
	bool hasBegun;
	unsigned short connectAttempts;
	bool doUpdate;
	vector<pair<MessageID, int> > listo;
	float maintenanceTime;
	time_t prevTime;
	vector<string> bootlist;
	string broadcastAdd;
	string mainServerInitAdd;
public:
	ServerManager();
	~ServerManager();
	void startThread();
	bool initialize(const unsigned int &sleepTime = 0);
	void begin();
	void runLoop();
	void printChat(Packet *p, RakNet::BitStream *bitStream, const char* pmTarget = "");
	void printChat(const OwnerToken &sender, RakNet::BitStream *bitStream, const char* pmTarget = "");
	void sendNewPlayerProcedure(const OwnerToken &playerToken, const string &mapName, const unsigned char &dimension);
	void broadcastInMap(const string &mapName, const unsigned char &dimension, RakPeerInterface *peer, RakNet::BitStream *bitStream, const PacketPriority &priority, const PacketReliability &reliability, const char &orderingChannel, const SystemAddress &exceptionAddress);
	void sendMyPlayers(RakPeerInterface *peer, RakNet::BitStream *bitStream, const PacketPriority &priority, const PacketReliability &reliability, const char &orderingChannel, const SystemAddress &exceptionAddress);
	void relayPacket(Packet *p, const PacketPriority &priority, const PacketReliability &reliability, const char &orderingChannel);
	void sendAllServers(RakNet::BitStream *bitStream, const PacketPriority &priority, const PacketReliability &reliability, const char &orderingChannel, const SystemAddress &exceptionAddress);
	void sendNextServer(RakNet::BitStream *bitStream, const PacketPriority &priority, const PacketReliability &reliability, const char &orderingChannel);
	const OwnerToken getOwnerToken(Packet *p, string &mapName, string &playerName);
	const OwnerToken getOwnerToken(Packet *p, string &mapName);
	const OwnerToken getOwnerToken(Packet *p, string &mapName, unsigned char &dimension);
	const OwnerToken getOwnerToken(Packet *p);
	const OwnerToken getTokenByName(const string &playerName, bool caseSensitive = false);
	const OwnerToken getTokenByName(const string &playerName, bool caseSensitive, string &map, unsigned char &dimension);
	const string toLowerCase(string text);
	void shutdown();
	void quit();
	void resetData();
	const OwnerToken unassignToken(Packet *p);
	void unassignToken(const OwnerToken &token);
	const unsigned short playersInMap(const string &mapName, const unsigned char &dimension);
	void assignPlayerData(const OwnerToken &token, bool isMine, Packet *p, const string &playerName, const string &mapName, const unsigned char &dimension/*, const unsigned short &charID*/);
	void sendTokenDisconnectProcedure(const OwnerToken &token);
	void sendDisconnectProcedure(Packet *p, bool normalDisconnect);
	void sendRemoteDisconnectProcedure(Packet *p);
	void sendDisconnectProcedureToMyPlayers(const OwnerToken &token, bool normalDisconnect);
	void logNumPlayers();
	void registerServer(Packet *p);
	bool unregisterServer(const SystemAddress &add);
	bool unregisterServer(Packet *p);
	bool isServerAdd(const SystemAddress &add);
	bool imTheOwner(Packet *p);
	void unregisterAllServers();
	void sendTokenDisconnected(const SystemAddress &add);
	void sendTokenDisconnected(const OwnerToken &tToken);
	void sendDisconnectProcedureOfAllTokensConnectedTo(const SystemAddress &add);
	bool needsUpdate();
	//void updateMemCheck(bool &doReset);
	void printMemInfo();
	void bootPlayer(const char *tName);
	bool ShowChat();
	bool HideText();
	RakNetStatistics* getStatistics();
	int getAveragePing();
	const bool readIPs();
};