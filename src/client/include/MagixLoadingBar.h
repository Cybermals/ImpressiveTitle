#pragma once

#include "OgreResourceGroupManager.h"
#include "OgreException.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreRenderWindow.h"

using namespace Ogre;

/*
Based heavily off of Ogre's ExampleLoadingBar.h
Description: Defines an example loading progress bar which you can use during 
startup, level changes etc to display loading progress. 
IMPORTANT: Note that this progress bar relies on you having the OgreCore.zip 
package already added to a resource group called 'Bootstrap' - this provides
the basic resources required for the progress bar and will be loaded automatically.
*/
class MagixLoadingBar : public ResourceGroupListener
{
protected:
	RenderWindow* mWindow;
	Overlay* mLoadOverlay;
	Real mInitProportion;
	unsigned short mNumGroupsInit;
	unsigned short mNumGroupsLoad;
	Real mProgressBarMaxSize;
	Real mProgressBarScriptSize;
	Real mProgressBarInc;
	OverlayElement* mLoadingBarElement;
	OverlayElement* mLoadingDescriptionElement;
	OverlayElement* mLoadingCommentElement;

public:
	MagixLoadingBar();
	virtual ~MagixLoadingBar();

	/** Show the loading bar and start listening.
	@param window The window to update 
	@param numGroupsInit The number of groups you're going to be initialising
	@param numGroupsLoad The number of groups you're going to be loading
	@param initProportion The proportion of the progress which will be taken
		up by initialisation (ie script parsing etc). Defaults to 0.7 since
		script parsing can often take the majority of the time.
	*/
	virtual void start(RenderWindow* window,
		unsigned short numGroupsInit = 1,
		unsigned short numGroupsLoad = 1,
		Real initProportion = 0.70f);

	/** Hide the loading bar and stop listening. 
	*/
	virtual void finish(void);

	// ResourceGroupListener callbacks
	void resourceGroupScriptingStarted(const String& groupName, size_t scriptCount);

	void scriptParseStarted(const String& scriptName, bool &skipThisScript);

	void scriptParseEnded(const String& scriptName, bool skipped);

	void resourceGroupScriptingEnded(const String& groupName);

	void resourceGroupLoadStarted(const String& groupName, size_t resourceCount);

	void resourceLoadStarted(const ResourcePtr& resource);

	void resourceLoadEnded(void);

	void worldGeometryStageStarted(const String& description);

	void worldGeometryStageEnded(void);

	void resourceGroupLoadEnded(const String& groupName);

	Ogre::DataStreamPtr resourceLoading(const String &name, const String &group, Resource *resource);

	bool resourceCollision(ResourcePtr &resource, ResourceManager *resourceManager);
};

