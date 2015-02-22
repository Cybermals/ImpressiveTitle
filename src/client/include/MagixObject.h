#pragma once

#include "Ogre.h"
using namespace Ogre;


#define FRICTION 3
#define GRAVITY 0.9
#define JUMP_ACCEL 0.3
#define TERMINAL_VEL 800
#define GROUND_THRESHOLD 2

#define ALLIANCE_PLAYER 1
#define ALLIANCE_FRIEND 2
#define ALLIANCE_ENEMY 3

enum OBJECT_TYPE
{
	OBJECT_BASIC,
	OBJECT_ANIMATED,
	OBJECT_LIVING,
	OBJECT_UNIT,
	OBJECT_PLAYER,
	OBJECT_ITEM,
	OBJECT_CRITTER
};
struct HitInfo
{
	unsigned short ID;
	Real hp;
	Vector3 force;
	bool isMine;
	HitInfo(const unsigned short &i, const Real &h, const Vector3 &f, bool iM=false)
	{
		ID = i;
		hp = h;
		force = f;
		isMine = iM;
	}
};

using namespace Ogre;

class MagixObject
{
protected:
	SceneNode *mObjectNode;
public:
	MagixObject();
	~MagixObject();
	virtual const OBJECT_TYPE getType();
	virtual const Vector3 getPosition();
	virtual SceneNode* getObjectNode();
};
