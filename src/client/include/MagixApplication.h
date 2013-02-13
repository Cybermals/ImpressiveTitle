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
Filename:    ExampleApplication.h
Description: Base class for all the OGRE examples
-----------------------------------------------------------------------------
*/

#ifndef __MagixApplication_H__
#define __MagixApplication_H__

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "MagixHandler.h"
#include "MagixFrameListener.h"
#include "MagixLoadingBar.h"


#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>

// This function will locate the path to our application on OS X,
// unlike windows you can not rely on the curent working directory
// for locating your configuration files and resources.
std::string macBundlePath()
{
    char path[1024];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    assert(mainBundle);

    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
    assert(mainBundleURL);

    CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
    assert(cfStringRef);

    CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

    CFRelease(mainBundleURL);
    CFRelease(cfStringRef);

    return std::string(path);
}
#endif

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
    }

    /// Start the example
    virtual void go(void)
    {
        if (!setup())
            return;

        mRoot->startRendering();

        // clean up
        destroyScene();
    }

protected:
    Root *mRoot;
    Camera* mCamera;
    SceneManager* mSceneMgr;
    MagixFrameListener* mFrameListener;
    RenderWindow* mWindow;
	Ogre::String mResourcePath;
	MagixHandler *mMagixHandler;
	MagixLoadingBar* mLoadingBar;

    // These internal methods package up the stages in the startup process
    /** Sets up the application - returns false if the user chooses to abandon configuration. */
    virtual bool setup(void)
    {

		String pluginsPath;
		// only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
#ifdef _DEBUG
		pluginsPath = mResourcePath + "plugins_d.cfg";
#else
		pluginsPath = mResourcePath + "plugins.cfg";
#endif
#endif
		
        mRoot = new Root(pluginsPath, 
            mResourcePath + "ogre.cfg", mResourcePath + "Ogre.log");

        setupResources();

        bool carryOn = configure();
        if (!carryOn) return false;

        

        // Set default mipmap level (NB some APIs ignore this)
        TextureManager::getSingleton().setDefaultNumMipmaps(5);
		chooseSceneManager();
        createCamera();
        createViewports();

		// Create any resource listeners (for loading screens)
		createResourceListener();
		// Load resources
		loadResources();
		
		// Create the scene
        createScene();
        createFrameListener();
		
        return true;

    }
    /** Configures the application - returns false if the user chooses to abandon configuration. */
    virtual bool configure(void)
    {
        // Show the configuration dialog and initialise the system
        // You can skip this and use root.restoreConfig() to load configuration
        // settings if you were sure there are valid ones saved in ogre.cfg
        if(mRoot->showConfigDialog())
        {
            // If returned true, user clicked OK so initialise
            // Here we choose to let the system create a default rendering window by passing 'true'
            mWindow = mRoot->initialise(true);
			mWindow->setDeactivateOnFocusChange(false);
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual void chooseSceneManager(void)
    {
        // Create the SceneManager, in this case a generic one
        mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE);
    }
    virtual void createCamera(void)
    {
        // Create the camera
        mCamera = mSceneMgr->createCamera("PlayerCam");

        // Position it at 500 in Z direction
        mCamera->setPosition(Vector3(0,0,500));
        // Look back along -Z
        mCamera->lookAt(Vector3(0,0,-300));
        mCamera->setNearClipDistance(5);

    }
    virtual void createFrameListener(void)
    {
        mFrameListener= new MagixFrameListener(mMagixHandler, mWindow, mCamera);
        mFrameListener->showDebugOverlay(true);
        mRoot->addFrameListener(mFrameListener);
    }

    virtual void createScene(void) = 0;    // pure virtual - this has to be overridden

    virtual void destroyScene(void){}    // Optional to override this

    virtual void createViewports(void)
    {
        // Create one viewport, entire window
        Viewport* vp = mWindow->addViewport(mCamera);
        vp->setBackgroundColour(ColourValue(0,0,0));

        // Alter the camera aspect ratio to match the viewport
        mCamera->setAspectRatio(
            Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
    }

    /// Method which will define the source of resources (other than current folder)
    virtual void setupResources(void)
    {
        // Load resource paths from config file
        ConfigFile cf;
        cf.load(mResourcePath + "resources.cfg");

        // Go through all sections & settings in the file
        ConfigFile::SectionIterator seci = cf.getSectionIterator();

        String secName, typeName, archName;
        while (seci.hasMoreElements())
        {
            secName = seci.peekNextKey();
            ConfigFile::SettingsMultiMap *settings = seci.getNext();
            ConfigFile::SettingsMultiMap::iterator i;

			
			
            for (i = settings->begin(); i != settings->end(); ++i)
            {
                typeName = i->first;
                archName = i->second;

				//Check if file exists first if it's a preset file
				/*if(typeName=="Zip")
				{
					String tFilename,tPath;
					StringUtil::splitFilename(archName,tFilename,tPath);
					if(StringUtil::startsWith(tFilename,"preset"))
					{
						ifstream inFile(archName.c_str());
						if(!inFile.good())
						{
							Ogre::LogManager::getSingleton().logMessage("Preset File not found:"+archName);
							continue;
						}
						inFile.close();
						continue;
					}
				}*/

				//Scan for wildcard filenames like media/terrains/(World.txt)
				if(archName.at(archName.length()-1)==')')
				{
					const vector<String>::type tFilenameVect = StringUtil::split(archName,"()");
					if(tFilenameVect.size()>1)
					{
						const String tFilename = tFilenameVect[0]+tFilenameVect[1];
						long tSize = 0;
						char *tBuffer;
						String tData="";
						
						std::ifstream inFile;
						inFile.open(tFilename.c_str(), std::ifstream::in);
						if(!inFile.good())continue;
						inFile.seekg(0,std::ios::end);
						tSize = inFile.tellg();
						inFile.seekg(0,std::ios::beg);
						tBuffer = new char[tSize];
						strcpy(tBuffer,"");
						inFile.getline(tBuffer,tSize,'#');
						inFile.close();
						tData = tBuffer;
						delete[] tBuffer;

						const vector<String>::type tLine = StringUtil::split(tData,"\n#");
						for(int j=0;j<(int)tLine.size();j++)
						{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
							ResourceGroupManager::getSingleton().addResourceLocation(
								String(macBundlePath() + "/" + tFilenameVect[0] + tLine[j]), typeName, secName);
#else
							ResourceGroupManager::getSingleton().addResourceLocation(
								tFilenameVect[0] + tLine[j], typeName, secName);
#endif
						}
						continue;
					}
				}
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
                // OS X does not set the working directory relative to the app,
                // In order to make things portable on OS X we need to provide
                // the loading with it's own bundle path location
                ResourceGroupManager::getSingleton().addResourceLocation(
                    String(macBundlePath() + "/" + archName), typeName, secName);
#else
                ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
#endif
            }
        }
		for(int j=0;j<2;j++)
		{
			for(int i=1;i<=200;i++)
			{
				const String archName = "media/materials/Presets/Preset"+StringConverter::toString(j*200+i)+".dat";
				std::ifstream inFile(archName.c_str());
				if(!inFile.good())
				{
					Ogre::LogManager::getSingleton().logMessage("Preset File not found: "+archName);
					inFile.close();
					break;
				}
				inFile.close();
	#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
					// OS X does not set the working directory relative to the app,
					// In order to make things portable on OS X we need to provide
					// the loading with it's own bundle path location
					ResourceGroupManager::getSingleton().addResourceLocation(
						String(macBundlePath() + "/" + archName), "Zip", "General");
	#else
					ResourceGroupManager::getSingleton().addResourceLocation(
						archName, "Zip", "General");
	#endif
			}
		}
    }

	/// Optional override method where you can create resource listeners (e.g. for loading screens)
	virtual void createResourceListener(void)
	{
		mLoadingBar = new MagixLoadingBar();
	}

	/// Optional override method where you can perform resource group loading
	/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	virtual void loadResources(void)
	{
		mLoadingBar->start(mWindow, 1, 1, 1);
		// Initialise, parse scripts etc
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		Ogre::ResourceManager::ResourceMapIterator iter = Ogre::FontManager::getSingleton().getResourceIterator();
		while (iter.hasMoreElements()) 
		{ 
			 iter.getNext()->load(); 
		}
		mLoadingBar->finish();
	}



};


#endif
