#ifndef __MagixUnit_h_
#define __MagixUnit_h_

#include "MagixObject.h"
#include "MovableTextOverlay.h"

#define MAX_JUMPHEIGHT 10
#define EMOTE_TRANSITIONTIME 1.0

enum TARGET_ACTION
{
	TARGET_NONE,
	TARGET_RUNTO,
	TARGET_LOOKAT,
	TARGET_RUNTOPICKUP
};

struct AllocFlags
{
	bool mapEffect;
	bool soundWaterWade;
	bool soundWalkGrass;
	bool soundRunGrass;
	bool needsSoundRoar;
	bool hasOwnGroundHeight;
	AllocFlags()
	{
		clear();
	}
	void clear()
	{
		mapEffect = false;
		soundWaterWade = false;
		soundWalkGrass = false;
		soundRunGrass = false;
		needsSoundRoar = false;
		hasOwnGroundHeight = false;
	}
};

using namespace Ogre;

class MagixUnit : public MagixLiving
{
protected:
	Entity *mBodyEnt;
	Entity *mHeadEnt;
	Entity *mManeEnt;
	Entity *mTailEnt;
	Entity *mWingEnt;
	Entity *mTuftEnt;
	Entity *mEquipEnt[MAX_EQUIP];
	String matGroupName;
	unsigned char bodyMarkID;
	unsigned char headMarkID;
	unsigned char tailMarkID;
	String animBase;
	String prevAnimBase;
	Real animSpeed;
	Real animCount;
	String animSecond;
	String prevAnimSecond;
	Real animSecondCount;
	Real animSecondSpeed;
	short blinkFlag;
	short idleTurnFlag;
	bool freeLook;
	bool freeLookTimeout;
	unsigned short unitID;
	Vector3 targetVector;
	Vector3 lookTargetVector;
	Quaternion lookDirection;
	Quaternion lookDirectionBuffer;
	unsigned char targetAction;
	MagixObject *mAutoTrackTarget;
	MovableTextOverlay *mNameText;
	MovableTextOverlay *mUserText;
	bool nameTagVisible;
	bool userTagVisible;
	MovableTextOverlay *mChatText;
	bool chatBubbleVisible;
	Real chatBubbleCount;
	String emoteMode;
	String prevEmoteMode;
	Real emoteCount;
	Real eyelidState;
	bool jump;
	bool isWalking;
	bool isStance;
	bool isCustomAnimation;
	bool isSwimming;
	bool isCrouching;
	String attackName;
	String nextAttackName;
	Real attackTimeout;
	bool hasNewAttack;
	bool hasStoppedAttack;
	Real hitTimeout;
	vector<const String>::type attackList;
	unsigned short attackCounter;
	Real headTurnCount;
	bool isInWater;
	bool hasDoubleJumped;
	String lipSync;
	String lipSyncAnim;
	String prevLipSyncAnim;
	Real lipSyncSpeed;
	Real lipSyncCounter;
public:
	AllocFlags allocFlags;
	MagixUnit()
	{
		mBodyEnt = 0;
		mHeadEnt = 0;
		mManeEnt = 0;
		mTailEnt = 0;
		mWingEnt = 0;
		mTuftEnt = 0;
		for(int i=0;i<MAX_EQUIP;i++)mEquipEnt[i] = 0;
		matGroupName = "";
		bodyMarkID = 0;
		headMarkID = 0;
		tailMarkID = 0;
		animBase = "";
		prevAnimBase = "";
		animSpeed = 0;
		animCount = 0;
		animSecond = "";
		prevAnimSecond = "";
		animSecondCount = 0;
		blinkFlag = 0;
		idleTurnFlag = 0;
		freeLook = true;
		freeLookTimeout = false;
		unitID = 0;
		targetVector = Vector3(0,0,0);
		lookTargetVector = Vector3(0,0,0);
		lookDirection = Quaternion(Degree(0),Vector3::UNIT_Y);
		lookDirectionBuffer = Quaternion(Degree(0),Vector3::UNIT_Y);
		targetAction = 0;
		mAutoTrackTarget = 0;
		mNameText = 0;
		mUserText = 0;
		nameTagVisible = false;
		userTagVisible = false;
		mChatText = 0;
		chatBubbleVisible = false;
		chatBubbleCount = 0;
		emoteMode = "";
		prevEmoteMode = "";
		emoteCount = 0;
		eyelidState = 0;
		jump = false;
		isWalking = false;
		isStance = false;
		isCustomAnimation = false;
		isSwimming = false;
		isCrouching = false;
		attackName = "";
		nextAttackName = "";
		attackTimeout = 0;
		hasNewAttack = false;
		hasStoppedAttack = false;
		hitTimeout = 0;
		attackList.clear();
		attackCounter = 0;
		headTurnCount = 0;
		isInWater = 0;
		hasDoubleJumped = false;
		lipSync = "";
		lipSyncAnim = "";
		prevLipSyncAnim = "";
		lipSyncSpeed = 0;
		lipSyncCounter = 0;
	}
	~MagixUnit()
	{
		//if(mNameText)delete mNameText;
		//if(mChatText)delete mChatText;
	}
	void createUnit(const unsigned short iID, SceneManager *sceneMgr, const String &bodyMesh, const String &headMesh, const String &maneMesh, const String &tailMesh, const String &wingMesh, const String &matName, const String &tuftMesh)
	{
		if(unitID)destroyUnit(sceneMgr);
		unitID = iID;

		mBodyEnt = sceneMgr->createEntity("Body"+StringConverter::toString(iID), bodyMesh+".mesh");
		mHeadEnt = sceneMgr->createEntity("Head"+StringConverter::toString(iID), headMesh+".mesh");
		if(maneMesh!="Maneless")mManeEnt = sceneMgr->createEntity("Mane"+StringConverter::toString(iID), maneMesh+".mesh");
		mTailEnt = sceneMgr->createEntity("Tail"+StringConverter::toString(iID), tailMesh+".mesh");
		if(wingMesh!="Wingless")mWingEnt = sceneMgr->createEntity("Wing"+StringConverter::toString(iID), wingMesh+".mesh");
		if(tuftMesh!="Tuftless")mTuftEnt = sceneMgr->createEntity("Tuft"+StringConverter::toString(iID), tuftMesh+".mesh");

		setMaterial(matName);
		bodyMarkID = 0;
		headMarkID = 0;
		tailMarkID = 0;

		mObjectNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
		mObjectNode->attachObject(mBodyEnt);
		mObjectNode->attachObject(mHeadEnt);
		if(mManeEnt)mObjectNode->attachObject(mManeEnt);
		mObjectNode->attachObject(mTailEnt);
		if(mWingEnt)mObjectNode->attachObject(mWingEnt);
		if(mTuftEnt)mObjectNode->attachObject(mTuftEnt);
		mObjectNode->setScale(1,1,1);

		mBodyEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		mHeadEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		if(mManeEnt)mManeEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		mTailEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		if(mWingEnt)mWingEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		if(mTuftEnt)mTuftEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

		mBodyEnt->getSkeleton()->getBlendMode();
		mHeadEnt->getSkeleton()->getBlendMode();
		if(mManeEnt)mManeEnt->getSkeleton()->getBlendMode();
		mTailEnt->getSkeleton()->getBlendMode();
		if(mWingEnt)mWingEnt->getSkeleton()->getBlendMode();
		if(mTuftEnt)mTuftEnt->getSkeleton()->getBlendMode();

		mBodyEnt->setQueryFlags(UNIT_MASK);
		mHeadEnt->setQueryFlags(UNIT_MASK);
		if(mManeEnt)mManeEnt->setQueryFlags(UNIT_MASK);
		mTailEnt->setQueryFlags(UNIT_MASK);
		if(mWingEnt)mWingEnt->setQueryFlags(UNIT_MASK);
		if(mTuftEnt)mTuftEnt->setQueryFlags(UNIT_MASK);
//mBodyEnt->getParentSceneNode()->showBoundingBox(true);
		animBase = "";
		prevAnimBase = "";
		animSpeed = 1;
		animCount = 0;
		animSecond = "";
		prevAnimSecond = "";
		animSecondCount = 0;
		emoteMode = "Normal";
		prevEmoteMode = "";
		emoteCount = 0.001;
		headTurnCount = 0;
	}
	void destroyUnit(SceneManager *sceneMgr)
	{
		if(!unitID)return;

		if(mObjectNode)
		{
			mObjectNode->detachAllObjects();
			sceneMgr->destroySceneNode(mObjectNode->getName());
		}
		if(mBodyEnt)mBodyEnt->detachAllObjectsFromBone();
		if(mHeadEnt)mHeadEnt->detachAllObjectsFromBone();
		if(mManeEnt)mManeEnt->detachAllObjectsFromBone();
		if(mTailEnt)mTailEnt->detachAllObjectsFromBone();
		if(mWingEnt)mWingEnt->detachAllObjectsFromBone();
		if(mTuftEnt)mTuftEnt->detachAllObjectsFromBone();
		for(int i=0;i<MAX_EQUIP;i++)
		{
			if(mEquipEnt[i] && sceneMgr->hasEntity("Equip"+StringConverter::toString(i)+"_"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mEquipEnt[i]);
			mEquipEnt[i] = 0;
		}
		if(mBodyEnt && sceneMgr->hasEntity("Body"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mBodyEnt);
		if(mHeadEnt && sceneMgr->hasEntity("Head"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mHeadEnt);
		if(mManeEnt && sceneMgr->hasEntity("Mane"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mManeEnt);
		if(mTailEnt && sceneMgr->hasEntity("Tail"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mTailEnt);
		if(mWingEnt && sceneMgr->hasEntity("Wing"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mWingEnt);
		if(mTuftEnt && sceneMgr->hasEntity("Tuft"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mTuftEnt);

		MaterialPtr tMat = MaterialManager::getSingleton().getByName("DynamicBodyMat"+StringConverter::toString(unitID));
		if(!tMat.isNull())MaterialManager::getSingleton().remove(tMat->getHandle());
		tMat = MaterialManager::getSingleton().getByName("DynamicHeadMat"+StringConverter::toString(unitID));
		if(!tMat.isNull())MaterialManager::getSingleton().remove(tMat->getHandle());
		tMat = MaterialManager::getSingleton().getByName("DynamicTailMat"+StringConverter::toString(unitID));
		if(!tMat.isNull())MaterialManager::getSingleton().remove(tMat->getHandle());
		tMat = MaterialManager::getSingleton().getByName("DynamicManeMat"+StringConverter::toString(unitID));
		if(!tMat.isNull())MaterialManager::getSingleton().remove(tMat->getHandle());
		tMat = MaterialManager::getSingleton().getByName("DynamicEyeMat"+StringConverter::toString(unitID));
		if(!tMat.isNull())MaterialManager::getSingleton().remove(tMat->getHandle());
		tMat = MaterialManager::getSingleton().getByName("DynamicWingMat"+StringConverter::toString(unitID));
		if(!tMat.isNull())MaterialManager::getSingleton().remove(tMat->getHandle());

		if(mNameText)
		{
			delete mNameText;
			mNameText = 0;
		}
		if(mUserText)
		{
			delete mUserText;
			mUserText = 0;
		}
		if(mChatText)
		{
			delete mChatText;
			mChatText = 0;
		}

		unitID = 0;
		mObjectNode = 0;
		mBodyEnt = 0;
		mHeadEnt = 0;
		mManeEnt = 0;
		mTailEnt = 0;
		mWingEnt = 0;
		mTuftEnt = 0;
		matGroupName = "";
		blinkFlag = 0;
		eyelidState = 0;
		idleTurnFlag = 0;
		jump = false;
		isWalking = false;
		isStance = false;
		isCustomAnimation = false;
		isSwimming = false;
		isCrouching = false;
		attackName = "";
		nextAttackName = "";
		attackTimeout = 0;
		hasNewAttack = false;
		hasStoppedAttack = false;
		hitTimeout = 0;
		alliance = 0;
		attackList.clear();
		attackCounter = 0;
		isHidden = false;
		isInWater = false;
		hasDoubleJumped = false;
		lipSync = "";
		lipSyncAnim = "";
		prevLipSyncAnim = "";
		lipSyncSpeed = 1;
		lipSyncCounter = 0;
	}
	void setMaterial(const String &matName)
	{
		if(!unitID)return;
		matGroupName = matName;
		if(matGroupName=="CustomMat")return;

		const String tBodyMat = matName + "/BodyMat";
		const String tHeadMat = matName + "/HeadMat";
		const String tEyeMat = matName + "/EyeMat";
		const String tTeethMat = matName + "/TeethMat";
		const String tManeMat = matName + "/ManeMat";
		const String tTailMat = matName + "/TailMat";
		const String tWingMat = matName + "/WingMat";

		mBodyEnt->setMaterialName(tBodyMat);
		mHeadEnt->getSubEntity(0)->setMaterialName(tHeadMat);
		mHeadEnt->getSubEntity(1)->setMaterialName(tTeethMat);
		mHeadEnt->getSubEntity(2)->setMaterialName(tEyeMat);
		if(mManeEnt)mManeEnt->setMaterialName(tManeMat);
		mTailEnt->setMaterialName(tTailMat);
		if(mWingEnt)mWingEnt->setMaterialName(tWingMat);
		if(mTuftEnt)mTuftEnt->setMaterialName(tManeMat);
	}
	void setColours(const ColourValue &pelt, const ColourValue &underfur, const ColourValue &mane, const ColourValue &nose, const ColourValue &eyeTop, const ColourValue &eyeBot, const ColourValue &ears, const ColourValue &tailtip, const ColourValue &eyes, const ColourValue &wing, const ColourValue &marking)
	{
		matGroupName = "CustomMat";

		const Vector4 peltVect(pelt.r,pelt.g,pelt.b,1);
		const Vector4 underfurVect(underfur.r,underfur.g,underfur.b,1);
		const Vector4 maneVect(mane.r,mane.g,mane.b,1);
		const Vector4 noseVect(nose.r,nose.g,nose.b,1);
		const Vector4 eyeTopVect(eyeTop.r,eyeTop.g,eyeTop.b,1);
		const Vector4 eyeBotVect(eyeBot.r,eyeBot.g,eyeBot.b,1);
		const Vector4 earsVect(ears.r,ears.g,ears.b,1);
		const Vector4 tailtipVect(tailtip.r,tailtip.g,tailtip.b,1);
		const Vector4 eyesVect(eyes.r,eyes.g,eyes.b,1);
		const Vector4 wingVect(wing.r,wing.g,wing.b,1);
		const Vector4 markingVect(marking.r,marking.g,marking.b,1);

		String headMeshID = mHeadEnt->getMesh()->getName();
		headMeshID.erase(0,4);
		headMeshID.erase(headMeshID.find_first_of("."),5);
		//Common headID mods
		if(headMeshID=="5")headMeshID = "1";
		else if(headMeshID=="6")headMeshID = "3";

		String wingMeshID = "";
		if(mWingEnt)
		{
			wingMeshID = mWingEnt->getMesh()->getName();
			if(wingMeshID!="Wings.mesh")
			{
				wingMeshID.erase(0,5);
				wingMeshID.erase(wingMeshID.find_first_of("."),5);
			}
			else wingMeshID = "";
		}
		const String tBodyMat = (bodyMarkID==0?"BodyModColour":"BodyMark"+StringConverter::toString(bodyMarkID));
		const String tHeadMat = (headMarkID==0?"HeadModColour"+headMeshID:"HeadMark"+StringConverter::toString(headMarkID)+"_"+headMeshID);
		const String tTailMat = (tailMarkID==0?"TailModColour":"TailMark"+StringConverter::toString(tailMarkID));
		const String tWingMat = "WingModColour"+wingMeshID;

		mBodyEnt->setMaterialName(tBodyMat);
		mHeadEnt->getSubEntity(0)->setMaterialName(tHeadMat);
		mHeadEnt->getSubEntity(1)->setMaterialName("TeethMat/TWOSIDE");
		mHeadEnt->getSubEntity(2)->setMaterialName("EyeModColour");
		if(mManeEnt)mManeEnt->setMaterialName("ManeModColour");
		mTailEnt->setMaterialName(tTailMat);
		if(mWingEnt)mWingEnt->setMaterialName(tWingMat);
		if(mTuftEnt)mTuftEnt->setMaterialName("ManeModColour");

		mBodyEnt->getSubEntity(0)->setCustomParameter(1,peltVect); //pelt
		mBodyEnt->getSubEntity(0)->setCustomParameter(2,underfurVect); //underfur
		if(bodyMarkID!=0)mBodyEnt->getSubEntity(0)->setCustomParameter(3,markingVect); //body markings
		mHeadEnt->getSubEntity(0)->setCustomParameter(1,peltVect); //pelt(head)
		mHeadEnt->getSubEntity(0)->setCustomParameter(2,underfurVect); //underfur(head)
		if(mManeEnt)mManeEnt->getSubEntity(0)->setCustomParameter(1,maneVect); //mane
		mHeadEnt->getSubEntity(0)->setCustomParameter(3,noseVect); //nose
		mHeadEnt->getSubEntity(0)->setCustomParameter(4,eyeTopVect); //around eyes top
		mHeadEnt->getSubEntity(0)->setCustomParameter(5,eyeBotVect); //around eyes bottom
		mHeadEnt->getSubEntity(0)->setCustomParameter(6,Vector4(0,0,0,1)); //eyebrows
		mHeadEnt->getSubEntity(0)->setCustomParameter(7,earsVect); //inside ears
		if(headMarkID!=0)mHeadEnt->getSubEntity(0)->setCustomParameter(8,markingVect); //head markings
		mTailEnt->getSubEntity(0)->setCustomParameter(1,peltVect); //pelt(tail)
		mTailEnt->getSubEntity(0)->setCustomParameter(2,tailtipVect); //tailtip
		if(tailMarkID!=0)mTailEnt->getSubEntity(0)->setCustomParameter(3,markingVect); //tail markings
		mHeadEnt->getSubEntity(2)->setCustomParameter(1,eyeTopVect); //eyelid
		mHeadEnt->getSubEntity(2)->setCustomParameter(2,eyesVect); //eyes
		if(mWingEnt)mWingEnt->getSubEntity(0)->setCustomParameter(1,wingVect); //wings
		if(mTuftEnt)mTuftEnt->getSubEntity(0)->setCustomParameter(1,maneVect); //tuft

		#pragma warning(push)
		#pragma warning(disable : 4482)
		//If fragment programs are not supported
		if(mBodyEnt->getSubEntity(0)->getTechnique()->getName()=="2")
		{
			MaterialPtr tMat = MaterialManager::getSingleton().getByName("DynamicBodyMat"+StringConverter::toString(unitID));
			if(!tMat.isNull())MaterialManager::getSingleton().remove(tMat->getHandle());
			static_cast<MaterialPtr>(MaterialManager::getSingleton().getByName(tBodyMat))->clone("DynamicBodyMat"+StringConverter::toString(unitID));
			mBodyEnt->setMaterialName("DynamicBodyMat"+StringConverter::toString(unitID));
			mBodyEnt->getSubEntity(0)->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(peltVect.x,
																															peltVect.y,
																															peltVect.z));
			mBodyEnt->getSubEntity(0)->getTechnique()->getPass(1)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(underfurVect.x,
																															underfurVect.y,
																															underfurVect.z));
			if(bodyMarkID!=0)
			mBodyEnt->getSubEntity(0)->getTechnique()->getPass(3)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(markingVect.x,
																															markingVect.y,
																															markingVect.z));
			if(mManeEnt || mTuftEnt)
			{
				if(MaterialManager::getSingleton().getByName("DynamicManeMat"+StringConverter::toString(unitID)).isNull())
					static_cast<MaterialPtr>(MaterialManager::getSingleton().getByName("ManeModColour"))->clone("DynamicManeMat"+StringConverter::toString(unitID));
				if(mManeEnt)
				{
					mManeEnt->setMaterialName("DynamicManeMat"+StringConverter::toString(unitID));
					mManeEnt->getSubEntity(0)->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
																													LayerBlendSource::LBS_MANUAL,
																													LayerBlendSource::LBS_CURRENT,
																													ColourValue(maneVect.x,
																																maneVect.y,
																																maneVect.z));
				}
				if(mTuftEnt)
				{
					mTuftEnt->setMaterialName("DynamicManeMat"+StringConverter::toString(unitID));
					mTuftEnt->getSubEntity(0)->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
																													LayerBlendSource::LBS_MANUAL,
																													LayerBlendSource::LBS_CURRENT,
																													ColourValue(maneVect.x,
																																maneVect.y,
																																maneVect.z));
				}
			}
			tMat = MaterialManager::getSingleton().getByName("DynamicTailMat"+StringConverter::toString(unitID));
			if(!tMat.isNull())MaterialManager::getSingleton().remove(tMat->getHandle());
			static_cast<MaterialPtr>(MaterialManager::getSingleton().getByName(tTailMat))->clone("DynamicTailMat"+StringConverter::toString(unitID));
			mTailEnt->setMaterialName("DynamicTailMat"+StringConverter::toString(unitID));
			mTailEnt->getSubEntity(0)->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(peltVect.x,
																															peltVect.y,
																															peltVect.z));
			mTailEnt->getSubEntity(0)->getTechnique()->getPass(1)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(tailtipVect.x,
																															tailtipVect.y,
																															tailtipVect.z));
			if(tailMarkID!=0)
			mTailEnt->getSubEntity(0)->getTechnique()->getPass(2)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(markingVect.x,
																															markingVect.y,
																															markingVect.z));
			if(MaterialManager::getSingleton().getByName("DynamicEyeMat"+StringConverter::toString(unitID)).isNull())
				static_cast<MaterialPtr>(MaterialManager::getSingleton().getByName("EyeModColour"))->clone("DynamicEyeMat"+StringConverter::toString(unitID));
			mHeadEnt->getSubEntity(2)->setMaterialName("DynamicEyeMat"+StringConverter::toString(unitID));
			mHeadEnt->getSubEntity(2)->getTechnique()->getPass(1)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(eyeTopVect.x,
																															eyeTopVect.y,
																															eyeTopVect.z));
			mHeadEnt->getSubEntity(2)->getTechnique()->getPass(2)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(eyesVect.x,
																															eyesVect.y,
																															eyesVect.z));
			tMat = MaterialManager::getSingleton().getByName("DynamicHeadMat"+StringConverter::toString(unitID));
			if(!tMat.isNull())MaterialManager::getSingleton().remove(tMat->getHandle());
			static_cast<MaterialPtr>(MaterialManager::getSingleton().getByName(tHeadMat))->clone("DynamicHeadMat"+StringConverter::toString(unitID));
			mHeadEnt->getSubEntity(0)->setMaterialName("DynamicHeadMat"+StringConverter::toString(unitID));
			mHeadEnt->getSubEntity(0)->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(peltVect.x,
																															peltVect.y,
																															peltVect.z));
			mHeadEnt->getSubEntity(0)->getTechnique()->getPass(1)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(underfurVect.x,
																															underfurVect.y,
																															underfurVect.z));
			mHeadEnt->getSubEntity(0)->getTechnique()->getPass(3)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(noseVect.x,
																															noseVect.y,
																															noseVect.z));
			mHeadEnt->getSubEntity(0)->getTechnique()->getPass(4)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(eyeTopVect.x,
																															eyeTopVect.y,
																															eyeTopVect.z));
			mHeadEnt->getSubEntity(0)->getTechnique()->getPass(5)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(eyeBotVect.x,
																															eyeBotVect.y,
																															eyeBotVect.z));
			mHeadEnt->getSubEntity(0)->getTechnique()->getPass(7)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(earsVect.x,
																															earsVect.y,
																															earsVect.z));
			if(headMarkID!=0)
			mHeadEnt->getSubEntity(0)->getTechnique()->getPass(8)->getTextureUnitState(1)->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_DIFFUSE_ALPHA,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												ColourValue(markingVect.x,
																															markingVect.y,
																															markingVect.z));
			if(mWingEnt)
			{
				tMat = MaterialManager::getSingleton().getByName("DynamicWingMat"+StringConverter::toString(unitID));
				if(!tMat.isNull())MaterialManager::getSingleton().remove(tMat->getHandle());
				static_cast<MaterialPtr>(MaterialManager::getSingleton().getByName(tWingMat))->clone("DynamicWingMat"+StringConverter::toString(unitID));
				mWingEnt->setMaterialName("DynamicWingMat"+StringConverter::toString(unitID));
				mWingEnt->getSubEntity(0)->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_MODULATE,
																													LayerBlendSource::LBS_MANUAL,
																													LayerBlendSource::LBS_TEXTURE,
																													ColourValue(wingVect.x,
																																wingVect.y,
																																wingVect.z));
			}
		}
		#pragma warning(pop)
	}
	void setFullColour(const ColourValue &fullColour)
	{
		matGroupName = "CustomFullMat";

		mBodyEnt->setMaterialName("FullModColour");
		mHeadEnt->setMaterialName("FullModColour");
		mHeadEnt->getSubEntity(0)->setMaterialName("FullModColour");
		mHeadEnt->getSubEntity(1)->setMaterialName("FullModColour");
		mHeadEnt->getSubEntity(2)->setMaterialName("FullModColour");
		if(mManeEnt)mManeEnt->setMaterialName("FullModColour");
		mTailEnt->setMaterialName("FullModColour");
		if(mWingEnt)mWingEnt->setMaterialName("FullModColour");
		if(mTuftEnt)mTuftEnt->setMaterialName("FullModColour");

		mBodyEnt->getSubEntity(0)->setCustomParameter(1,Vector4(fullColour.r,fullColour.g,fullColour.b,fullColour.a));
		mHeadEnt->getSubEntity(0)->setCustomParameter(1,Vector4(fullColour.r,fullColour.g,fullColour.b,fullColour.a));
		mHeadEnt->getSubEntity(1)->setCustomParameter(1,Vector4(fullColour.r,fullColour.g,fullColour.b,fullColour.a));
		mHeadEnt->getSubEntity(2)->setCustomParameter(1,Vector4(fullColour.r,fullColour.g,fullColour.b,fullColour.a));
		if(mManeEnt)mManeEnt->getSubEntity(0)->setCustomParameter(1,Vector4(fullColour.r,fullColour.g,fullColour.b,fullColour.a));
		mTailEnt->getSubEntity(0)->setCustomParameter(1,Vector4(fullColour.r,fullColour.g,fullColour.b,fullColour.a));
		if(mWingEnt)mWingEnt->getSubEntity(0)->setCustomParameter(1,Vector4(fullColour.r,fullColour.g,fullColour.b,fullColour.a));
		if(mTuftEnt)mTuftEnt->getSubEntity(0)->setCustomParameter(1,Vector4(fullColour.r,fullColour.g,fullColour.b,fullColour.a));

		#pragma warning(push)
		#pragma warning(disable : 4482)
		//If fragment programs are not supported
		if(mBodyEnt->getSubEntity(0)->getTechnique()->getName()=="2")
		{
			mBodyEnt->getSubEntity(0)->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												fullColour);
			mBodyEnt->getSubEntity(0)->getTechnique()->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LayerBlendOperationEx::LBX_SOURCE1,
																												LayerBlendSource::LBS_MANUAL,
																												LayerBlendSource::LBS_CURRENT,
																												fullColour.a);
		}
		#pragma warning(pop)
	}
	void updateAnimation(const FrameEvent &evt, MagixExternalDefinitions *def)
	{
		updateAnimBase(evt);
		updateAnimSecond(evt);
		updateEmote(evt,def);
		updateLipSync(evt);

		//Blinkblink!
		if(blinkFlag == 2)
		{
			if(!mHeadEnt->getAnimationState("Blink")->getEnabled())
			{
				mHeadEnt->getAnimationState("Blink")->setEnabled(true);
				mHeadEnt->getAnimationState("Blink")->setLoop(false);
			}
			//Transition from eye < eyelidState to eyelidState
			if(mHeadEnt->getAnimationState("Blink")->getTimePosition()<eyelidState*mHeadEnt->getAnimationState("Blink")->getLength())
			{
				mHeadEnt->getAnimationState("Blink")->addTime(evt.timeSinceLastFrame*1.5);
				if(mHeadEnt->getAnimationState("Blink")->getTimePosition()>=eyelidState*mHeadEnt->getAnimationState("Blink")->getLength())
				{
					mHeadEnt->getAnimationState("Blink")->setTimePosition(eyelidState*mHeadEnt->getAnimationState("Blink")->getLength());
					blinkFlag = 0;
				}
			}
			//Transition from eye > eyelidState to eyelidState
			else if(mHeadEnt->getAnimationState("Blink")->getTimePosition()>eyelidState*mHeadEnt->getAnimationState("Blink")->getLength())
			{
				mHeadEnt->getAnimationState("Blink")->addTime(evt.timeSinceLastFrame*-1.5);
				if(mHeadEnt->getAnimationState("Blink")->getTimePosition()<=eyelidState*mHeadEnt->getAnimationState("Blink")->getLength())
				{
					mHeadEnt->getAnimationState("Blink")->setTimePosition(eyelidState*mHeadEnt->getAnimationState("Blink")->getLength());
					blinkFlag = 0;
				}
			}
			//eye == eyelideState, no transition needed
			else blinkFlag = 0;
		}
		else if(blinkFlag != 0)
		{
			if(!mHeadEnt->getAnimationState("Blink")->getEnabled())
			{
				mHeadEnt->getAnimationState("Blink")->setEnabled(true);
				mHeadEnt->getAnimationState("Blink")->setLoop(false);
			}

			if(mHeadEnt->getAnimationState("Blink")->getTimePosition()>=eyelidState*mHeadEnt->getAnimationState("Blink")->getLength())
				mHeadEnt->getAnimationState("Blink")->addTime(blinkFlag*evt.timeSinceLastFrame*1.5);
			if(blinkFlag==1 && mHeadEnt->getAnimationState("Blink")->hasEnded())blinkFlag = -1;
			else if(blinkFlag==-1 && mHeadEnt->getAnimationState("Blink")->getTimePosition()<=eyelidState*mHeadEnt->getAnimationState("Blink")->getLength())
			{
				blinkFlag = 0;
				if(eyelidState!=0)mHeadEnt->getAnimationState("Blink")->setTimePosition(eyelidState*mHeadEnt->getAnimationState("Blink")->getLength());
				else mHeadEnt->getAnimationState("Blink")->setEnabled(false);
			}
		}
		else
		{
			if(Math::RangeRandom(0,100)>99)blinkFlag = 1;
		}

		//Random head turning during idle
		if(!freeLook)
		{
			if(idleTurnFlag!=0)
			{
				String tTurnAnim = ((idleTurnFlag==1||idleTurnFlag==-1)?"TurnLeft":"TurnRight");
				Real tRandomSpeed = Math::RangeRandom(0.1,0.5);

				if(idleTurnFlag>0 && (mBodyEnt->getAnimationState(tTurnAnim)->hasEnded() || animBase!="Idle"))
				{
					idleTurnFlag *= -1;
				}

				mBodyEnt->getAnimationState(tTurnAnim)->addTime((idleTurnFlag<0?-1:1) * evt.timeSinceLastFrame * tRandomSpeed);
				mHeadEnt->getAnimationState(tTurnAnim)->addTime((idleTurnFlag<0?-1:1) * evt.timeSinceLastFrame * tRandomSpeed);
				if(mManeEnt)mManeEnt->getAnimationState(tTurnAnim)->addTime((idleTurnFlag<0?-1:1) * evt.timeSinceLastFrame * tRandomSpeed);
				if(mTuftEnt)mTuftEnt->getAnimationState(tTurnAnim)->addTime((idleTurnFlag<0?-1:1) * evt.timeSinceLastFrame * tRandomSpeed);

				if(mBodyEnt->getAnimationState(tTurnAnim)->getTimePosition()<=0)
				{
					mBodyEnt->getAnimationState(tTurnAnim)->setEnabled(false);
					mHeadEnt->getAnimationState(tTurnAnim)->setEnabled(false);
					if(mManeEnt)mManeEnt->getAnimationState(tTurnAnim)->setEnabled(false);
					if(mTuftEnt)mTuftEnt->getAnimationState(tTurnAnim)->setEnabled(false);
					idleTurnFlag = 0;
				}
			}
			else if(animBase == "Idle" && Math::UnitRandom()>0.997)
			{
				idleTurnFlag = (Math::RangeRandom(0,10)>5?1:2);
				String tTurnAnim = (idleTurnFlag==1?"TurnLeft":"TurnRight");
				
				mBodyEnt->getAnimationState(tTurnAnim)->setEnabled(true);
				mBodyEnt->getAnimationState(tTurnAnim)->setLoop(false);
				mBodyEnt->getAnimationState(tTurnAnim)->setTimePosition(0);
				mHeadEnt->getAnimationState(tTurnAnim)->setEnabled(true);
				mHeadEnt->getAnimationState(tTurnAnim)->setLoop(false);
				mHeadEnt->getAnimationState(tTurnAnim)->setTimePosition(0);
				if(mManeEnt)
				{
					mManeEnt->getAnimationState(tTurnAnim)->setEnabled(true);
					mManeEnt->getAnimationState(tTurnAnim)->setLoop(false);
					mManeEnt->getAnimationState(tTurnAnim)->setTimePosition(0);
				}
				if(mTuftEnt)
				{
					mTuftEnt->getAnimationState(tTurnAnim)->setEnabled(true);
					mTuftEnt->getAnimationState(tTurnAnim)->setLoop(false);
					mTuftEnt->getAnimationState(tTurnAnim)->setTimePosition(0);
				}
			}
		}

		//Turn head to look at lookDirection
		if(freeLook)
		{
			//if freeLook disabled, return to facing front
			if(freeLookTimeout)lookDirection = mObjectNode->getOrientation()*Quaternion(0,0,1,0);

			//pan lookDirectionBuffer to lookDirection
			if(lookDirectionBuffer.equals(lookDirection,Degree(0.25)))lookDirectionBuffer = lookDirection;
			else if(5*evt.timeSinceLastFrame<1)lookDirectionBuffer = lookDirectionBuffer.nlerp(5*evt.timeSinceLastFrame,lookDirectionBuffer,lookDirection,true);
			else lookDirectionBuffer = lookDirection;

			//if lookDirectionBuffer returns to front, end freelook
			if(freeLookTimeout && lookDirectionBuffer == lookDirection)
			{
				freeLook = false;
				freeLookTimeout = false;
			}

			Real tFaceAngle = (mObjectNode->getOrientation().getYaw().valueDegrees()>0 ? mObjectNode->getOrientation().getYaw().valueDegrees() : (360+mObjectNode->getOrientation().getYaw().valueDegrees()) );
			Real tLookAngle = 180 + lookDirectionBuffer.getYaw().valueDegrees();
			Real tTurnRatio = 0;
			Real tTurnEyes = 0;
			String tTurnAnim = "";
			//avoid "ecstacy head swing"
			if(Math::Abs(180-Math::Abs(tLookAngle-tFaceAngle))<1)tFaceAngle += 1;
			//because the world is round
			if(Math::Abs(tLookAngle-tFaceAngle)<=180)
			{
				tTurnRatio =  Math::Abs(tLookAngle - tFaceAngle)/90;
				tTurnAnim = ((tLookAngle-tFaceAngle)>=0?"TurnLeft":"TurnRight");
			}
			else
			{
				tTurnRatio =  Math::Abs((tLookAngle-(tLookAngle>180?360:0)) - (tFaceAngle-(tFaceAngle>180?360:0)))/90;
				tTurnAnim = ((tFaceAngle-tLookAngle)>=0?"TurnLeft":"TurnRight");
			}

			//Flip left/right if upside down :|
			if(isRolledOver())tTurnAnim = (tTurnAnim=="TurnLeft"?"TurnRight":"TurnLeft");

			String tOppTurn = (tTurnAnim=="TurnLeft"?"TurnRight":"TurnLeft");
			
			if(tTurnRatio>1)
			{
				tTurnEyes = tTurnRatio - 1;
				if(tTurnEyes>1)tTurnEyes = 1;
				tTurnRatio = 1;
			}
			
			//Disable opposite turn animation, enable current turn animation and set time position at turn ratio
			if(mBodyEnt->getAnimationState(tOppTurn)->getEnabled())
			{
				//mBodyEnt->getAnimationState(tOppTurn)->setEnabled(false);
				headTurnCount = mBodyEnt->getAnimationState(tOppTurn)->getTimePosition()/(mBodyEnt->getAnimationState(tOppTurn)->getLength()*0.99);
			}
			if(headTurnCount>0)
			{
				headTurnCount -= evt.timeSinceLastFrame;
				if(headTurnCount<=0)
				{
					headTurnCount = 0;
					mBodyEnt->getAnimationState(tOppTurn)->setEnabled(false);
					mHeadEnt->getAnimationState(tOppTurn)->setEnabled(false);
					if(mManeEnt)mManeEnt->getAnimationState(tOppTurn)->setEnabled(false);
					if(mTuftEnt)mTuftEnt->getAnimationState(tOppTurn)->setEnabled(false);
				}
				else
				{
					mBodyEnt->getAnimationState(tOppTurn)->setTimePosition(headTurnCount*(mBodyEnt->getAnimationState(tOppTurn)->getLength()*0.99));
					mHeadEnt->getAnimationState(tOppTurn)->setTimePosition(headTurnCount*(mHeadEnt->getAnimationState(tOppTurn)->getLength()*0.99));
					if(mManeEnt)mManeEnt->getAnimationState(tOppTurn)->setTimePosition(headTurnCount*(mManeEnt->getAnimationState(tOppTurn)->getLength()*0.99));
					if(mTuftEnt)mTuftEnt->getAnimationState(tOppTurn)->setTimePosition(headTurnCount*(mTuftEnt->getAnimationState(tOppTurn)->getLength()*0.99));
				}
			}
			mBodyEnt->getAnimationState(tTurnAnim)->setEnabled(true);
			mBodyEnt->getAnimationState(tTurnAnim)->setTimePosition((1-headTurnCount)*tTurnRatio*(mBodyEnt->getAnimationState(tTurnAnim)->getLength()*0.99) );
			//if(mHeadEnt->getAnimationState(tOppTurn)->getEnabled())mHeadEnt->getAnimationState(tOppTurn)->setEnabled(false);
			mHeadEnt->getAnimationState(tTurnAnim)->setEnabled(true);
			mHeadEnt->getAnimationState(tTurnAnim)->setTimePosition((1-headTurnCount)*tTurnRatio*(mHeadEnt->getAnimationState(tTurnAnim)->getLength()*0.99) );
			if(mManeEnt)
			{
				//if(mManeEnt->getAnimationState(tOppTurn)->getEnabled())mManeEnt->getAnimationState(tOppTurn)->setEnabled(false);
				mManeEnt->getAnimationState(tTurnAnim)->setEnabled(true);
				mManeEnt->getAnimationState(tTurnAnim)->setTimePosition((1-headTurnCount)*tTurnRatio*(mManeEnt->getAnimationState(tTurnAnim)->getLength()*0.99) );
			}
			if(mTuftEnt)
			{
				//if(mManeEnt->getAnimationState(tOppTurn)->getEnabled())mManeEnt->getAnimationState(tOppTurn)->setEnabled(false);
				mTuftEnt->getAnimationState(tTurnAnim)->setEnabled(true);
				mTuftEnt->getAnimationState(tTurnAnim)->setTimePosition((1-headTurnCount)*tTurnRatio*(mTuftEnt->getAnimationState(tTurnAnim)->getLength()*0.99) );
			}

			//For first person look mode
			if(tTurnEyes>0 && getControlMode()==CONTROL_FIRSTPERSON)
			{
				yaw(Degree(90*tTurnEyes*( !isRolledOver() ? (tTurnAnim=="TurnLeft"?1:-1) : (tTurnAnim=="TurnLeft"?-1:1) )),true);
				tTurnEyes = 0;
			}

			if(tTurnEyes>0)
			{
				tTurnAnim = (tTurnAnim=="TurnLeft"?"LookLeft":"LookRight");
				tOppTurn = (tOppTurn=="TurnLeft"?"LookLeft":"LookRight");

				if(mHeadEnt->getAnimationState(tOppTurn)->getEnabled())mHeadEnt->getAnimationState(tOppTurn)->setEnabled(false);
				mHeadEnt->getAnimationState(tTurnAnim)->setEnabled(true);
				mHeadEnt->getAnimationState(tTurnAnim)->setTimePosition(tTurnEyes*(mHeadEnt->getAnimationState(tTurnAnim)->getLength()*0.99) );
			}
			else
			{
				if(mHeadEnt->getAnimationState("LookLeft")->getEnabled())mHeadEnt->getAnimationState("LookLeft")->setEnabled(false);
				if(mHeadEnt->getAnimationState("LookRight")->getEnabled())mHeadEnt->getAnimationState("LookRight")->setEnabled(false);
			}

			tLookAngle = lookDirectionBuffer.getPitch().valueDegrees();
			tFaceAngle = (mObjectNode->getOrientation().getPitch().valueDegrees()>0 ? mObjectNode->getOrientation().getPitch().valueDegrees() : (360+mObjectNode->getOrientation().getPitch().valueDegrees()) ) - 360;
			tTurnRatio = 0;
			tTurnEyes = 0;
			tTurnAnim = "";
			//because the world is round
			if(Math::Abs(tLookAngle-tFaceAngle)<180)
			{
				tTurnRatio =  Math::Abs(tLookAngle - tFaceAngle)/45;
				tTurnAnim = ((tLookAngle-tFaceAngle)>=0?"TurnUp":"TurnDown");
			}
			else
			{
				tTurnRatio =  Math::Abs((tLookAngle-(tLookAngle>180?360:0)) - (tFaceAngle-(tFaceAngle>180?360:0)))/45;
				tTurnAnim = ((tFaceAngle-tLookAngle)>=0?"TurnUp":"TurnDown");
			}
			tOppTurn = (tTurnAnim=="TurnUp"?"TurnDown":"TurnUp");
			
			if(tTurnRatio>1)
			{
				tTurnEyes = tTurnRatio - 1;
				if(tTurnEyes>1)tTurnEyes = 1;
				tTurnRatio = 1;
			}
			
			//Disable opposite turn animation, enable current turn animation and set time position at turn ratio
			if(mBodyEnt->getAnimationState(tOppTurn)->getEnabled())mBodyEnt->getAnimationState(tOppTurn)->setEnabled(false);
			mBodyEnt->getAnimationState(tTurnAnim)->setEnabled(true);
			mBodyEnt->getAnimationState(tTurnAnim)->setTimePosition(tTurnRatio*(mBodyEnt->getAnimationState(tTurnAnim)->getLength()*0.99) );
			if(mHeadEnt->getAnimationState(tOppTurn)->getEnabled())mHeadEnt->getAnimationState(tOppTurn)->setEnabled(false);
			mHeadEnt->getAnimationState(tTurnAnim)->setEnabled(true);
			mHeadEnt->getAnimationState(tTurnAnim)->setTimePosition(tTurnRatio*(mHeadEnt->getAnimationState(tTurnAnim)->getLength()*0.99) );
			if(mManeEnt)
			{
				if(mManeEnt->getAnimationState(tOppTurn)->getEnabled())mManeEnt->getAnimationState(tOppTurn)->setEnabled(false);
				mManeEnt->getAnimationState(tTurnAnim)->setEnabled(true);
				mManeEnt->getAnimationState(tTurnAnim)->setTimePosition(tTurnRatio*(mManeEnt->getAnimationState(tTurnAnim)->getLength()*0.99) );
			}
			if(mTuftEnt)
			{
				if(mTuftEnt->getAnimationState(tOppTurn)->getEnabled())mTuftEnt->getAnimationState(tOppTurn)->setEnabled(false);
				mTuftEnt->getAnimationState(tTurnAnim)->setEnabled(true);
				mTuftEnt->getAnimationState(tTurnAnim)->setTimePosition(tTurnRatio*(mTuftEnt->getAnimationState(tTurnAnim)->getLength()*0.99) );
			}

			if(tTurnEyes>0)
			{
				tTurnAnim = (tTurnAnim=="TurnUp"?"LookUp":"LookDown");
				tOppTurn = (tOppTurn=="TurnUp"?"LookUp":"LookDown");

				if(mHeadEnt->getAnimationState(tOppTurn)->getEnabled())mHeadEnt->getAnimationState(tOppTurn)->setEnabled(false);
				mHeadEnt->getAnimationState(tTurnAnim)->setEnabled(true);
				mHeadEnt->getAnimationState(tTurnAnim)->setTimePosition(tTurnEyes*(mHeadEnt->getAnimationState(tTurnAnim)->getLength()*0.99) );
			}
			else
			{
				if(mHeadEnt->getAnimationState("LookUp")->getEnabled())mHeadEnt->getAnimationState("LookUp")->setEnabled(false);
				if(mHeadEnt->getAnimationState("LookDown")->getEnabled())mHeadEnt->getAnimationState("LookDown")->setEnabled(false);
			}
		}
	}
	void updateAnimBase(const FrameEvent &evt)
	{
		if(animBase!="")
		{
			mBodyEnt->getAnimationState(animBase)->addTime(animSpeed*evt.timeSinceLastFrame);
			mHeadEnt->getAnimationState(animBase)->addTime(animSpeed*evt.timeSinceLastFrame);
			if(mManeEnt)mManeEnt->getAnimationState(animBase)->addTime(animSpeed*evt.timeSinceLastFrame);
			mTailEnt->getAnimationState(animBase)->addTime(animSpeed*evt.timeSinceLastFrame);
			if(mWingEnt)mWingEnt->getAnimationState(animBase)->addTime(animSpeed*evt.timeSinceLastFrame);
			if(mTuftEnt)mTuftEnt->getAnimationState(animBase)->addTime(animSpeed*evt.timeSinceLastFrame);
		}
		if(animCount>0)
		{
			animCount -= evt.timeSinceLastFrame;
			if(animCount<=0)
			{
				animCount = 0;
				if(prevAnimBase!="")
				{
					mBodyEnt->getAnimationState(prevAnimBase)->setEnabled(false);
					mHeadEnt->getAnimationState(prevAnimBase)->setEnabled(false);
					if(mManeEnt)mManeEnt->getAnimationState(prevAnimBase)->setEnabled(false);
					mTailEnt->getAnimationState(prevAnimBase)->setEnabled(false);
					if(mWingEnt)mWingEnt->getAnimationState(prevAnimBase)->setEnabled(false);
					if(mTuftEnt)mTuftEnt->getAnimationState(prevAnimBase)->setEnabled(false);
				}
				if(animBase!="")
				{
					mBodyEnt->getAnimationState(animBase)->setWeight(1);
					mHeadEnt->getAnimationState(animBase)->setWeight(1);
					if(mManeEnt)mManeEnt->getAnimationState(animBase)->setWeight(1);
					mTailEnt->getAnimationState(animBase)->setWeight(1);
					if(mWingEnt)mWingEnt->getAnimationState(animBase)->setWeight(1);
					if(mTuftEnt)mTuftEnt->getAnimationState(animBase)->setWeight(1);
				}
			}
			else
			{
				if(prevAnimBase!="")
				{
					mBodyEnt->getAnimationState(prevAnimBase)->addTime(animSpeed*evt.timeSinceLastFrame);
					mHeadEnt->getAnimationState(prevAnimBase)->addTime(animSpeed*evt.timeSinceLastFrame);
					if(mManeEnt)mManeEnt->getAnimationState(prevAnimBase)->addTime(animSpeed*evt.timeSinceLastFrame);
					mTailEnt->getAnimationState(prevAnimBase)->addTime(animSpeed*evt.timeSinceLastFrame);
					if(mWingEnt)mWingEnt->getAnimationState(prevAnimBase)->addTime(animSpeed*evt.timeSinceLastFrame);
					if(mTuftEnt)mTuftEnt->getAnimationState(prevAnimBase)->addTime(animSpeed*evt.timeSinceLastFrame);

					mBodyEnt->getAnimationState(prevAnimBase)->setWeight(animCount/0.1);
					mHeadEnt->getAnimationState(prevAnimBase)->setWeight(animCount/0.1);
					if(mManeEnt)mManeEnt->getAnimationState(prevAnimBase)->setWeight(animCount/0.1);
					mTailEnt->getAnimationState(prevAnimBase)->setWeight(animCount/0.1);
					if(mWingEnt)mWingEnt->getAnimationState(prevAnimBase)->setWeight(animCount/0.1);
					if(mTuftEnt)mTuftEnt->getAnimationState(prevAnimBase)->setWeight(animCount/0.1);
				}
				if(animBase!="")
				{
					mBodyEnt->getAnimationState(animBase)->setWeight((0.1-animCount)/0.1);
					mHeadEnt->getAnimationState(animBase)->setWeight((0.1-animCount)/0.1);
					if(mManeEnt)mManeEnt->getAnimationState(animBase)->setWeight((0.1-animCount)/0.1);
					mTailEnt->getAnimationState(animBase)->setWeight((0.1-animCount)/0.1);
					if(mWingEnt)mWingEnt->getAnimationState(animBase)->setWeight((0.1-animCount)/0.1);
					if(mTuftEnt)mTuftEnt->getAnimationState(animBase)->setWeight((0.1-animCount)/0.1);
				}
			}
		}
	}
	void updateAnimSecond(const FrameEvent &evt)
	{
		if(animSecond!="")
		{
			mBodyEnt->getAnimationState(animSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);
			mHeadEnt->getAnimationState(animSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);
			if(mManeEnt)mManeEnt->getAnimationState(animSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);
			mTailEnt->getAnimationState(animSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);
			if(mWingEnt)mWingEnt->getAnimationState(animSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);
			if(mTuftEnt)mTuftEnt->getAnimationState(animSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);
		}
		if(animSecondCount>0)
		{
			animSecondCount -= evt.timeSinceLastFrame;
			if(animSecondCount<=0)
			{
				animSecondCount = 0;
				if(prevAnimSecond!="")
				{
					mBodyEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
					mHeadEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
					if(mManeEnt)mManeEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
					mTailEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
					if(mWingEnt)mWingEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
					if(mTuftEnt)mTuftEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
				}
				if(animSecond!="")
				{
					mBodyEnt->getAnimationState(animSecond)->setWeight(1);
					mHeadEnt->getAnimationState(animSecond)->setWeight(1);
					if(mManeEnt)mManeEnt->getAnimationState(animSecond)->setWeight(1);
					mTailEnt->getAnimationState(animSecond)->setWeight(1);
					if(mWingEnt)mWingEnt->getAnimationState(animSecond)->setWeight(1);
					if(mTuftEnt)mTuftEnt->getAnimationState(animSecond)->setWeight(1);
				}
			}
			else
			{
				if(prevAnimSecond!="")
				{
					mBodyEnt->getAnimationState(prevAnimSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);
					mHeadEnt->getAnimationState(prevAnimSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);
					if(mManeEnt)mManeEnt->getAnimationState(prevAnimSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);
					mTailEnt->getAnimationState(prevAnimSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);
					if(mWingEnt)mWingEnt->getAnimationState(prevAnimSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);
					if(mTuftEnt)mTuftEnt->getAnimationState(prevAnimSecond)->addTime(animSecondSpeed*evt.timeSinceLastFrame);

					mBodyEnt->getAnimationState(prevAnimSecond)->setWeight(animSecondCount/0.1);
					mHeadEnt->getAnimationState(prevAnimSecond)->setWeight(animSecondCount/0.1);
					if(mManeEnt)mManeEnt->getAnimationState(prevAnimSecond)->setWeight(animSecondCount/0.1);
					mTailEnt->getAnimationState(prevAnimSecond)->setWeight(animSecondCount/0.1);
					if(mWingEnt)mWingEnt->getAnimationState(prevAnimSecond)->setWeight(animSecondCount/0.1);
					if(mTuftEnt)mTuftEnt->getAnimationState(prevAnimSecond)->setWeight(animSecondCount/0.1);
				}
				if(animSecond!="")
				{
					mBodyEnt->getAnimationState(animSecond)->setWeight((0.1-animSecondCount)/0.1);
					mHeadEnt->getAnimationState(animSecond)->setWeight((0.1-animSecondCount)/0.1);
					if(mManeEnt)mManeEnt->getAnimationState(animSecond)->setWeight((0.1-animSecondCount)/0.1);
					mTailEnt->getAnimationState(animSecond)->setWeight((0.1-animSecondCount)/0.1);
					if(mWingEnt)mWingEnt->getAnimationState(animSecond)->setWeight((0.1-animSecondCount)/0.1);
					if(mTuftEnt)mTuftEnt->getAnimationState(animSecond)->setWeight((0.1-animSecondCount)/0.1);
				}
			}
		}
	}
	virtual void updateMovement(const FrameEvent &evt)
	{
		if(isDisabled())return;

		//Jump
		if(jump && distanceFromGround()<(MAX_JUMPHEIGHT*hasDoubleJumped?2:1) && mForce.y>=0)
			addForce(Vector3(0,GRAVITY*evt.timeSinceLastFrame + (distanceFromGround()<=GROUND_THRESHOLD?2:0),0));
		//Doublejump
		if(hasDoubleJumped && distanceFromGround()<=GROUND_THRESHOLD)hasDoubleJumped = false;

		//Runto
		if(targetAction != TARGET_RUNTO && targetAction != TARGET_RUNTOPICKUP)return;
		
		//targetVector.y = mObjectNode->getPosition().y;
		const Vector3 tPosition = mObjectNode->getPosition();

		Real tDistance = tPosition.squaredDistance(targetVector);

		//Runto target
		if(tDistance>0)
		{
			//Lateral movement
			const Real tX = targetVector.x - tPosition.x;
			const Real tZ = targetVector.z - tPosition.z;

			tDistance = Vector2(tX,tZ).squaredLength();
			const Real tSpeedRatio = maxSpeed*0.01;
		
			if(tDistance>( isCrouching?0.5:(isWalking?1:4)*Math::Sqr(tSpeedRatio) ))
			{
				Radian tYaw = Degree(0);
				if(tZ!=0)tYaw = Math::ATan(tX/tZ);
				else tYaw = Degree(tX>=0?90:270);
				if(tX<0)tYaw += Degree(360);
				if(tZ<0)tYaw += Degree(180);

				if(tYaw<Degree(0))tYaw += Degree(360);
				if(tYaw>=Degree(360))tYaw -= Degree(360);

				Vector2 tVect = Vector2(0,0);
				if(tYaw==Degree(0)||tYaw==Degree(180))
				{
					tVect.y = 10 * (tYaw==Degree(180)?-1:1);
				}
				else if(tYaw==Degree(90)||tYaw==Degree(270))
				{
					tVect.x = 10 * (tYaw==Degree(270)?-1:1);
				}
				else
				{
					tVect.y = 10 * Math::Cos(tYaw);
					tVect.x = 10 * Math::Sin(tYaw);
				}
				addForce(Vector3(tVect.x*evt.timeSinceLastFrame,0,tVect.y*evt.timeSinceLastFrame));
			}
			else if(tDistance>0)
			{
				lastPosition = tPosition;
				mObjectNode->setPosition(targetVector.x,mObjectNode->getPosition().y,targetVector.z);
				mForce.x = 0;
				mForce.z = 0;
				tDistance = 0;
			}

			//Vertical movement
			Real tYDistance = 0;
			if(antiGravity)
			{
				tYDistance = targetVector.y - tPosition.y;
				if(Math::Abs(tYDistance)>1)
				{
					addForce(Vector3(0,5*(tYDistance>0?1:-1)*evt.timeSinceLastFrame,0));
				}
				else if(Math::Abs(tYDistance)>0)
				{
					lastPosition = tPosition;
					mObjectNode->setPosition(mObjectNode->getPosition().x,targetVector.y,mObjectNode->getPosition().z);
					mForce.y = 0;
					tYDistance = 0;
				}
			}

			//Stop action
			if(tDistance==0 && tYDistance==0 && targetAction!=TARGET_RUNTOPICKUP)targetAction = 0;

			return;
		}
		//Stop running
		else if(targetAction!=TARGET_RUNTOPICKUP)
		{
			targetAction = 0;
		}
	}
	void updateAction(const FrameEvent &evt, MagixExternalDefinitions *def)
	{
		if(isCustomAnimation)return;

		//Actions for AnimSecond
		if(isOneTimeHeadAction() && mHeadEnt->getAnimationState(animSecond)->hasEnded())changeAnimSecond("",animSecondSpeed);
		if(isOneTimeTailAction() && mTailEnt->getAnimationState(animSecond)->hasEnded())changeAnimSecond("",animSecondSpeed);
		
		if(isLick() && animSecondCount>0 && emoteMode!="SoftLick")setEmote("SoftLick",def);
		else if(prevAnimSecond=="Lick" && animSecondCount>0 && emoteMode=="SoftLick")setEmote(isStance?"Anger":"Normal",def);
		else if(isLaugh() && animSecondCount>0 && emoteMode!="SneakySmirk")setEmote("SneakySmirk",def);
		else if(prevAnimSecond=="Laugh" && !(isLaugh()||isRofling()) && animSecondCount>0 && emoteMode=="SneakySmirk")setEmote(isStance?"Anger":"Normal",def);
		else if(isChuckle() && animSecondCount>0 && emoteMode!="Smirk")setEmote("Smirk",def);
		else if(prevAnimSecond=="Chuckle" && !isChuckle() && animSecondCount>0 && emoteMode=="Smirk")setEmote(isStance?"Anger":"Normal",def);
		else if(isRofling() && animCount>0 && emoteMode!="SneakySmirk")setEmote("SneakySmirk",def);
		else if(prevAnimBase=="Rofl" && !(isLaugh()||isRofling()) && animCount>0 && emoteMode=="SneakySmirk")setEmote(isStance?"Anger":"Normal",def);
		else if(isRoaring() && animCount>0 && emoteMode!="Roar")setEmote("Roar",def);
		else if(prevAnimBase=="Roar" && animCount>0 && emoteMode=="Roar")setEmote(isStance?"Anger":"Normal",def);
		else if(isFainting() && animCount>0 && emoteMode!="Dead")setEmote("Dead",def);
		

		//Actions for AnimBase
		//Hit
		if(hitTimeout>0)
		{
			speedMultiplier = 1;
			hitTimeout -= evt.timeSinceLastFrame;
			//changeAnimBase("Hit", 1, animBase!="Hit");
			if(hitTimeout<0)
			{
				hitTimeout = 0;
				if(emoteMode=="Pain")setEmote(isStance?"Anger":"Normal",def);
			}
			return;
		}
		//Death
		if(hitTimeout<0)
		{
			speedMultiplier = 1;
			hitTimeout += evt.timeSinceLastFrame;
			if(hitTimeout>-0.01)
			{
				hitTimeout = -0.01;
			}
			return;
		}
		//Attack
		if(updateAttack(evt,def))return;

		speedMultiplier = 1;
		//Swim
		if(isSwimming)
		{
			speedMultiplier = (isMovingReverse()?0.25:1);
			changeAnimBase("Run", 0.5, animBase!="Run");
			return;
		}

		//Hover
		if(antiGravity)
		{
			speedMultiplier = 1;
			changeAnimBase("Hover", 0.8, animBase!="Hover");
			return;
		}

		//Jump
		if(distanceFromGround()>0)
		{
			speedMultiplier = 1;
			if(!hasDoubleJumped || mBodyEnt->getAnimationState("Attack5")->hasEnded() || animBase!="Attack5")changeAnimBase("Run", 0.5, animBase!="Run");
		}
		else
		{
			Real tSpeed = Math::Abs(Vector2(mForce.x,mForce.z).squaredLength());
			bool bReverse = isMovingReverse();

			//if(isPositionChanged() && tSpeed==0)tSpeed = 0.001;
			if(isCrouching)speedMultiplier = 0.15;
			else if(isWalking||bReverse)speedMultiplier = 0.25;

			//Run
			if(tSpeed>0 && !bReverse && !isWalking && !isCrouching)
			{
				Real tAnimSpeed = (evt.timeSinceLastFrame==0? 1.0 : tSpeed/ (30000*evt.timeSinceLastFrame));
				if(tAnimSpeed>1.0)tAnimSpeed = 1.0;
				changeAnimBase("Run", 1.5 + tAnimSpeed, animBase!="Run");
			}
			//Crawl
			else if(tSpeed>0 && isCrouching)
			{
				changeAnimBase("Crawl", 1, animBase!="Crawl", bReverse);
			}
			//Walk
			else if(tSpeed>0 && (isWalking||bReverse))
			{
				changeAnimBase("Walk", 1.18, animBase!="Walk", bReverse);
			}
			//Sit transition
			else if(animBase=="SitTransition")
			{
				if(mBodyEnt->getAnimationState("SitTransition")->hasEnded())changeAnimBase("SitIdle", 0.1, true);
			}
			//Lay transition
			else if(animBase=="LayTransition")
			{
				if(mBodyEnt->getAnimationState("LayTransition")->hasEnded())changeAnimBase("LayIdle", 0.1, true);
			}
			//SideLay transition
			else if(animBase=="LayTransition2")
			{
				if(mBodyEnt->getAnimationState("LayTransition2")->hasEnded())changeAnimBase("LayIdle2", 0.1, true);
			}
			//Lean transition
			else if(animBase=="LayTransition3" || isFainting())
			{
				if(mBodyEnt->getAnimationState(animBase)->hasEnded())changeAnimBase("LayIdle3", 0.07, true);
			}
			//Rolledover transition
			else if(animBase=="LayTransition4" || isRofling())
			{
				if(mBodyEnt->getAnimationState(animBase)->hasEnded())changeAnimBase("LayIdle4", 0.07, true);
			}
			//Plop transition
			else if(animBase=="PlopTransition")
			{
				if(mBodyEnt->getAnimationState("PlopTransition")->hasEnded())changeAnimBase("PlopIdle", 0.07, true);
			}
			//Curl transition
			else if(animBase=="CurlTransition")
			{
				if(mBodyEnt->getAnimationState("CurlTransition")->hasEnded())changeAnimBase("CurlIdle", 0.07, true);
			}
			//Crouch transition
			else if(animBase=="CrouchTransition")
			{
				if(isCrouching && mBodyEnt->getAnimationState("CrouchTransition")->hasEnded())changeAnimBase("Crouch", 0.1, true);
				else if(!isCrouching && mBodyEnt->getAnimationState("CrouchTransition")->getTimePosition()<=0)changeAnimBase("Idle", 0.05, true);
			}
			//Crouch idle
			else if(isCrouching && !isSitting() && !isLaying() && !isSideLaying() && !isPlopped() && !isRolledOver() && !isLeaning() && !isCurled())
			{
				changeAnimBase("Crouch", 0.1, animBase!="Crouch");
			}
			//Stance transition
			else if(animBase=="StanceTransition")
			{
				if(isStance && mBodyEnt->getAnimationState("StanceTransition")->hasEnded())changeAnimBase("Stance", 0.3, true);
				else if(!isStance && mBodyEnt->getAnimationState("StanceTransition")->getTimePosition()<=0)changeAnimBase("Idle", 0.05, true);
			}
			//Stance idle
			else if(isStance && !isSitting() && !isLaying() && !isSideLaying() && !isPlopped() && !isRolledOver() && !isLeaning() && !isCurled() && !isDancing() && !isOneTimeAction())
			{
				changeAnimBase("Stance", 0.3, animBase!="Stance");
			}
			//Idle
			else if(animBase!="Idle" && 
					animBase!="SitIdle" &&
					animBase!="LayIdle" &&
					animBase!="LayIdle2" &&
					animBase!="LayIdle3" &&
					animBase!="LayIdle4" &&
					animBase!="PlopIdle" &&
					animBase!="CurlIdle" &&
					!isDancing() &&
					(!isOneTimeAction() || mBodyEnt->getAnimationState(animBase)->hasEnded()))
				changeAnimBase("Idle", 0.05, true);
		}
	}
	void doSit(bool skipTransition=false)
	{
		if(isDisabled()||isAttacking())return;
		if(isSideLaying() || isLeaning() || isRolledOver() || isCurled())return;
		if(!isSitting() && !isLaying() && !isPlopped())
		{
			if(skipTransition)changeAnimBase("SitIdle", 0.1, true);
			else changeAnimBase("SitTransition",1,true,false,false);
		}
		else if(isSitting(true))
		{
			if(skipTransition)changeAnimBase("LayIdle", 0.1, true);
			else changeAnimBase("LayTransition",1,true,false,false);
		}
		else if(isLaying(true))
		{
			if(skipTransition)changeAnimBase("PlopIdle", 0.07, true);
			else changeAnimBase("PlopTransition",1,true,false,false);
		}
	}
	bool isSitting(bool excludeTransition=false)
	{
		if(excludeTransition)return (animBase=="SitIdle");
		return (animBase=="SitTransition"||animBase=="SitIdle");
	}
	void doSideLay(bool skipTransition=false)
	{
		if(isDisabled()||isAttacking())return;
		if(isSitting(true))
		{
			if(skipTransition)changeAnimBase("SitIdle", 0.1, true);
			else changeAnimBase("LayTransition2",1,true,false,false);
		}
		else if(isSideLaying(true))
		{
			if(skipTransition)changeAnimBase("LayIdle3", 0.07, true);
			else changeAnimBase("LayTransition3",1,true,false,false);
		}
		else if(isLeaning(true))
		{
			if(skipTransition)changeAnimBase("LayIdle4", 0.07, true);
			else changeAnimBase("LayTransition4",1,true,false,false);
		}
	}
	void doCurl(bool skipTransition=false)
	{
		if(isDisabled()||isAttacking())return;
		if(isCurled())return;
		if(isSideLaying(true))
		{
			if(skipTransition)changeAnimBase("CurlIdle", 0.07, true);
			else changeAnimBase("CurlTransition",1,true,false,false);
		}
		else if(!isSitting())doSit(skipTransition);
		else doSideLay(skipTransition);
	}
	bool isLaying(bool excludeTransition=false)
	{
		if(excludeTransition)return (animBase=="LayIdle");
		return (animBase=="LayTransition"||animBase=="LayIdle");
	}
	bool isSideLaying(bool excludeTransition=false)
	{
		if(excludeTransition)return (animBase=="LayIdle2");
		return (animBase=="LayTransition2"||animBase=="LayIdle2");
	}
	bool isLeaning(bool excludeTransition=false)
	{
		if(excludeTransition)return (animBase=="LayIdle3");
		return (animBase=="LayTransition3"||animBase=="LayIdle3"||isFainting());
	}
	bool isRolledOver()
	{
		return (animBase=="LayTransition4"||animBase=="LayIdle4"||isRofling());
	}
	bool isPlopped()
	{
		return (animBase=="PlopTransition"||animBase=="PlopIdle");
	}
	bool isCurled()
	{
		return (animBase=="CurlTransition"||animBase=="CurlIdle");
	}
	virtual void update(const FrameEvent &evt, MagixExternalDefinitions *def)
	{
		if(!unitID)return;

		updatePhysics(evt);
		updateMovement(evt);
		updateAction(evt,def);
		updateAutoTrack();
		updateAnimation(evt,def);
		updateFaceDirection(evt);
		updateNameTag(evt);
		updateUserTag(evt);
		updateChatBubble(evt);
	}
	void updateAutoTrack()
	{
		if(mAutoTrackTarget)
		{
			//Freelook disabled, stop autotracking
			if(!freeLook)
			{
				mAutoTrackTarget = 0;
				return;
			}
			//Trigger setLookTarget only if unit/target's position changed
			if(!isAttacking() && (isPositionChanged() || mAutoTrackTarget->getPosition()!=lookTargetVector))
			{
				if(mAutoTrackTarget->getType()==OBJECT_UNIT || mAutoTrackTarget->getType()==OBJECT_PLAYER)
					setLookTarget(static_cast<MagixUnit*>(mAutoTrackTarget)->getPosition(true));
				else if(mAutoTrackTarget->getType()==OBJECT_CRITTER)
				{
					const Real tHeight = static_cast<MagixLiving*>(mAutoTrackTarget)->getHeight()*0.5;
					setLookTarget(mAutoTrackTarget->getPosition()+Vector3(0,tHeight,0));
				}
				else setLookTarget(mAutoTrackTarget->getPosition());
			}
		}
	}
	void setFreeLook(bool flag, bool timeout=true)
	{
		if((freeLook==flag && !freeLookTimeout) || getControlMode()==CONTROL_FIRSTPERSON)return;
		if(flag)
		{
			freeLook = true;
			freeLookTimeout = false;
			idleTurnFlag = 0;
		}
		else
		{
			if(freeLook && timeout)freeLookTimeout = true;
			else freeLook = false;
		}
	}
	bool getFreeLook()
	{
		return freeLook;
	}
	void setTarget(const Real &x, const Real &y, const Real &z, const unsigned char &action=TARGET_RUNTO)
	{
		targetVector = Vector3(x,y,z);
		if(targetVector==mObjectNode->getPosition() && action!=TARGET_RUNTOPICKUP)return;
		targetAction = action;

		if(action==TARGET_RUNTO || action==TARGET_LOOKAT || action==TARGET_RUNTOPICKUP)
		{
			//Yaw to target
			if(targetVector.x==mObjectNode->getPosition().x && targetVector.z==mObjectNode->getPosition().z)return;
			const Real tX = targetVector.x - mObjectNode->getPosition().x;
			const Real tZ = targetVector.z - mObjectNode->getPosition().z;
			Radian tYaw = Degree(0);
			if(tZ!=0)tYaw = Math::ATan(tX/tZ);
			else tYaw = Degree(tX>=0?90:270);
			if(tX<0)tYaw += Degree(360);
			if(tZ<0)tYaw += Degree(180);
			setYaw(Degree(tYaw));
		}
	}
	virtual void setTarget(const Vector3 &dest, const unsigned char &action=TARGET_RUNTO)
	{
		setTarget(dest.x,dest.y,dest.z,action);
	}
	const Vector3 getTarget()
	{
		return targetVector;
	}
	unsigned short getTargetAction()
	{
		return targetAction;
	}
	void resetTarget()
	{
		targetVector = mObjectNode->getPosition();
		lookTargetVector = mObjectNode->getPosition();
		lookDirection = mObjectNode->getOrientation()*Quaternion(0,0,1,0);
		lookDirectionBuffer = mObjectNode->getOrientation()*Quaternion(0,0,1,0);
		faceDirectionYaw = Degree(0);
		mAutoTrackTarget = 0;
		targetAction = 0;
		freeLook = true;
		freeLookTimeout = false;
	}
	virtual void stopAction()
	{
		isWalking = false;
		isCrouching = false;
		jump = false;
		antiGravity = false;
		isSwimming = false;
		targetAction = 0;
		stopAttack();
	}
	void setLookDirection(const Quaternion &dir, bool setBuffer=false)
	{
		if(isAttacking())return;
		lookDirection = dir;
		if(setBuffer)lookDirectionBuffer = dir;
	}
	void setLookTarget(const Vector3 &target, bool fromHead=true)
	{
		lookTargetVector = target;

		if(fromHead)mObjectNode->translate( (mObjectNode->getOrientation().zAxis()*3 + Vector3(0,10,0))*mObjectNode->getScale() );
		
		Quaternion tOrig = mObjectNode->getOrientation();
		Radian tYaw, tPitch;

		#pragma warning(push)
		#pragma warning(disable : 4482)
		mObjectNode->lookAt(target,Node::TransformSpace::TS_WORLD);
		#pragma warning(pop)
		tYaw = mObjectNode->getOrientation().getYaw();

		Real tDistance = mObjectNode->getPosition().distance(target);
		if(tDistance!=0)tPitch = Math::ASin((target.y-mObjectNode->getPosition().y)/tDistance);
		else tPitch = Degree(0);

		Matrix3 tMat;
		tMat.FromEulerAnglesXYZ(tPitch,tYaw,Degree(0));
		lookDirection = Quaternion(tMat);

		mObjectNode->setOrientation(tOrig);
		if(fromHead)mObjectNode->translate( (mObjectNode->getOrientation().zAxis()*-3 - Vector3(0,10,0))*mObjectNode->getScale() );
	}
	void setAutoTrackObject(MagixObject *target)
	{
		mAutoTrackTarget = target;
		if(target==0)
		{
			lookDirection = mObjectNode->getOrientation()*Quaternion(0,0,1,0);
			lookTargetVector = Vector3::ZERO;
		}
		else setFreeLook(true);
	}
	MagixObject* getAutoTrackObject()
	{
		return mAutoTrackTarget;
	}
	const Quaternion getLookDirection()
	{
		return lookDirection;
	}
	const Vector3 getPosition(bool headPosition=false)
	{
		if(!mObjectNode)return Vector3(0,0,0);
		if(headPosition)
		{
			Vector3 tOffset = Vector3(0,10,0);
			if(isSitting(true))tOffset = Vector3(0,9,-2);
			else if(isLaying(true))tOffset = Vector3(0,6,0);
			else if(isSideLaying(true))tOffset = Vector3(0,6,0);
			else if(isLeaning(true))tOffset = Vector3(-6,1.5,0);
			else if(isPlopped())tOffset = Vector3(0,1.5,0);
			else if(isRolledOver())tOffset = Vector3(0,1.5,0);

			return (mObjectNode->getPosition() + (mObjectNode->getOrientation().zAxis()*3 + mObjectNode->getOrientation()*tOffset)*mObjectNode->getScale() );
		}
		return mObjectNode->getPosition();
	}
	void changeAnimBase(const String &type, const Real &speed, bool resetTimePosition=false, bool reverseAnimation=false, bool loop=true)
	{
		animSpeed = speed * (reverseAnimation?-1:1);

		if(animBase!=type)
		{
			//Check if previous transition not complete, disable it hence
			if(prevAnimBase!="" && animCount>0)
			{
				mBodyEnt->getAnimationState(prevAnimBase)->setEnabled(false);
				mHeadEnt->getAnimationState(prevAnimBase)->setEnabled(false);
				if(mManeEnt)mManeEnt->getAnimationState(prevAnimBase)->setEnabled(false);
				mTailEnt->getAnimationState(prevAnimBase)->setEnabled(false);
				if(mWingEnt)mWingEnt->getAnimationState(prevAnimBase)->setEnabled(false);
				if(mTuftEnt)mTuftEnt->getAnimationState(prevAnimBase)->setEnabled(false);
			}
			prevAnimBase = animBase;
			if(prevAnimBase!="")animCount = 0.1;
			
			animBase = type;
			
			mBodyEnt->getAnimationState(animBase)->setEnabled(true);
			mHeadEnt->getAnimationState(animBase)->setEnabled(true);
			if(mManeEnt)mManeEnt->getAnimationState(animBase)->setEnabled(true);
			mTailEnt->getAnimationState(animBase)->setEnabled(true);
			if(mWingEnt)mWingEnt->getAnimationState(animBase)->setEnabled(true);
			if(mTuftEnt)mTuftEnt->getAnimationState(animBase)->setEnabled(true);

			mBodyEnt->getAnimationState(animBase)->setLoop(loop);
			mHeadEnt->getAnimationState(animBase)->setLoop(loop);
			if(mManeEnt)mManeEnt->getAnimationState(animBase)->setLoop(loop);
			mTailEnt->getAnimationState(animBase)->setLoop(loop);
			if(mWingEnt)mWingEnt->getAnimationState(animBase)->setLoop(loop);
			if(mTuftEnt)mTuftEnt->getAnimationState(animBase)->setLoop(loop);

			if(prevAnimBase!="")
			{
				mBodyEnt->getAnimationState(animBase)->setWeight(0);
				mHeadEnt->getAnimationState(animBase)->setWeight(0);
				if(mManeEnt)mManeEnt->getAnimationState(animBase)->setWeight(0);
				mTailEnt->getAnimationState(animBase)->setWeight(0);
				if(mWingEnt)mWingEnt->getAnimationState(animBase)->setWeight(0);
				if(mTuftEnt)mTuftEnt->getAnimationState(animBase)->setWeight(0);
			}
		}

		if(resetTimePosition)
		{
			mBodyEnt->getAnimationState(animBase)->setTimePosition(reverseAnimation?mBodyEnt->getAnimationState(animBase)->getLength():0);
			mHeadEnt->getAnimationState(animBase)->setTimePosition(reverseAnimation?mHeadEnt->getAnimationState(animBase)->getLength():0);
			if(mManeEnt)mManeEnt->getAnimationState(animBase)->setTimePosition(reverseAnimation?mManeEnt->getAnimationState(animBase)->getLength():0);
			mTailEnt->getAnimationState(animBase)->setTimePosition(reverseAnimation?mTailEnt->getAnimationState(animBase)->getLength():0);
			if(mWingEnt)mWingEnt->getAnimationState(animBase)->setTimePosition(reverseAnimation?mWingEnt->getAnimationState(animBase)->getLength():0);
			if(mTuftEnt)mTuftEnt->getAnimationState(animBase)->setTimePosition(reverseAnimation?mTuftEnt->getAnimationState(animBase)->getLength():0);
		}
	}
	void changeAnimSecond(const String &type, const Real &speed, bool resetTimePosition=false, bool reverseAnimation=false, bool loop=true)
	{
		animSecondSpeed = speed * (reverseAnimation?-1:1);

		if(animSecond!=type)
		{
			//Check if previous transition not complete, disable it hence
			if(prevAnimSecond!="" && animSecondCount>0)
			{
				mBodyEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
				mHeadEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
				if(mManeEnt)mManeEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
				mTailEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
				if(mWingEnt)mWingEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
				if(mTuftEnt)mTuftEnt->getAnimationState(prevAnimSecond)->setEnabled(false);
			}
			prevAnimSecond = animSecond;
			/*if(prevAnimSecond!="")*/animSecondCount = 0.1;
			
			animSecond = type;
			
			if(animSecond!="")
			{
				mBodyEnt->getAnimationState(animSecond)->setEnabled(true);
				mHeadEnt->getAnimationState(animSecond)->setEnabled(true);
				if(mManeEnt)mManeEnt->getAnimationState(animSecond)->setEnabled(true);
				mTailEnt->getAnimationState(animSecond)->setEnabled(true);
				if(mWingEnt)mWingEnt->getAnimationState(animSecond)->setEnabled(true);
				if(mTuftEnt)mTuftEnt->getAnimationState(animSecond)->setEnabled(true);

				mBodyEnt->getAnimationState(animSecond)->setLoop(loop);
				mHeadEnt->getAnimationState(animSecond)->setLoop(loop);
				if(mManeEnt)mManeEnt->getAnimationState(animSecond)->setLoop(loop);
				mTailEnt->getAnimationState(animSecond)->setLoop(loop);
				if(mWingEnt)mWingEnt->getAnimationState(animSecond)->setLoop(loop);
				if(mTuftEnt)mTuftEnt->getAnimationState(animSecond)->setLoop(loop);

				mBodyEnt->getAnimationState(animSecond)->setWeight(/*prevAnimSecond!=""?0:1*/0);
				mHeadEnt->getAnimationState(animSecond)->setWeight(/*prevAnimSecond!=""?0:1*/0);
				if(mManeEnt)mManeEnt->getAnimationState(animSecond)->setWeight(/*prevAnimSecond!=""?0:1*/0);
				mTailEnt->getAnimationState(animSecond)->setWeight(/*prevAnimSecond!=""?0:1*/0);
				if(mWingEnt)mWingEnt->getAnimationState(animSecond)->setWeight(/*prevAnimSecond!=""?0:1*/0);
				if(mTuftEnt)mTuftEnt->getAnimationState(animSecond)->setWeight(/*prevAnimSecond!=""?0:1*/0);

				if(resetTimePosition)
				{
					mBodyEnt->getAnimationState(animSecond)->setTimePosition(reverseAnimation?mBodyEnt->getAnimationState(animSecond)->getLength():0);
					mHeadEnt->getAnimationState(animSecond)->setTimePosition(reverseAnimation?mHeadEnt->getAnimationState(animSecond)->getLength():0);
					if(mManeEnt)mManeEnt->getAnimationState(animSecond)->setTimePosition(reverseAnimation?mManeEnt->getAnimationState(animSecond)->getLength():0);
					mTailEnt->getAnimationState(animSecond)->setTimePosition(reverseAnimation?mTailEnt->getAnimationState(animSecond)->getLength():0);
					if(mWingEnt)mWingEnt->getAnimationState(animSecond)->setTimePosition(reverseAnimation?mWingEnt->getAnimationState(animSecond)->getLength():0);
					if(mTuftEnt)mTuftEnt->getAnimationState(animSecond)->setTimePosition(reverseAnimation?mTuftEnt->getAnimationState(animSecond)->getLength():0);
				}
			}
		}
	}
	virtual const OBJECT_TYPE getType()
	{
		return OBJECT_UNIT;
	}
	const unsigned short getUnitID()
	{
		return unitID;
	}
	void createNameTag(const String &name, MovableTextOverlayAttributes *attrs)
	{
		if(!unitID)return;
		if(mNameText)delete mNameText;
		mNameText = new MovableTextOverlay("NameTag"+StringConverter::toString(unitID),name,mBodyEnt,attrs);
		mNameText->enable(false);
		mNameText->setUpdateFrequency(0.01);
		nameTagVisible = true;
	}
	void createUserTag(const String &name, MovableTextOverlayAttributes *attrs)
	{
		if(!unitID)return;
		if(mUserText)delete mUserText;
		mUserText = new MovableTextOverlay("UserTag"+StringConverter::toString(unitID),name,mBodyEnt,attrs,0,0,16);
		mUserText->enable(false);
		mUserText->setUpdateFrequency(0.01);
		userTagVisible = true;
	}
	void createChatBubble(String caption, MovableTextOverlayAttributes *attrs, const Real &tLeft=0.0, const Real &tTop=0.0)
	{
		if(!unitID)return;
		if(mChatText)delete mChatText;

		//size caption
		int tLength = 0;
		int tSpacePos = -1;
		for(int i=0;i<int(caption.length());i++)
		{
			tLength += 1;
			if(caption[i]==' ')tSpacePos = i;
			if(tLength>=20)
			{
				if(tSpacePos!=-1)
				{
					caption[tSpacePos] = '\n';
					tLength = i-tSpacePos;
					tSpacePos = -1;
				}
				else 
				{
					caption.insert(i+1,String(1,'\n'));
					tLength = -1;
				}
			}
		}

		mChatText = new MovableTextOverlay("ChatBubble"+StringConverter::toString(unitID),caption,mHeadEnt,attrs,tLeft,tTop);
		mChatText->enable(false);
		mChatText->setUpdateFrequency(0.01);
		chatBubbleVisible = true;
		chatBubbleCount = 15;
	}
	MovableTextOverlay* getNameTag()
	{
		return mNameText;
	}
	MovableTextOverlay* getUserTag()
	{
		return mUserText;
	}
	MovableTextOverlay* getChatBubble()
	{
		return mChatText;
	}
	void showNameTag(bool flag)
	{
		if(nameTagVisible==flag)return;
		nameTagVisible = flag;
		if(!flag && mNameText)mNameText->enable(false);
	}
	void showUserTag(bool flag)
	{
		if(userTagVisible==flag)return;
		userTagVisible = flag;
		if(!flag && mUserText)mUserText->enable(false);
	}
	void showChatBubble(bool flag)
	{
		if(chatBubbleVisible==flag)return;
		chatBubbleVisible = flag;
		if(!flag && mChatText)mChatText->enable(false);
	}
	bool isNameTagVisible()
	{
		return (nameTagVisible && mNameText);
	}
	bool isUserTagVisible()
	{
		return (userTagVisible && mUserText);
	}
	bool isChatBubbleVisible()
	{
		return (chatBubbleVisible && mChatText);
	}
	void updateNameTag(const FrameEvent &evt)
	{
		if(mNameText && nameTagVisible)mNameText->update(evt.timeSinceLastFrame,false);
	}
	void updateUserTag(const FrameEvent &evt)
	{
		if(mUserText && userTagVisible)mUserText->update(evt.timeSinceLastFrame,false);
	}
	void updateChatBubble(const FrameEvent &evt)
	{
		if(chatBubbleCount>0)
		{
			chatBubbleCount -= evt.timeSinceLastFrame;
			if(chatBubbleCount<=0)
			{
				chatBubbleCount = 0;
				if(mChatText)delete mChatText;
				mChatText = 0;
				chatBubbleVisible = false;
			}
		}
		if(mChatText && chatBubbleVisible)mChatText->update(evt.timeSinceLastFrame,true,0.01,true);
	}

	void updateEmote(const FrameEvent &evt, MagixExternalDefinitions *def)
	{
		if(emoteCount>0)
		{
			emoteCount -= evt.timeSinceLastFrame;
			if(emoteCount<0)emoteCount = 0;

			setEmoteState(prevEmoteMode,emoteCount/EMOTE_TRANSITIONTIME,def);
			setEmoteState(emoteMode,(EMOTE_TRANSITIONTIME-emoteCount)/EMOTE_TRANSITIONTIME,def);
		}
	}
	void setEmote(const String &mode, MagixExternalDefinitions *def)
	{
		if(emoteMode==mode)return;
		if(emoteCount!=0)
		{
			setEmoteState(prevEmoteMode,0,def);
			setEmoteState(emoteMode,1,def);
		}
		prevEmoteMode = emoteMode;
		emoteMode = mode;
		emoteCount = EMOTE_TRANSITIONTIME;
	}
	void setEmoteState(const String &mode, const Real &weight, MagixExternalDefinitions *def)
	{
		for(int i=0;i<def->maxEmotes;i++)
		{
			if(mode == def->emoteName[i])
			{
				const vector<String>::type tAnim = StringUtil::split(def->emoteAnims[i],";");
				for(int j=0;j<int(tAnim.size());j++)
				{
					setFaceAnimState(tAnim[j],weight,mode==emoteMode);
				}
				if(mode==emoteMode)
				{
					eyelidState = def->emoteEyelidState[i];
					blinkFlag = 2;
				}
				return;
			}
		}
	}
	void setFaceAnimState(const String &anim, const Real &weight, bool isCurrent)
	{
		if(weight>0)
		{
			if(!mHeadEnt->getAnimationState(anim)->getEnabled())
			{
				mHeadEnt->getAnimationState(anim)->setEnabled(true);
				mHeadEnt->getAnimationState(anim)->setLoop(false);
			}
			//Current emote doesn't reduce weight
			if(!isCurrent || mHeadEnt->getAnimationState(anim)->getTimePosition()< (weight*mHeadEnt->getAnimationState(anim)->getLength()) )
				mHeadEnt->getAnimationState(anim)->setTimePosition(weight*mHeadEnt->getAnimationState(anim)->getLength());
		}
		else
		{
			mHeadEnt->getAnimationState(anim)->setTimePosition(weight*mHeadEnt->getAnimationState(anim)->getLength());
			if(mHeadEnt->getAnimationState(anim)->getEnabled())
			{
				mHeadEnt->getAnimationState(anim)->setEnabled(false);
			}
		}
	}
	void replaceHeadMesh(SceneManager *sceneMgr, const String &meshName, bool resetMaterial=false)
	{
		if(!mHeadEnt)return;
		Entity *tHead = sceneMgr->createEntity("Headtemp",meshName+".mesh");
		mHeadEnt->getAllAnimationStates()->copyMatchingState(tHead->getAllAnimationStates());

		mObjectNode->detachObject(mHeadEnt);
		if(sceneMgr->hasEntity("Head"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mHeadEnt);

		MaterialPtr tMat = MaterialManager::getSingleton().getByName("DynamicHeadMat"+StringConverter::toString(unitID));
		if(!tMat.isNull())MaterialManager::getSingleton().remove(tMat->getHandle());

		mHeadEnt = sceneMgr->createEntity("Head"+StringConverter::toString(unitID), meshName+".mesh");
		mObjectNode->attachObject(mHeadEnt);
		mHeadEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		mHeadEnt->getSkeleton()->getBlendMode();
		mHeadEnt->setQueryFlags(UNIT_MASK);

		tHead->getAllAnimationStates()->copyMatchingState(mHeadEnt->getAllAnimationStates());
		sceneMgr->destroyEntity(tHead);
		if(resetMaterial)setMaterial(matGroupName);
	}
	void replaceManeMesh(SceneManager *sceneMgr, const String &meshName, bool resetMaterial=false)
	{
		if(meshName=="Maneless")
		{
			if(mManeEnt)
			{
				mObjectNode->detachObject(mManeEnt);
				if(sceneMgr->hasEntity("Mane"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mManeEnt);
			}
			mManeEnt = 0;
			return;
		}
		Entity *tMane = sceneMgr->createEntity("Manetemp",meshName+".mesh");
		mHeadEnt->getAllAnimationStates()->copyMatchingState(tMane->getAllAnimationStates()); //to avoid mane animation differing from head

		if(mManeEnt)
		{
			mObjectNode->detachObject(mManeEnt);
			if(sceneMgr->hasEntity("Mane"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mManeEnt);
		}
		mManeEnt = sceneMgr->createEntity("Mane"+StringConverter::toString(unitID), meshName+".mesh");
		mObjectNode->attachObject(mManeEnt);
		mManeEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		mManeEnt->getSkeleton()->getBlendMode();
		mManeEnt->setQueryFlags(UNIT_MASK);

		tMane->getAllAnimationStates()->copyMatchingState(mManeEnt->getAllAnimationStates());
		sceneMgr->destroyEntity(tMane);
		if(resetMaterial)setMaterial(matGroupName);
	}
	void replaceTailMesh(SceneManager *sceneMgr, const String &meshName, bool resetMaterial=false)
	{
		if(!mTailEnt)return;
		Entity *tTail = sceneMgr->createEntity("Tailtemp",meshName+".mesh");
		mTailEnt->getAllAnimationStates()->copyMatchingState(tTail->getAllAnimationStates());

		mObjectNode->detachObject(mTailEnt);
		if(sceneMgr->hasEntity("Tail"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mTailEnt);
		mTailEnt = sceneMgr->createEntity("Tail"+StringConverter::toString(unitID), meshName+".mesh");
		mObjectNode->attachObject(mTailEnt);
		mTailEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		mTailEnt->getSkeleton()->getBlendMode();
		mTailEnt->setQueryFlags(UNIT_MASK);

		tTail->getAllAnimationStates()->copyMatchingState(mTailEnt->getAllAnimationStates());
		sceneMgr->destroyEntity(tTail);
		if(resetMaterial)setMaterial(matGroupName);
	}
	void replaceWingMesh(SceneManager *sceneMgr, const String &meshName, bool resetMaterial=false)
	{
		if(meshName=="Wingless")
		{
			if(mWingEnt)
			{
				mObjectNode->detachObject(mWingEnt);
				if(sceneMgr->hasEntity("Wing"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mWingEnt);
			}
			mWingEnt = 0;
			return;
		}
		Entity *tWing = sceneMgr->createEntity("Wingtemp",meshName+".mesh");
		mBodyEnt->getAllAnimationStates()->copyMatchingState(tWing->getAllAnimationStates()); //to avoid wing animation differing fom body

		if(mWingEnt)
		{
			mObjectNode->detachObject(mWingEnt);
			if(sceneMgr->hasEntity("Wing"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mWingEnt);
		}
		mWingEnt = sceneMgr->createEntity("Wing"+StringConverter::toString(unitID), meshName+".mesh");
		mObjectNode->attachObject(mWingEnt);
		mWingEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		mWingEnt->getSkeleton()->getBlendMode();
		mWingEnt->setQueryFlags(UNIT_MASK);

		tWing->getAllAnimationStates()->copyMatchingState(mWingEnt->getAllAnimationStates());
		sceneMgr->destroyEntity(tWing);
		if(resetMaterial)setMaterial(matGroupName);
	}
	void replaceTuftMesh(SceneManager *sceneMgr, const String &meshName, bool resetMaterial=false)
	{
		if(meshName=="Tuftless")
		{
			if(mTuftEnt)
			{
				mObjectNode->detachObject(mTuftEnt);
				if(sceneMgr->hasEntity("Tuft"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mTuftEnt);
			}
			mTuftEnt = 0;
			return;
		}
		Entity *tTuft = sceneMgr->createEntity("Tufttemp",meshName+".mesh");
		mBodyEnt->getAllAnimationStates()->copyMatchingState(tTuft->getAllAnimationStates()); //to avoid tuft animation differing from body

		if(mTuftEnt)
		{
			mObjectNode->detachObject(mTuftEnt);
			if(sceneMgr->hasEntity("Tuft"+StringConverter::toString(unitID)))sceneMgr->destroyEntity(mTuftEnt);
		}
		mTuftEnt = sceneMgr->createEntity("Tuft"+StringConverter::toString(unitID), meshName+".mesh");
		mObjectNode->attachObject(mTuftEnt);
		mTuftEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		mTuftEnt->getSkeleton()->getBlendMode();
		mTuftEnt->setQueryFlags(UNIT_MASK);

		tTuft->getAllAnimationStates()->copyMatchingState(mTuftEnt->getAllAnimationStates());
		sceneMgr->destroyEntity(tTuft);
		if(resetMaterial)setMaterial(matGroupName);
	}
	const String getMatGroupName()
	{
		return matGroupName;
	}
	const String getMeshString()
	{
		String bodyMeshID = mBodyEnt->getMesh()->getName();
		bodyMeshID.erase(bodyMeshID.find_first_of("."),5);
		String headMeshID = mHeadEnt->getMesh()->getName();
		headMeshID.erase(headMeshID.find_first_of("."),5);
		String maneMeshID = "Maneless";
		if(mManeEnt)
		{
			maneMeshID = mManeEnt->getMesh()->getName();
			maneMeshID.erase(maneMeshID.find_first_of("."),5);
		}
		String tailMeshID = mTailEnt->getMesh()->getName();
		tailMeshID.erase(tailMeshID.find_first_of("."),5);
		String wingMeshID = "Wingless";
		if(mWingEnt)
		{
			wingMeshID = mWingEnt->getMesh()->getName();
			wingMeshID.erase(wingMeshID.find_first_of("."),5);
		}

		return (bodyMeshID+";"+headMeshID+";"+maneMeshID+";"+tailMeshID+";"+wingMeshID);
	}
	const String getMeshString2()
	{
		String tuftMeshID = "Tuftless";
		if(mTuftEnt)
		{
			tuftMeshID = mTuftEnt->getMesh()->getName();
			tuftMeshID.erase(tuftMeshID.find_first_of("."),5);
		}

		return (tuftMeshID);
	}
	const String getColourString()
	{
		const Vector4 pelt = mBodyEnt->getSubEntity(0)->getCustomParameter(1);
		const Vector4 underfur = mBodyEnt->getSubEntity(0)->getCustomParameter(2);
		Vector4 mane = Vector4::ZERO;
		if(mManeEnt)mane = mManeEnt->getSubEntity(0)->getCustomParameter(1);
		else if(mTuftEnt)mane = mTuftEnt->getSubEntity(0)->getCustomParameter(1);
		const Vector4 nose = mHeadEnt->getSubEntity(0)->getCustomParameter(3);
		const Vector4 eyeTop = mHeadEnt->getSubEntity(0)->getCustomParameter(4);
		const Vector4 eyeBot = mHeadEnt->getSubEntity(0)->getCustomParameter(5);
		const Vector4 ears = mHeadEnt->getSubEntity(0)->getCustomParameter(7);
		const Vector4 tailtip = mTailEnt->getSubEntity(0)->getCustomParameter(2);
		const Vector4 eyes = mHeadEnt->getSubEntity(2)->getCustomParameter(2);
		Vector4 wing = Vector4::ZERO;
		if(mWingEnt)wing = mWingEnt->getSubEntity(0)->getCustomParameter(1);
		Vector4 marking = Vector4::ZERO;
		if(bodyMarkID!=0)marking = mBodyEnt->getSubEntity(0)->getCustomParameter(3);
		else if(headMarkID!=0)marking = mHeadEnt->getSubEntity(0)->getCustomParameter(8);
		else if(tailMarkID!=0)marking = mTailEnt->getSubEntity(0)->getCustomParameter(3);

		return (vect4ToString(pelt,3) + ";" +
				vect4ToString(underfur,3) + ";" +
				vect4ToString(mane,3) + ";" +
				vect4ToString(nose,3) + ";" +
				vect4ToString(eyeTop,3) + ";" +
				vect4ToString(eyeBot,3) + ";" +
				vect4ToString(ears,3) + ";" +
				vect4ToString(tailtip,3) + ";" +
				vect4ToString(eyes,3) + ";" +
				vect4ToString(wing,3) +
				(bodyMarkID==0&&headMarkID==0&&tailMarkID==0?"":";"+vect4ToString(marking,3)) );
	}
	const String vect4ToString(Vector4 vect, const unsigned short &tDeci)
	{
		unsigned short deci = 1;
		for(int i=0;i<tDeci;i++)deci *= 10;
		vect.x *= deci; vect.x = (int)vect.x; vect.x /= deci;
		vect.y *= deci; vect.y = (int)vect.y; vect.y /= deci;
		vect.z *= deci; vect.z = (int)vect.z; vect.z /= deci;
		vect.w *= deci; vect.w = (int)vect.w; vect.w /= deci;

		return (StringConverter::toString(vect.x) + " " +
				StringConverter::toString(vect.y) + " " +
				StringConverter::toString(vect.z) + " " +
				StringConverter::toString(vect.w));
	}
	void setColours(const String &colourString)
	{
		vector<String>::type tParam = StringUtil::split(colourString,";",11);
		if(tParam.size()==10 || tParam.size()==11)
		{
			setColours(StringConverter::parseColourValue(tParam[0]),
				StringConverter::parseColourValue(tParam[1]),
				StringConverter::parseColourValue(tParam[2]),
				StringConverter::parseColourValue(tParam[3]),
				StringConverter::parseColourValue(tParam[4]),
				StringConverter::parseColourValue(tParam[5]),
				StringConverter::parseColourValue(tParam[6]),
				StringConverter::parseColourValue(tParam[7]),
				StringConverter::parseColourValue(tParam[8]),
				StringConverter::parseColourValue(tParam[9]),
				(tParam.size()==11?StringConverter::parseColourValue(tParam[10]):ColourValue(0,0,0)));
		}
	}
	const String getName()
	{
		if(mNameText)return mNameText->getCaption();
		return "";
	}
	const String getUser()
	{
		if(mUserText)return mUserText->getCaption();
		return "";
	}
	void setJump(bool flag)
	{
		jump = flag;
	}
	bool getJump()
	{
		return jump;
	}
	void setIsWalking(bool flag)
	{
		isWalking = flag;
	}
	bool getIsWalking()
	{
		return isWalking;
	}
	const String doAttack(MagixExternalDefinitions *def, bool doEmote=true)
	{
		if(isDisabled())return "";
		if(!isStance)
		{
			isStance = true;
			changeAnimBase("StanceTransition",1,true,false,false);
			if(doEmote)setEmote("Anger",def);

			return "";
		}
		if(isReadyToAttack())
		{
			nextAttackName = attackList[attackCounter];
			attackCounter++;

			return nextAttackName;
		}
		return "";
	}
	void forceAttack(const String &name)
	{
		nextAttackName = name;
	}
	bool isReadyToAttack()
	{
		return (nextAttackName=="" && attackList.size()>attackCounter && !isDisabled() &&
				( (attackName=="" && attackTimeout==0)
				||(attackName!="" &&(attackTimeout>0 || mBodyEnt->getAnimationState(animBase)->getTimePosition()>mBodyEnt->getAnimationState(animBase)->getLength()*0.6))));
	}
	virtual bool updateAttack(const FrameEvent &evt, MagixExternalDefinitions *def)
	{
		if(isAttacking())
		{
			if(mBodyEnt->getAnimationState(animBase)->hasEnded())
			{
				attackTimeout = 0.25;
				hasStoppedAttack = true;
			}
			else if(getControlMode()!=CONTROL_FIRSTPERSON)lookDirection = mObjectNode->getOrientation()*Quaternion(0,0,1,0);
		}
		if(!isAttacking() && nextAttackName!="")
		{
			attackName = nextAttackName;
			nextAttackName = "";
			attackTimeout = 0;
			const String tAnim = def->getAttackAnim(attackName);
			changeAnimBase(tAnim,1.5,true,false,false);
			hasNewAttack = true;
		}
		if(attackTimeout>0)
		{
			attackTimeout -= evt.timeSinceLastFrame;
			if(attackTimeout<0)attackTimeout = 0;
			if(attackTimeout==0)
			{
				attackName = "";
				attackCounter = 0;
			}
		}
		return isAttacking();
	}
	bool isAttacking()
	{
		return (attackName!="" && attackTimeout==0);
	}
	void stopStance(MagixExternalDefinitions *def)
	{
		if(!isAttacking())
		{
			attackName = "";
			nextAttackName = "";
			attackTimeout = 0;
			attackCounter = 0;
			isStance = false;
			changeAnimBase("StanceTransition",1,true,true,false);
			setEmote("Normal",def);
		}
	}
	bool getIsStance()
	{
		return isStance;
	}
	void toggleStance(MagixExternalDefinitions *def, bool doEmote=true)
	{
		if(isStance)stopStance(def);
		else doAttack(def,doEmote);
	}
	void stopAttack()
	{
		attackName = "";
		nextAttackName = "";
		attackTimeout = 0;
		attackCounter = 0;
		hasStoppedAttack = true;
	}
	void setAttack(const String &name)
	{
		stopAttack();
		nextAttackName = name;
	}
	Entity* getBodyEnt()
	{
		return mBodyEnt;
	}
	Entity* getHeadEnt()
	{
		return mHeadEnt;
	}
	Entity* getTailEnt()
	{
		return mTailEnt;
	}
	Entity* getManeEnt()
	{
		return mManeEnt;
	}
	Entity* getWingEnt()
	{
		return mWingEnt;
	}
	Entity* getTuftEnt()
	{
		return mTuftEnt;
	}
	bool popHasNewAttack()
	{
		bool tFlag = hasNewAttack;
		hasNewAttack = false;
		return tFlag;
	}
	bool popHasStoppedAttack()
	{
		bool tFlag = hasStoppedAttack;
		hasStoppedAttack = false;
		return tFlag;
	}
	const String getAttackName()
	{
		return attackName;
	}
	void cancelAction(bool hitAnim, const Real &duration=0, MagixExternalDefinitions *def=0)
	{
		targetAction = 0;
		stopAttack();
		if(hitAnim)
		{
			changeAnimBase("Hit",1,true,false,false);
			if(def)setEmote("Pain",def);
		}
		hitTimeout = duration;
	}
	void cancelAction(const Real &duration, const String &anim, bool reverseAnim=false)
	{
		targetAction = 0;
		stopAttack();
		if(anim!="")changeAnimBase(anim,1,true,reverseAnim,false);
		hitTimeout = duration;
	}
	const Real getHitTimeout()
	{
		return hitTimeout;
	}
	bool isDisabled()
	{
		return (hitTimeout!=0);
	}
	void setDeathTimeout(const Real &duration)
	{
		hitTimeout = -duration;
	}
	const Real getDeathTimeout()
	{
		return -hitTimeout;
	}
	bool isDying()
	{
		return (hitTimeout<0);
	}
	void kill()
	{
		hitTimeout = -0.01;
	}
	bool isDead()
	{
		return (hitTimeout<0 && hitTimeout>=-0.01);
	}
	void clearAttackList()
	{
		attackList.clear();
	}
	void pushAttackList(const String &attackName)
	{
		attackList.push_back(attackName);
	}
	void setCustomAnimation(bool flag, const String &animName="", Real animSpeed=1, bool loop=false, bool reverse=false)
	{
		isCustomAnimation = flag;
		if(flag)changeAnimBase(animName,animSpeed,true,reverse,loop);
	}
	virtual unsigned short getControlMode()
	{
		return 0;
	}
	void setIsSwimming(bool flag)
	{
		isSwimming = flag;
	}
	bool getIsSwimming()
	{
		return isSwimming;
	}
	void equip(SceneManager *sceneMgr, MagixExternalDefinitions *mDef, const String &meshName, const Vector3 &offset=Vector3::ZERO, const short &slot=-1, bool altAnim=false)
	{
		if(unitID==0)return;
		if(slot<0||slot>=MAX_EQUIP)return;
		if(mEquipEnt[slot])return;

		mEquipEnt[slot] = sceneMgr->createEntity("Equip"+StringConverter::toString(slot)+"_"+StringConverter::toString(unitID),meshName+".mesh");
		mEquipEnt[slot]->getAnimationState((altAnim?"Equip2":"Equip"))->setEnabled(true);

		const String tBone = mDef->getItemBone(meshName);
		if(mHeadEnt && StringUtil::startsWith(tBone,"Head"))
			mHeadEnt->attachObjectToBone(tBone,mEquipEnt[slot],Quaternion::IDENTITY,offset);
		else if(mTailEnt && StringUtil::startsWith(tBone,"Tail"))
			mTailEnt->attachObjectToBone(tBone,mEquipEnt[slot],Quaternion::IDENTITY,offset);
		else if(mBodyEnt)
			mBodyEnt->attachObjectToBone(tBone,mEquipEnt[slot],Quaternion::IDENTITY,offset);
	}
	const String unequip(SceneManager *sceneMgr, MagixExternalDefinitions *mDef,const unsigned short &iID)
	{
		if(iID<0||iID>=MAX_EQUIP)return "";
		if(!mEquipEnt[iID])return "";

		const String tEquip = getEquip(iID);
		mEquipEnt[iID]->detachFromParent();

		sceneMgr->destroyEntity(mEquipEnt[iID]);
		mEquipEnt[iID] = 0;
		return tEquip;
	}
	const String getEquip(const unsigned short &iID)
	{
		if(iID<0||iID>=MAX_EQUIP)return "";
		if(mEquipEnt[iID])
		{
			String tName = mEquipEnt[iID]->getMesh()->getName();
			tName.erase(tName.find_first_of("."),5);
			return tName;
		}
		return "";
	}
	void doCrouch(bool flag)
	{
		if(flag && !isCrouching)
		{
			if(isDisabled()||isAttacking())return;
			isCrouching = true;
			changeAnimBase("CrouchTransition",1,animBase!="CrouchTransition",false,false);
		}
		else if(!flag && isCrouching)
		{
			isCrouching = false;
			if(isDisabled()||isAttacking())return;
			changeAnimBase("CrouchTransition",1,animBase!="CrouchTransition",true,false);
		}
	}
	bool getIsCrouching()
	{
		return isCrouching;
	}
	void doHeadswing()
	{
		if(isDisabled()||isAttacking())return;
		if(!isHeadswinging())changeAnimBase("Headswing",1,true,false,true);
	}
	bool isHeadswinging()
	{
		return (animBase=="Headswing");
	}
	void doHeadbang()
	{
		if(isDisabled()||isAttacking())return;
		if(!isHeadbanging())changeAnimBase("Headbang",1,true,false,true);
	}
	bool isHeadbanging()
	{
		return (animBase=="Headbang");
	}
	void doButtswing()
	{
		if(isDisabled()||isAttacking())return;
		if(!isButtswinging())changeAnimBase("Buttswing",1,true,false,true);
	}
	bool isButtswinging()
	{
		return (animBase=="Buttswing");
	}
	void doWingwave()
	{
		if(isDisabled()||isAttacking())return;
		if(!isWingwaving())changeAnimBase("Wingwave",1,true,false,true);
	}
	bool isWingwaving()
	{
		return (animBase=="Wingwave");
	}
	void doMoonwalk()
	{
		if(isDisabled()||isAttacking())return;
		if(!isMoonwalking())changeAnimBase("Moonwalk",1,true,false,true);
	}
	bool isMoonwalking()
	{
		return (animBase=="Moonwalk");
	}
	void doThriller()
	{
		if(isDisabled()||isAttacking())return;
		if(!isThrillering())changeAnimBase("Thriller",1,true,false,true);
	}
	bool isThrillering()
	{
		return (animBase=="Thriller");
	}
	void doRoar()
	{
		if(isDisabled()||isAttacking())return;
		if(!isRoaring())
		{
			changeAnimBase("Roar",1,true,false,false);
			allocFlags.needsSoundRoar = true;
		}
	}
	bool isRoaring()
	{
		return (animBase=="Roar");
	}
	void doRofl()
	{
		if(isDisabled()||isAttacking())return;
		if(!isRofling())changeAnimBase("Rofl",1,true,false,false);
	}
	bool isRofling()
	{
		return (animBase=="Rofl");
	}
	void doFaint()
	{
		if(isDisabled()||isAttacking())return;
		if(!isFainting())changeAnimBase("Faint",1,true,false,false);
	}
	bool isFainting()
	{
		return (animBase=="Faint");
	}
	bool isDancing()
	{
		return (isHeadswinging()||isHeadbanging()||isButtswinging()||isWingwaving()||isMoonwalking()||isThrillering());
	}
	void doNodHead()
	{
		if(!isNodHead())changeAnimSecond("NodHead",0.8,animSecond!="NodHead",false,false);
	}
	bool isNodHead()
	{
		return (animSecond=="NodHead" && animSecondSpeed>=0.5);
	}
	void doShakeHead()
	{
		if(!isShakeHead())changeAnimSecond("ShakeHead",0.8,animSecond!="ShakeHead",false,false);
	}
	bool isShakeHead()
	{
		return (animSecond=="ShakeHead" && animSecondSpeed>=0.5);
	}
	void doNodHeadSlow()
	{
		if(!isNodHeadSlow())changeAnimSecond("NodHead",0.3,animSecond!="NodHead",false,false);
	}
	bool isNodHeadSlow()
	{
		return (animSecond=="NodHead" && animSecondSpeed<0.5);
	}
	void doShakeHeadSlow()
	{
		if(!isShakeHeadSlow())changeAnimSecond("ShakeHead",0.3,animSecond!="ShakeHead",false,false);
	}
	bool isShakeHeadSlow()
	{
		return (animSecond=="ShakeHead" && animSecondSpeed<0.5);
	}
	void doHeadTilt()
	{
		if(!isHeadTilt())changeAnimSecond("HeadTilt",0.5,true,false,false);
	}
	bool isHeadTilt()
	{
		return (animSecond=="HeadTilt");
	}
	void toggleLick()
	{
		if(!isLick())changeAnimSecond("Lick",0.5,true,false,true);
		else changeAnimSecond("",animSecondSpeed);
	}
	bool isLick()
	{
		return (animSecond=="Lick");
	}
	void toggleNuzzle()
	{
		if(!isNuzzle())changeAnimSecond("Nuzzle",0.5,true,false,true);
		else changeAnimSecond("",animSecondSpeed);
	}
	bool isNuzzle()
	{
		return (animSecond=="Nuzzle");
	}
	void doSniff()
	{
		if(!isSniff())changeAnimSecond("Sniff",0.75,true,false,false);
	}
	bool isSniff()
	{
		return (animSecond=="Sniff");
	}
	void doLaugh()
	{
		if(!isLaugh())changeAnimSecond("Laugh",0.75,true,false,false);
	}
	bool isLaugh()
	{
		return (animSecond=="Laugh");
	}
	void doChuckle()
	{
		if(!isChuckle())changeAnimSecond("Chuckle",0.75,true,false,false);
	}
	bool isChuckle()
	{
		return (animSecond=="Chuckle");
	}
	void doTailFlick()
	{
		if(!isTailFlick())changeAnimSecond("TailFlick",0.75,true,false,false);
	}
	bool isTailFlick()
	{
		return (animSecond=="TailFlick");
	}
	void doPoint()
	{
		if(isDisabled()||isAttacking())return;
		if(!isPoint())changeAnimBase("Point",0.5,true,false,false);
	}
	bool isPoint()
	{
		return (animBase=="Point");
	}
	void doStretch()
	{
		if(isDisabled()||isAttacking())return;
		if(!isStretch())changeAnimBase("Stretch",0.3,true,false,false);
	}
	bool isStretch()
	{
		return (animBase=="Stretch");
	}
	bool isOneTimeAction()
	{
		return (isPoint()||isStretch()||isRoaring()||isRofling()||isFainting());
	}
	bool isOneTimeHeadAction()
	{
		return (isNodHead()||isShakeHead()||isNodHeadSlow()||isShakeHeadSlow()||isHeadTilt()||isSniff()||isLaugh()||isChuckle());
	}
	bool isOneTimeTailAction()
	{
		return isTailFlick();
	}
	void setMarkings(const unsigned char &bodyID, const unsigned char &headID, const unsigned char &tailID)
	{
		if(matGroupName!="CustomMat")return;
		bodyMarkID = bodyID;
		headMarkID = headID;
		tailMarkID = tailID;
	}
	void getMarkings(short &bodyID, short &headID, short &tailID)
	{
		bodyID = bodyMarkID;
		headID = headMarkID;
		tailID = tailMarkID;
	}
	void setIsInWater(bool flag)
	{
		isInWater = flag;
	}
	bool getIsInWater()
	{
		return isInWater;
	}
	void doDoubleJump()
	{
		if(hasDoubleJumped)return;
		if(mWingEnt)return;
		hasDoubleJumped = true;
		addForce(Vector3(0,5,0));
		changeAnimBase("Attack5",1.5,true,false,false);
	}
	bool getHasDoubleJumped()
	{
		return hasDoubleJumped;
	}
	void updateLipSync(const FrameEvent &evt)
	{
		if(lipSyncAnim!="")
		{
			//Unweigh emote
			if(mHeadEnt->getAnimationState("FaceNormal")->getEnabled()&&mHeadEnt->getAnimationState("FaceNormal")->getWeight()>0)mHeadEnt->getAnimationState("FaceNormal")->setWeight(mHeadEnt->getAnimationState("FaceNormal")->getWeight()-evt.timeSinceLastFrame*lipSyncSpeed*6);
			if(mHeadEnt->getAnimationState("FaceSmile")->getEnabled()&&mHeadEnt->getAnimationState("FaceSmile")->getWeight()>0)mHeadEnt->getAnimationState("FaceSmile")->setWeight(mHeadEnt->getAnimationState("FaceSmile")->getWeight()-evt.timeSinceLastFrame*lipSyncSpeed*6);
			if(mHeadEnt->getAnimationState("FaceOpenJaw")->getEnabled()&&mHeadEnt->getAnimationState("FaceOpenJaw")->getWeight()>0)mHeadEnt->getAnimationState("FaceOpenJaw")->setWeight(mHeadEnt->getAnimationState("FaceOpenJaw")->getWeight()-evt.timeSinceLastFrame*lipSyncSpeed*6);
			if(prevLipSyncAnim!=""&&prevLipSyncAnim!=lipSyncAnim)mHeadEnt->getAnimationState(prevLipSyncAnim)->addTime(-evt.timeSinceLastFrame*lipSyncSpeed*4);
			if(!mHeadEnt->getAnimationState(lipSyncAnim)->hasEnded())
			{
				mHeadEnt->getAnimationState(lipSyncAnim)->addTime(evt.timeSinceLastFrame*lipSyncSpeed*4);
				return;
			}
		}
		else resumeEmote(evt);
		if(lipSyncCounter>0)
		{
			lipSyncCounter -= evt.timeSinceLastFrame*lipSyncSpeed*4;
			if(lipSyncCounter<0)lipSyncCounter = 0;
			if(lipSyncCounter>0)return;
		}
		//Stopped and resuming emote
		if(lipSyncCounter<0)
		{
			resumeEmote(evt);
			if(mHeadEnt->getAnimationState("FaceNormal")->getWeight()==1&&mHeadEnt->getAnimationState("FaceSmile")->getWeight()==1&&mHeadEnt->getAnimationState("FaceOpenJaw")->getWeight()==1&&(prevLipSyncAnim==""||mHeadEnt->getAnimationState(prevLipSyncAnim)->getTimePosition()==0))
			{
				if(prevLipSyncAnim!="")mHeadEnt->getAnimationState(prevLipSyncAnim)->setEnabled(false);
				prevLipSyncAnim = "";
				lipSyncCounter = 0;
			}
		}
		if(lipSync.length()<=0)
		{
			if(lipSyncAnim!="")
			{
				prevLipSyncAnim = lipSyncAnim;
				lipSyncAnim = "";
			}
			if(prevLipSyncAnim!="")stopLipSync();
			return;
		}
		if(prevLipSyncAnim!=""&&prevLipSyncAnim!=lipSyncAnim&&lipSyncAnim!="")
		{
			mHeadEnt->getAnimationState(prevLipSyncAnim)->setEnabled(false);
			prevLipSyncAnim = "";
		}
		if(lipSyncAnim!="")prevLipSyncAnim = lipSyncAnim;
		char tNext = lipSync[0];
		lipSync.erase(0,1);
		while(tNext=='(' && lipSync.length()>0)
		{
			//Obtain speed
			String tSpeedString = lipSync;
			if(tSpeedString.find_first_of(")")!=String::npos)tSpeedString.erase(tSpeedString.find_first_of(")"));
			else{stopLipSync(); return;}
			lipSync.erase(0,lipSync.find_first_of(")")+1);
			lipSyncSpeed = StringConverter::parseReal(tSpeedString);
			if(lipSyncSpeed<0)lipSyncSpeed *= -1;
			if(lipSyncSpeed>5)lipSyncSpeed = 5;
			//Read next
			if(lipSync.length()>0)
			{
				tNext = lipSync[0];
				lipSync.erase(0,1);
			}
		}
		if(tNext=='('){stopLipSync(); return;}
		if(tNext=='a')lipSyncAnim = "MouthA";
		else if(tNext=='e'||tNext=='n'||tNext=='s'||tNext=='j'||tNext=='t'||tNext=='d'||tNext=='x'||tNext=='z')lipSyncAnim = "MouthE";
		else if(tNext=='m'||tNext=='b'||tNext=='p')lipSyncAnim = "MouthM";
		else if(tNext=='o'||tNext=='u'||tNext=='w')lipSyncAnim = "MouthO";
		else if(tNext=='f'||tNext=='v')lipSyncAnim = "MouthF";
		else if(tNext=='i'||tNext=='y')lipSyncAnim = "MouthI";
		else if(tNext=='l'||tNext=='r')lipSyncAnim = "MouthL";
		else if(tNext==' '){lipSyncCounter = 0.2; lipSyncAnim = "";}
		else if(tNext==','){lipSyncCounter = 1; lipSyncAnim = "";}
		else if(tNext=='.'){lipSyncCounter = 2; lipSyncAnim = "";}
		if(lipSyncAnim==prevLipSyncAnim){lipSyncCounter = 0.4; return;}
		if(lipSyncAnim!="")
		{
			mHeadEnt->getAnimationState(lipSyncAnim)->setEnabled(true);
			mHeadEnt->getAnimationState(lipSyncAnim)->setLoop(false);
			mHeadEnt->getAnimationState(lipSyncAnim)->setTimePosition(0);
		}
	}
	void doLipSync(const String &data)
	{
		stopLipSync();
		lipSyncCounter = 0;
		lipSyncSpeed = 1;
		lipSync = data;
		StringUtil::toLowerCase(lipSync);
	}
	void stopLipSync()
	{
		if(lipSyncAnim!="")mHeadEnt->getAnimationState(lipSyncAnim)->setEnabled(false);
		lipSyncAnim = "";
		lipSync = "";
		lipSyncCounter = -1;
	}
	void resumeEmote(const FrameEvent &evt)
	{
		if(prevLipSyncAnim!=""&&mHeadEnt->getAnimationState(prevLipSyncAnim)->getTimePosition()>0)mHeadEnt->getAnimationState(prevLipSyncAnim)->addTime(-evt.timeSinceLastFrame*lipSyncSpeed*2);
		if(mHeadEnt->getAnimationState("FaceNormal")->getWeight()<1)mHeadEnt->getAnimationState("FaceNormal")->setWeight(mHeadEnt->getAnimationState("FaceNormal")->getWeight()+evt.timeSinceLastFrame*lipSyncSpeed*6);
		if(mHeadEnt->getAnimationState("FaceSmile")->getWeight()<1)mHeadEnt->getAnimationState("FaceSmile")->setWeight(mHeadEnt->getAnimationState("FaceSmile")->getWeight()+evt.timeSinceLastFrame*lipSyncSpeed*6);
		if(mHeadEnt->getAnimationState("FaceOpenJaw")->getWeight()<1)mHeadEnt->getAnimationState("FaceOpenJaw")->setWeight(mHeadEnt->getAnimationState("FaceOpenJaw")->getWeight()+evt.timeSinceLastFrame*lipSyncSpeed*6);
		if(mHeadEnt->getAnimationState("FaceNormal")->getWeight()>1)mHeadEnt->getAnimationState("FaceNormal")->setWeight(1);
		if(mHeadEnt->getAnimationState("FaceSmile")->getWeight()>1)mHeadEnt->getAnimationState("FaceSmile")->setWeight(1);
		if(mHeadEnt->getAnimationState("FaceOpenJaw")->getWeight()>1)mHeadEnt->getAnimationState("FaceOpenJaw")->setWeight(1);
	}
	virtual bool isIndexedUnit()
	{
		return false;
	}
};

#endif