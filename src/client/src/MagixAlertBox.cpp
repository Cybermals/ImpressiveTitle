
#include "MagixAlertBox.h"

#include "Ogre.h"
#include "OgrePrerequisites.h"
#include "OgreFont.h"
#include "OgreFontManager.h"

using namespace Ogre;

void MagixAlertBox::initialize()
{
	OverlayManager::getSingleton().getByName("GUIOverlay/AlertBox")->hide();
	mAlertBox = OverlayManager::getSingleton().getOverlayElement("GUI/AlertBox");
	mAlertBoxText = OverlayManager::getSingleton().getOverlayElement("GUI/AlertBoxText");
}
void MagixAlertBox::update(const FrameEvent evt)
{
	//Transition in
	if (alertTimeout>0)
	{
		if (!mAlertBox->isVisible())mAlertBox->show();
		if (mAlertBoxText->isVisible())mAlertBoxText->hide();
		alertTimeout -= evt.timeSinceLastFrame;
		if (alertTimeout <= 0)
		{
			alertTimeout = 0;
			mAlertBoxText->show();
		}
		Real tRatio = (Real)((TRANSITION_TIME - alertTimeout) / TRANSITION_TIME);
		mAlertBox->setDimensions(tRatio*startWidth, tRatio*startHeight);
		mAlertBox->setPosition(startLeft + (1 - tRatio)*startWidth / 2, startTop + (1 - tRatio)*startHeight / 2);

		return;
	}
	//Show alert
	if (alertCount>0)
	{
		alertCount -= evt.timeSinceLastFrame;
		if (alertCount <= 0)hide();

		return;
	}
	//Transition out
	if (alertTimeout<0)
	{
		if (mAlertBoxText->isVisible())mAlertBoxText->hide();
		alertTimeout += evt.timeSinceLastFrame;
		if (alertTimeout >= 0)
		{
			alertTimeout = 0;
			mAlertBox->hide();
			OverlayManager::getSingleton().getByName("GUIOverlay/AlertBox")->hide();
		}
		Real tRatio = (Real)((-1 * alertTimeout) / TRANSITION_TIME);
		mAlertBox->setDimensions(tRatio*startWidth, tRatio*startHeight);
		mAlertBox->setPosition(startLeft + (1 - tRatio)*startWidth / 2, startTop + (1 - tRatio)*startHeight / 2);

		return;
	}
}
void MagixAlertBox::showAlert(const String &caption, Real left, Real top, Real count)
{
	OverlayManager::getSingleton().getByName("GUIOverlay/AlertBox")->show();
	mAlertBoxText->setCaption(caption);
	mAlertBox->setPosition(left, top);

	//compute text width/height
	const Font *pFont = dynamic_cast<Ogre::Font*>(Ogre::FontManager::getSingleton().getByName(ALERT_FONT).getPointer());

	vector<String>::type tText = StringUtil::split(caption, "\n");
	Real tLongestWidth = 0;

	for (int i = 0; i<int(tText.size()); i++)
	{
		String tLine = tText[i];
		Real tTextWidth = 0;
		for (Ogre::String::iterator i = tLine.begin(); i < tLine.end(); i++)
		{
			if (*i == 0x0020)
				tTextWidth += 0.055;//pFont->getGlyphAspectRatio(0x0030);
			else
			{
				tTextWidth += pFont->getGlyphAspectRatio(*i);
			}
		}
		if (tTextWidth>tLongestWidth)tLongestWidth = tTextWidth;
	}

	startHeight = StringConverter::parseReal(mAlertBoxText->getParameter("char_height"));
	startWidth = tLongestWidth * startHeight;
	startHeight *= int(tText.size());

	startLeft = left - startWidth / 2;
	startTop = top - startHeight / 2;
	mAlertBox->setDimensions(0, 0);
	alertCount = count;
	alertTimeout = TRANSITION_TIME;
}
void MagixAlertBox::hide(bool immediate)
{
	alertCount = 0;
	if (immediate)
	{
		OverlayManager::getSingleton().getByName("GUIOverlay/AlertBox")->hide();
		mAlertBox->hide();
		alertTimeout = 0;
	}
	else alertTimeout = -TRANSITION_TIME;
}
OverlayElement* MagixAlertBox::getAlertBox()
{
	return mAlertBox;
}