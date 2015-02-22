#pragma once

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "MagixHandler.h"
#include "MagixFrameListener.h"
#include "MagixLoadingBar.h"
#include "MagixEncryption.h"

using namespace Ogre;

/** Base class which manages the standard startup of an Ogre application.
    Designed to be subclassed for specific examples if required.
*/
class MagixApplication
{
public:
    /// Standard constructor
	MagixApplication(MagixHandler *magixHandler):
		mMagixHandler(magixHandler)
    {
        mFrameListener = 0;
        mRoot = 0;
		mLoadingBar = 0;
		// Provide a nice cross platform solution for locating the configuration files
		// On windows files are searched for in the current working directory, on OS X however
		// you must provide the full path, the helper function macBundlePath does this for us.
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		mResourcePath = macBundlePath() + "/Contents/Resources/";
#else
		mResourcePath = "";
#endif
    }
    /// Standard destructor
    virtual ~MagixApplication()
    {
		if (mFrameListener)
            delete mFrameListener;
		if (mRoot)
			delete mRoot;
		if(mMagixEncryptionZipFactory)
			delete mMagixEncryptionZipFactory;
    }

    /// Start the example
	virtual void go(void);

protected:
    Root *mRoot;
    Camera* mCamera;
    SceneManager* mSceneMgr;
    MagixFrameListener* mFrameListener;
    RenderWindow* mWindow;
	Ogre::String mResourcePath;
	MagixHandler *mMagixHandler;
	MagixLoadingBar* mLoadingBar;
	MagixEncryptionZipFactory *mMagixEncryptionZipFactory;

    // These internal methods package up the stages in the startup process
    /** Sets up the application - returns false if the user chooses to abandon configuration. */
	virtual bool setup(void);

    /** Configures the application - returns false if the user chooses to abandon configuration. */
	virtual bool configure(void);

	virtual void chooseSceneManager(void);

	virtual void createCamera(void);

	virtual void createFrameListener(void);

    virtual void createScene(void) = 0;    // pure virtual - this has to be overridden

	virtual void destroyScene(void);

	virtual void createViewports(void);

    /// Method which will define the source of resources (other than current folder)
	virtual void setupResources(void);

	/// Optional override method where you can create resource listeners (e.g. for loading screens)
	virtual void createResourceListener(void);

	/// Optional override method where you can perform resource group loading
	/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	virtual void loadResources(void);
};
