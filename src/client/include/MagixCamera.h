#pragma once

#include "MagixCollisionManager.h"
#include "MagixPlayer.h"

#define CAMERA_LOCKED 1
#define CAMERA_FREECURSOR 2
#define CAMERA_FREE 3
#define CAMERA_FREECURSORNEW 4

#define DIR_FORWARD 1
#define DIR_BACK 2
#define DIR_LEFT 3
#define DIR_RIGHT 4
#define DIR_UP 5
#define DIR_DOWN 6

using namespace Ogre;

class MagixCamera
{
protected:
	MagixCollisionManager *mCollisionManager;
	SceneManager *mSceneMgr;
	Camera *mCamera;
	SceneNode *mCamNode;
	SceneNode *mCamPivotNode;
	SceneNode *mCamPivotPitchNode;
	SceneNode *mCamTrackNode;
	NodeAnimationTrack *mCamNodeTrack;
	MagixPlayer *mPlayer;
	RaySceneQuery *mRayQuery;
	unsigned char camMode;
	bool moveForward;
	bool moveBack;
	bool moveLeft;
	bool moveRight;
	bool moveUp;
	bool moveDown;
	bool turnUp;
	bool turnDown;
	bool turnLeft;
	bool turnRight;
	Degree camPitch;
	Real camZoom;
	Real camSpeed;
	Quaternion camTransitionOrientation;
	bool isFirstPerson;
	bool isUnderwater;
	bool isQuake;
	bool isQuakeFadeOut;
	Vector3 quakeOffset;
	Real quakeCountdown;
public:
	MagixCamera()
	{
		mCollisionManager = 0;
		mSceneMgr = 0;
		mPlayer = 0;
		mRayQuery = 0;
		mCamera = 0;
		mCamNode = 0;
		mCamPivotNode = 0;
		mCamPivotPitchNode = 0;
		mCamTrackNode = 0;
		mCamNodeTrack = 0;
		camMode = CAMERA_FREECURSOR;
		moveForward = false;
		moveBack = false;
		moveLeft = false;
		moveRight = false;
		moveUp = false;
		moveDown = false;
		turnUp = false;
		turnDown = false;
		turnLeft = false;
		turnRight = false;
		camPitch = Degree(0);
		camZoom = 2.5;
		camSpeed = 50;
		camTransitionOrientation = Quaternion::ZERO;
		isFirstPerson = false;
		isUnderwater = false;
		isQuake = false;
		isQuakeFadeOut = false;
		quakeOffset = Vector3::ZERO;
		quakeCountdown = 0;
	}
	~MagixCamera()
	{
		if(mRayQuery)delete mRayQuery;
	}
	void initialize(SceneManager *sceneMgr, MagixPlayer *player, MagixCollisionManager *collMgr);

	void reset(bool resetCamZoom = true);

	void update(const FrameEvent &evt);

	const unsigned char getCameraMode();

	void setCameraMode(const unsigned char &mode, bool firstPerson = false, bool forceUpdate = false);

	bool getIsFirstPerson();

	void yaw(const Degree &angle);

	void pitch(const Degree &angle);

	void setOrientation(const Quaternion &direction);

	void clampCameraToTerrain();

	const Real getGroundHeight(const Real &x, const Real &z);

	void translate(const Vector3 &vec);

	void heldMove(const unsigned short dir, bool flag);

	void heldTurn(const unsigned short dir, bool flag);

	SceneNode* getCamPivotNode();

	SceneNode* getCamNode();

	SceneNode* getActiveCamNode();

	Camera* getCamera();

	const Vector3 getPosition();

	const Quaternion getCameraOrientation();

	void doCameraZoom(Real dY);

	void resetCameraTrack(Real length);

	void addCameraTrackKeyFrame(const Real &timePosition, const Vector3 &translation, const Quaternion &rotation = Quaternion(Degree(180), Vector3::UNIT_Y));

	void doCameraTrack(bool loop, bool autoTrack = false, SceneNode *node = 0);

	void stopCameraTrack();

	bool updateCameraTrack(const FrameEvent &evt);

	void updateWaterCollision();

	bool getIsUnderwater();

	const Degree getYaw();

	void doQuake(bool flag, bool fade = false);

	void updateCameraEffects(const Real &timeElapsed);

	void changeSpeed(const Real &ratio);
};
