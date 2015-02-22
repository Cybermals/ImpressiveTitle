#pragma once
#include "Ogre.h"
using namespace Ogre;


#define TRANSITION_TIME 0.1
#define ALERT_FONT "TempusSans"

class MagixAlertBox
{
protected:
	OverlayElement *mAlertBox;
	OverlayElement *mAlertBoxText;
	Real alertCount;
	Real alertTimeout;
	Real startWidth;
	Real startHeight;
	Real startLeft;
	Real startTop;
public:
	MagixAlertBox()
	{
		mAlertBox = 0;
		mAlertBoxText = 0;
		alertCount = 0;
		alertTimeout = 0;
		startWidth = 0;
		startHeight = 0;
	}
	~MagixAlertBox()
	{
	}
	void initialize();

	void update(const FrameEvent evt);

	void showAlert(const String &caption, Real left, Real top, Real count = 2.5);

	void hide(bool immediate = false);

	OverlayElement* getAlertBox();
};
