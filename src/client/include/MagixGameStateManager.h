#ifndef __MagixGameStateManager_h_
#define __MagixGameStateManager_h_

#define GAMESTATE_INITIALIZE 1
#define GAMESTATE_LOGO 2
#define GAMESTATE_STARTSCREEN 3
#define GAMESTATE_STARTCHARSCREEN 4
#define GAMESTATE_RETURNSTARTSCREEN 5
#define GAMESTATE_QUIT 6
#define GAMESTATE_CHARSCREEN 7
#define GAMESTATE_STARTCONNECTION 8
#define GAMESTATE_CONNECTING 9
#define GAMESTATE_STARTGAME 10
#define GAMESTATE_INGAME 11
#define GAMESTATE_STARTCAMPAIGN 12
#define GAMESTATE_INGAMECAMPAIGN 13
#define GAMESTATE_CREDITS 14
#define GAMESTATE_GAMEOVER 15
#define GAMESTATE_CONTINUECAMPAIGN 16
#define GAMESTATE_QUITCAMPAIGN 17

using namespace Ogre;

class MagixGameStateManager
{
protected:
	unsigned short gameState;
	unsigned short tempGameState;
	Real gameStateCount;
	String campaignName;
	bool pause;
	bool cinematic;
	unsigned short serverID;
	bool mapChange;
	bool mapChangeDone;
	unsigned char dimension;
	bool gameStarted;
public:
	MagixGameStateManager()
	{
		gameState = GAMESTATE_INITIALIZE;
		tempGameState = gameState;
		gameStateCount = 0;
		campaignName = "";
		pause = false;
		cinematic = false;
		serverID = 0;
		mapChange = false;
		mapChangeDone = false;
		dimension = 0;
		gameStarted = false;
	}
	~MagixGameStateManager()
	{
	}
	void initialize()
	{
	}
	void setGameState(const unsigned short &state)
	{
		gameState = state;
	}
	const unsigned short getGameState()
	{
		return gameState;
	}
	void setTempGameState(const unsigned short &state)
	{
		tempGameState = state;
	}
	const unsigned short getTempGameState()
	{
		return tempGameState;
	}
	void setCampaign(const String &name)
	{
		campaignName = name;
	}
	const String getCampaignName()
	{
		return campaignName;
	}
	bool isInGame()
	{
		return (gameState==GAMESTATE_INGAME||gameState==GAMESTATE_INGAMECAMPAIGN);
	}
	bool isCampaign()
	{
		return gameState==GAMESTATE_INGAMECAMPAIGN;
	}
	bool isPaused()
	{
		return pause;
	}
	void togglePause()
	{
		pause = !pause;
	}
	bool isCinematic()
	{
		return cinematic;
	}
	void toggleCinematic()
	{
		cinematic = !cinematic;
	}
	void setServerID(const unsigned short &iID)
	{
		serverID = iID;
	}
	const unsigned short getServerID()
	{
		return serverID;
	}
	bool isMapChange()
	{
		return mapChange;
	}
	void toggleMapChange()
	{
		mapChange = !mapChange;
	}
	bool popMapChangeDone()
	{
		const bool tFlag = mapChangeDone;
		mapChangeDone = false;
		return tFlag;
	}
	void pushMapChangeDone()
	{
		mapChangeDone = true;
	}
	void setDimension(const unsigned char &d)
	{
		dimension = d; //% MAXDIMS;
	}
	const unsigned char getDimension()
	{
		return dimension;
	}
	void setGameStarted(bool flag)
	{
		gameStarted = flag;
	}
	bool getGameStarted()
	{
		return gameStarted;
	}
};

#endif