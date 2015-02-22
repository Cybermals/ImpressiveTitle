#pragma once

#define CONTROL_RELATIVE 1
#define CONTROL_ABSOLUTE 2
#define CONTROL_FIRSTPERSON 3

#include "MagixExternalDefinitions.h"
#include "MagixLiving.h"
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
	MagixUnit();
	~MagixUnit();
	void createUnit(const unsigned short iID, SceneManager *sceneMgr, const String &bodyMesh, const String &headMesh, const String &maneMesh, const String &tailMesh, const String &wingMesh, const String &matName, const String &tuftMesh);
	void destroyUnit(SceneManager *sceneMgr);
	void setMaterial(const String &matName);
	void setColours(const ColourValue &pelt, const ColourValue &underfur, const ColourValue &mane, const ColourValue &nose, const ColourValue &eyeTop, const ColourValue &eyeBot, const ColourValue &ears, const ColourValue &tailtip, const ColourValue &eyes, const ColourValue &wing, const ColourValue &marking);
	void setFullColour(const ColourValue &fullColour);
	void updateAnimation(const FrameEvent &evt, MagixExternalDefinitions *def);
	void updateAnimBase(const FrameEvent &evt);
	void updateAnimSecond(const FrameEvent &evt);
	virtual void updateMovement(const FrameEvent &evt);
	void updateAction(const FrameEvent &evt, MagixExternalDefinitions *def);
	void doSit(bool skipTransition = false);
	bool isSitting(bool excludeTransition = false);
	void doSideLay(bool skipTransition = false);
	void doCurl(bool skipTransition = false);
	bool isLaying(bool excludeTransition = false);
	bool isSideLaying(bool excludeTransition = false);
	bool isLeaning(bool excludeTransition = false);
	bool isRolledOver();
	bool isPlopped();
	bool isCurled();
	virtual void update(const FrameEvent &evt, MagixExternalDefinitions *def);
	void updateAutoTrack();
	void setFreeLook(bool flag, bool timeout = true);
	bool getFreeLook();
	void setTarget(const Real &x, const Real &y, const Real &z, const unsigned char &action = TARGET_RUNTO);
	virtual void setTarget(const Vector3 &dest, const unsigned char &action = TARGET_RUNTO);
	const Vector3 getTarget();
	unsigned short getTargetAction();
	void resetTarget();
	virtual void stopAction();
	void setLookDirection(const Quaternion &dir, bool setBuffer = false);
	void setLookTarget(const Vector3 &target, bool fromHead = true);
	void setAutoTrackObject(MagixObject *target);
	MagixObject* getAutoTrackObject();
	const Quaternion getLookDirection();
	const Vector3 getPosition(bool headPosition = false);
	void changeAnimBase(const String &type, const Real &speed, bool resetTimePosition = false, bool reverseAnimation = false, bool loop = true);
	void changeAnimSecond(const String &type, const Real &speed, bool resetTimePosition = false, bool reverseAnimation = false, bool loop = true);
	virtual const OBJECT_TYPE getType();
	const unsigned short getUnitID();
	void createNameTag(const String &name, MovableTextOverlayAttributes *attrs);
	void createUserTag(const String &name, MovableTextOverlayAttributes *attrs);
	void createChatBubble(String caption, MovableTextOverlayAttributes *attrs, const Real &tLeft = 0.0, const Real &tTop = 0.0);
	MovableTextOverlay* getNameTag();
	MovableTextOverlay* getUserTag();
	MovableTextOverlay* getChatBubble();
	void showNameTag(bool flag);
	void showUserTag(bool flag);
	void showChatBubble(bool flag);
	bool isNameTagVisible();
	bool isUserTagVisible();
	bool isChatBubbleVisible();
	void updateNameTag(const FrameEvent &evt);
	void updateUserTag(const FrameEvent &evt);
	void updateChatBubble(const FrameEvent &evt);
	void updateEmote(const FrameEvent &evt, MagixExternalDefinitions *def);
	void setEmote(const String &mode, MagixExternalDefinitions *def);
	void setEmoteState(const String &mode, const Real &weight, MagixExternalDefinitions *def);
	void setFaceAnimState(const String &anim, const Real &weight, bool isCurrent);
	void replaceHeadMesh(SceneManager *sceneMgr, const String &meshName, bool resetMaterial = false);
	void replaceManeMesh(SceneManager *sceneMgr, const String &meshName, bool resetMaterial = false);
	void replaceTailMesh(SceneManager *sceneMgr, const String &meshName, bool resetMaterial = false);
	void replaceWingMesh(SceneManager *sceneMgr, const String &meshName, bool resetMaterial = false);
	void replaceTuftMesh(SceneManager *sceneMgr, const String &meshName, bool resetMaterial = false);
	const String getMatGroupName();
	const String getMeshString();
	const String getMeshString2();
	const String getColourString();
	const String vect4ToString(Vector4 vect, const unsigned short &tDeci);
	void setColours(const String &colourString);
	const String getName();
	const String getUser();
	void setJump(bool flag);
	bool getJump();
	void setIsWalking(bool flag);
	bool getIsWalking();
	const String doAttack(MagixExternalDefinitions *def, bool doEmote = true);
	void forceAttack(const String &name);
	bool isReadyToAttack();
	virtual bool updateAttack(const FrameEvent &evt, MagixExternalDefinitions *def);
	bool isAttacking();
	void stopStance(MagixExternalDefinitions *def);
	bool getIsStance();
	void toggleStance(MagixExternalDefinitions *def, bool doEmote = true);
	void stopAttack();
	void setAttack(const String &name);
	Entity* getBodyEnt();
	Entity* getHeadEnt();
	Entity* getTailEnt();
	Entity* getManeEnt();
	Entity* getWingEnt();
	Entity* getTuftEnt();
	bool popHasNewAttack();
	bool popHasStoppedAttack();
	const String getAttackName();
	void cancelAction(bool hitAnim, const Real &duration = 0, MagixExternalDefinitions *def = 0);
	void cancelAction(const Real &duration, const String &anim, bool reverseAnim = false);
	const Real getHitTimeout();
	bool isDisabled();
	void setDeathTimeout(const Real &duration);
	const Real getDeathTimeout();
	bool isDying();
	void kill();
	bool isDead();
	void clearAttackList();
	void pushAttackList(const String &attackName);
	void setCustomAnimation(bool flag, const String &animName = "", Real animSpeed = 1, bool loop = false, bool reverse = false);
	virtual unsigned short getControlMode();
	void setIsSwimming(bool flag);
	bool getIsSwimming();
	void equip(SceneManager *sceneMgr, MagixExternalDefinitions *mDef, const String &meshName, const Vector3 &offset = Vector3::ZERO, const short &slot = -1, bool altAnim = false);
	const String unequip(SceneManager *sceneMgr, MagixExternalDefinitions *mDef, const unsigned short &iID);
	const String getEquip(const unsigned short &iID);
	void doCrouch(bool flag);
	bool getIsCrouching();
	void doHeadswing();
	bool isHeadswinging();
	void doHeadbang();
	bool isHeadbanging();
	void doButtswing();
	bool isButtswinging();
	void doWingwave();
	bool isWingwaving();
	void doMoonwalk();
	bool isMoonwalking();
	void doThriller();
	bool isThrillering();
	void doRoar();
	bool isRoaring();
	void doRofl();
	bool isRofling();
	void doFaint();
	bool isFainting();
	bool isDancing();
	void doNodHead();
	bool isNodHead();
	void doShakeHead();
	bool isShakeHead();
	void doNodHeadSlow();
	bool isNodHeadSlow();
	void doShakeHeadSlow();
	bool isShakeHeadSlow();
	void doHeadTilt();
	bool isHeadTilt();
	void toggleLick();
	bool isLick();
	void toggleNuzzle();
	bool isNuzzle();
	void doSniff();
	bool isSniff();
	void doLaugh();
	bool isLaugh();
	void doChuckle();
	bool isChuckle();
	void doTailFlick();
	bool isTailFlick();
	void doPoint();
	bool isPoint();
	void doStretch();
	bool isStretch();
	bool isOneTimeAction();
	bool isOneTimeHeadAction();
	bool isOneTimeTailAction();
	void setMarkings(const unsigned char &bodyID, const unsigned char &headID, const unsigned char &tailID);
	void getMarkings(short &bodyID, short &headID, short &tailID);
	void setIsInWater(bool flag);
	bool getIsInWater();
	void doDoubleJump();
	bool getHasDoubleJumped();
	void updateLipSync(const FrameEvent &evt);
	void doLipSync(const String &data);
	void stopLipSync();
	void resumeEmote(const FrameEvent &evt);
	virtual bool isIndexedUnit();
};
