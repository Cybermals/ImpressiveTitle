#ifndef __MagixEffectsManager_h_
#define __MagixEffectsManager_h_

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
	void initialize(SceneManager *sceneMgr)
	{
		mSceneMgr = sceneMgr;
	}
	void reset(bool ignoreUnresettable=false)
	{
		for(int i=0;i<MAX_RIBBONTRAILS;i++)destroyRibbonTrail(i);
		for(int i=0;i<MAX_PARTICLES;i++)if(ignoreUnresettable||!particleUnresettable[i])destroyParticle(i);
		for(int i=0;i<MAX_WATERRIPPLES;i++)destroyWaterRipple(i);
		for(int i=0;i<MAX_BREATHERS;i++)destroyBreather(i);
		for(int i=0;i<MAX_ITEMSPARKLE;i++)destroyItemParticle(i);
		if(ignoreUnresettable)for(int i=0;i<MAX_ITEMEFFECTS;i++)destroyItemEffect(i);
	}
	void update(const FrameEvent &evt)
	{
		for(int i=0;i<MAX_PARTICLES;i++)
		{
			if(mParticle[i])
			{
				//cease emissions when duration is up
				if(particleDuration[i]>0)
				{
					particleDuration[i] -= evt.timeSinceLastFrame;
					if(particleDuration[i]<=0)
					{
						particleDuration[i] = 0;
						mParticle[i]->getEmitter(0)->setEmissionRate(0);
					}
				}
				//destroy particle system after all particles are gone
				else if(particleDuration[i]==0 && mParticle[i]->getNumParticles()==0)
				{
					destroyParticle(i);
				}
			}
		}
		for(int i=0;i<MAX_WATERRIPPLES;i++)
		{
			if(mWaterRipple[i])
			{
				//Update position
				if(mWaterRippleOwner[i])
					mWaterRippleNode[i]->setPosition(mWaterRippleOwner[i]->getPosition().x,mWaterRippleNode[i]->getPosition().y,mWaterRippleOwner[i]->getPosition().z);
				//Destroy afterall all particles are gone
				if(mWaterRipple[i]->getEmitter(0)->getEmissionRate()==0 && mWaterRipple[i]->getNumParticles()==0)
				{
					destroyWaterRipple(i);
				}
			}
		}
		for(int i=0;i<MAX_BREATHERS;i++)
		{
			if(mBreather[i] && mBreatherOwner[i])
			{
				SceneNode* tBreatherNode = mSceneMgr->getSceneNode("BreatherNode"+StringConverter::toString(i));
				Bone *tBone = mBreatherOwner[i]->getSkeleton()->getBone("Mouth");
				tBreatherNode->setOrientation(tBone->_getDerivedOrientation());
				tBreatherNode->setPosition(tBone->_getDerivedPosition() + tBone->_getDerivedOrientation()*breatherOffset[i]);
			}
		}
		for(int i=0;i<MAX_ITEMEFFECTS;i++)
		{
			if(mItemEffect[i] && mItemEffectOwner[i] && itemEffectBone[i]!="")
			{
				SceneNode* tItemEffectNode = mSceneMgr->getSceneNode("ItemFXNode"+StringConverter::toString(i));
				Bone *tBone = mItemEffectOwner[i]->getSkeleton()->getBone(itemEffectBone[i]);
				tItemEffectNode->setOrientation(tBone->_getDerivedOrientation());
				tItemEffectNode->setPosition(tBone->_getDerivedPosition() + tBone->_getDerivedOrientation()*itemEffectOffset[i]);
			}
		}
	}
	void createRibbonTrail(Entity *ent, const String &boneName, const String &trailMat, const Real &trailWidth=2, const ColourValue &trailColour=ColourValue(1,0.9,0.75,1), const ColourValue &colourChange=ColourValue(7,7,7,7), const String &headMat="")
	{
		short tID = -1;
		for(int i=0;i<MAX_RIBBONTRAILS;i++)
		{
			if(mTrail[i]==0)
			{
				tID = i;
				break;
			}
		}
		if(tID==-1)
		{
			tID = 0;
			destroyRibbonTrail(tID);
		}

		mTrailHead[tID] = mSceneMgr->createBillboardSet("FXTrailHead"+StringConverter::toString(tID));
		mTrailHead[tID]->setMaterialName(headMat==""?"Sky/StarMat":headMat);
		mTrailHead[tID]->setDefaultDimensions(trailWidth,trailWidth);
		mTrailHead[tID]->createBillboard(0,0,0);
		mTrailHead[tID]->setQueryFlags(EFFECT_MASK);
		mTrailHeadEnt[tID] = ent;
		TagPoint *tTP = mTrailHeadEnt[tID]->attachObjectToBone(boneName,mTrailHead[tID]);
		if(headMat=="")mTrailHead[tID]->setVisible(false);

		mTrail[tID] = mSceneMgr->createRibbonTrail("FXTrail"+StringConverter::toString(tID));
		mTrail[tID]->setVisible(true);
		mTrail[tID]->setMaterialName(trailMat);
		mTrail[tID]->setTrailLength(50);
		mTrail[tID]->setMaxChainElements(50);
		mTrail[tID]->setInitialColour(0,trailColour);
		mTrail[tID]->setColourChange(0, colourChange);
		mTrail[tID]->setInitialWidth(0,trailWidth);
		mTrail[tID]->setWidthChange(0,7);
		mTrail[tID]->addNode((Node*)tTP);
		mTrail[tID]->setQueryFlags(EFFECT_MASK);
		mSceneMgr->getRootSceneNode()->attachObject(mTrail[tID]);
	}
	void destroyRibbonTrail(unsigned short tID)
	{
		if(mTrailHead[tID] && mSceneMgr->hasBillboardSet("FXTrailHead"+StringConverter::toString(tID)))
		{
			mTrailHeadEnt[tID]->detachObjectFromBone(mTrailHead[tID]);
			mSceneMgr->destroyBillboardSet(mTrailHead[tID]);
			mTrailHead[tID] = 0;
			mTrailHeadEnt[tID] = 0;
		}
		if(mTrail[tID] && mSceneMgr->hasRibbonTrail("FXTrail"+StringConverter::toString(tID)))
		{
			mSceneMgr->getRootSceneNode()->detachObject(mTrail[tID]);
			mSceneMgr->destroyRibbonTrail(mTrail[tID]);
			mTrail[tID] = 0;
		}
	}
	ParticleSystem* createParticle(SceneNode *node, const String &name, const Real &duration, bool unresettable=false, bool attachToBone=false, bool hasOwnNode=false, const Vector3 &position=Vector3::ZERO, const String &ownName="")
	{
		short tID = -1;
		for(int i=0;i<MAX_PARTICLES;i++)
		{
			if(mParticle[i]==0)
			{
				tID = i;
				break;
			}
		}
		if(tID==-1)
		{
			tID = 0;
			destroyParticle(tID);
		}

		mParticle[tID] = mSceneMgr->createParticleSystem(ownName==""?"FXParticle"+StringConverter::toString(tID):ownName,name);
		mParticleNode[tID] = (hasOwnNode?mSceneMgr->getRootSceneNode()->createChildSceneNode("FXParticleNode"+StringConverter::toString(tID)):node);
		if(hasOwnNode)mParticleNode[tID]->setPosition(position);
		if(!attachToBone)node->attachObject(mParticle[tID]);
		particleDuration[tID] = duration;
		particleUnresettable[tID] = unresettable;
		particleHasOwnNode[tID] = hasOwnNode;
		particleType[tID] = name;

		return mParticle[tID];
	}
	void destroyParticle(unsigned short tID)
	{
		if(mParticle[tID]/* && mSceneMgr->hasParticleSystem("FXParticle"+StringConverter::toString(tID))*/)
		{
			mParticle[tID]->detachFromParent();
			mSceneMgr->destroyParticleSystem(mParticle[tID]);
			mParticle[tID] = 0;
			if(particleHasOwnNode[tID])mSceneMgr->destroySceneNode(mParticleNode[tID]);
			mParticleNode[tID] = 0;
			particleDuration[tID] = 0;
			particleUnresettable[tID] = false;
			particleHasOwnNode[tID] = false;
			particleType[tID] = "";
		}
	}
	void destroyParticle(ParticleSystem *pSys)
	{
		for(int i=0;i<MAX_PARTICLES;i++)
		{
			if(mParticle[i]==pSys){destroyParticle(i); return;}
		}
	}
	void destroyParticle(const String &name)
	{
		for(int i=0;i<MAX_PARTICLES;i++)
		{
			if(mParticle[i]&&mParticle[i]->getName()==name){destroyParticle(i); return;}
		}
	}
	ParticleSystem* getParticle(const String &name)
	{
		for(int i=0;i<MAX_PARTICLES;i++)
		{
			if(mParticle[i]&&mParticle[i]->getName()==name){return mParticle[i];}
		}
		return 0;
	}
	void destroyRibbonTrailByEntity(Entity *ent)
	{
		for(int i=0;i<MAX_RIBBONTRAILS;i++)
		{
			if(mTrailHeadEnt[i]==ent)destroyRibbonTrail(i);
		}
	}
	void destroyParticleByObjectNode(SceneNode *node, const String &type="")
	{
		for(int i=0;i<MAX_PARTICLES;i++)
		{
			if(mParticleNode[i]==node&&(type==""||particleType[i]==type))destroyParticle(i);
		}
	}
	void stopParticleByObjectNode(SceneNode *node, const String &type="")
	{
		for(int i=0;i<MAX_PARTICLES;i++)
		{
			if(mParticleNode[i]==node&&(type==""||particleType[i]==type))
			{
				mParticle[i]->getEmitter(0)->setEmissionRate(0);
				particleDuration[i] = 0;
			}
		}
	}
	void createWaterRipple(SceneNode *owner, const Real &waterHeight, const Real &scale)
	{
		short tID = -1;
		for(int i=0;i<MAX_WATERRIPPLES;i++)
		{
			if(mWaterRipple[i]==0)
			{
				tID = i;
				break;
			}
		}
		if(tID==-1)
		{
			tID = 0;
			destroyWaterRipple(tID);
		}

		mWaterRipple[tID] = mSceneMgr->createParticleSystem("FXWaterRipple"+StringConverter::toString(tID),"WaterRipple");
		mWaterRipple[tID]->setDefaultDimensions(scale,scale);
		mWaterRippleNode[tID] = mSceneMgr->getRootSceneNode()->createChildSceneNode("WaterRippleNode"+StringConverter::toString(tID));
		mWaterRippleNode[tID]->attachObject(mWaterRipple[tID]);
		mWaterRippleOwner[tID] = owner;
		mWaterRippleNode[tID]->setPosition(owner->getPosition().x,waterHeight,owner->getPosition().z);
	}
	void destroyWaterRipple(unsigned short tID)
	{
		if(mWaterRipple[tID] && mSceneMgr->hasParticleSystem("FXWaterRipple"+StringConverter::toString(tID)))
		{
			mWaterRippleNode[tID]->detachObject(mWaterRipple[tID]);
			mSceneMgr->destroyParticleSystem(mWaterRipple[tID]);
			mSceneMgr->destroySceneNode("WaterRippleNode"+StringConverter::toString(tID));
			mWaterRipple[tID] = 0;
			mWaterRippleNode[tID] = 0;
			mWaterRippleOwner[tID] = 0;
		}
	}
	void destroyWaterRippleByOwner(SceneNode *node)
	{
		for(int i=0;i<MAX_WATERRIPPLES;i++)
		{
			if(mWaterRippleOwner[i]==node)destroyWaterRipple(i);
		}
	}
	void setWaterRippleEmissionByOwner(SceneNode *node, const Real &rate)
	{
		for(int i=0;i<MAX_WATERRIPPLES;i++)
		{
			if(mWaterRipple[i] && mWaterRippleOwner[i]==node)
			{
				mWaterRipple[i]->getEmitter(0)->setEmissionRate(rate);
				return;
			}
		}
	}
	void setWaterRippleHeightByOwner(SceneNode *node, const Real &height)
	{
		for(int i=0;i<MAX_WATERRIPPLES;i++)
		{
			if(mWaterRipple[i] && mWaterRippleOwner[i]==node)
			{
				mWaterRippleNode[i]->setPosition(node->getPosition().x,height,node->getPosition().z);
				return;
			}
		}
	}
	bool hasWaterRippleOwner(SceneNode *node)
	{
		for(int i=0;i<MAX_WATERRIPPLES;i++)
		{
			if(mWaterRippleOwner[i]==node)return true;
		}
		return false;
	}
	void createBreather(Entity *owner, const String &type, const Vector3 &offset, const Real &scale)
	{
		short tID = -1;
		for(int i=0;i<MAX_BREATHERS;i++)
		{
			if(mBreather[i]==0)
			{
				tID = i;
				break;
			}
		}
		if(tID==-1)
		{
			tID = 0;
			destroyBreather(tID);
		}

		mBreather[tID] = mSceneMgr->createParticleSystem("FXBreather"+StringConverter::toString(tID),type);
		mBreather[tID]->setDefaultDimensions(scale,scale);
		//TagPoint *tTP = owner->attachObjectToBone("Mouth",mBreather[tID],Quaternion::IDENTITY,offset);
		SceneNode* tBreatherNode = owner->getParentSceneNode()->createChildSceneNode("BreatherNode"+StringConverter::toString(tID));
		tBreatherNode->attachObject(mBreather[tID]);
		mBreatherOwner[tID] = owner;
		breatherType[tID] = type;
		breatherOffset[tID] = offset;
	}
	void destroyBreather(unsigned short tID)
	{
		if(mBreather[tID] && mSceneMgr->hasParticleSystem("FXBreather"+StringConverter::toString(tID)))
		{
			SceneNode* tBreatherNode = mSceneMgr->getSceneNode("BreatherNode"+StringConverter::toString(tID));
			if(!tBreatherNode)return;
			//mBreatherOwner[tID]->detachObjectFromBone(mBreather[tID]);
			mSceneMgr->destroyParticleSystem(mBreather[tID]);
			mSceneMgr->destroySceneNode("BreatherNode"+StringConverter::toString(tID));
			mBreather[tID] = 0;
			mBreatherOwner[tID] = 0;
			breatherType[tID] = "";
			breatherOffset[tID] = Vector3::ZERO;
		}
	}
	void destroyBreatherByOwner(Entity *ent, const String &type="")
	{
		for(int i=0;i<MAX_BREATHERS;i++)
		{
			if(mBreatherOwner[i]==ent && (type=="" || breatherType[i]==type))destroyBreather(i);
		}
	}
	void setBreatherEmissionByOwner(Entity *ent, const Real &rate, const String &type="")
	{
		for(int i=0;i<MAX_BREATHERS;i++)
		{
			if(mBreather[i] && mBreatherOwner[i]==ent && (type=="" || breatherType[i]==type))
			{
				mBreather[i]->getEmitter(0)->setEmissionRate(rate);
			}
		}
	}
	bool hasBreatherOwner(Entity *ent, const String &type="")
	{
		for(int i=0;i<MAX_BREATHERS;i++)
		{
			if(mBreatherOwner[i]==ent && (type=="" || breatherType[i]==type))return true;
		}
		return false;
	}
	void createItemParticle(SceneNode *node)
	{
		short tID = -1;
		for(int i=0;i<MAX_ITEMSPARKLE;i++)
		{
			if(mItemParticle[i]==0)
			{
				tID = i;
				break;
			}
		}
		if(tID==-1)
		{
			tID = 0;
			destroyItemParticle(tID);
		}

		mItemParticle[tID] = mSceneMgr->createParticleSystem("ItemParticle"+StringConverter::toString(tID),"item");
		mItemParticleNode[tID] = node;
		node->attachObject(mItemParticle[tID]);
	}
	void destroyItemParticle(unsigned short tID)
	{
		if(mItemParticle[tID] && mSceneMgr->hasParticleSystem("ItemParticle"+StringConverter::toString(tID)))
		{
			mItemParticleNode[tID]->detachObject(mItemParticle[tID]);
			mSceneMgr->destroyParticleSystem(mItemParticle[tID]);
			mItemParticle[tID] = 0;
			mItemParticleNode[tID] = 0;
		}
	}
	void destroyItemParticleByObjectNode(SceneNode *node)
	{
		for(int i=0;i<MAX_ITEMSPARKLE;i++)
		{
			if(mItemParticleNode[i]==node)destroyItemParticle(i);
		}
	}
	void createItemEffect(Entity *owner, const String &type, const String &bone, const Vector3 &offset, const Real &scale)
	{
		short tID = -1;
		for(int i=0;i<MAX_ITEMEFFECTS;i++)
		{
			if(mItemEffect[i]==0)
			{
				tID = i;
				break;
			}
		}
		if(tID==-1)
		{
			tID = 0;
			destroyItemEffect(tID);
		}

		mItemEffect[tID] = mSceneMgr->createParticleSystem("ItemFX"+StringConverter::toString(tID),type);
		mItemEffect[tID]->setDefaultDimensions(mItemEffect[tID]->getDefaultWidth()*scale,mItemEffect[tID]->getDefaultHeight()*scale);
		SceneNode* tItemEffectNode = owner->getParentSceneNode()->createChildSceneNode("ItemFXNode"+StringConverter::toString(tID));
		tItemEffectNode->attachObject(mItemEffect[tID]);
		mItemEffectOwner[tID] = owner;
		itemEffectBone[tID] = bone;
		itemEffectType[tID] = type;
		itemEffectOffset[tID] = offset;
		//No bone to attach to, apply offset directly
		if(bone=="")tItemEffectNode->setPosition(offset);
	}
	void destroyItemEffect(unsigned short tID)
	{
		if(mItemEffect[tID] && mSceneMgr->hasParticleSystem("ItemFX"+StringConverter::toString(tID)))
		{
			SceneNode* tItemEffectNode = mSceneMgr->getSceneNode("ItemFXNode"+StringConverter::toString(tID));
			if(!tItemEffectNode)return;
			mSceneMgr->destroyParticleSystem(mItemEffect[tID]);
			mSceneMgr->destroySceneNode("ItemFXNode"+StringConverter::toString(tID));
			mItemEffect[tID] = 0;
			mItemEffectOwner[tID] = 0;
			itemEffectBone[tID] = "";
			itemEffectType[tID] = "";
			itemEffectOffset[tID] = Vector3::ZERO;
		}
	}
	void destroyItemEffectByOwnerNode(SceneNode *node, const String &type="")
	{
		for(int i=0;i<MAX_ITEMEFFECTS;i++)
		{
			if(mItemEffectOwner[i])
			if(mItemEffectOwner[i]->getParentSceneNode()==node && (type=="" || itemEffectType[i]==type))destroyItemEffect(i);
		}
	}
};

#endif