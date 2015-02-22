#pragma once 

#include "Ogre.h"
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
	MagixGameStateManager();
	~MagixGameStateManager();
	void initialize();
	void setGameState(const unsigned short &state);
	const unsigned short getGameState();
	void setTempGameState(const unsigned short &state);
	const unsigned short getTempGameState();
	void setCampaign(const String &name);
	const String getCampaignName();
	bool isInGame();
	bool isCampaign();
	bool isPaused();
	void togglePause();
	bool isCinematic();
	void toggleCinematic();
	void setServerID(const unsigned short &iID);
	const unsigned short getServerID();
	bool isMapChange();
	void toggleMapChange();
	bool popMapChangeDone();
	void pushMapChangeDone();
	void setDimension(const unsigned char &d);
	const unsigned char getDimension();
	void setGameStarted(bool flag);
	bool getGameStarted();
};
