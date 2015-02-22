#include "ogremagixApp.h"
#include "ogremagixFrameListener.h"

void ogremagixApp::createCamera(void)
{
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");
	mCamera->setNearClipDistance(0.1);
	mCamera->setFarClipDistance(20000);
}


bool ogremagixApp::configure(void)
{
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if (mRoot->showConfigDialog())
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true, "Magix");
		mWindow->setDeactivateOnFocusChange(false);
		// Let's add a nice window icon
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		HWND hwnd;
		mWindow->getCustomAttribute("WINDOW", (void*)&hwnd);
		LONG iconID = (LONG)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON));
		SetClassLong(hwnd, GCL_HICON, iconID);
#endif
		return true;
	}
	else
	{
		return false;
	}
}


// Just override the mandatory create scene method
void ogremagixApp::createScene(void)
{
	ColourValue fadeColour = DEFAULT_FOG_COLOUR;
	mSceneMgr->setFog(FOG_LINEAR, fadeColour, .0001, 500, 1500);
	mWindow->getViewport(0)->setBackgroundColour(fadeColour);

	mMagixHandler->initialize(mSceneMgr, mWindow);
	mMagixHandler->getExternalDefinitions()->initializeCapabilities(mRoot->getRenderSystem()->getCapabilities());

	// Infinite far plane?
	if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_INFINITE_FAR_PLANE))
	{
		mCamera->setFarClipDistance(0);
	}

	if (mMagixHandler->getExternalDefinitions()->hasVertexProgram && mMagixHandler->getExternalDefinitions()->hasFragmentProgram)
	{
		CompositorManager::getSingleton().addCompositor(mWindow->getViewport(0), "Bloom");
		CompositorManager::getSingleton().addCompositor(mWindow->getViewport(0), "MotionBlur");
	}
}

void ogremagixApp::destroyScene()
{
	mMagixHandler->shutdown();
}

// Create new frame listener
void ogremagixApp::createFrameListener(void)
{
	mFrameListener = new ogremagixFrameListener(mMagixHandler, mSceneMgr, mWindow, mCamera);
	mRoot->addFrameListener(mFrameListener);
}
