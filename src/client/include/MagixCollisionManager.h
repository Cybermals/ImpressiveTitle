#pragma once

#include "Ogre.h"
using namespace Ogre;

struct Collision
{
	SceneNode *mNode;
	Real range;
	Vector3 offset;
	Vector3 force;
	vector<unsigned short>::type hitID;
	vector<unsigned short>::type critterHitID;
	short hp;
	unsigned char alliance;
	bool hitAlly;
	Collision()
	{
		mNode = 0;
		range = 0;
		offset = Vector3::ZERO;
		force = Vector3::ZERO;
		hitID.clear();
		critterHitID.clear();
		hp = 0;
		alliance = 0;
		hitAlly = false;
	}
	const Ogre::Vector3 getPivot()
	{
		//if(!mCollOwnerNode[iID])return Vector3::ZERO;
		//return mCollOwnerNode[iID]->getPosition()+mCollOwnerNode[iID]->getOrientation()*collOffset[iID];
		if(!mNode)return Vector3::ZERO;
		return mNode->getPosition()+mNode->getOrientation()*offset;
	}
	const Vector3 getForce(bool fromPivot=false, const Vector3 &targetPosition=Vector3::ZERO)
	{
		if(fromPivot)
		{
			const Vector3 tVect = targetPosition-getPivot();
			const Quaternion tQuat = force.getRotationTo(tVect);
			return tQuat*force;
		}
		return force;
	}
	bool collides(const AxisAlignedBox &target)
	{
		if(!mNode)return false;
		//if(mCollEnt[iID]->getWorldBoundingSphere().intersects(target))return true;
		if(Sphere(getPivot(),range).intersects(target))return true;
		return false;
	}
	bool hasHitID(const unsigned short &id)
	{
		for(int i=0;i<(int)hitID.size();i++)
		{
			if(hitID[i]==id)return true;
		}
		return false;
	}
	bool hasCritterHitID(const unsigned short &id)
	{
		for(int i=0;i<(int)critterHitID.size();i++)
		{
			if(critterHitID[i]==id)return true;
		}
		return false;
	}
};

struct Wall
{
	bool isActive;
	bool isSphere;
	Vector3 center;
	Vector3 range;
	bool isInside;
	Wall()
	{
		isActive = false;
		isSphere = false;
		center = Vector3::ZERO;
		range = Vector3::ZERO;
		isInside = false;
	}
	bool collides(const Vector3 &target)
	{
		if(!isActive)return false;
		
		if(isSphere)
		{
			//Fix y coordinate to that of target's to create a Cylinder wall
			const Sphere tSphere(Vector3(center.x,target.y,center.z),range.x);
			if(isInside)return(tSphere.intersects(target));
			else return(!tSphere.intersects(target));
		}
		else
		{
			//Fix y coordinate to that of target's to create an infinitely tall wall
			const AxisAlignedBox tBox(Vector3(center.x-range.x/2,target.y,center.z-range.z/2),
								Vector3(center.x+range.x/2,target.y,center.z+range.z/2));
			if(isInside)return(tBox.intersects(target));
			else return(!tBox.intersects(target));
		}
		return false;
	}
};

struct Portal
{
	SceneNode *mNode;
	Entity *mEnt;
	String dest;
	Portal()
	{
		mNode = 0;
		mEnt = 0;
		dest = "";
	}
	bool collides(const AxisAlignedBox &target)
	{
		if(!mEnt || dest=="")return false;
		if(mEnt->getWorldBoundingSphere().intersects(target))return true; // getWorldBoundingSphere doesn't work properly
		return false;
	}
	void disable()
	{
		dest = "";
	}
};

struct Gate
{
	SceneNode *mNode;
	Entity *mEnt;
	String dest;
	Vector3 destVect;
	bool hasVectY;
	Gate()
	{
		mNode = 0;
		mEnt = 0;
		dest = "";
		destVect = Vector3::ZERO;
		hasVectY = false;
	}
	bool collides(const AxisAlignedBox &target)
	{
		if(!mEnt || dest=="")return false;
		if(mEnt->getWorldBoundingSphere().intersects(target))return true; // getWorldBoundingSphere doesn't work properly
		return false;
	}
	void disable()
	{
		dest = "";
	}
};

struct WaterBox
{
	bool isActive;
	Vector3 center;
	Vector2 scale;
	bool isSolid;
	WaterBox()
	{
		isActive = false;
		center = Vector3::ZERO;
		scale = Vector2::ZERO;
		isSolid = false;
	}
	bool collides(const Vector3 &target)
	{
		if(!isActive)return false;
		if(target.y > center.y)return false;
		//Create infinite depth box
		const AxisAlignedBox tBox(Vector3(center.x-scale.x/2,target.y,center.z-scale.y/2),
							Vector3(center.x+scale.x/2,target.y,center.z+scale.y/2));
		if(tBox.intersects(target))return true;
		return false;
	}
};

struct CollBox
{
	//Collbox takes center like a typical mesh, i.e. midpoint of "feet"
	Vector3 center;
	Vector3 range;
	CollBox()
	{
		center = Vector3::ZERO;
		range = Vector3::ZERO;
	}
	bool collides(const Vector3 &target, const Real &headHeight)
	{
		//Extend below by unit height to detect head collision
		const AxisAlignedBox tBox(Vector3(center.x-range.x/2,center.y-headHeight,center.z-range.z/2),
								Vector3(center.x+range.x/2,center.y+range.y,center.z+range.z/2));
		if(tBox.intersects(target))return true;
		return false;
	}
};
struct CollSphere
{
	Vector3 center;
	Real range;
	CollSphere()
	{
		center = Vector3::ZERO;
		range = 0;
	}
	bool collides(const Vector3 &target)
	{
		const Sphere tSphere(center,range);
		if(tSphere.intersects(target))return true;
		return false;
	}
};

class MagixCollisionManager
{
protected:
	SceneManager *mSceneMgr;
	
	list<Wall>::type wall;
	list<Portal>::type portal;
	list<Gate>::type gate;
	list<WaterBox>::type waterBox;
	list<CollBox>::type collBox;
	list<CollSphere>::type collSphere;
public:
	list<Collision>::type coll;
	MagixCollisionManager()
	{
		mSceneMgr = 0;
		coll.clear();
		wall.clear();
		portal.clear();
		gate.clear();
		waterBox.clear();
		collBox.clear();
		collSphere.clear();
	}
	~MagixCollisionManager()
	{
		reset();
		destroyAllWaterBoxes();
		destroyAllPortals();
		destroyAllGates();
	}
	void initialize(SceneManager *sceneMgr);

	void reset();

	void createSphereMesh(const std::string& strName, const float r, const int nRings = 16, const int nSegments = 16);

	void createCollision(SceneNode *node, const Real &range, const Vector3 &force, const Vector3 &offset = Vector3::ZERO, const short &hp = 0, const unsigned char &alliance = 0, bool hitAlly = false);

	const list<Collision>::type::iterator destroyCollision(const list<Collision>::type::iterator &it);

	void destroyCollisionByOwnerNode(SceneNode *node);

	const vector<Collision*>::type getCollisionHitList(const unsigned short &unitID, const unsigned short &alliance, const AxisAlignedBox &target);

	const vector<Collision*>::type getCollisionHitListForCritter(const unsigned short &iID, const unsigned short &alliance, const AxisAlignedBox &target);

	void createBoxWall(const Vector3 &center, const Vector3 &range, bool isInside);

	void createSphereWall(const Vector3 &center, const Real &range, bool isInside);

	const list<Wall>::type::iterator destroyWall(const list<Wall>::type::iterator &it);

	void destroyWall(const unsigned short &iID);

	const vector<Wall*>::type getWallHitList(const Vector3 &target);

	void createPortal(const Vector3 &center, const Real &range, String destName);

	const list<Portal>::type::iterator destroyPortal(list<Portal>::type::iterator it);

	void destroyAllPortals();

	Portal* getPortalHit(const AxisAlignedBox &target);

	void getPortalMap(vector<const std::pair<Vector2, String>>::type &map);

	void createWaterBox(const Vector3 &center, const Real &scaleX, const Real &scaleZ, bool isSolid = false);

	const list<WaterBox>::type::iterator destroyWaterBox(const list<WaterBox>::type::iterator &it);

	void destroyAllWaterBoxes();

	WaterBox* getWaterBoxHit(const Vector3 &target);

	void createGate(const Vector3 &center, String destName, const Vector3 &destVect, const String &matName, bool hasVectY);

	const list<Gate>::type::iterator destroyGate(list<Gate>::type::iterator it);

	void destroyAllGates();

	Gate* getGateHit(const AxisAlignedBox &target);

	void getGateMap(vector<const std::pair<Vector2, String>>::type &map);

	void createCollBox(const Vector3 &center, const Vector3 &range);

	const vector<CollBox*>::type getCollBoxHitList(const Vector3 &target, const Real &headHeight);

	void createCollSphere(const Vector3 &center, const Real &range);

	const vector<CollSphere*>::type getCollSphereHitList(const Vector3 &target);
};
