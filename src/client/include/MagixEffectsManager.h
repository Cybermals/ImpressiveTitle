#pragma once

#include "Ogre.h"
using namespace Ogre;

#define MAX_RIBBONTRAILS 20
#define MAX_PARTICLES 32
#define MAX_WATERRIPPLES 20
#define MAX_BREATHERS 20
#define MAX_ITEMSPARKLE 10
#define MAX_ITEMEFFECTS 20

class MagixEffectsManager
{
protected:
	SceneManager *mSceneMgr;
	RibbonTrail *mTrail[MAX_RIBBONTRAILS];
	BillboardSet *mTrailHead[MAX_RIBBONTRAILS];
	Entity *mTrailHeadEnt[MAX_RIBBONTRAILS];
	ParticleSystem *mParticle[MAX_PARTICLES];
	SceneNode *mParticleNode[MAX_PARTICLES];
	Real particleDuration[MAX_PARTICLES];
	bool particleUnresettable[MAX_PARTICLES];
	bool particleHasOwnNode[MAX_PARTICLES];
	String particleType[MAX_PARTICLES];

	ParticleSystem *mWaterRipple[MAX_WATERRIPPLES];
	SceneNode *mWaterRippleNode[MAX_WATERRIPPLES];
	SceneNode *mWaterRippleOwner[MAX_WATERRIPPLES];

	ParticleSystem *mBreather[MAX_BREATHERS];
	Entity *mBreatherOwner[MAX_BREATHERS];
	String breatherType[MAX_BREATHERS];
	Vector3 breatherOffset[MAX_BREATHERS];

	ParticleSystem *mItemParticle[MAX_ITEMSPARKLE];
	SceneNode *mItemParticleNode[MAX_ITEMSPARKLE];

	ParticleSystem *mItemEffect[MAX_ITEMEFFECTS];
	Entity *mItemEffectOwner[MAX_ITEMEFFECTS];
	String itemEffectType[MAX_ITEMEFFECTS];
	Vector3 itemEffectOffset[MAX_ITEMEFFECTS];
	String itemEffectBone[MAX_ITEMEFFECTS];
public:
	MagixEffectsManager()
	{
		mSceneMgr = 0;
		for(int i=0;i<MAX_RIBBONTRAILS;i++)
		{
			mTrail[i] = 0;
			mTrailHead[i] = 0;
			mTrailHeadEnt[i] = 0;
		}
		for(int i=0;i<MAX_PARTICLES;i++)
		{
			mParticle[i] = 0;
			mParticleNode[i] = 0;
			particleDuration[i] = 0;
			particleUnresettable[i] = false;
			particleHasOwnNode[i] = false;
			particleType[i] = "";
		}
		for(int i=0;i<MAX_WATERRIPPLES;i++)
		{
			mWaterRipple[i] = 0;
			mWaterRippleNode[i] = 0;
			mWaterRippleOwner[i] = 0;
		}
		for(int i=0;i<MAX_BREATHERS;i++)
		{
			mBreather[i] = 0;
			mBreatherOwner[i] = 0;
			breatherType[i] = "";
			breatherOffset[i] = Vector3::ZERO;
		}
		for(int i=0;i<MAX_ITEMSPARKLE;i++)
		{
			mItemParticle[i] = 0;
			mItemParticleNode[i] = 0;
		}
		for(int i=0;i<MAX_ITEMEFFECTS;i++)
		{
			mItemEffect[i] = 0;
			mItemEffectOwner[i] = 0;
			itemEffectType[i] = "";
			itemEffectOffset[i] = Vector3::ZERO;
			itemEffectBone[i] = "";
		}
	}
	~MagixEffectsManager()
	{
		reset(true);
	}

	void initialize(SceneManager *sceneMgr);

	void reset(bool ignoreUnresettable = false);

	void update(const FrameEvent &evt);

	void createRibbonTrail(Entity *ent, const String &boneName, const String &trailMat, const Real &trailWidth = 2,
		const ColourValue &trailColour = ColourValue(1, 0.9, 0.75, 1), const ColourValue &colourChange = ColourValue(7, 7, 7, 7), const String &headMat = "");

	void destroyRibbonTrail(unsigned short tID);

	ParticleSystem* createParticle(SceneNode *node, const String &name, const Real &duration, bool unresettable = false,
		bool attachToBone = false, bool hasOwnNode = false, const Vector3 &position = Vector3::ZERO, const String &ownName = "");

	void destroyParticle(unsigned short tID);

	void destroyParticle(ParticleSystem *pSys);

	void destroyParticle(const String &name);

	ParticleSystem* getParticle(const String &name);

	void destroyRibbonTrailByEntity(Entity *ent);

	void destroyParticleByObjectNode(SceneNode *node, const String &type = "");

	void stopParticleByObjectNode(SceneNode *node, const String &type = "");

	void createWaterRipple(SceneNode *owner, const Real &waterHeight, const Real &scale);

	void destroyWaterRipple(unsigned short tID);

	void destroyWaterRippleByOwner(SceneNode *node);

	void setWaterRippleEmissionByOwner(SceneNode *node, const Real &rate);

	void setWaterRippleHeightByOwner(SceneNode *node, const Real &height);

	bool hasWaterRippleOwner(SceneNode *node);

	void createBreather(Entity *owner, const String &type, const Vector3 &offset, const Real &scale);

	void destroyBreather(unsigned short tID);

	void destroyBreatherByOwner(Entity *ent, const String &type = "");

	void setBreatherEmissionByOwner(Entity *ent, const Real &rate, const String &type = "");

	bool hasBreatherOwner(Entity *ent, const String &type = "");

	void createItemParticle(SceneNode *node);

	void destroyItemParticle(unsigned short tID);

	void destroyItemParticleByObjectNode(SceneNode *node);

	void createItemEffect(Entity *owner, const String &type, const String &bone, const Vector3 &offset, const Real &scale);

	void destroyItemEffect(unsigned short tID);

	void destroyItemEffectByOwnerNode(SceneNode *node, const String &type = "");
};
