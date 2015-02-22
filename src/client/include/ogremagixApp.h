#pragma once

#include "MagixApplication.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "../res/resource.h"
#endif

class ogremagixApp : public MagixApplication
{
private:
	MagixHandler *mMagixHandler;

	public:
		ogremagixApp():
		  MagixApplication(mMagixHandler)
    {
		mMagixHandler = new MagixHandler();
	}

	~ogremagixApp()
	{
		//Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
		//overlayManager->destroyAll();
		delete mMagixHandler;
	}

protected:

	virtual void createCamera(void);

	virtual bool configure(void);

	// Just override the mandatory create scene method
	virtual void createScene(void);

	virtual void destroyScene();

   // Create new frame listener
	void createFrameListener(void);
};
