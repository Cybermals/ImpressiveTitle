#include "MagixGameStateManager.h"
#include "MagixConst.h"

MagixGameStateManager::MagixGameStateManager()
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
MagixGameStateManager::~MagixGameStateManager()
{
}
void MagixGameStateManager::initialize()
{
}
void MagixGameStateManager::setGameState(const unsigned short &state)
{
	gameState = state;
}
const unsigned short MagixGameStateManager::getGameState()
{
	return gameState;
}
void MagixGameStateManager::setTempGameState(const unsigned short &state)
{
	tempGameState = state;
}
const unsigned short MagixGameStateManager::getTempGameState()
{
	return tempGameState;
}
void MagixGameStateManager::setCampaign(const String &name)
{
	campaignName = name;
}
const String MagixGameStateManager::getCampaignName()
{
	return campaignName;
}
bool MagixGameStateManager::isInGame()
{
	return (gameState == GAMESTATE_INGAME || gameState == GAMESTATE_INGAMECAMPAIGN);
}
bool MagixGameStateManager::isCampaign()
{
	return gameState == GAMESTATE_INGAMECAMPAIGN;
}
bool MagixGameStateManager::isPaused()
{
	return pause;
}
void MagixGameStateManager::togglePause()
{
	pause = !pause;
}
bool MagixGameStateManager::isCinematic()
{
	return cinematic;
}
void MagixGameStateManager::toggleCinematic()
{
	cinematic = !cinematic;
}
void MagixGameStateManager::setServerID(const unsigned short &iID)
{
	serverID = iID;
}
const unsigned short MagixGameStateManager::getServerID()
{
	return serverID;
}
bool MagixGameStateManager::isMapChange()
{
	return mapChange;
}
void MagixGameStateManager::toggleMapChange()
{
	mapChange = !mapChange;
}
bool MagixGameStateManager::popMapChangeDone()
{
	const bool tFlag = mapChangeDone;
	mapChangeDone = false;
	return tFlag;
}
void MagixGameStateManager::pushMapChangeDone()
{
	mapChangeDone = true;
}
void MagixGameStateManager::setDimension(const unsigned char &d)
{
	dimension = d; //% MAXDIMS;
}
const unsigned char MagixGameStateManager::getDimension()
{
	return dimension;
}
void MagixGameStateManager::setGameStarted(bool flag)
{
	gameStarted = flag;
}
bool MagixGameStateManager::getGameStarted()
{
	return gameStarted;
}