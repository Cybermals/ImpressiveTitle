#pragma once

#include "MagixExternalDefinitions.h"
#include "MagixEffectsManager.h"
#include "MagixAlertBox.h"
#include "MagixUnit.h"
#include "MagixConst.h"

using namespace Ogre;

#define FILENAME_CHARACTERS "characters.sav"

#define MAX_COLOURS 11
//#define MAX_SERVERS 2

#define PAGE_SELECT 0
#define PAGE_CUSTOM1 1
#define PAGE_CUSTOM2 2
#define PAGE_CUSTOM3 3
#define PAGE_CUSTOM4 4
#define PAGE_EDIT 5
#define PAGE_PRESET1 6
#define PAGE_PRESET2 7

#define CHARSCREEN_STARTGAME 1
#define CHARSCREEN_CREATECHAR 2
#define CHARSCREEN_DELETECHAR 3
#define CHARSCREEN_UPDATEGUI 4
#define CHARSCREEN_EDITCHAR 5

class MagixCharScreenManager
{
protected:
	MagixExternalDefinitions *mDef;
	MagixEffectsManager *mEffectsManager;
	MagixAlertBox *mAlertBox;
	MagixUnit *mUnit;
	SceneManager *mSceneMgr;
	OverlayElement *mName;
	OverlayElement *mText[MAX_CHARSCREENTEXT];
	OverlayElement *mLButton[MAX_CHARSCREENTEXT];
	OverlayElement *mRButton[MAX_CHARSCREENTEXT];
	OverlayElement *mSlider[3];
	OverlayElement *mSliderMarker[3];
	OverlayElement *mColourBox;
	OverlayElement *mBackdrop;
	OverlayElement *mBackButton;
	OverlayElement *mNextButton;
	OverlayElement *mDeleteButton;
	OverlayElement *mBackButtonText;
	OverlayElement *mNextButtonText;
	OverlayElement *mDeleteButtonText;
	short page;
	short charID;
	short numChars;
	//bool isMale;
	short maneID;
	short headID;
	short tailID;
	short wingID;
	short tuftID;
	short bodyMarkID;
	short headMarkID;
	short tailMarkID;
	short colourPartID;
	ColourValue colourVal[MAX_COLOURS];
	bool isRotateUnit;
	Real rotateStartX;
	Real rotateStartY;
	Degree dYaw;
	Real camZoom;
	Real startGameCount;
	bool isCharIDChanged;
	bool isEditMode;
	bool doReequip;
	String deleteConfirmation;
public:
	MagixCharScreenManager()
	{
		mDef = 0;
		mEffectsManager = 0;
		mUnit = 0;
		mSceneMgr = 0;
		mName = 0;
		for(int i=0;i<MAX_CHARSCREENTEXT;i++)
		{
			mText[i] = 0;
			mLButton[i] = 0;
			mRButton[i] = 0;
		}
		for(int i=0;i<3;i++)
		{
			mSlider[i] = 0;
			mSliderMarker[i] = 0;
		}
		mColourBox = 0;
		mAlertBox = 0;
		mBackdrop = 0;
		mBackButton = 0;
		mNextButton = 0;
		mBackButtonText = 0;
		mNextButtonText = 0;
	}
	~MagixCharScreenManager()
	{
	}
	void initialize(MagixExternalDefinitions *def, MagixEffectsManager *effectsMgr, MagixAlertBox *alertBox, MagixUnit *unit, SceneManager *sceneMgr, OverlayElement *name, OverlayElement *backdrop);

	void initializeText(const unsigned short &i, OverlayElement *text);

	void initializeBackNextButtons(OverlayElement *back, OverlayElement *next, OverlayElement *del, OverlayElement *backText, OverlayElement *nextText, OverlayElement *delText);

	void initializeLRButtons(const unsigned short &i, OverlayElement *lButton, OverlayElement *rButton);

	void initializeSliders(const unsigned short &i, OverlayElement *slider, OverlayElement *sliderMarker);

	void reset(bool load = false);

	const unsigned short buttonClicked(OverlayElement *button);

	void changeTrait(const unsigned short &iID, bool increment);

	const String meshName(const String &type, const unsigned short &iID);

	const String partName(const unsigned short &iID);

	void applyRestrictions(const String &partChanged, bool increment);

	void refreshPage();

	bool previousPage();

	const unsigned short nextPage();

	void updateSliders(const Real &one, const Real &two, const Real &three);

	void getSliderValue(Real &one, Real &two, Real &three);

	const String getName();

	void setName(const String &name);

	bool validateName();

	void doRotateUnit(Real x, Real y);

	void endRotateUnit();

	void stopRotateUnit();

	bool isRotatingUnit();

	void startGame();

	bool update(const FrameEvent &evt, const Real &x, const Real &y);

	void deleteChar();

	const String getCharData();

	void updateCharData(bool isPreset = false);

	void updateChar(const String &data);

	void createCharSuccess();

	void editCharSuccess();

	void createDefaultUnit();

	void positionUnitForEdit();

	void positionUnitForSelect();

	void setNumChars(const short &tNum);

	const short getNumChars();

	bool popIsCharIDChanged();

	bool popDoReequip();

	const short getCharID();
};
