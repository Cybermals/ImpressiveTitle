/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    ExampleFrameListener.h
Description: Defines an example frame listener which responds to frame events.
This frame listener just moves a specified camera around based on
keyboard and mouse movements.
Mouse:    Freelook
W or Up:  Forward
S or Down:Backward
A:        Step left
D:        Step right
             PgUp:     Move upwards
             PgDown:   Move downwards
             F:        Toggle frame rate stats on/off
			 R:        Render mode
             T:        Cycle texture filtering
                       Bilinear, Trilinear, Anisotropic(8)
             P:        Toggle on/off display of camera position / orientation
-----------------------------------------------------------------------------
*/

#ifndef __MagixFrameListener_H__
#define __MagixFrameListener_H__

#include "Ogre.h"
#include "OgreStringConverter.h"
#include "OgreException.h"

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS.h>

using namespace Ogre;

class MagixFrameListener: public FrameListener, public WindowEventListener
{
protected:
	void updateStats(void)
	{
		static String currFps = "Current FPS: ";
		static String avgFps = "Average FPS: ";
		static String bestFps = "Best FPS: ";
		static String worstFps = "Worst FPS: ";
		static String tris = "Triangle Count: ";
		static String batches = "Batch Count: ";

		// update stats when necessary
		try {
			OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
			OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
			OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
			OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

			const RenderTarget::FrameStats& stats = mWindow->getStatistics();
			guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
			guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
			guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
				+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
			guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
				+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

			OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
			guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

			OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
			guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

			OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
			guiDbg->setCaption(mDebugText);
		}
		catch(...) { /* ignore */ }
	}

public:
	// Constructor takes a RenderWindow because it uses that to determine input context
	MagixFrameListener(MagixHandler *magixHandler, RenderWindow* win, Camera* cam, bool bufferedKeys = true, bool bufferedMouse = true) :
		mMagixHandler(magixHandler), /*mCamera(cam), mTranslateVector(Vector3::ZERO),*/ mWindow(win), isStatsOn(false), /*mNumScreenShots(0),
		mMoveScale(0.0f), mRotScale(0.0f), mTimeUntilNextToggle(0), mFiltering(TFO_BILINEAR),
		mAniso(1), mSceneDetailIndex(0), mMoveSpeed(100), mRotateSpeed(36),*/ mDebugOverlay(0),
		mInputManager(0), mMouse(0), mKeyboard(0)
	{
		using namespace OIS;

		mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");

		LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
		ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;

		win->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

		bool useWindowsCursor = false;
		std::ifstream inFile("useWindowsCursor.dat");
		if(inFile.good())useWindowsCursor = true;
		inFile.close();

		if(useWindowsCursor)
		{
			#if defined OIS_WIN32_PLATFORM
			pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
			pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
			//pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
			//pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
			#elif defined OIS_LINUX_PLATFORM
			pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
			pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
			//pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
			//pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
			#endif
			if(ShowCursor(false)<-1)ShowCursor(true);
		}

		mInputManager = InputManager::createInputSystem( pl );

		//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
		mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject( OISKeyboard, bufferedKeys ));
		mMouse = static_cast<Mouse*>(mInputManager->createInputObject( OISMouse, bufferedMouse ));

		//Set initial mouse clipping size
		windowResized(mWindow);

		showDebugOverlay(false);

		//Register as a Window listener
		WindowEventUtilities::addWindowEventListener(mWindow, this);
	}

	//Adjust mouse clipping area
	virtual void windowResized(RenderWindow* rw)
	{
		unsigned int width, height, depth;
		int left, top;
		rw->getMetrics(width, height, depth, left, top);

		const OIS::MouseState &ms = mMouse->getMouseState();
		ms.width = width;
		ms.height = height;
	}

	//Unattach OIS before window shutdown (very important under Linux)
	virtual void windowClosed(RenderWindow* rw)
	{
		//Only close for window that created OIS (the main window in these demos)
		if( rw == mWindow )
		{
			if( mInputManager )
			{
				mInputManager->destroyInputObject( mMouse );
				mInputManager->destroyInputObject( mKeyboard );

				OIS::InputManager::destroyInputSystem(mInputManager);
				mInputManager = 0;
			}
		}
	}

	virtual ~MagixFrameListener()
	{
		//Remove ourself as a Window listener
		WindowEventUtilities::removeWindowEventListener(mWindow, this);
		windowClosed(mWindow);
	}

	/*virtual bool processUnbufferedKeyInput(const FrameEvent& evt)
	{
		using namespace OIS;

		if(mKeyboard->isKeyDown(KC_A))
			mTranslateVector.x = -mMoveScale;	// Move camera left

		if(mKeyboard->isKeyDown(KC_D))
			mTranslateVector.x = mMoveScale;	// Move camera RIGHT

		if(mKeyboard->isKeyDown(KC_W) )
			mTranslateVector.z = -mMoveScale;	// Move camera forward

		if(mKeyboard->isKeyDown(KC_S) )
			mTranslateVector.z = mMoveScale;	// Move camera backward

		if(mKeyboard->isKeyDown(KC_PGUP))
			mTranslateVector.y = mMoveScale;	// Move camera up

		if(mKeyboard->isKeyDown(KC_PGDOWN))
			mTranslateVector.y = -mMoveScale;	// Move camera down

		if(mKeyboard->isKeyDown(KC_RIGHT))
		{
			if(mMagixHandler->getCameraMode()==CAMERA_FREE)mCamera->yaw(-mRotScale);
			else mMagixHandler->getCamPivotNode()->yaw(-mRotScale);
		}

		if(mKeyboard->isKeyDown(KC_LEFT))
		{
			if(mMagixHandler->getCameraMode()==CAMERA_FREE)mCamera->yaw(mRotScale);
			else mMagixHandler->getCamPivotNode()->yaw(mRotScale);
		}

		if(mKeyboard->isKeyDown(KC_UP))
			mCamera->pitch(mRotScale);

		if(mKeyboard->isKeyDown(KC_DOWN))
			mCamera->pitch(-mRotScale);

		
		if( mKeyboard->isKeyDown(KC_ESCAPE) || mKeyboard->isKeyDown(KC_Q) )
			return false;

       	if( mKeyboard->isKeyDown(KC_F) && mTimeUntilNextToggle <= 0 )
		{
			mStatsOn = !mStatsOn;
			showDebugOverlay(mStatsOn);
			mTimeUntilNextToggle = 1;
		}

		if( mKeyboard->isKeyDown(KC_T) && mTimeUntilNextToggle <= 0 )
		{
			switch(mFiltering)
			{
			case TFO_BILINEAR:
				mFiltering = TFO_TRILINEAR;
				mAniso = 1;
				break;
			case TFO_TRILINEAR:
				mFiltering = TFO_ANISOTROPIC;
				mAniso = 8;
				break;
			case TFO_ANISOTROPIC:
				mFiltering = TFO_BILINEAR;
				mAniso = 1;
				break;
			default: break;
			}
			MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
			MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);

			showDebugOverlay(mStatsOn);
			mTimeUntilNextToggle = 1;
		}

		if(mKeyboard->isKeyDown(KC_SYSRQ) && mTimeUntilNextToggle <= 0)
		{
			std::ostringstream ss;
			ss << "screenshot_" << ++mNumScreenShots << ".png";
			mWindow->writeContentsToFile(ss.str());
			mTimeUntilNextToggle = 0.5;
			mDebugText = "Saved: " + ss.str();
		}

		if(mKeyboard->isKeyDown(KC_R) && mTimeUntilNextToggle <=0)
		{
			mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
			switch(mSceneDetailIndex) {
				case 0 : mCamera->setPolygonMode(PM_SOLID); break;
				case 1 : mCamera->setPolygonMode(PM_WIREFRAME); break;
				case 2 : mCamera->setPolygonMode(PM_POINTS); break;
			}
			mTimeUntilNextToggle = 0.5;
		}

		static bool displayCameraDetails = false;
		if(mKeyboard->isKeyDown(KC_P) && mTimeUntilNextToggle <= 0)
		{
			displayCameraDetails = !displayCameraDetails;
			mTimeUntilNextToggle = 0.5;
			if (!displayCameraDetails)
				mDebugText = "";
		}

		if(mKeyboard->isKeyDown(KC_SPACE) && mTimeUntilNextToggle <= 0 )
		{
			mMagixHandler->resetScene();
			mTimeUntilNextToggle = 0.5;
		}
		if(mKeyboard->isKeyDown(KC_RETURN) && mTimeUntilNextToggle <= 0 )
		{
			mMagixHandler->togglePause();
			mTimeUntilNextToggle = 0.5;
		}
		if(mKeyboard->isKeyDown(KC_Z) && mTimeUntilNextToggle <= 0 )
		{
			mMagixHandler->fadeBlack();
			mTimeUntilNextToggle = 0.5;
		}
		if(mKeyboard->isKeyDown(KC_X) && mTimeUntilNextToggle <= 0 )
		{
			mMagixHandler->fadeBlack(-2.5);
			mTimeUntilNextToggle = 0.5;
		}
		if(mKeyboard->isKeyDown(KC_TAB) && mTimeUntilNextToggle <= 0 )
		{
			mMagixHandler->toggleCameraMode();
			mTimeUntilNextToggle = 0.5;
		}
		
		if(mMagixHandler->getCameraMode()==CAMERA_LOCKED)
		{
			mMagixHandler->getPlayer()->setLeft(mKeyboard->isKeyDown(KC_A));
			mMagixHandler->getPlayer()->setRight(mKeyboard->isKeyDown(KC_D));
			mMagixHandler->getPlayer()->setUp(mKeyboard->isKeyDown(KC_W));
			mMagixHandler->getPlayer()->setDown(mKeyboard->isKeyDown(KC_S));
		}

		// Print camera details
		if(displayCameraDetails)
			mDebugText = "P: " + StringConverter::toString(mCamera->getDerivedPosition()) +
						 " " + "O: " + StringConverter::toString(mCamera->getDerivedOrientation());
		
		// Return true to continue rendering
		return true;
	}

	bool processUnbufferedMouseInput(const FrameEvent& evt)
	{
		using namespace OIS;

		// Rotation factors, may not be used if the second mouse button is pressed
		// 2nd mouse button - slide, otherwise rotate
		const MouseState &ms = mMouse->getMouseState();

		if(mMagixHandler->getCameraMode()==CAMERA_LOCKED)
		{
			if( ms.buttonDown( MB_Right ) )
			{
				mTranslateVector.x += ms.X.rel * 0.13;
				mTranslateVector.y -= ms.Y.rel * 0.13;
			}
			else
			{
				if(mMagixHandler->getMagixGUI()->isTablet())
				{
					mRotX = Degree((Math::Abs(ms.X.rel)>8000?0:-ms.X.rel) * 0.0013);
					mRotY = Degree((Math::Abs(ms.Y.rel)>8000?0:-ms.Y.rel) * 0.0013);
				}
				else
				{
					mRotX = Degree(-ms.X.rel * 0.13);
					mRotY = Degree(-ms.Y.rel * 0.13);
				}
			}
		}
		else
		{
			//right button down, so rotate view
			if( ms.buttonDown( MB_Right ) )
			{
				mMagixHandler->getMagixGUI()->showMouse(false);
				if(mMagixHandler->getMagixGUI()->isTablet())
				{
					mRotX = Degree((Math::Abs(ms.X.rel)>8000?0:-ms.X.rel) * 0.0013);
					mRotY = Degree((Math::Abs(ms.Y.rel)>8000?0:-ms.Y.rel) * 0.0013);
				}
				else
				{
					mRotX = Degree(-ms.X.rel * 0.13);
					mRotY = Degree(-ms.Y.rel * 0.13);
				}
			}
			//move the dayum cursor
			else
			{
				mMagixHandler->getMagixGUI()->showMouse(true);
				if(mMagixHandler->getMagixGUI()->isTablet())
				{
					//tablet cursor always moves, see HERE
				}
				else
				{
					mMagixHandler->getMagixGUI()->scrollMouse( ms.X.rel * 1, ms.Y.rel * 1, mWindow->getWidth(), mWindow->getHeight());
				}

				//Handle left click
				mMagixHandler->getMagixGUI()->setLeftClick(ms.buttonDown( MB_Left ));
			}
		}
		//HERE
		if(mMagixHandler->getMagixGUI()->isTablet())
		{
			mMagixHandler->getMagixGUI()->scrollMouse( ms.X.rel * 0.015, ms.Y.rel * 0.015, mWindow->getWidth(), mWindow->getHeight());
		}

		return true;
	}

	void moveCamera()
	{
		// Make all the changes to the camera
		// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW
		//(e.g. airplane)
		if(mMagixHandler->getCameraMode()==CAMERA_FREE)
		{
			mCamera->yaw(mRotX);
			mCamera->pitch(mRotY);
			mCamera->moveRelative(mTranslateVector);
		}
		else 
		{
			mMagixHandler->getCamera()->yaw(mRotX);
			mCamera->pitch(mRotY);
		}
	}*/

	void showDebugOverlay(bool show)
	{
		if (mDebugOverlay)
		{
			if (show)
			{
				mDebugOverlay->show();
				isStatsOn = true;
			}
			else
			{
				mDebugOverlay->hide();
				isStatsOn = false;
			}
		}
	}

	// Override frameStarted event to process that (don't care about frameEnded)
	bool frameStarted(const FrameEvent& evt)
	{
		using namespace OIS;

		if(mWindow->isClosed())	return false;

		mContinue = true;
		if(mMagixHandler->getDebugText()!="")mDebugText = mMagixHandler->getDebugText();

		//Need to capture/update each device
		mKeyboard->capture();
		mMouse->capture();

		//Check if one of the devices is not buffered
		/*if(!mMouse->buffered() || !mKeyboard->buffered())
		{
			// one of the input modes is immediate, so setup what is needed for immediate movement
			if (mTimeUntilNextToggle >= 0)
				mTimeUntilNextToggle -= evt.timeSinceLastFrame;

			// If this is the first frame, pick a speed
			if (evt.timeSinceLastFrame == 0)
			{
				mMoveScale = 1;
				mRotScale = 0.1;
			}
			// Otherwise scale movement units by time passed since last frame
			else
			{
				// Move about 100 units per second,
				mMoveScale = mMoveSpeed * evt.timeSinceLastFrame;
				// Take about 10 seconds for full rotation
				mRotScale = mRotateSpeed * evt.timeSinceLastFrame;
			}
			mRotX = 0;
			mRotY = 0;
			mTranslateVector = Ogre::Vector3::ZERO;
		}

		//Check to see which device is not buffered, and handle it
		if( !mKeyboard->buffered() )
			if( processUnbufferedKeyInput(evt) == false )
				return false;
		if( !mMouse->buffered() )
			if( processUnbufferedMouseInput(evt) == false )
				return false;

		if( !mMouse->buffered() || !mKeyboard->buffered())
			moveCamera();*/

		return mContinue;
	}

	bool frameEnded(const FrameEvent& evt)
	{
		if(mDebugOverlay->isVisible())updateStats();
		return true;
	}



protected:
	//Camera* mCamera;

	//Vector3 mTranslateVector;
	RenderWindow* mWindow;
	bool isStatsOn;

	std::string mDebugText;

	//unsigned int mNumScreenShots;
	//float mMoveScale;
	//Degree mRotScale;
	// just to stop toggles flipping too fast
	//Real mTimeUntilNextToggle ;
	//Radian mRotX, mRotY;
	//TextureFilterOptions mFiltering;
	//int mAniso;

	//int mSceneDetailIndex ;
	//Real mMoveSpeed;
	//Degree mRotateSpeed;
	Overlay* mDebugOverlay;

	//OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;

	MagixHandler *mMagixHandler;
	bool mContinue;
};

#endif
