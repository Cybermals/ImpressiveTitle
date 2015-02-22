#pragma once

#include "GameConfig.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "SimpleMutex.h"
//#include "OgreString.h"
#include "StringCompressor.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <sstream>
#ifdef _WIN32
#include <conio.h>
#endif
#include <vector>
#include <iostream>
#include <utility>
#include <string>
#include <fstream>
#include <time.h>
#include "MagixNetworkDefines.h"

using namespace RakNet;
using namespace std;

struct PlayerToken
{
	SystemAddress add;
	unsigned char serverID;
	string name;
	unsigned short charID;
	string item[MAX_EQUIP];
	pair<unsigned short, unsigned short> hp;
	vector<pair<string, unsigned char> > skill;
	string pet;
	PlayerToken()
	{
		add = UNASSIGNED_SYSTEM_ADDRESS;
		serverID = MAX_SERVERS;
		name = "";
		charID = 0;
		for (int i = 0; i<MAX_EQUIP; i++)item[i] = "";
		hp.first = 0;
		hp.second = 0;
		skill.clear();
		pet = "";
	}
};
struct BanInfo
{
	string name;
	int year;
	int yDay;
	vector<string> IPList;
	BanInfo(const string &n, const int &y, const int &yD)
	{
		name = n;
		year = y;
		yDay = yD;
		IPList.clear();
	}
};


class ServerManager
{
protected:
	bool alive;
	SimpleMutex serversListMutex;
	RakPeerInterface *server;
	RakNetStatistics *rss;
	unsigned short numClients;
	unsigned short numServers;
	bool showTraffic;
	SystemAddress serverAdd[MAX_SERVERS];
	SystemAddress serverTunnelAdd[MAX_SERVERS];
	bool serverFull[MAX_SERVERS];
	PlayerToken playerToken[MAX_CLIENTS];
	vector<pair<string, SystemAddress> > loginSession;
	//vector<const pair<string,SystemAddress> > loginSession;
	float dayTime;
	float weatherTime;
	float maintenanceTime;
	time_t prevTime;
	unsigned short lowPing;
	unsigned short highPing;
	vector<BanInfo> banlist;

public:
	ServerManager();
	~ServerManager();
	void startThread();
	bool initialize();
	void runLoop();
	const string getFilename(const char *name, const char *fileExtension, bool replaceSpaces = true, bool replaceUnderscores = false);
	void shutdown();
	void quit();
	const string toLowerCase(string text);
	const unsigned char registerServer(Packet *p);
	const unsigned char getServerID(const SystemAddress &add);
	bool unregisterServer(Packet *p);
	void sendServerList(const SystemAddress &target, const bool &isClient);
	void notifyServerID(const unsigned char &iID, const SystemAddress &add);
	void broadcastServerConnected(const unsigned char &iID, const SystemAddress &exceptionAdd);
	void broadcastServerDisconnected(const unsigned char &iID, const SystemAddress &exceptionAdd);
	void broadcastMaintenance(const unsigned char &time);
	const OwnerToken assignToken(const SystemAddress &add, const unsigned char &serverID, const string &name, const unsigned short &charID);
	void unassignToken(const OwnerToken &token);
	void unassignToken(const SystemAddress &add);
	const OwnerToken getTokenByAdd(const SystemAddress &add);
	const OwnerToken getTokenByName(const string &name);
	void unassignTokens(const SystemAddress &serverAdd);
	void broadcastServerUpdate();
	void loadPlayerData(const OwnerToken &token);
	void savePlayerData(const OwnerToken &token);
	const vector<string> tokenize(const string& str, const string& delimiters = " ");
	void updateTimer();
	void updateServers(bool forceUpdate = false);
	const unsigned short getNumClientsInServer(const unsigned char &serverID);
	const string XOR7(const string &input);
	const string XOR7OLD(const string &input);
	void saveBanlist();
	void loadBanlist();
	void updateBanlist();
	void clearBanlist();
	void endSession(Packet *p);
	void loadPingRange();
	void savePingRange();
	RakNetStatistics* getStatistics();
	int getAveragePing();
	void addToBanList(const char *input);
	bool ShowTraffic();
	unsigned short getNumClients();
	unsigned short getNumServers();
	void printServers();
	void printTimeWeather();
	void enterPingRange();
	void enterServerPing();
	void reloadBanList();
};