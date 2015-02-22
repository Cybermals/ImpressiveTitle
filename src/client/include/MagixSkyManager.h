#pragma once

//#include <OgreTerrainGroup.h>
#include "GameConfig.h"
#include "MagixExternalDefinitions.h"
#include "MagixCamera.h"
#include "MagixSoundManager.h"

#include "Ogre.h"
using namespace Ogre;

//#define MAX_CLOUDS 10
#define MAX_STARS 77
#define NORTH Vector3::UNIT_Z
#define EAST Vector3::NEGATIVE_UNIT_X
#define SOUTH Vector3::NEGATIVE_UNIT_Z
#define WEST Vector3::UNIT_X
#define DAYSPEED 1//0.5
#define DEFAULT_FOG_COLOUR ColourValue(192.0f/255, 229.0f/255, 253.0f/255)
#define DEFAULT_SUNLIGHT_COLOUR ColourValue(1.0, 1.0, 0.8)
#define DEFAULT_MOONLIGHT_COLOUR ColourValue(0.25, 0.25, 0.25)
#define FOG_DENSITY .0001
//#define FOG_START 1000 //500
//#define FOG_END 2500 //1500

class MagixSkyManager
{
protected:
	MagixExternalDefinitions *mDef;
	MagixCamera *mCamera;
	MagixSoundManager *mSoundManager;
	SceneManager *mSceneMgr;
	SceneNode *mSkyNode;
	Entity *mSkyDome;
	//BillboardSet *mCloudSet;
	BillboardSet *mSun;
	BillboardSet *mSunRadiance;
	BillboardSet *mMoon;
	BillboardSet *mStars;
	Light *mSunLight;
	Light *mMoonLight;
	//Ogre::TerrainGlobalOptions* mTerrainGlobals;
	//Real cloudSpeed[MAX_CLOUDS];
	Real skyDayTime;
	bool dayTimeFrozen;
	Real frozenDayTime;
	SceneNode *mWeatherNode;
	ParticleSystem *mWeatherSystem;
	Real weatherTime;
	bool weatherLocked;
	String weatherType;
	String weatherCycle;
	ColourValue weatherSkyShader;
	//ColourValue weatherSkyAdder;
	String weatherEffect;
	Real weatherEffectCountdown;
	vector<const WeatherEvent>::type weatherCycleEventList;
	bool hasWeatherSound;
	bool interiorSky;
	Real FOG_START, FOG_END;
public:
	MagixSkyManager();
	~MagixSkyManager();
	void initialize(SceneManager *sceneMgr, MagixExternalDefinitions *def, MagixCamera *camera, MagixSoundManager *soundMgr);
	void updateSky();
	void resetSky();
	void updateCelestials(const Real &dayTime);
	void updateWeather();
	void updateWeatherEffect(const Real &timeElapsed, bool clearEffect = false);
	void update(const FrameEvent &evt);
	void showSky(bool flag);
	void showWeather(bool flag);
	bool isWeatherVisible();
	void setDayTime(const Ogre::Real &time);
	const Ogre::Real getDayTime();
	const Ogre::Real getWeatherTime();
	void freezeDayTime(bool flag, const Real &time = 0);
	void setWeatherTime(const Real &time);
	void lockWeather(bool flag);
	void setWeather(const String &type);
	void destroyWeather();
	const Vector4 Vec4(const ColourValue &colour);
	void setWeatherEffect(const String &type);
	void setInteriorSky(bool flag, const ColourValue &fogColour = ColourValue(0.06, 0.05, 0.07));
	void setWeatherCycle(const String &type);
};
