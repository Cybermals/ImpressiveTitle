#pragma once

#include "GameConfig.h"
#include "PagedGeometry.h"
#include "GrassLoader.h"
#include "BatchPage.h"
#include "ImpostorPage.h"
#include "TreeLoader2D.h"
#include "TreeLoader3D.h"
#include "MagixSkyManager.h"
#include "MagixExternalDefinitions.h"
#include "MagixGameStateManager.h"
//#include "MagixHandler.h"

using namespace Forests;

#define MAX_LIGHTS 3

class MagixWorld
{
protected:
	MagixExternalDefinitions *mDef;
	MagixGameStateManager *mGameStateManager;
	MagixSoundManager *mSoundManager;
	MagixCollisionManager *mCollisionManager;
	MagixSkyManager *mSkyManager;
	SceneManager *mSceneMgr;
	RenderWindow *mWindow;
	PagedGeometry *mGrass,*mTrees,*mBushes,*mLargeTrees,*mFloatingBushes;
	unsigned short numTreeMeshes;
	unsigned short numBushMeshes;
	Entity *mCeiling;
	SceneNode *mCeilingNode;
	Light *mLight[MAX_LIGHTS];
	String worldName;
	Vector2 worldSize;
	Vector2 worldBounds;
	Vector3 spawnSquare;
	unsigned short numWaterPlanes;
	unsigned short numObjects;
	unsigned short numParticles;
	unsigned short numLights;
	unsigned short numBillboards;
	Real ceilingHeight;
	bool isInterior;
	String mapEffect;
	int staticGeometryX;
	int staticGeometryY;
	vector<const WorldCritter>::type critterSpawnList;
	vector<const std::pair<Vector3,Vector3>>::type critterRoamAreaList;
	unsigned short critterSpawnLimit;
public:
	MagixWorld();
	~MagixWorld();
	void initialize(SceneManager *sceneMgr, RenderWindow *window, MagixExternalDefinitions *def, MagixGameStateManager *gameStateMgr, MagixSoundManager *soundMgr, MagixCollisionManager *collisionMgr, MagixSkyManager *skyMgr);
	void initializeCustomMeshes();
	void reset();
	void update();
	void loadWorld(const String &name);
	void unloadWorld();
	const Vector3 getSpawnSquare();
	const Vector2 getWorldSize();
	const Vector2 getWorldBounds();
	const String getWorldName();
	void addWaterPlane(const Vector3 &position, const Real &scaleX, const Real &scaleZ, const String &matName = "Terrain/CalmWater", const String &playSound = "");
	void addObject(const String &meshName, const Vector3 &position, const Vector3 &scale, const Real &pitch, const Real &yaw, const Real &roll, const String &playSound = "", const String &matName = "");
	void addParticle(const String &sysName, const Vector3 &position, const String &playSound = "");
	void addLight(const Vector3 &position, const ColourValue &colour);
	void addBillboard(const Vector3 &position, const Vector2 &scale, const String &matName);
	void destroyAllWaterPlanes();
	void destroyAllObjects();
	void destroyAllParticles();
	void destroyAllLights();
	void destroyAllBillboards();
	bool getIsInterior();
	const Real getCeilingHeight();
	const String getMapEffect();
	void initializeStaticGeometry();
	void addStaticGeometry(Entity *ent, const Vector3 &position, const Quaternion &orientation, const Vector3 &scale);
	void buildStaticGeometry();
	void destroyStaticGeometry();
	const unsigned short getCritterSpawnLimit();
	const vector<const WorldCritter>::type getCritterSpawnList();
	const WorldCritter getCritterSpawnList(const unsigned short &iID);
	const std::pair<Vector3, Vector3> getCritterRoamArea(const unsigned short &iID);
	void clearCritterSpawnList();
};
