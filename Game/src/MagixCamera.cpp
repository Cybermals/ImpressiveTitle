#include "MagixCamera.h"
#include "MagixConst.h"


//Camera Functions
//================================================================================
void MagixCamera::initialize(SceneManager *sceneMgr, MagixPlayer *player,
    MagixCollisionManager *collMgr)
{
    mCollisionManager = collMgr;
    mSceneMgr = sceneMgr;
    mPlayer = player;

    mRayQuery = sceneMgr->createRayQuery(Ray());
    mCamera = sceneMgr->getCamera("PlayerCam");
    mCamera->setQueryFlags(WORLDOBJECT_MASK);
    mCamera->setPosition(0, 0, 0);
    mCamPivotNode = sceneMgr->getRootSceneNode()->createChildSceneNode(
        "CamPivotNode");
    mCamPivotPitchNode = mCamPivotNode->createChildSceneNode("CamPivotPitchNode");
    mCamNode = mCamPivotPitchNode->createChildSceneNode("CameraNode");
    mCamTrackNode = sceneMgr->getRootSceneNode()->createChildSceneNode(
        "CamTrackNode");
    mCamNode->attachObject(mCamera);
}


void MagixCamera::reset(bool resetCamZoom)
{
    stopCameraTrack();

    if(resetCamZoom)
    {
        camZoom = 2.5;
    }

    setCameraMode(camMode, isFirstPerson, true);

    if(camMode != CAMERA_FREE && mPlayer->getObjectNode())
    {
        setOrientation(mPlayer->getLookDirection());
    }

    isUnderwater = false;
    isQuake = false;
    isQuakeFadeOut = false;
    quakeOffset = Vector3::ZERO;
}


void MagixCamera::update(const FrameEvent &evt)
{
    if(updateCameraTrack(evt))
    {
        updateWaterCollision();
        updateCameraEffects(evt.timeSinceLastFrame);
        return;
    }

    //Locked camera
    if(camMode == CAMERA_LOCKED || camMode == CAMERA_FREECURSOR ||
        camMode == CAMERA_FREECURSORNEW)
    {
        //Camera follows unit
        mCamPivotNode->setPosition(mPlayer->getPosition(isFirstPerson) +
            (isFirstPerson ? mPlayer->getObjectNode()->getOrientation() *
            Vector3(0, 0, (Real)(3.5 * mPlayer->getObjectNode()->getScale().x)) :
            Vector3::ZERO));

        //Make unit look at camera's perspective
        if(mPlayer->getAutoTrackObject() == 0)
        {
            mPlayer->setLookDirection(getCameraOrientation());
        }
        //Lock on target
        else if(mPlayer->getIsLockedOn() && camMode != CAMERA_FREECURSOR &&
            camMode != CAMERA_FREECURSORNEW)
        {
            //tMat.FromEulerAnglesXYZ(mCamera->getOrientation().getPitch(),mCamPivotNode->getOrientation().getYaw(),Degree(0));
            Quaternion tCamOrientationBuffer = getCameraOrientation(); //Quaternion(tMat);

            mCamera->lookAt(mPlayer->getAutoTrackObject()->getPosition());

            Matrix3 tMat;
            tMat.FromEulerAnglesXYZ(mCamera->getOrientation().getPitch(),
                mPlayer->getLookDirection().getYaw(), Degree(0));
            camTransitionOrientation = Quaternion(tMat);
            //mCamera->setOrientation(Quaternion(Degree(0),Vector3::UNIT_Y));
            //mCamera->pitch(camPitch);

            //pan tCamOrientationBuffer to camTransitionOrientation
            if(!tCamOrientationBuffer.equals(camTransitionOrientation, Degree(1)))
            {
                if(3 * evt.timeSinceLastFrame < 1)
                {
                    tCamOrientationBuffer = tCamOrientationBuffer.Slerp(3 *
                        evt.timeSinceLastFrame, tCamOrientationBuffer,
                        camTransitionOrientation, true);
                }
                else
                {
                    tCamOrientationBuffer = camTransitionOrientation;
                }
            }

            setOrientation(tCamOrientationBuffer);
            clampCameraToTerrain();
            updateWaterCollision();
            updateCameraEffects(evt.timeSinceLastFrame);
            return;
        }

        //Hide features on first person
        if(isFirstPerson)
        {
            if(mPlayer->getHeadEnt()->isVisible())
            {
                mPlayer->getHeadEnt()->setVisible(false);
            }

            if(mPlayer->getManeEnt() && mPlayer->getManeEnt()->isVisible())
            {
                mPlayer->getManeEnt()->setVisible(false);
            }
        }
    }
    //Free camera
    else if(camMode == CAMERA_FREE)
    {
        Vector3 translateVector = Vector3::ZERO;

        if(moveForward)
        {
            translateVector.z = -camSpeed * evt.timeSinceLastFrame;
        }
        else if(moveBack)
        {
            translateVector.z = camSpeed * evt.timeSinceLastFrame;
        }

        if(moveLeft)
        {
            translateVector.x = -camSpeed * evt.timeSinceLastFrame;
        }
        else if(moveRight)
        {
            translateVector.x = camSpeed * evt.timeSinceLastFrame;
        }

        if(moveUp)
        {
            translateVector.y = camSpeed * evt.timeSinceLastFrame;
        }
        else if(moveDown)
        {
            translateVector.y = -camSpeed * evt.timeSinceLastFrame;
        }

        if(translateVector != Vector3::ZERO)
        {
            translate(translateVector);
        }
    }

    if(turnUp)
    {
        pitch(Degree((Real)(45 * camSpeed * 0.02 * evt.timeSinceLastFrame)));
    }
    else if(turnDown)
    {
        pitch(Degree((Real)(-45 * camSpeed * 0.02 * evt.timeSinceLastFrame)));
    }

    if(turnLeft)
    {
        yaw(Degree((Real)(45 * camSpeed * 0.02 * evt.timeSinceLastFrame)));
    }
    else if(turnRight)
    {
        yaw(Degree((Real)(-45 * camSpeed * 0.02 * evt.timeSinceLastFrame)));
    }

    clampCameraToTerrain();
    updateWaterCollision();
    updateCameraEffects(evt.timeSinceLastFrame);
}


const unsigned char MagixCamera::getCameraMode()
{
    return camMode;
}


void MagixCamera::setCameraMode(const unsigned char &mode, bool firstPerson, bool forceUpdate)
{
    if(!mPlayer->getObjectNode())
    {
        return;
    }

    if(camMode == mode && isFirstPerson == firstPerson && !forceUpdate)
    {
        return;
    }

    camMode = mode;
    isFirstPerson = firstPerson;

    switch(camMode)
    {
        //Locked 3rd person camera
    case CAMERA_LOCKED:
        if(!mCamera->isAttached())
        {
            mCamNode->attachObject(mCamera);
            mCamera->setPosition(0, 0, 0);
        }

        if(!isFirstPerson)
        {
            doCameraZoom(0);
            pitch(Degree(0));
        }

        setOrientation(mPlayer->getLookDirection());
        mPlayer->setPlayerTargetOnHit = true;
        break;

        //Locked 3rd person camera with free cursor
    case CAMERA_FREECURSOR:
    case CAMERA_FREECURSORNEW:
        if(!mCamera->isAttached())
        {
            mCamNode->attachObject(mCamera);
            mCamera->setPosition(0, 0, 0);
        }

        if(!isFirstPerson)
        {
            doCameraZoom(0);
            pitch(Degree(0));
        }

        setOrientation(mPlayer->getLookDirection());
        mPlayer->setPlayerTargetOnHit = false;
        break;

        //Free 3rd person camera
    case CAMERA_FREE:
        if(mCamera->isAttached())
        {
            mCamNode->detachObject(mCamera);
        }

        mCamera->setPosition(mPlayer->getPosition() +
            Vector3(0, mPlayer->getObjectNode()->getScale().y * 20,
            mPlayer->getObjectNode()->getScale().z * 24));
        mCamera->setOrientation(Quaternion(Degree(-15), Vector3::UNIT_X));
        camPitch = Degree(-15);
        mPlayer->setPlayerTargetOnHit = false;
        break;
    }

    if(isFirstPerson)
    {
        mCamNode->setPosition(0, 0, 0);
        mPlayer->setFreeLook(true);
        mPlayer->setControlMode(CONTROL_FIRSTPERSON);
        pitch(Degree(0));
    }
    else if(mPlayer->getControlMode() == CONTROL_FIRSTPERSON)
    {
        mPlayer->setControlMode((mPlayer->defaultControlModeIsAbsolute ? CONTROL_ABSOLUTE : CONTROL_RELATIVE));
    }

    if(!isFirstPerson)
    {
        if(!mPlayer->getHeadEnt()->isVisible())
        {
            mPlayer->getHeadEnt()->setVisible(true);
        }

        if(mPlayer->getManeEnt() && !mPlayer->getManeEnt()->isVisible())
        {
            mPlayer->getManeEnt()->setVisible(true);
        }
    }

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
}


bool MagixCamera::getIsFirstPerson()
{
    return isFirstPerson;
}


void MagixCamera::yaw(const Degree &angle)
{
    if(camMode == CAMERA_FREE)
    {
        mCamera->yaw(angle);
    }
    else
    {
        mCamPivotNode->yaw(angle);
    }
}


void MagixCamera::pitch(const Degree &angle)
{
    if(camMode == CAMERA_FREE || camMode == CAMERA_FREECURSOR)
    {
        mCamera->pitch(angle);
    }
    else
    {
        mCamPivotPitchNode->pitch(angle * 0.75);
        mCamera->pitch(angle*0.25);
    }

    camPitch += angle;
    const Real tMaxUpPitch = (Real)(isFirstPerson ? 75 : (camMode == CAMERA_FREE ? 90 : 89));
    const Real tMaxDownPitch = (Real)(isFirstPerson ? -75 : (camMode == CAMERA_FREE ? -90 : -89));

    if(camPitch > Degree(tMaxUpPitch))
    {
        if(camMode == CAMERA_FREE || camMode == CAMERA_FREECURSOR)
        {
            mCamera->pitch(Degree(tMaxUpPitch) - camPitch);
        }
        else
        {
            mCamPivotPitchNode->pitch((Degree(tMaxUpPitch) - camPitch) * 0.75);
            mCamera->pitch((Degree(tMaxUpPitch) - camPitch) * 0.25);
        }

        camPitch = Degree(tMaxUpPitch);
    }
    else if(camPitch < Degree(tMaxDownPitch))
    {
        if(camMode == CAMERA_FREE || camMode == CAMERA_FREECURSOR)
        {
            mCamera->pitch(Degree(tMaxDownPitch) - camPitch);
        }
        else
        {
            mCamPivotPitchNode->pitch((Degree(tMaxDownPitch) - camPitch) * 0.75);
            mCamera->pitch((Degree(tMaxDownPitch) - camPitch) * 0.25);
        }

        camPitch = Degree(tMaxDownPitch);
    }

}


void MagixCamera::setOrientation(const Quaternion &direction)
{
    if(camMode == CAMERA_FREE)
    {
        mCamera->setOrientation(direction);
        camPitch = direction.getPitch();
        pitch(Degree(0));
    }
    else
    {
        mCamPivotNode->setOrientation(Quaternion(Degree(0), Vector3::UNIT_Y));
        mCamPivotNode->yaw(direction.getYaw());
        mCamPivotPitchNode->setOrientation(Quaternion(Degree(0), Vector3::UNIT_Y));
        mCamera->setOrientation(Quaternion(Degree(0), Vector3::UNIT_Y));
        //mCamera->pitch(direction.getPitch());
        camPitch = Degree(0);
        pitch(direction.getPitch());
    }
}


void MagixCamera::clampCameraToTerrain()
{
    if(mCamera->isAttached())
    {
        mCamera->setPosition(0, 0, 0);
        const Real tHeight = getGroundHeight(mCamera->getDerivedPosition().x,
            mCamera->getDerivedPosition().z);
        const Real tDiff = tHeight + 5 - mCamera->getDerivedPosition().y;

        if(tDiff > 0)
        {
            mCamera->setPosition(0, tDiff, 0);
        }
    }
    else
    {
        const Real tHeight = getGroundHeight(mCamera->getPosition().x,
            mCamera->getPosition().z);

        if(mCamera->getPosition().y < tHeight + 5)
        {
            mCamera->setPosition(mCamera->getPosition().x, tHeight + 5, mCamera->getPosition().z);
        }
    }
}


const Real MagixCamera::getGroundHeight(const Real &x, const Real &z)
{
    Real tHeight = 0;
    static Ray updateRay;
    updateRay.setOrigin(Vector3(x, 5000, z));
    updateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
    mRayQuery->setRay(updateRay);
    RaySceneQueryResult& qryResult = mRayQuery->execute();
    RaySceneQueryResult::iterator i = qryResult.begin();

    if(i != qryResult.end() && i->worldFragment)
    {
        tHeight = i->worldFragment->singleIntersection.y;
    }

    return tHeight;
}


void MagixCamera::translate(const Vector3 &vec)
{
    mCamera->moveRelative(vec);
}


void MagixCamera::heldMove(const unsigned short dir, bool flag)
{
    switch(dir)
    {
    case DIR_FORWARD:
        moveForward = flag;
        break;

    case DIR_BACK:
        moveBack = flag;
        break;

    case DIR_LEFT:
        moveLeft = flag;
        break;

    case DIR_RIGHT:
        moveRight = flag;
        break;

    case DIR_UP:
        moveUp = flag;
        break;

    case DIR_DOWN:
        moveDown = flag;
        break;
    }
}


void MagixCamera::heldTurn(const unsigned short dir, bool flag)
{
    switch(dir)
    {
    case DIR_LEFT:
        turnLeft = flag;
        break;

    case DIR_RIGHT:
        turnRight = flag;
        break;

    case DIR_UP:
        turnUp = flag;
        break;

    case DIR_DOWN:
        turnDown = flag;
        break;
    }
}


SceneNode* MagixCamera::getCamPivotNode()
{
    return mCamPivotNode;
}


SceneNode* MagixCamera::getCamNode()
{
    return mCamNode;
}


SceneNode* MagixCamera::getActiveCamNode()
{
    if(mCamera->getParentSceneNode())
    {
        return mCamera->getParentSceneNode();
    }

    return mCamNode;
}


Camera* MagixCamera::getCamera()
{
    return mCamera;
}


const Vector3 MagixCamera::getPosition()
{
    if(mCamera->isAttached())
    {
        return (mCamera->getDerivedPosition() + mCamera->getPosition());
    }

    return mCamera->getPosition();
}


const Quaternion MagixCamera::getCameraOrientation()
{
    Matrix3 tLookMat;
    tLookMat.FromEulerAnglesXYZ(camPitch,
        mCamPivotNode->getOrientation().getYaw(), Degree(0));
    return Quaternion(tLookMat);
}


void MagixCamera::doCameraZoom(Real dY)
{
    camZoom += (Real)(dY * -0.1);

    if(camZoom < 0.25)
    {
        camZoom = 0.25;
    }
    else if(camZoom > 5)
    {
        camZoom = 5;
    }

    mCamNode->setPosition(0, 0, 0);
    mCamNode->translate(0, mPlayer->getObjectNode()->getScale().y * 3 * camZoom +
        mPlayer->getObjectNode()->getScale().y * 13,
        mPlayer->getObjectNode()->getScale().z * 16 * camZoom);
}


void MagixCamera::resetCameraTrack(Real length)
{
    stopCameraTrack();

    if(mCamNodeTrack)
    {
        mCamNodeTrack->removeAllKeyFrames();
    }

    if(mSceneMgr->hasAnimation("CameraTrack"))
    {
        mSceneMgr->getAnimation("CameraTrack")->destroyAllNodeTracks();
        mSceneMgr->destroyAnimation("CameraTrack");
    }

    Animation *tAnim = mSceneMgr->createAnimation("CameraTrack", length);
    tAnim->setInterpolationMode(Animation::IM_SPLINE);
    mCamNodeTrack = tAnim->createNodeTrack(0, mCamTrackNode);
}


void MagixCamera::addCameraTrackKeyFrame(const Real &timePosition,
    const Vector3 &translation, const Quaternion &rotation)
{
    if(!mCamNodeTrack)
    {
        return;
    }

    TransformKeyFrame *tKey = mCamNodeTrack->createNodeKeyFrame(timePosition);
    tKey->setTranslate(translation);
    tKey->setRotation(rotation);
}


void MagixCamera::doCameraTrack(bool loop, bool autoTrack, SceneNode *node)
{
    if(!mCamNodeTrack)
    {
        return;
    }

    AnimationState *tAnimState = mSceneMgr->createAnimationState("CameraTrack");
    tAnimState->setEnabled(true);
    tAnimState->setLoop(loop);

    mCamera->setPosition(0, 0, 0);
    mCamera->setOrientation(Quaternion(Degree(0), Vector3::UNIT_Y));

    if(camMode != CAMERA_FREE && mCamera->isAttached())
    {
        mCamNode->detachObject(mCamera);
    }

    mCamTrackNode->attachObject(mCamera);

    if(autoTrack)
    {
        mCamera->setAutoTracking(true, node);
    }
}


void MagixCamera::stopCameraTrack()
{
    if(mSceneMgr->hasAnimationState("CameraTrack"))
    {
        mSceneMgr->getAnimationState("CameraTrack")->setEnabled(false);
        mSceneMgr->destroyAnimationState("CameraTrack");
        mCamera->setAutoTracking(false);
    }

    if(mCamera->isAttached() && mCamera->getParentSceneNode() == mCamTrackNode)
    {
        mCamTrackNode->detachObject(mCamera);
        if(camMode != CAMERA_FREE)mCamNode->attachObject(mCamera);
    }
}


bool MagixCamera::updateCameraTrack(const FrameEvent &evt)
{
    if(mSceneMgr->hasAnimationState("CameraTrack"))
    {
        mSceneMgr->getAnimationState("CameraTrack")->addTime(
            evt.timeSinceLastFrame);
        return true;
    }
    return false;
}


void MagixCamera::updateWaterCollision()
{
    isUnderwater = (mCollisionManager->getWaterBoxHit(
        (mCamera->isAttached() ? mCamera->getDerivedPosition() :
        mCamera->getPosition())) != 0);
}


bool MagixCamera::getIsUnderwater()
{
    return isUnderwater;
}


const Degree MagixCamera::getYaw()
{
    if(mCamera->isAttached())
    {
        return mCamPivotNode->getOrientation().getYaw();
    }

    return mCamera->getOrientation().getYaw();
}


void MagixCamera::doQuake(bool flag, bool fade)
{
    isQuake = flag;
    quakeCountdown = (Real)(fade ? 1 : 0);

    if(!flag)
    {
        mCamera->move(-quakeOffset);
    }

    if(!flag && fade)
    {
        isQuakeFadeOut = true;
    }

    quakeOffset = Vector3::ZERO;
}


void MagixCamera::updateCameraEffects(const Real &timeElapsed)
{
    if(isQuake || isQuakeFadeOut)
    {
        if(quakeCountdown > 0)
        {
            quakeCountdown -= timeElapsed;
        }
        else if(quakeCountdown < 0)
        {
            quakeCountdown = 0;
        }

        mCamera->move(-quakeOffset);
        quakeOffset = (isQuakeFadeOut ? quakeCountdown : 1 - quakeCountdown) *
            Vector3(Math::RangeRandom(-3, 3), Math::RangeRandom(-3, 3),
            Math::RangeRandom(-3, 3));
        mCamera->move(quakeOffset);

        if(isQuakeFadeOut && quakeCountdown == 0)
        {
            isQuakeFadeOut = false;
        }
    }
}


void MagixCamera::changeSpeed(const Real &ratio)
{
    camSpeed *= ratio;

    if(camSpeed > 200)
    {
        camSpeed = 200;
    }
    else if(camSpeed < 12.5)
    {
        camSpeed = 12.5;
    }
}