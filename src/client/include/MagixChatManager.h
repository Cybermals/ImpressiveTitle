#pragma once

#define MAX_LINES 200
#define MAX_CHANNELS 3

#define CHAT_INVALID 0
#define CHAT_GENERAL 1
#define CHAT_PRIVATE 2
#define CHAT_PARTY 3
#define CHAT_ADMIN 4
#define CHAT_ACTION 5
#define CHAT_PARTYACTION 6
#define CHAT_COMMAND 7
#define CHAT_LOCAL 8
#define CHAT_LOCALACTION 9
#define CHAT_EVENT 10
#define CHAT_LOCALEVENT 11
#define CHAT_MOD 12
#define CHAT_LIPSYNC 13
#define CHAT_PARTYEVENT 14

#define COMMAND_SETHOME "/sethome"
#define COMMAND_GOHOME "/gohome"
#define COMMAND_RESETHOME "/resethome"
#define COMMAND_POSITION "/position"
#define COMMAND_FRIEND "/friend "
#define COMMAND_UNFRIEND "/unfriend "
#define COMMAND_BLOCK "/block "
#define COMMAND_UNBLOCK "/unblock "
#define COMMAND_KICK "/kick "
#define COMMAND_ROLL "/roll "
#define COMMAND_CREATEITEM "/crooteitom "
#define COMMAND_GODSPEAK "/godspeak "
#define COMMAND_MODON "/modon"
#define COMMAND_MODOFF "/modoff"
#define COMMAND_EARTHQUAKE "/earthquake"
#define COMMAND_CLEARALL "/clearall"
#define COMMAND_CLEAR "/clear"
#define COMMAND_LIPSYNC "/lipsync "
#define COMMAND_GOTO "/goto "
#define COMMAND_SPAWN "/spoon "
#define COMMAND_WHERE "/where "
#define COMMAND_WHOIS "/whois "
#define COMMAND_RELEASEPET "/releasepet"
#define COMMAND_PETFOLLOW "/petfollow"
#define COMMAND_COMMANDS "/?"
#define COMMAND_MASSBLOCK "/massblock "
#define COMMAND_BAN "/ban "
#define COMMAND_PARTY "/party "
#define COMMAND_SAVECHAT "/savechat"
// KITO
#define COMMAND_SPAWNITEM "/spoonitom "

#include "GameConfig.h"
#include "MagixUnitManager.h"

class MagixChatManager
{
protected:
	vector<String>::type chatString[MAX_CHANNELS];
	vector<String>::type chatSayer[MAX_CHANNELS];
	vector<unsigned char>::type chatType[MAX_CHANNELS];
	bool hasNewLine[MAX_CHANNELS];
	unsigned char channel;
public:
	bool doLocalUsername;
	bool doGeneralCharname;
	MagixChatManager()
	{
		reset();
		channel = 0;
		doLocalUsername = false;
		doGeneralCharname = false;
	}
	~MagixChatManager()
	{
	}
	void reset(bool clearHistory = false);

	void push(const String &caption, const String &sayer = "", const unsigned char &type = CHAT_GENERAL);

	const String getChatBlock(const unsigned short &lines, const Real &boxWidth, const Real &charHeight, const Real &lastOffset);

	void processInput(String &caption, unsigned char &type, String &param);

	void say(MagixUnitManager *unitMgr, MagixUnit *target, const String &caption, const unsigned char &type = CHAT_GENERAL);

	void message(const String &caption, const unsigned char &type = 0);

	bool popHasNewLine(const unsigned char &chan);

	bool getHasNewLine(const unsigned char &chan);

	const String prefixChatLine(const String &caption, const String &sayer, const unsigned char &type, bool isPostfixLength = false);

	const unsigned short getPrefixLength(const String &sayer, const unsigned char &type);

	const unsigned short getPostfixLength(const String &sayer, const unsigned char &type);

	const String processChatString(const String &caption, const String &sayer, const unsigned char &type, const Real &boxWidth, const Real &charHeight);

	void toggleChannel();

	void setChannel(const unsigned char &value);

	const unsigned char getChannel();

	const vector<const unsigned char>::type getOtherChannels();

	void saveChatLog(const String &directory, const String &filename);
};
