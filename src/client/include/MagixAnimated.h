#pragma once

#include "MagixObject.h"

class MagixAnimated : public MagixObject
{
protected:
	unsigned short objectID;
	Vector3 mForce;
	Real groundHeight;
	Real maxSpeed;
	Real speedMultiplier;
	Vector3 lastPosition;
	Real updateTime;
	Degree faceDirectionYaw;
	bool antiGravity;
	Vector3 penaltyVelocity;
	int record;
public:
	bool isHidden;
	MagixAnimated();
	~MagixAnimated();
	void createObject(const unsigned short &iID, SceneManager *sceneMgr, const String &objectMesh, const String &matName);
	void destroyObject(SceneManager *sceneMgr);
	virtual void updateAnimation(const FrameEvent &evt);
	virtual void update(const FrameEvent &evt);
	void updateFaceDirection(const FrameEvent &evt);
	void updatePhysics(const FrameEvent &evt);
	void addForce(const Vector3 &value);
	void setYForce(const Real &value);
	void setForce(const Vector3 &value);
	const Vector3 getForce();
	void setPosition(const Real &x, const Real &y, const Real &z);
	void setPosition(const Vector3 &position);
	virtual const Vector3 getPosition();
	SceneNode* getObjectNode();
	void setGroundHeight(const Real &value);
	const Real getGroundHeight();
	const Real distanceFromGround();
	virtual const OBJECT_TYPE getType();
	bool isPositionChanged(bool excludeYAxis = false);
	const Vector3 getLastPosition();
	bool isMovingReverse();
	void yaw(const Degree &angle, bool instant = false);
	void setYaw(const Degree &angle, bool instant = false);
	const Degree getYaw();
	const Quaternion getFaceDirection();
	void setAntiGravity(bool flag);
	bool isAntiGravity();
	void toggleAntiGravity();
	void addPenaltyVelocity(const Vector3 &value);
	void setSpeedMultiplier(const Real &value);
	const Real getSpeedMultiplier();
	void setMaxSpeed(const Real &value);
};
