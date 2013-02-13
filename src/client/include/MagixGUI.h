#ifndef __MagixGUI_h_
#define __MagixGUI_h_

#define MAX_BOXES 23
#define MAX_BUTTONS 84
#define MAX_SIZERS 1
#define MAX_MOVERS 8
#define MAX_SCROLLERS 14
#define MAX_CHECKBOXES 1
#define MAX_CHARSCREENTEXT 4
#define MAX_CREATEACCOUNTINFO 6
#define MAX_EDITACCOUNTINFO 4
#define MAX_PRIMARYACTIONS 15
#define MAX_SECONDARYACTIONS 11
#define MAX_ACTIONS MAX_PRIMARYACTIONS + MAX_SECONDARYACTIONS

#include "GameConfig.h"
#include "MagixExternalDefinitions.h"
#include "MagixGameStateManager.h"
#include "MagixUnitManager.h"
#include "MagixInputManager.h"
#include "MagixNetworkManager.h"
#include "MagixCamera.h"
#include "MagixChatManager.h"
#include "MagixAlertBox.h"
#include "MagixCharScreenManager.h"
#include "MagixSoundManager.h"
#include "MagixMiniMap.h"

#define SIZEDIR_TOP 1
#define SIZEDIR_BOTTOM 2
#define SIZEDIR_LEFT 3
#define SIZEDIR_RIGHT 4

enum GUIBOX
{
	GUI_OPTIONS,
	GUI_SETTINGS,
	GUI_INTERFACE,
	GUI_LISTSELECTBOX,
	GUI_SELECTBOX,
	GUI_TEXTINPUT,
	GUI_TEXTOUTPUT,
	GUI_STARTSCREENBOX,
	GUI_CHARSCREENBOX,
	GUI_CAMPAIGNBOX,
	GUI_SERVERBOX,
	GUI_MULTIPLAYER,
	GUI_FRIENDBOX,
	GUI_BIOBOX,
	GUI_TARGETBIOBOX,
	GUI_HOMEBOX,
	GUI_EMOTEBOX,
	GUI_ITEMBOX,
	GUI_LOGONBOX,
	GUI_CREATEACCOUNTBOX,
	GUI_EDITACCOUNTBOX,
	GUI_PARTYBOX,
	GUI_STASHBOX
};

enum BUTTON
{
 BUTTON_TEXTINPUT,
 BUTTON_TEXTINPUT2,
 BUTTON_TEXTINPUT3,
 BUTTON_TEXTOUTPUT,
 BUTTON_LISTSELECT1,
 BUTTON_LISTSELECT2,
 BUTTON_LISTSELECT3,
 BUTTON_SELECTBOX1,
 BUTTON_SELECTBOX2,
 BUTTON_SELECTBOX3,
 BUTTON_SELECTBOX4,
 BUTTON_NEWGAME,
 BUTTON_MULTIPLAYER,
 BUTTON_QUIT,
 BUTTON_CHARSCREEN_L1,
 BUTTON_CHARSCREEN_R1,
 BUTTON_CHARSCREEN_L2,
 BUTTON_CHARSCREEN_R2,
 BUTTON_CHARSCREEN_L3,
 BUTTON_CHARSCREEN_R3,
 BUTTON_CHARSCREEN_L4,
 BUTTON_CHARSCREEN_R4,
 BUTTON_CHARSCREENBACK,
 BUTTON_CHARSCREENNEXT,
 BUTTON_CHARSCREENDELETE,
 BUTTON_OPTIONRESUME,
 BUTTON_OPTIONSETTINGS,
 BUTTON_OPTIONINTERFACE,
 BUTTON_OPTIONRESTART,
 BUTTON_OPTIONQUIT,
 BUTTON_SETTINGS1,
 BUTTON_SETTINGS2,
 BUTTON_SETTINGS3,
 BUTTON_SETTINGSBACK,
 BUTTON_INTERFACE1,
 BUTTON_INTERFACE2,
 BUTTON_INTERFACE3,
 BUTTON_INTERFACE4,
 BUTTON_INTERFACE5,
 BUTTON_INTERFACE6,
 BUTTON_INTERFACE7,
 BUTTON_INTERFACE8,
 BUTTON_INTERFACEBACK,
 BUTTON_CAMPAIGNBACK,
 BUTTON_CAMPAIGNNEXT,
 BUTTON_CAMPAIGN1,
 BUTTON_CAMPAIGN2,
 BUTTON_CAMPAIGN3,
 BUTTON_GAMEOVERCONTINUE,
 BUTTON_GAMEOVERQUIT,
 BUTTON_SERVER1,
 BUTTON_SERVER2,
 BUTTON_MINIMAP1,
 BUTTON_MINIMAP2,
 BUTTON_MULTIPLAYER1,
 BUTTON_MULTIPLAYER2,
 BUTTON_MULTIPLAYER3,
 BUTTON_MULTIPLAYER4,
 BUTTON_MULTIPLAYER5,
 BUTTON_MULTIPLAYER6,
 BUTTON_FRIENDBOX,
 BUTTON_BIOBOX,
 BUTTON_TARGETBIOBOX,
 BUTTON_HOMEBOX,
 BUTTON_HOMEBOX1,
 BUTTON_HOMEBOX2,
 BUTTON_HOMEBOX3,
 BUTTON_HOMEBOX4,
 BUTTON_EMOTEBOX,
 BUTTON_ITEMBOX,
 BUTTON_ITEMBOX1,
 BUTTON_LOGON1,
 BUTTON_NEWACCOUNT,
 BUTTON_EDITACCOUNT,
 BUTTON_LOGONBACK,
 BUTTON_CREATEACCOUNT1,
 BUTTON_CREATEACCOUNTBACK,
 BUTTON_EDITACCOUNT1,
 BUTTON_EDITACCOUNTBACK,
 BUTTON_PARTYBOX,
 BUTTON_PARTYBOX1,
 BUTTON_PARTYBOX2,
 BUTTON_PARTYBOX3,
 BUTTON_STASHBOX
};

#define SIZER_TEXTOUTPUT 0

enum MOVER
{
 MOVER_FRIENDBOX,
 MOVER_BIOBOX,
 MOVER_TARGETBIOBOX,
 MOVER_HOMEBOX,
 MOVER_EMOTEBOX,
 MOVER_ITEMBOX,
 MOVER_PARTYBOX,
 MOVER_STASHBOX
};

enum SCROLLER
{
 SCROLLER_TEXTOUTPUT,
 SCROLLER_CHARSCREEN1,
 SCROLLER_CHARSCREEN2,
 SCROLLER_CHARSCREEN3,
 SCROLLER_SETTINGS1,
 SCROLLER_SETTINGS2,
 SCROLLER_SETTINGS3,
 SCROLLER_SETTINGS4,
 SCROLLER_INTERFACE1,
 SCROLLER_FRIENDBOX,
 SCROLLER_BIOBOX,
 SCROLLER_TARGETBIOBOX,
 SCROLLER_EMOTEBOX,
 SCROLLER_STASHBOX
};

enum CHECKBOX
{
 CHECKBOX_LOGON
};

enum LISTBUTTON
{
 LISTBUTTON_NONE,
 LISTBUTTON_ONLINEFRIEND,
 LISTBUTTON_OFFLINEFRIEND,
 LISTBUTTON_BLOCKED,
 LISTBUTTON_EMOTE
};

enum GUITYPE
{
	GUITYPE_NONE,
	GUITYPE_BUTTON,
	GUITYPE_SIZER,
	GUITYPE_MOVER,
	GUITYPE_SCROLLER,
	GUITYPE_CHECKBOX
};

using namespace Ogre;

class MagixGUI
{
protected:
	SceneManager *mSceneMgr;
	RenderWindow *mWindow;
	MagixExternalDefinitions *mDef;
	MagixGameStateManager *mGameStateManager;
	MagixItemManager *mItemManager;
	MagixCritterManager *mCritterManager;
	MagixUnitManager *mUnitManager;
	MagixInputManager *mInputManager;
	MagixNetworkManager *mNetworkManager;
	MagixCamera *mCamera;
	MagixChatManager *mChatManager;
	MagixCharScreenManager *mCharScreenManager;
	MagixAlertBox *mAlertBox;
	MagixSoundManager *mSoundManager;
	MagixEffectsManager *mEffectsManager;
	MagixMiniMap *mMiniMap;

	OverlayElement *mFader;
	OverlayElement *mCursor;
	OverlayElement *mBackdrop;
	OverlayElement *mHPBar;
	OverlayElement *mSkillText;
	OverlayElement *mPickupText;
	OverlayElement *mTargetPanel;
	OverlayElement *mTargetHP;
	OverlayElement *mPseudoShader;
	OverlayElement *mSelectPanel;
	OverlayElement *mListSelectHighlight;

	OverlayElement *mBox[MAX_BOXES];
	OverlayElement *mBoxText[MAX_BOXES];
	OverlayElement *mButton[MAX_BUTTONS];
	OverlayElement *mButtonText[MAX_BUTTONS];
	OverlayElement *mSizer[MAX_SIZERS];
	OverlayElement *mMover[MAX_MOVERS];
	OverlayElement *mScroller[MAX_SCROLLERS];
	OverlayElement *mScrollerMarker[MAX_SCROLLERS];
	bool isScrollerHorz[MAX_SCROLLERS];
	OverlayElement *mCharScreenText[MAX_CHARSCREENTEXT];
	OverlayElement *mCheckBox[MAX_CHECKBOXES];

	OverlayElement *mHoverButton;
	unsigned char hoverButtonType;
	OverlayElement *mHoverWindow;
	OverlayElement *mHeldButtonWindow;
	OverlayElement *mMoverWindow;
	OverlayElement *mListSelectTarget;
	OverlayElement *mCurrentWindow;
	String mDebugText;
	Real cursorX;
	Real cursorY;
	bool leftClick;
	bool rightClick;
	RaySceneQuery *mRayQuery;
	SceneNode *mCurrentObject;
	unsigned short sizeWindowDir;
	Real dragStartX;
	Real dragStartY;
	Real sizeWindowWidth;
	Real sizeWindowHeight;
	Real sizeWindowLeft;
	Real sizeWindowTop;
	Real sizeWindowMinWidth;
	Real sizeWindowMinHeight;
	short scrollerHeldID;
	Real logoCount;

	bool isTabletFlag;
	bool isBloomFlag;
	bool isShadowsOnFlag;
	Real mouseSensitivity;
	bool motionBlurFlag;
	bool isGUIVisible;
	bool underwaterShaderFlag;
	bool doChannelBlink;
	bool isDoubleJumpOnFlag;
	bool wasChatBoxVisible;
	bool wasGUIVisible;
	bool isAutoLipSyncFlag;
	bool useWindowsCursor;

	Real faderTimeout;
	Real faderTimeoutMax;
	ColourValue faderColour;
	Real cinematicBarsTimeout;
	Real cinematicCaptionTimeout;
	Real pickupTextTimeout;
	Real buttonBlinkTimeout;
	Real selectPanelTimeout;

	BillboardSet *mTargetIndicator;
	SceneNode *mTargetIndicatorNode;

	String commandLine;
	String commandLineParam;
	String listButtonData;
	unsigned short listButtonType;
	unsigned short listButtonLine;
	vector<String>::type friendBoxList;
	vector<unsigned short>::type friendBoxListButton;
	vector<String>::type bioList;
	vector<String>::type targetBioList;

	bool isCreateAccount;
	bool isEditAccount;
	String logonUsername;
	String logonPassword;
	OverlayElement *mCreateAccountTextBox[MAX_CREATEACCOUNTINFO];
	String createAccountInfo[MAX_CREATEACCOUNTINFO];
	OverlayElement *mEditAccountTextBox[MAX_EDITACCOUNTINFO];
	String editAccountInfo[MAX_EDITACCOUNTINFO];

	vector<String>::type campaignList;
	unsigned short campaignPage;

public:
	MagixGUI()
	{
		mSceneMgr = 0;
		mWindow = 0;
		mChatManager = new MagixChatManager();
		mCharScreenManager = new MagixCharScreenManager();
		mInputManager = new MagixInputManager();
		mAlertBox = new MagixAlertBox();
		mMiniMap = new MagixMiniMap();
		mDef = 0;
		mGameStateManager = 0;
		mItemManager = 0;
		mCritterManager = 0;
		mUnitManager = 0;
		mNetworkManager = 0;
		mSoundManager = 0;
		mEffectsManager = 0;
		mCamera = 0;
		mFader = 0;
		mCursor = 0;
		mBackdrop = 0;
		mHPBar = 0;
		mTargetPanel = 0;
		mTargetHP = 0;
		mPseudoShader = 0;
		mSelectPanel = 0;
		for(int i=0;i<MAX_BOXES;i++)
		{
			mBox[i] = 0;
			mBoxText[i] = 0;
		}
		for(int i=0;i<MAX_BUTTONS;i++)
		{
			mButton[i] = 0;
			mButtonText[i] = 0;
		}
		for(int i=0;i<MAX_SIZERS;i++)mSizer[i] = 0;
		for(int i=0;i<MAX_MOVERS;i++)mMover[i] = 0;
		for(int i=0;i<MAX_SCROLLERS;i++)
		{
			mScroller[i] = 0;
			mScrollerMarker[i] = 0;
			isScrollerHorz[i] = false;
		}
		for(int i=0;i<MAX_CHARSCREENTEXT;i++)mCharScreenText[i] = 0;
		for(int i=0;i<MAX_CHECKBOXES;i++)mCheckBox[i] = 0;
		mHoverButton = 0;
		hoverButtonType = GUITYPE_NONE;
		mHoverWindow = 0;
		mHeldButtonWindow = 0;
		mMoverWindow = 0;
		mListSelectTarget = 0;
		mCurrentWindow = 0;

		mDebugText = "";
		cursorX = 0;
		cursorY = 0;
		leftClick = false;
		mRayQuery = 0;
		mCurrentObject = 0;
		sizeWindowDir = 0;
		dragStartX = 0;
		dragStartY = 0;
		sizeWindowWidth = 0;
		sizeWindowHeight = 0;
		sizeWindowLeft = 0;
		sizeWindowTop = 0;
		sizeWindowMinWidth = 0;
		sizeWindowMinHeight = 0;
		scrollerHeldID = -1;
		logoCount = 0;

		isTabletFlag = false;
		isBloomFlag = false;
		isShadowsOnFlag = false;
		mouseSensitivity = 0.5;
		motionBlurFlag = false;
		isGUIVisible = true;
		underwaterShaderFlag = false;
		doChannelBlink = true;
		isDoubleJumpOnFlag = true;
		wasChatBoxVisible = false;
		wasGUIVisible = false;
		isAutoLipSyncFlag = false;
		useWindowsCursor = false;

		faderTimeout = 0;
		faderTimeoutMax = 0;
		faderColour = ColourValue(0,0,0);
		cinematicBarsTimeout = 0;
		cinematicCaptionTimeout = 0;
		pickupTextTimeout = 0;
		buttonBlinkTimeout = 0;
		selectPanelTimeout = 0;

		mTargetIndicator = 0;
		mTargetIndicatorNode = 0;

		commandLine = "";
		commandLineParam = "";
		listButtonData = "";
		listButtonType = 0;
		listButtonLine = 0;
		mListSelectTarget = 0;
		friendBoxList.clear();
		friendBoxListButton.clear();
		bioList.clear();
		targetBioList.clear();

		isCreateAccount = false;
		isEditAccount = false;
		logonUsername = "";
		logonPassword = "";
		for(int i=0;i<MAX_CREATEACCOUNTINFO;i++)
		{
			mCreateAccountTextBox[i] = 0;
			createAccountInfo[i] = (i==3?"@":"");
		}
		for(int i=0;i<MAX_EDITACCOUNTINFO;i++)
		{
			mEditAccountTextBox[i] = 0;
			editAccountInfo[i] = "";
		}

		campaignList.clear();
		campaignPage = 0;
	}
	~MagixGUI()
	{
		delete mChatManager;
		delete mCharScreenManager;
		delete mInputManager;
		delete mAlertBox;
		delete mMiniMap;
		if(mRayQuery)delete mRayQuery;
	}
	void initialize(SceneManager *sceneMgr, RenderWindow *window, MagixExternalDefinitions *def, MagixGameStateManager *gameStateMgr, MagixItemManager *itemManager, MagixCritterManager *critterMgr, MagixUnitManager *unitManager, MagixNetworkManager *networkManager, MagixCamera *camera, MagixSoundManager *soundMgr, MagixEffectsManager *effectsMgr, MagixCollisionManager *collisionMgr)
	{
		mSceneMgr = sceneMgr;
		mWindow = window;
		mDef = def;
		mGameStateManager = gameStateMgr;
		mItemManager = itemManager;
		mCritterManager = critterMgr;
		mUnitManager = unitManager;
		mNetworkManager = networkManager;
		mCamera = camera;
		mSoundManager = soundMgr;
		mEffectsManager = effectsMgr;
		mRayQuery = sceneMgr->createRayQuery( Ray());
		mAlertBox->initialize();

		//MiniMap
		mMiniMap->initialize(def,unitManager,collisionMgr,camera);
		mButton[BUTTON_MINIMAP1] = OverlayManager::getSingleton().getOverlayElement("GUI/MiniMapButton1");
		mButtonText[BUTTON_MINIMAP1] = OverlayManager::getSingleton().getOverlayElement("GUI/MiniMapButtonText1");
		mButton[BUTTON_MINIMAP2] = OverlayManager::getSingleton().getOverlayElement("GUI/MiniMapButton2");
		mButtonText[BUTTON_MINIMAP2] = OverlayManager::getSingleton().getOverlayElement("GUI/MiniMapButtonText2");

		mMiniMap->show(false);
		
		//Target Indicator
		mTargetIndicator = sceneMgr->createBillboardSet("TargetIndicator",1);
		mTargetIndicator->setMaterialName("GUIMat/TargetIndicator");
		mTargetIndicator->setDefaultDimensions(7,7);
		mTargetIndicator->createBillboard(Vector3(0,0,0),ColourValue(1,1,1,1));
		mTargetIndicator->setQueryFlags(EFFECT_MASK);
		mTargetIndicator->setCommonDirection(Vector3::UNIT_Y);
		mTargetIndicator->setCommonUpVector(Vector3::UNIT_X);
		mTargetIndicator->setVisible(true);
		mTargetIndicatorNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
		mTargetIndicatorNode->attachObject(mTargetIndicator);


		//Fader
		mFader = OverlayManager::getSingleton().getOverlayElement("GUI/Fader");
		fadeOut(0);
		showFaderOverlay(false);

		//PseudoShader
		mPseudoShader = OverlayManager::getSingleton().getOverlayElement("GUI/PseudoShader");
		showPseudoShaderOverlay(false);

		//Cursor
		mCursor = OverlayManager::getSingleton().getOverlayElement("GUI/Cursor");
		mCursor->setPosition(400,300);
		cursorX = 0.5;
		cursorY = 0.5;

		showCursorOverlay(false);

		//Credits
		mButton[BUTTON_GAMEOVERCONTINUE] = OverlayManager::getSingleton().getOverlayElement("GUI/GameOverContinueButton");
		mButtonText[BUTTON_GAMEOVERCONTINUE] = OverlayManager::getSingleton().getOverlayElement("GUI/GameOverContinueButtonText");
		mButton[BUTTON_GAMEOVERQUIT] = OverlayManager::getSingleton().getOverlayElement("GUI/GameOverQuitButton");
		mButtonText[BUTTON_GAMEOVERQUIT] = OverlayManager::getSingleton().getOverlayElement("GUI/GameOverQuitButtonText");

		showCreditsOverlay(false);

		//Backdrop
		mBackdrop = OverlayManager::getSingleton().getOverlayElement("GUI/Backdrop");
		OverlayManager::getSingleton().getOverlayElement("GUI/CinematicCaption")->hide();

		showBackdropOverlay(false);

		//HP Panel
		mHPBar = OverlayManager::getSingleton().getOverlayElement("GUI/HPBar");
		mSkillText = OverlayManager::getSingleton().getOverlayElement("GUI/SkillText");
		mPickupText = OverlayManager::getSingleton().getOverlayElement("GUI/PickupText");

		showHPPanelOverlay(false);

		//Target Panel
		mTargetPanel = OverlayManager::getSingleton().getOverlayElement("GUI/TargetPanel");
		mTargetHP = OverlayManager::getSingleton().getOverlayElement("GUI/TargetHP");

		showTargetPanelOverlay(false);

		//Options
		mBox[GUI_OPTIONS] = OverlayManager::getSingleton().getOverlayElement("GUI/Options");
		mButton[BUTTON_OPTIONRESUME] = OverlayManager::getSingleton().getOverlayElement("GUI/OptionResumeButton");
		mButtonText[BUTTON_OPTIONRESUME] = OverlayManager::getSingleton().getOverlayElement("GUI/OptionResumeButtonText");
		mButton[BUTTON_OPTIONSETTINGS] = OverlayManager::getSingleton().getOverlayElement("GUI/OptionSettingsButton");
		mButtonText[BUTTON_OPTIONSETTINGS] = OverlayManager::getSingleton().getOverlayElement("GUI/OptionSettingsButtonText");
		mButton[BUTTON_OPTIONINTERFACE] = OverlayManager::getSingleton().getOverlayElement("GUI/OptionInterfaceButton");
		mButtonText[BUTTON_OPTIONINTERFACE] = OverlayManager::getSingleton().getOverlayElement("GUI/OptionInterfaceButtonText");
		mButton[BUTTON_OPTIONRESTART] = OverlayManager::getSingleton().getOverlayElement("GUI/OptionRestartButton");
		mButtonText[BUTTON_OPTIONRESTART] = OverlayManager::getSingleton().getOverlayElement("GUI/OptionRestartButtonText");
		mButton[BUTTON_OPTIONQUIT] = OverlayManager::getSingleton().getOverlayElement("GUI/OptionQuitButton");
		mButtonText[BUTTON_OPTIONQUIT] = OverlayManager::getSingleton().getOverlayElement("GUI/OptionQuitButtonText");

		mBox[GUI_SETTINGS] = OverlayManager::getSingleton().getOverlayElement("GUI/Settings");
		for(int i=0;i<3;i++)
		{
			const String tNum = StringConverter::toString(i+1);
			mButton[BUTTON_SETTINGS1+i] = OverlayManager::getSingleton().getOverlayElement("GUI/SettingsButton" + tNum);
			mButtonText[BUTTON_SETTINGS1+i] = OverlayManager::getSingleton().getOverlayElement("GUI/SettingsButtonText" + tNum);
		}
		mButton[BUTTON_SETTINGSBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/SettingsButtonBack");
		mButtonText[BUTTON_SETTINGSBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/SettingsButtonBackText");
		for(int i=0;i<4;i++)
		{
			const String tNum = StringConverter::toString(i+1);
			mScroller[SCROLLER_SETTINGS1+i] = OverlayManager::getSingleton().getOverlayElement("GUI/SettingsSlider" + tNum);
			mScrollerMarker[SCROLLER_SETTINGS1+i] = OverlayManager::getSingleton().getOverlayElement("GUI/SettingsSliderMarker" + tNum);
			isScrollerHorz[SCROLLER_SETTINGS1+i] = true;
			setScroll(SCROLLER_SETTINGS1+i,1);
		}

		mBox[GUI_INTERFACE] = OverlayManager::getSingleton().getOverlayElement("GUI/Interface");
		for(int i=0;i<8;i++)
		{
			const String tNum = StringConverter::toString(i+1);
			mButton[BUTTON_INTERFACE1+i] = OverlayManager::getSingleton().getOverlayElement("GUI/InterfaceButton" + tNum);
			mButtonText[BUTTON_INTERFACE1+i] = OverlayManager::getSingleton().getOverlayElement("GUI/InterfaceButtonText" + tNum);
		}
		mButton[BUTTON_INTERFACEBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/InterfaceButtonBack");
		mButtonText[BUTTON_INTERFACEBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/InterfaceButtonBackText");
		mScroller[SCROLLER_INTERFACE1] = OverlayManager::getSingleton().getOverlayElement("GUI/InterfaceSlider1");
		mScrollerMarker[SCROLLER_INTERFACE1] = OverlayManager::getSingleton().getOverlayElement("GUI/InterfaceSliderMarker1");
		isScrollerHorz[SCROLLER_INTERFACE1] = true;
		setScroll(SCROLLER_INTERFACE1,1);

		showOptionsOverlay(false);

		//StartScreenGUI
		mBox[GUI_STARTSCREENBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenBox");
		mButton[BUTTON_NEWGAME] = OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenButton1");
		mButtonText[BUTTON_NEWGAME] = OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenButtonText1");
		mButton[BUTTON_MULTIPLAYER] = OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenButton2");
		mButtonText[BUTTON_MULTIPLAYER] = OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenButtonText2");
		mButton[BUTTON_QUIT] = OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenButton3");
		mButtonText[BUTTON_QUIT] = OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenButtonText3");

		OverlayManager::getSingleton().getOverlayElement("GUI/UpdateBox")->hide();
		OverlayManager::getSingleton().getOverlayElement("GUI/UpdateBoxText")->setCaption(mDef->loadUpdateCaption());

		showStartScreenOverlay(false);

		//LogonGUI
		mBox[GUI_LOGONBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/LogonBox");
		mButton[BUTTON_LOGON1] = OverlayManager::getSingleton().getOverlayElement("GUI/LogonButton1");
		mButtonText[BUTTON_LOGON1] = OverlayManager::getSingleton().getOverlayElement("GUI/LogonButtonText1");
		mButton[BUTTON_NEWACCOUNT] = OverlayManager::getSingleton().getOverlayElement("GUI/LogonButton2");
		mButtonText[BUTTON_NEWACCOUNT] = OverlayManager::getSingleton().getOverlayElement("GUI/LogonButtonText2");
		mButton[BUTTON_EDITACCOUNT] = OverlayManager::getSingleton().getOverlayElement("GUI/LogonButton3");
		mButtonText[BUTTON_EDITACCOUNT] = OverlayManager::getSingleton().getOverlayElement("GUI/LogonButtonText3");
		mButton[BUTTON_LOGONBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/LogonButtonBack");
		mButtonText[BUTTON_LOGONBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/LogonButtonTextBack");
		mCheckBox[CHECKBOX_LOGON] = OverlayManager::getSingleton().getOverlayElement("GUI/LogonCheckBox");

		showLogonOverlay(false);

		//CreateAccountGUI
		mBox[GUI_CREATEACCOUNTBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/CreateAccountBox");
		mButton[BUTTON_CREATEACCOUNT1] = OverlayManager::getSingleton().getOverlayElement("GUI/CreateAccountButton1");
		mButtonText[BUTTON_CREATEACCOUNT1] = OverlayManager::getSingleton().getOverlayElement("GUI/CreateAccountButtonText1");
		mButton[BUTTON_CREATEACCOUNTBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/CreateAccountButtonBack");
		mButtonText[BUTTON_CREATEACCOUNTBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/CreateAccountButtonTextBack");

		for(int i=0;i<MAX_CREATEACCOUNTINFO;i++)
			mCreateAccountTextBox[i] = OverlayManager::getSingleton().getOverlayElement("GUI/CreateAccountText"+StringConverter::toString(i+1));

		showCreateAccountOverlay(false);

		//EditAccountGUI
		mBox[GUI_EDITACCOUNTBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/EditAccountBox");
		mButton[BUTTON_EDITACCOUNT1] = OverlayManager::getSingleton().getOverlayElement("GUI/EditAccountButton1");
		mButtonText[BUTTON_EDITACCOUNT1] = OverlayManager::getSingleton().getOverlayElement("GUI/EditAccountButtonText1");
		mButton[BUTTON_EDITACCOUNTBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/EditAccountButtonBack");
		mButtonText[BUTTON_EDITACCOUNTBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/EditAccountButtonTextBack");

		for(int i=0;i<MAX_EDITACCOUNTINFO;i++)
			mEditAccountTextBox[i] = OverlayManager::getSingleton().getOverlayElement("GUI/EditAccountText"+StringConverter::toString(i+1));

		showEditAccountOverlay(false);

		//CampaignScreenGUI
		mBox[GUI_CAMPAIGNBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/CampaignBox");
		mButton[BUTTON_CAMPAIGNBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/CampaignButtonBack");
		mButtonText[BUTTON_CAMPAIGNBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/CampaignButtonTextBack");
		mButton[BUTTON_CAMPAIGNNEXT] = OverlayManager::getSingleton().getOverlayElement("GUI/CampaignButtonNext");
		mButtonText[BUTTON_CAMPAIGNNEXT] = OverlayManager::getSingleton().getOverlayElement("GUI/CampaignButtonTextNext");
		for(int i=0;i<3;i++)
		{
			const String tNum = StringConverter::toString(i+1);
			mButton[BUTTON_CAMPAIGN1+i] = OverlayManager::getSingleton().getOverlayElement("GUI/CampaignButton"+tNum);
			mButtonText[BUTTON_CAMPAIGN1+i] = OverlayManager::getSingleton().getOverlayElement("GUI/CampaignButtonText"+tNum);
		}

		showCampaignOverlay(false);

		//CharScreenGUI
		mBox[GUI_CHARSCREENBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenBox");
		mBoxText[GUI_CHARSCREENBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenNameText");
		mButton[BUTTON_CHARSCREENBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenButtonBack");
		mButtonText[BUTTON_CHARSCREENBACK] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenButtonTextBack");
		mButton[BUTTON_CHARSCREENNEXT] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenButtonNext");
		mButtonText[BUTTON_CHARSCREENNEXT] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenButtonTextNext");
		mButton[BUTTON_CHARSCREENDELETE] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenButtonDelete");
		mButtonText[BUTTON_CHARSCREENDELETE] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenButtonTextDelete");
		for(int i=0;i<MAX_CHARSCREENTEXT;i++)
		{
			const String tNum = StringConverter::toString(i+1);
			mCharScreenText[i] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenText" + tNum); 
			mButton[BUTTON_CHARSCREEN_L1+i*2] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenButtonL" + tNum);
			mButtonText[BUTTON_CHARSCREEN_L1+i*2] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenButtonTextL" + tNum);
			mButton[BUTTON_CHARSCREEN_R1+i*2] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenButtonR" + tNum);
			mButtonText[BUTTON_CHARSCREEN_R1+i*2] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenButtonTextR" + tNum);
			mCharScreenManager->initializeText(i,mCharScreenText[i]);
			mCharScreenManager->initializeLRButtons(i,mButton[BUTTON_CHARSCREEN_L1+i*2],mButton[BUTTON_CHARSCREEN_R1+i*2]);
		}
		for(int i=0;i<3;i++)
		{
			const String tNum = StringConverter::toString(i+1);
			mScroller[SCROLLER_CHARSCREEN1+i] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenSlider" + tNum);
			mScrollerMarker[SCROLLER_CHARSCREEN1+i] = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenSliderMarker" + tNum);
			isScrollerHorz[SCROLLER_CHARSCREEN1+i] = true;
			mCharScreenManager->initializeSliders(i,mScroller[SCROLLER_CHARSCREEN1+i],mScrollerMarker[SCROLLER_CHARSCREEN1+i]);
		}
		mCharScreenManager->initialize(mDef,mEffectsManager,mAlertBox,mUnitManager->getPlayer(),sceneMgr,mBoxText[GUI_CHARSCREENBOX],mBackdrop);
		mCharScreenManager->initializeBackNextButtons(mButton[BUTTON_CHARSCREENBACK],mButton[BUTTON_CHARSCREENNEXT],mButton[BUTTON_CHARSCREENDELETE],mButtonText[BUTTON_CHARSCREENBACK],mButtonText[BUTTON_CHARSCREENNEXT],mButtonText[BUTTON_CHARSCREENDELETE]);

		showCharScreenOverlay(false);

		//Server List
		mBox[GUI_SERVERBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/ServerBox");
		mButton[BUTTON_SERVER1] = OverlayManager::getSingleton().getOverlayElement("GUI/ServerButton1");
		mButtonText[BUTTON_SERVER1] = OverlayManager::getSingleton().getOverlayElement("GUI/ServerButtonText1");
		mButton[BUTTON_SERVER2] = OverlayManager::getSingleton().getOverlayElement("GUI/ServerButton2");
		mButtonText[BUTTON_SERVER2] = OverlayManager::getSingleton().getOverlayElement("GUI/ServerButtonText2");

		showServerOverlay(false);

		//ChatGUI
		mBox[GUI_TEXTINPUT] = OverlayManager::getSingleton().getOverlayElement("GUI/TextInputBox");
		mBoxText[GUI_TEXTINPUT] = OverlayManager::getSingleton().getOverlayElement("GUI/TextInputBoxText");
		mButton[BUTTON_TEXTINPUT] = OverlayManager::getSingleton().getOverlayElement("GUI/TextInputBoxButton");
		mButtonText[BUTTON_TEXTINPUT] = OverlayManager::getSingleton().getOverlayElement("GUI/TextInputBoxButtonText");
		mButton[BUTTON_TEXTINPUT2] = OverlayManager::getSingleton().getOverlayElement("GUI/TextInputBoxButton2");
		mButtonText[BUTTON_TEXTINPUT2] = OverlayManager::getSingleton().getOverlayElement("GUI/TextInputBoxButtonText2");
		mButton[BUTTON_TEXTINPUT3] = OverlayManager::getSingleton().getOverlayElement("GUI/TextInputBoxButton3");
		mButtonText[BUTTON_TEXTINPUT3] = OverlayManager::getSingleton().getOverlayElement("GUI/TextInputBoxButtonText3");

		mBox[GUI_TEXTOUTPUT] = OverlayManager::getSingleton().getOverlayElement("GUI/TextOutputBox");
		mBoxText[GUI_TEXTOUTPUT] = OverlayManager::getSingleton().getOverlayElement("GUI/TextOutputBoxText");
		mButton[BUTTON_TEXTOUTPUT] = OverlayManager::getSingleton().getOverlayElement("GUI/TextOutputBoxButton");
		mButtonText[BUTTON_TEXTOUTPUT] = OverlayManager::getSingleton().getOverlayElement("GUI/TextOutputBoxButtonText");
		mSizer[SIZER_TEXTOUTPUT] = OverlayManager::getSingleton().getOverlayElement("GUI/TextOutputBoxSizer");
		mScroller[SCROLLER_TEXTOUTPUT] = OverlayManager::getSingleton().getOverlayElement("GUI/TextOutputBoxScroller");
		mScrollerMarker[SCROLLER_TEXTOUTPUT] = OverlayManager::getSingleton().getOverlayElement("GUI/TextOutputBoxScrollerMarker");
		isScrollerHorz[SCROLLER_TEXTOUTPUT] = false;
		setScroll(SCROLLER_TEXTOUTPUT,1);

		showChatOverlay(false);

		//Friend box
		mBox[GUI_FRIENDBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/FriendBox");
		mBoxText[GUI_FRIENDBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/FriendBoxText");
		mButton[BUTTON_FRIENDBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/FriendButton");
		mButtonText[BUTTON_FRIENDBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/FriendButtonText");
		mScroller[SCROLLER_FRIENDBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/FriendScroller");
		mScrollerMarker[SCROLLER_FRIENDBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/FriendScrollerMarker");
		isScrollerHorz[SCROLLER_FRIENDBOX] = false;
		setScroll(SCROLLER_FRIENDBOX,0);
		mMover[MOVER_FRIENDBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/FriendMover");

		refreshFriendList();
		showFriendBox(false);

		//Bio box
		mBox[GUI_BIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/BioBox");
		mBoxText[GUI_BIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/BioBoxText");
		mButton[BUTTON_BIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/BioButton");
		mButtonText[BUTTON_BIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/BioButtonText");
		mScroller[SCROLLER_BIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/BioScroller");
		mScrollerMarker[SCROLLER_BIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/BioScrollerMarker");
		isScrollerHorz[SCROLLER_BIOBOX] = false;
		setScroll(SCROLLER_BIOBOX,0);
		mMover[MOVER_BIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/BioMover");

		showBioBox(false);

		//Target Bio box
		mBox[GUI_TARGETBIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/TargetBioBox");
		mBoxText[GUI_TARGETBIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/TargetBioBoxText");
		mButton[BUTTON_TARGETBIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/TargetBioButton");
		mButtonText[BUTTON_TARGETBIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/TargetBioButtonText");
		mScroller[SCROLLER_TARGETBIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/TargetBioScroller");
		mScrollerMarker[SCROLLER_TARGETBIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/TargetBioScrollerMarker");
		isScrollerHorz[SCROLLER_TARGETBIOBOX] = false;
		setScroll(SCROLLER_TARGETBIOBOX,0);
		mMover[MOVER_TARGETBIOBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/TargetBioMover");

		showTargetBioBox(false);

		//Home box
		mBox[GUI_HOMEBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeBox");
		mButton[BUTTON_HOMEBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeButton");
		mButtonText[BUTTON_HOMEBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeButtonText");
		mButton[BUTTON_HOMEBOX1] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeButton1");
		mButtonText[BUTTON_HOMEBOX1] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeButtonText1");
		mButton[BUTTON_HOMEBOX2] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeButton2");
		mButtonText[BUTTON_HOMEBOX2] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeButtonText2");
		mButton[BUTTON_HOMEBOX3] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeButton3");
		mButtonText[BUTTON_HOMEBOX3] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeButtonText3");
		mButton[BUTTON_HOMEBOX4] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeButton4");
		mButtonText[BUTTON_HOMEBOX4] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeButtonText4");

		mMover[MOVER_HOMEBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/HomeMover");

		showHomeBox(false);

		//Emote box
		mBox[GUI_EMOTEBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/EmoteBox");
		mBoxText[GUI_EMOTEBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/EmoteBoxText");
		mButton[BUTTON_EMOTEBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/EmoteButton");
		mButtonText[BUTTON_EMOTEBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/EmoteButtonText");
		mScroller[SCROLLER_EMOTEBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/EmoteScroller");
		mScrollerMarker[SCROLLER_EMOTEBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/EmoteScrollerMarker");
		isScrollerHorz[SCROLLER_EMOTEBOX] = false;
		setScroll(SCROLLER_EMOTEBOX,0);
		mMover[MOVER_EMOTEBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/EmoteMover");

		showEmoteBox(false);

		//Item box
		mBox[GUI_ITEMBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/ItemBox");
		mBoxText[GUI_ITEMBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/ItemBoxText");
		mButton[BUTTON_ITEMBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/ItemButton");
		mButtonText[BUTTON_ITEMBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/ItemButtonText");
		mButton[BUTTON_ITEMBOX1] = OverlayManager::getSingleton().getOverlayElement("GUI/ItemButton1");
		mButtonText[BUTTON_ITEMBOX1] = OverlayManager::getSingleton().getOverlayElement("GUI/ItemButtonText1");
		mMover[MOVER_ITEMBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/ItemMover");

		showItemBox(false);

		//Stash box
		mBox[GUI_STASHBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/StashBox");
		mBoxText[GUI_STASHBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/StashBoxText");
		mButton[BUTTON_STASHBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/StashButton");
		mButtonText[BUTTON_STASHBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/StashButtonText");
		mMover[MOVER_STASHBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/StashMover");
		mScroller[SCROLLER_STASHBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/StashScroller");
		mScrollerMarker[SCROLLER_STASHBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/StashScrollerMarker");
		isScrollerHorz[SCROLLER_STASHBOX] = false;
		setScroll(SCROLLER_STASHBOX,0);

		showItemBox(false);

		//Party box
		mBox[GUI_PARTYBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/PartyBox");
		mBoxText[GUI_PARTYBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/PartyBoxText");
		mButton[BUTTON_PARTYBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/PartyButton");
		mButtonText[BUTTON_PARTYBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/PartyButtonText");
		mButton[BUTTON_PARTYBOX1] = OverlayManager::getSingleton().getOverlayElement("GUI/PartyButton1");
		mButtonText[BUTTON_PARTYBOX1] = OverlayManager::getSingleton().getOverlayElement("GUI/PartyButtonText1");
		mButton[BUTTON_PARTYBOX2] = OverlayManager::getSingleton().getOverlayElement("GUI/PartyButton2");
		mButtonText[BUTTON_PARTYBOX2] = OverlayManager::getSingleton().getOverlayElement("GUI/PartyButtonText2");
		mButton[BUTTON_PARTYBOX3] = OverlayManager::getSingleton().getOverlayElement("GUI/PartyButton3");
		mButtonText[BUTTON_PARTYBOX3] = OverlayManager::getSingleton().getOverlayElement("GUI/PartyButtonText3");
		mMover[MOVER_PARTYBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/PartyMover");

		mButton[BUTTON_PARTYBOX1]->hide();
		showPartyBox(false);

		//MultiplayerGUI
		mBox[GUI_MULTIPLAYER] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerBox");
		mButton[BUTTON_MULTIPLAYER1] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButton1");
		mButtonText[BUTTON_MULTIPLAYER1] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButtonText1");
		mButton[BUTTON_MULTIPLAYER2] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButton2");
		mButtonText[BUTTON_MULTIPLAYER2] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButtonText2");
		mButton[BUTTON_MULTIPLAYER3] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButton3");
		mButtonText[BUTTON_MULTIPLAYER3] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButtonText3");
		mButton[BUTTON_MULTIPLAYER4] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButton4");
		mButtonText[BUTTON_MULTIPLAYER4] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButtonText4");
		mButton[BUTTON_MULTIPLAYER5] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButton5");
		mButtonText[BUTTON_MULTIPLAYER5] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButtonText5");
		mButton[BUTTON_MULTIPLAYER6] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButton6");
		mButtonText[BUTTON_MULTIPLAYER6] = OverlayManager::getSingleton().getOverlayElement("GUI/MultiplayerButtonText6");

		showMultiplayerOverlay(false);
		
		//Select box
		mSelectPanel = OverlayManager::getSingleton().getOverlayElement("GUI/SelectPanel");
		mBox[GUI_SELECTBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/SelectBox");
		mBoxText[GUI_SELECTBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/SelectBoxText");
		mButton[BUTTON_SELECTBOX1] = OverlayManager::getSingleton().getOverlayElement("GUI/SelectButton1");
		mButtonText[BUTTON_SELECTBOX1] = OverlayManager::getSingleton().getOverlayElement("GUI/SelectButtonText1");
		mButton[BUTTON_SELECTBOX2] = OverlayManager::getSingleton().getOverlayElement("GUI/SelectButton2");
		mButtonText[BUTTON_SELECTBOX2] = OverlayManager::getSingleton().getOverlayElement("GUI/SelectButtonText2");
		mButton[BUTTON_SELECTBOX3] = OverlayManager::getSingleton().getOverlayElement("GUI/SelectButton3");
		mButtonText[BUTTON_SELECTBOX3] = OverlayManager::getSingleton().getOverlayElement("GUI/SelectButtonText3");
		mButton[BUTTON_SELECTBOX4] = OverlayManager::getSingleton().getOverlayElement("GUI/SelectButton4");
		mButtonText[BUTTON_SELECTBOX4] = OverlayManager::getSingleton().getOverlayElement("GUI/SelectButtonText4");

		showSelectPanel(false);

		//List Select box
		mListSelectHighlight = OverlayManager::getSingleton().getOverlayElement("GUI/ListSelectHighlight");
		mBox[GUI_LISTSELECTBOX] = OverlayManager::getSingleton().getOverlayElement("GUI/ListSelectBox");
		mButton[BUTTON_LISTSELECT1] = OverlayManager::getSingleton().getOverlayElement("GUI/ListSelectButton1");
		mButtonText[BUTTON_LISTSELECT1] = OverlayManager::getSingleton().getOverlayElement("GUI/ListSelectButtonText1");
		mButton[BUTTON_LISTSELECT2] = OverlayManager::getSingleton().getOverlayElement("GUI/ListSelectButton2");
		mButtonText[BUTTON_LISTSELECT2] = OverlayManager::getSingleton().getOverlayElement("GUI/ListSelectButtonText2");
		mButton[BUTTON_LISTSELECT3] = OverlayManager::getSingleton().getOverlayElement("GUI/ListSelectButton3");
		mButtonText[BUTTON_LISTSELECT3] = OverlayManager::getSingleton().getOverlayElement("GUI/ListSelectButtonText3");

		showListSelectBox(false);

		initializeSettings();
	}
	void showFaderOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Fader")->show();
			mFader->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Fader")->hide();
			mFader->hide();
		}
	}
	void showPseudoShaderOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/PseudoShader")->show();
			mPseudoShader->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/PseudoShader")->hide();
			mPseudoShader->hide();
		}
	}
	void showCursorOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Cursor")->show();
			//mCursor->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Cursor")->hide();
			mCursor->hide();
			mAlertBox->hide();
		}
	}
	void showBackdropOverlay(bool flag, bool showLoadingText=false, const String &loadingCaption="Loading...")
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Backdrop")->show();
			mBackdrop->show();
			if(showLoadingText)
			{
				OverlayManager::getSingleton().getOverlayElement("GUI/LoadingText")->show();
				OverlayManager::getSingleton().getOverlayElement("GUI/LoadingText")->setCaption(loadingCaption);
			}
			else
				OverlayManager::getSingleton().getOverlayElement("GUI/LoadingText")->hide();
		}
		else 
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Backdrop")->hide();
			mBackdrop->hide();
			OverlayManager::getSingleton().getOverlayElement("GUI/LoadingText")->hide();
		}
	}
	void showHPPanelOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/HPPanel")->show();
			OverlayManager::getSingleton().getOverlayElement("GUI/HPPanel")->show();
			mHPBar->show();
			mSkillText->show();
			updateSkillText(mUnitManager->getPlayer()->getCurrentSkill());
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/HPPanel")->hide();
			OverlayManager::getSingleton().getOverlayElement("GUI/HPPanel")->hide();
			mHPBar->hide();
			mSkillText->hide();
			mPickupText->hide();
		}
	}
	void showTargetPanelOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/TargetPanel")->show();
			mTargetPanel->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/TargetPanel")->hide();
			mTargetPanel->hide();
		}
	}
	void showOptionsOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Options")->show();
			mBox[GUI_OPTIONS]->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Options")->hide();
			mBox[GUI_OPTIONS]->hide();
			mBox[GUI_SETTINGS]->hide();
			mBox[GUI_INTERFACE]->hide();
		}
	}
	void showStartScreenOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/StartScreen")->show();
			mBox[GUI_STARTSCREENBOX]->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/StartScreen")->hide();
			mBox[GUI_STARTSCREENBOX]->hide();
		}
	}
	void showCampaignOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/CampaignScreen")->show();
			mBox[GUI_CAMPAIGNBOX]->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/CampaignScreen")->hide();
			mBox[GUI_CAMPAIGNBOX]->hide();
		}
	}
	void showLogonOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/LogonScreen")->show();
			mBox[GUI_LOGONBOX]->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/LogonScreen")->hide();
			mBox[GUI_LOGONBOX]->hide();
		}
	}
	void showCreateAccountOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/CreateAccountScreen")->show();
			mBox[GUI_CREATEACCOUNTBOX]->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/CreateAccountScreen")->hide();
			mBox[GUI_CREATEACCOUNTBOX]->hide();
		}
	}
	void showEditAccountOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/EditAccountScreen")->show();
			mBox[GUI_EDITACCOUNTBOX]->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/EditAccountScreen")->hide();
			mBox[GUI_EDITACCOUNTBOX]->hide();
		}
	}
	void showCharScreenOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/CharScreen")->show();
			mBox[GUI_CHARSCREENBOX]->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/CharScreen")->hide();
			mBox[GUI_CHARSCREENBOX]->hide();
		}
	}
	void showServerOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Server")->show();
			mBox[GUI_SERVERBOX]->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Server")->hide();
			mBox[GUI_SERVERBOX]->hide();
		}
	}
	void showMultiplayerOverlay(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Multiplayer")->show();
			mBox[GUI_MULTIPLAYER]->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Multiplayer")->hide();
			mBox[GUI_MULTIPLAYER]->hide();
			showFriendBox(false);
			showBioBox(false);
			showTargetBioBox(false);
			showHomeBox(false);
			showEmoteBox(false);
			showItemBox(false);
			showPartyBox(false);
			showStashBox(false);
		}
	}
	void showHomeBox(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/HomeBox")->show();
			mBox[GUI_HOMEBOX]->show();
			setCurrentWindow(mBox[GUI_HOMEBOX]);
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/HomeBox")->hide();
			mBox[GUI_HOMEBOX]->hide();
		}
	}
	void showFriendBox(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/FriendBox")->show();
			mBox[GUI_FRIENDBOX]->show();
			setCurrentWindow(mBox[GUI_FRIENDBOX]);
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/FriendBox")->hide();
			mBox[GUI_FRIENDBOX]->hide();
		}
	}
	void showEmoteBox(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/EmoteBox")->show();
			mBox[GUI_EMOTEBOX]->show();
			setCurrentWindow(mBox[GUI_EMOTEBOX]);
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/EmoteBox")->hide();
			mBox[GUI_EMOTEBOX]->hide();
		}
	}
	void showItemBox(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/ItemBox")->show();
			mBox[GUI_ITEMBOX]->show();
			setCurrentWindow(mBox[GUI_ITEMBOX]);
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/ItemBox")->hide();
			mBox[GUI_ITEMBOX]->hide();
		}
	}
	void showStashBox(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/StashBox")->show();
			mBox[GUI_STASHBOX]->show();
			setCurrentWindow(mBox[GUI_STASHBOX]);
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/StashBox")->hide();
			mBox[GUI_STASHBOX]->hide();
		}
	}
	void showPartyBox(bool flag, bool showAcceptDenyButtons=false)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/PartyBox")->show();
			mBox[GUI_PARTYBOX]->show();
			setCurrentWindow(mBox[GUI_PARTYBOX]);
			if(showAcceptDenyButtons)
			{
				mBox[GUI_PARTYBOX]->setHeight(0.35);
				mButton[BUTTON_PARTYBOX2]->show();
				mButton[BUTTON_PARTYBOX3]->show();
				OverlayManager::getSingleton().getOverlayElement("GUI/PartyBoxText2")->show();
			}
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/PartyBox")->hide();
			mBox[GUI_PARTYBOX]->hide();
			mButton[BUTTON_PARTYBOX2]->hide();
			mButton[BUTTON_PARTYBOX3]->hide();
			OverlayManager::getSingleton().getOverlayElement("GUI/PartyBoxText2")->hide();
			mBox[GUI_PARTYBOX]->setHeight(0.225);
		}
	}
	void showBioBox(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/BioBox")->show();
			mBox[GUI_BIOBOX]->show();
			setCurrentWindow(mBox[GUI_BIOBOX]);
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/BioBox")->hide();
			mBox[GUI_BIOBOX]->hide();
		}
	}
	void showTargetBioBox(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/TargetBioBox")->show();
			mBox[GUI_TARGETBIOBOX]->show();
			setCurrentWindow(mBox[GUI_TARGETBIOBOX]);
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/TargetBioBox")->hide();
			mBox[GUI_TARGETBIOBOX]->hide();
		}
	}
	void showListSelectBox(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/ListSelectBox")->show();
			mBox[GUI_LISTSELECTBOX]->show();
			mHoverWindow = mBox[GUI_LISTSELECTBOX];
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/ListSelectBox")->hide();
			mBox[GUI_LISTSELECTBOX]->hide();
			mListSelectHighlight->hide();
			listButtonData = "";
			listButtonType = LISTBUTTON_NONE;
			mListSelectTarget = 0;
		}
	}
	void showChatOverlay(bool flag, bool showChat=true, bool bShowChatBox=false)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Chat")->show();
			if(showChat)
			{
				mBox[GUI_TEXTINPUT]->show();
				showChatBox(bShowChatBox);
			}
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Chat")->hide();
			mBox[GUI_TEXTINPUT]->hide();
			mBox[GUI_TEXTOUTPUT]->hide();
		}
	}
	void showCreditsOverlay(bool flag, bool showCreditsCaption=true, bool showGameOverButtons=false)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Credits")->show();
			OverlayManager::getSingleton().getOverlayElement("GUI/Credits")->show();
			if(showCreditsCaption)OverlayManager::getSingleton().getOverlayElement("GUI/CreditsCaption")->show();
			if(showGameOverButtons)
			{
				mButton[BUTTON_GAMEOVERCONTINUE]->show();
				mButton[BUTTON_GAMEOVERQUIT]->show();
			}
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/Credits")->hide();
			OverlayManager::getSingleton().getOverlayElement("GUI/Credits")->hide();
			OverlayManager::getSingleton().getOverlayElement("GUI/CreditsCaption")->hide();
			mButton[BUTTON_GAMEOVERCONTINUE]->hide();
			mButton[BUTTON_GAMEOVERQUIT]->hide();
		}
	}
	void scrollMouse(int x, int y, int winWidth, int winHeight, bool setPosition=false)
	{
		if(setPosition)mCursor->setPosition(x,y);
		else mCursor->setPosition(mCursor->getLeft()+x,mCursor->getTop()+y);
		
		mCursor->setPosition( (mCursor->getLeft()<0 ? 0 : (mCursor->getLeft()>winWidth ? winWidth : mCursor->getLeft())),
								(mCursor->getTop()<0 ? 0 : (mCursor->getTop()>winHeight ? winHeight : mCursor->getTop())) );

		cursorX = mCursor->getLeft()/winWidth;
		cursorY = mCursor->getTop()/winHeight;

		//Sizer buttons
		if(sizeWindowDir && mHoverButton)
		{
			const Real dX = cursorX - dragStartX;
			const Real dY = cursorY - dragStartY;
			const Real tWidth = mHoverButton->getParent()->getWidth();
			const Real tHeight = mHoverButton->getParent()->getHeight();

			switch(sizeWindowDir)
			{
				case SIZEDIR_TOP:
					mHoverButton->getParent()->setHeight(sizeWindowHeight - dY);
					mHoverButton->getParent()->setTop(sizeWindowTop + dY);
				break;
				case SIZEDIR_BOTTOM:
					mHoverButton->getParent()->setHeight(sizeWindowHeight + dY);
				break;
				case SIZEDIR_LEFT:
					mHoverButton->getParent()->setWidth(sizeWindowWidth - dY);
					mHoverButton->getParent()->setLeft(sizeWindowLeft + dY);
				break;
				case SIZEDIR_RIGHT:
					mHoverButton->getParent()->setWidth(sizeWindowWidth + dY);
				break;
			}

			if(sizeWindowMinWidth!=0 && mHoverButton->getParent()->getWidth()<sizeWindowMinWidth)
			{
				if(SIZEDIR_LEFT)mHoverButton->getParent()->setLeft(mHoverButton->getParent()->getLeft()+(mHoverButton->getParent()->getWidth()-sizeWindowMinWidth));
				mHoverButton->getParent()->setWidth(sizeWindowMinWidth);
			}
			if(sizeWindowMinHeight!=0 && mHoverButton->getParent()->getHeight()<sizeWindowMinHeight)
			{
				if(SIZEDIR_TOP)mHoverButton->getParent()->setTop(mHoverButton->getParent()->getTop()+(mHoverButton->getParent()->getHeight()-sizeWindowMinHeight));
				mHoverButton->getParent()->setHeight(sizeWindowMinHeight);
			}

			//Update scrollbars
			resizeScrollbarByParent(mHoverButton->getParent(),tWidth,tHeight);
		}
		//Movers
		if(mMoverWindow)
		{
			mMoverWindow->setPosition(cursorX-dragStartX,cursorY-dragStartY);
		}
		//Scrollers
		if(scrollerHeldID != -1)
		{
			doScroll(true,scrollerHeldID);

			//Scroller actions
			if(scrollerHeldID==SCROLLER_CHARSCREEN1||scrollerHeldID==SCROLLER_CHARSCREEN2||scrollerHeldID==SCROLLER_CHARSCREEN3)	//charscreen sliders
			{
				mCharScreenManager->updateSliders(getScroll(SCROLLER_CHARSCREEN1),getScroll(SCROLLER_CHARSCREEN2),getScroll(SCROLLER_CHARSCREEN3));
			}
			else if(scrollerHeldID==SCROLLER_SETTINGS1)	//music volume
			{
				mSoundManager->setMusicVolume(getScroll(SCROLLER_SETTINGS1));
			}
			else if(scrollerHeldID==SCROLLER_SETTINGS2)	//guisound volume
			{
				mSoundManager->setGUISoundVolume(getScroll(SCROLLER_SETTINGS2));
			}
			else if(scrollerHeldID==SCROLLER_SETTINGS3)	//sound volume
			{
				mSoundManager->setSoundVolume(getScroll(SCROLLER_SETTINGS3));
			}
			else if(scrollerHeldID==SCROLLER_SETTINGS4)	//view distance
			{
				mDef->viewDistance = getScroll(SCROLLER_SETTINGS4);
			}
		}
	}
	void showMouse(bool flag)
	{
		if(flag)
		{
			if(!mCursor->isVisible())mCursor->show();
		}
		else 
		{
			if(mCursor->isVisible())mCursor->hide();
		}
	}
	const Real getMouseX()
	{
		return cursorX;
	}
	const Real getMouseY()
	{
		return cursorY;
	}
	void reset()
	{
		mChatManager->reset();
		mMiniMap->reset();
		showMouse(true);
		isGUIVisible = true;
		if(underwaterShaderFlag)toggleUnderwaterShader();
		setWounded(mUnitManager->getPlayer()->getIsWounded());
	}
	void update(const FrameEvent &evt)
	{
		updateHover();
		mInputManager->update(evt);
		updateChatBox(evt);
		mAlertBox->update(evt);
		updateLogo(evt);
		updateCharScreen(evt);
		
		updateFader(evt);
		updateCinematicBars(evt);
		updateCinematicCaption(evt);

		updateSelectPanel(evt);
		updateTargetIndicator();
		if(mGameStateManager->isCinematic()&&!isOptionsVisible())showMouse(false);
		if(!mGameStateManager->isCinematic() && mGameStateManager->isInGame())
		{
			updateHPPanel(evt);
			updateTargetPanel(evt);
			updatePlayer(evt);
		}
		mMiniMap->update(mCursor->isVisible(),cursorX,cursorY,evt);
		updateCompositors();

		if(!mGameStateManager->isCampaign())
		{
			updateListSelectBox();
			updateFriendBox();
			updateBioBox();
			updateTargetBioBox();
			updateEmoteBox();
			updateItemBox();
			updateHomeBox();
			updatePartyBox();
			updateStashBox();
		}
		updateNetworkMessage();
	}
	void updatePlayer(const FrameEvent &evt)
	{
		//Pickup Item
		if(mUnitManager->getPlayer()->popIsReadyToPickup())
		{
			MagixObject *tObject = mUnitManager->getPlayer()->getAutoTrackObject();
			if(!tObject)return;
			if(tObject->getType()==OBJECT_ITEM)
			{
				MagixItem *tItem = static_cast<MagixItem*>(tObject);
				pickupItem(tItem);
			}
			else if(tObject->getType()==OBJECT_CRITTER)
			{
				MagixCritter *tCritter = static_cast<MagixCritter*>(tObject);
				if(!tCritter->getIsDrawPoint())return;
				if(mGameStateManager->isCampaign())mUnitManager->killAndRewardCritter(tCritter);
				else mNetworkManager->sendCritterKilled(tCritter->getID(),0);
			}
		}
		//HP changes
		if(mUnitManager->getPlayer()->popMaxHPChanged())
		{
			mUnitManager->getPlayer()->popHPChanged();
			if(!mGameStateManager->isCampaign())mNetworkManager->sendHPUpdate(true);
		}
		else if(mUnitManager->getPlayer()->popHPChanged())
		{
			if(!mGameStateManager->isCampaign())mNetworkManager->sendHPUpdate();
		}
		//Skill changes
		const vector<const Skill>::type tSkillChange = mUnitManager->getPlayer()->popSkillChangedList();
		for(int i=0;i<(int)tSkillChange.size();i++)
		{
			if(!mGameStateManager->isCampaign())mNetworkManager->sendSkillUpdate(tSkillChange[i].name,tSkillChange[i].stock);
			if(getSkillText()==tSkillChange[i].name)updateSkillText(tSkillChange[i].stock>0?&tSkillChange[i]:0);
		}
		//Skill pickup
		const String tPickup = mUnitManager->popPickupText();
		if(tPickup!="")showPickupText(tPickup);
		//New attacks
		if(mUnitManager->popPlayerHasNewAttack())
		{
			const String tAttack = mUnitManager->getPlayer()->getAttackName();
			if(tAttack!="")mNetworkManager->sendAttack(tAttack);
		}
		//Wounded
		if(mUnitManager->getPlayer()->popWoundedChanged())
		{
			if(mUnitManager->getPlayer()->getIsWounded())
			{
				mUnitManager->getPlayer()->doCrouch(true);
				mUnitManager->getPlayer()->cancelAction(7,"Faint");
				mUnitManager->getPlayer()->setFreeLook(false);
				mUnitManager->getPlayer()->setAutoAttack(0);
				mChatManager->message("You're wounded!");
				if(!mGameStateManager->isCampaign())mNetworkManager->sendPlayerWounded();
			}
			else
			{
				mUnitManager->getPlayer()->setHPRatio(0.05);
				setWounded(false);
				mChatManager->message("You've recovered from your wound.");
				if(!mGameStateManager->isCampaign())mNetworkManager->sendPlayerUnwounded();
			}
		}
		else if(mUnitManager->getPlayer()->getIsWounded())
		{
			//Fresh wound
			if(mUnitManager->getPlayer()->getHP()==0 && mUnitManager->getPlayer()->getHitTimeout()<=1.5)
			{
				mUnitManager->getPlayer()->doCrouch(false);
				mUnitManager->getPlayer()->cancelAction(1.5,"CrouchTransition",true);
				setWounded(true);
				mUnitManager->getPlayer()->setHPRatio(1);
				mUnitManager->getPlayer()->doShakeHead();
				mUnitManager->getPlayer()->setFreeLook(true);
				mUnitManager->getPlayer()->setEmote("Normal",mDef);
				if(!mGameStateManager->isCampaign())mNetworkManager->sendEmote("Normal");
			}
		}
		//Tame Critter
		const TameData tTameFlag = mUnitManager->popTameFlag();
		if(tTameFlag.hasData)
		{
			mChatManager->message(tTameFlag.message);
			if(tTameFlag.success)mNetworkManager->sendCritterTamed(tTameFlag.id);
		}
		//Violations check
		static bool tSentViolation1 = false;
		if(mUnitManager->getPlayer()->getHP()>1000 && !mDef->isAdmin && !tSentViolation1)
		{
			tSentViolation1 = true;
			mNetworkManager->sendViolation("HP>1000");
		}
		static bool tSentViolation2 = false;
		if(mUnitManager->getPlayer()->getMaxHP()>1000 && !mDef->isAdmin && !tSentViolation2)
		{
			tSentViolation2 = true;
			mNetworkManager->sendViolation("MAXHP>1000");
		}
		static bool tSentViolation3 = false;
		if(mUnitManager->getPlayer()->getObjectNode()->getScale().y>2 && !mDef->isAdmin && !tSentViolation3)
		{
			tSentViolation3 = true;
			mNetworkManager->sendViolation("Size>2");
		}
		
	}
	void setWounded(bool flag)
	{
		if(flag)mHPBar->setMaterialName("GUIMat/WoundBar");
		else mHPBar->setMaterialName("GUIMat/HPBar");
	}
	void updateNetworkMessage()
	{
		const NetworkMessage tMessage = mNetworkManager->popNetworkMessage();
		switch(tMessage.message)
		{
		case NETWORK_CONNECTED:
			if(isCreateAccount)
			{
				mNetworkManager->sendCreateAccount(createAccountInfo[0],
												createAccountInfo[1],
												createAccountInfo[3],
												createAccountInfo[4],
												createAccountInfo[5]);
			}
			else if(isEditAccount)
			{
				mNetworkManager->sendEditAccount(editAccountInfo[0],
												editAccountInfo[1],
												editAccountInfo[2]);
			}
			else
			{
				mNetworkManager->sendLogon(logonUsername,logonPassword);
			}
			break;
		case NETWORK_CONNECTIONFAILED:
			if(isCreateAccount)showCreateAccountOverlay(true);
			else if(isEditAccount)showEditAccountOverlay(true);
			else showLogonOverlay(true);
			showBackdropOverlay(true,false);
			break;
		case NETWORK_LOGONSUCCESS:
			if(!isCheckBoxTrue(mCheckBox[CHECKBOX_LOGON]))
			{
				logonPassword = "";
				mDef->savePassword(false,logonPassword);
			}
			else mDef->savePassword(true,logonPassword);
			OverlayManager::getSingleton().getOverlayElement("GUI/LogonPasswordText")->setCaption("Password: ");
			mGameStateManager->setGameState(GAMESTATE_STARTCHARSCREEN);
			showOptionsOverlay(false);
			break;
		case NETWORK_LOGONFAIL:
			showLogonOverlay(true);
			showOptionsOverlay(false);
			showBackdropOverlay(true,false);
			mAlertBox->showAlert("Invalid username or password",0.5,0.5,3);
			mNetworkManager->disconnect();
			break;
		case NETWORK_LOGONINUSE:
			showLogonOverlay(true);
			showOptionsOverlay(false);
			showBackdropOverlay(true,false);
			mAlertBox->showAlert("Username already logged in",0.5,0.5,3);
			mNetworkManager->disconnect();
			break;
		case NETWORK_CREATEACCOUNTSUCCESS:
			showLogonOverlay(true);
			showOptionsOverlay(false);
			showBackdropOverlay(true,false);
			mAlertBox->showAlert("Account Created Successfully",0.5,0.5,2);
			mNetworkManager->disconnect();
			break;
		case NETWORK_CREATEACCOUNTFAIL:
			showCreateAccountOverlay(true);
			showOptionsOverlay(false);
			showBackdropOverlay(true,false);
			mAlertBox->showAlert("Username is already in use",0.5,0.5,3);
			mNetworkManager->disconnect();
			break;
		case NETWORK_EDITACCOUNTSUCCESS:
			showLogonOverlay(true);
			showOptionsOverlay(false);
			showBackdropOverlay(true,false);
			mAlertBox->showAlert("Password Changed Successfully",0.5,0.5,2);
			mNetworkManager->disconnect();
			break;
		case NETWORK_EDITACCOUNTFAIL:
			showEditAccountOverlay(true);
			showOptionsOverlay(false);
			showBackdropOverlay(true,false);
			mAlertBox->showAlert("Password change failed",0.5,0.5,3);
			mNetworkManager->disconnect();
			break;
		case NETWORK_LOSTCONNECTION:
			mGameStateManager->setGameState(GAMESTATE_RETURNSTARTSCREEN);
			showCharScreenOverlay(false);
			mBackdrop->setMaterialName("GUIMat/Backdrop1");
			showBackdropOverlay(true,false);
			showOptionsOverlay(false);
			break;
		case NETWORK_LOADCHARSUCCESS:
			showBackdropOverlay(true,false);
			if(mUnitManager->getPlayer()->getHP()<=0)
			{
				mUnitManager->getPlayer()->setWounded(true,true);
				mUnitManager->getPlayer()->setHPRatio(1);
			}
			break;
		case NETWORK_LOADCHARFAIL:
			showBackdropOverlay(true,false);
			mAlertBox->showAlert("Character load failed",0.5,0.5,3);
			break;
		case NETWORK_CREATECHARSUCCESS:
			showBackdropOverlay(true,false);
			showCharScreenOverlay(true);
			mCharScreenManager->createCharSuccess();
			break;
		case NETWORK_CREATECHARFAIL:
			showBackdropOverlay(false,false);
			showCharScreenOverlay(true);
			mAlertBox->showAlert("Create Character failed",0.5,0.5,3);
			mCharScreenManager->refreshPage();
			break;
		case NETWORK_EDITCHARSUCCESS:
			showBackdropOverlay(true,false);
			showCharScreenOverlay(true);
			mCharScreenManager->editCharSuccess();
			break;
		case NETWORK_EDITCHARFAIL:
			showBackdropOverlay(false,false);
			showCharScreenOverlay(true);
			mAlertBox->showAlert("Edit Character failed",0.5,0.5,3);
			mCharScreenManager->refreshPage();
			break;
		case NETWORK_PARTYREQUEST:
			{
				const OwnerToken tInviter = mUnitManager->getPartyInviter();
				MagixUnit* tUnit = mUnitManager->getUnitByIndex(tInviter);
				if(tUnit)
				{
					showPartyBox(true,true);
					String tCaption = tUnit->getUser()+" has\n";
					tCaption += mUnitManager->hasParty()?"requested":"invited you";
					tCaption += " to join party";
					OverlayManager::getSingleton().getOverlayElement("GUI/PartyBoxText2")->setCaption(tCaption);
				}
			}
			break;
		case NETWORK_BLOCKNAME:
			editBlockedList(tMessage.param,true);
		}
	}
	void updateCompositors()
	{
		if(mCamera->getIsUnderwater() && !underwaterShaderFlag)toggleUnderwaterShader();
		else if(!mCamera->getIsUnderwater() && underwaterShaderFlag)toggleUnderwaterShader();
	}
	void updateTargetIndicator()
	{
		if(mGameStateManager->isCinematic() || !mGameStateManager->isInGame())
		{
			if(mTargetIndicator->isVisible())mTargetIndicator->setVisible(false);
			return;
		}

		//Show indicator for locked on target in CAMERA_LOCKED mode
		if(mUnitManager->getPlayer()->getIsLockedOn() && mCamera->getCameraMode()==CAMERA_LOCKED)
			mCurrentObject = mUnitManager->getPlayer()->getAutoTrackObject()->getObjectNode();
		else if(mUnitManager->getPlayer()->getAutoTrackObject()==0)mCurrentObject = 0;

		//Show indicator for autoattack target
		if(!mCurrentObject && mUnitManager->getPlayer()->getAutoAttackTarget())mCurrentObject = mUnitManager->getPlayer()->getAutoAttackTarget()->getObjectNode();

		//Show indicator at runto target
		if(mUnitManager->getPlayer()->getTargetAction()==TARGET_RUNTO && !mCurrentObject)
		{
			const Vector3 tVect = mUnitManager->getPlayer()->getTarget();
			mTargetIndicatorNode->setPosition(tVect.x,mUnitManager->getGroundHeight(tVect.x,tVect.z),tVect.z);
			if(mTargetIndicator->getBillboardType()!=BBT_PERPENDICULAR_COMMON)mTargetIndicator->setBillboardType(BBT_PERPENDICULAR_COMMON);
			const Real tDist = mCamera->getPosition().distance(mTargetIndicatorNode->getPosition());
			const Real tScale = tDist>1000?20: (tDist<50?1: (1+(tDist-50)*0.02) );
			mTargetIndicatorNode->setScale(tScale,tScale,tScale);
			if(!mTargetIndicator->isVisible())mTargetIndicator->setVisible(true);
			return;
		}

		//No target, hide indicator
		if(!mCurrentObject || mCurrentObject == mUnitManager->getPlayer()->getObjectNode())
		{
			if(mTargetIndicator->isVisible())mTargetIndicator->setVisible(false);
			return;
		}

		//Update indicator
		MagixObject *tObject = 0;
		if(mUnitManager->getPlayer()->getAutoTrackObject() && mUnitManager->getPlayer()->getAutoTrackObject()->getObjectNode()==mCurrentObject)
			tObject = mUnitManager->getPlayer()->getAutoTrackObject();
		else if(mUnitManager->getPlayer()->getAutoAttackTarget() && mUnitManager->getPlayer()->getAutoAttackTarget()->getObjectNode()==mCurrentObject)
			tObject = mUnitManager->getPlayer()->getAutoAttackTarget();
		if(!tObject)
		{
			if(mTargetIndicator->isVisible())mTargetIndicator->setVisible(false);
			return;
		}
		const bool targetIsUnit = (tObject->getType()==OBJECT_UNIT||tObject->getType()==OBJECT_PLAYER);
		const bool targetIsCritter = (!targetIsUnit && tObject->getType()==OBJECT_CRITTER);
		if(targetIsUnit)mTargetIndicatorNode->setPosition(mCurrentObject->getPosition()+Vector3(0,mCurrentObject->getScale().y*5,0));
		else if(targetIsCritter) mTargetIndicatorNode->setPosition(static_cast<MagixCritter*>(tObject)->getPosition(true));
		else mTargetIndicatorNode->setPosition(mCurrentObject->getPosition());
		if(mTargetIndicator->getBillboardType()!=BBT_POINT)mTargetIndicator->setBillboardType(BBT_POINT);
		const Real tDist = mCamera->getPosition().distance(mTargetIndicatorNode->getPosition());
		const Real tScale = tDist>1000?20: (tDist<50?1: (1+(tDist-50)*0.02) );
		mTargetIndicatorNode->setScale(tScale,tScale,tScale);
		if(!mTargetIndicator->isVisible())mTargetIndicator->setVisible(true);
	}
	void updateFader(const FrameEvent &evt)
	{
		#pragma warning(push)
		#pragma warning(disable : 4482)
		//fade in
		if(faderTimeout>0)
		{
			faderTimeout -= evt.timeSinceLastFrame;
			if(faderTimeout<=0)
			{
				faderTimeout = 0;
				showFaderOverlay(false);
			}
			else
			{
				if(!mFader->isVisible())showFaderOverlay(true);
				mFader->setCustomParameter(1,Vector4(faderColour.r,faderColour.g,faderColour.b,faderTimeout/faderTimeoutMax));
				if(mFader->getTechnique()->getName()=="2")
				{
					mFader->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
																										LayerBlendSource::LBS_MANUAL,
																										LayerBlendSource::LBS_CURRENT,
																										ColourValue(faderColour.r,faderColour.g,faderColour.b));
					mFader->getTechnique()->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LayerBlendOperationEx::LBX_SOURCE1,
																									LayerBlendSource::LBS_MANUAL,
																									LayerBlendSource::LBS_CURRENT,
																									faderTimeout/faderTimeoutMax);
				}
			}
		}
		//fade out
		else if(faderTimeout<0)
		{
			faderTimeout += evt.timeSinceLastFrame;
			if(faderTimeout>=0)
			{
				faderTimeout = 0;
				if(!mFader->isVisible())showFaderOverlay(true);
				mFader->setCustomParameter(1,Vector4(faderColour.r,faderColour.g,faderColour.b,1));
				if(mFader->getTechnique()->getName()=="2")
				{
					mFader->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
																										LayerBlendSource::LBS_MANUAL,
																										LayerBlendSource::LBS_CURRENT,
																										ColourValue(faderColour.r,faderColour.g,faderColour.b));
					mFader->getTechnique()->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LayerBlendOperationEx::LBX_SOURCE1,
																									LayerBlendSource::LBS_MANUAL,
																									LayerBlendSource::LBS_CURRENT,
																									1);
				}
			}
			else
			{
				if(!mFader->isVisible())showFaderOverlay(true);
				mFader->setCustomParameter(1,Vector4(faderColour.r,faderColour.g,faderColour.b,(faderTimeoutMax+faderTimeout)/faderTimeoutMax));
				if(mFader->getTechnique()->getName()=="2")
				{
					mFader->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
																										LayerBlendSource::LBS_MANUAL,
																										LayerBlendSource::LBS_CURRENT,
																										ColourValue(faderColour.r,faderColour.g,faderColour.b));
					mFader->getTechnique()->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LayerBlendOperationEx::LBX_SOURCE1,
																									LayerBlendSource::LBS_MANUAL,
																									LayerBlendSource::LBS_CURRENT,
																									(faderTimeoutMax+faderTimeout)/faderTimeoutMax);
				}
			}
		}
		#pragma warning(pop)
	}
	void updateCinematicBars(const FrameEvent &evt)
	{
		//fade in
		if(cinematicBarsTimeout>0)
		{
			cinematicBarsTimeout -= evt.timeSinceLastFrame;
			if(cinematicBarsTimeout<=0)
			{
				cinematicBarsTimeout = 0;
			}
			mBackdrop->setDimensions(1.5,1 + 0.5*(cinematicBarsTimeout/2));
			mBackdrop->setPosition(-0.25,-0.25*(cinematicBarsTimeout/2));
		}
		//fade out
		else if(cinematicBarsTimeout<0)
		{
			cinematicBarsTimeout += evt.timeSinceLastFrame;
			if(cinematicBarsTimeout>=0)
			{
				cinematicBarsTimeout = 0;
				showBackdropOverlay(false);
			}
			else
			{
				mBackdrop->setDimensions(1.5,1 + 0.5*((2+cinematicBarsTimeout)/2));
				mBackdrop->setPosition(-0.25,-0.25*((2+cinematicBarsTimeout)/2));
			}
		}
	}
	void updateCinematicCaption(const FrameEvent &evt)
	{
		cinematicCaptionTimeout -= evt.timeSinceLastFrame;
		if(cinematicCaptionTimeout<=0)
		{
			cinematicCaptionTimeout = 0;
			OverlayManager::getSingleton().getOverlayElement("GUI/CinematicCaption")->hide();
		}
	}
	void updateHPPanel(const FrameEvent &evt)
	{
		Real tHPRatio = mUnitManager->getPlayer()->getHPRatio();
		mHPBar->setWidth(0.25 * tHPRatio);
		mHPBar->setLeft(0.25*(1-tHPRatio));
		if(pickupTextTimeout>0)
		{
			pickupTextTimeout -= evt.timeSinceLastFrame;
			if(pickupTextTimeout<=0)
			{
				pickupTextTimeout = 0;
				mPickupText->hide();
			}
			else
			{
				if(!mPickupText->isVisible())mPickupText->show();
				mPickupText->setColour(ColourValue(1,1,0,pickupTextTimeout/2));
			}
		}
	}
	void updateTargetPanel(const FrameEvent &evt)
	{
		if(!isGUIVisible)return;

		MagixLiving *tTarget = mUnitManager->getPlayerTarget();
		if(tTarget)
		{
			short tHP = tTarget->getHP();
			if(tHP>5500)
			{
				for(int i=1;i<=10;i++)
				{
					OverlayManager::getSingleton().getOverlayElement("GUI/TargetHPBar"+StringConverter::toString(i))->show();
				}
				while(tHP>500)
				{
					tHP -= 500;
				}
			}
			else
			{
				for(int i=1;i<=10;i++)
				{
					if(tHP>500)
					{
						tHP -= 500;
						OverlayManager::getSingleton().getOverlayElement("GUI/TargetHPBar"+StringConverter::toString(i))->show();
					}
					else OverlayManager::getSingleton().getOverlayElement("GUI/TargetHPBar"+StringConverter::toString(i))->hide();
				}
			}
			mTargetHP->setWidth(0.25 * Real(tHP)/500);
			mTargetHP->setLeft(0.025 + 0.25*(1-Real(tHP)/500));
			if(!mTargetHP->isVisible())mTargetHP->show();

			if(mUnitManager->popPlayerTargetChanged() && (tTarget->getType()==OBJECT_UNIT||tTarget->getType()==OBJECT_CRITTER))
			{
				if(tTarget->getType()==OBJECT_UNIT)
					OverlayManager::getSingleton().getOverlayElement("GUI/TargetText")->setCaption(static_cast<MagixUnit*>(tTarget)->getName());
				else if(tTarget->getType()==OBJECT_CRITTER)
					OverlayManager::getSingleton().getOverlayElement("GUI/TargetText")->setCaption(static_cast<MagixCritter*>(tTarget)->getCritterType());
				showTargetPanelOverlay(true);
			}
		}
		else if(mUnitManager->popPlayerTargetChanged())
		{
			showTargetPanelOverlay(false);
		}
	}
	void updateLogo(const FrameEvent &evt)
	{
		if(mGameStateManager->getGameState()==GAMESTATE_LOGO)
		{
			if(logoCount==0)
			{
				mBackdrop->setMaterialName("GUIMat/Logo");
				mBackdrop->setDimensions(1,1);
				mBackdrop->setPosition(0,0);
				fadeIn(3);
				logoCount += evt.timeSinceLastFrame;
			}
			else if(logoCount>9 && logoCount<12)
			{
				if(faderTimeout==0)fadeOut(3);
				logoCount += evt.timeSinceLastFrame;
			}
			else if(logoCount>=12)
			{
				mGameStateManager->setGameState(GAMESTATE_STARTSCREEN);
				logoCount = 0;
			}
			else logoCount += evt.timeSinceLastFrame;
			
			return;
		}
		if(mGameStateManager->getGameState()==GAMESTATE_STARTSCREEN && logoCount<=5)
		{
			if(logoCount==0)
			{
				showFaderOverlay(false);
				showStartScreenOverlay(true);
				mButton[BUTTON_NEWGAME]->hide();
				mButton[BUTTON_MULTIPLAYER]->hide();
				mButton[BUTTON_QUIT]->hide();
				OverlayManager::getSingleton().getOverlayElement("GUI/UpdateBox")->hide();
				OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenTitle")->setDimensions(8,1.2);
				OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenTitle")->setPosition(-3.5,-0.29);
				mBackdrop->setMaterialName("GUIMat/Backdrop1");
			}
			logoCount += evt.timeSinceLastFrame;
			if(logoCount>5)logoCount = 5;
			if(logoCount<=5)
			{
				const Real tRatio = 1-(logoCount>5?5:logoCount)/5;
				OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenTitle")->setDimensions(0.8+0.8*9*tRatio,0.12+0.12*9*tRatio);
				OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenTitle")->setPosition(0.5 - (0.8+0.8*9*tRatio)/2, 0.25 - (0.12+0.12*9*tRatio)/2);
			}
			if(logoCount>=5)
			{
				showCursorOverlay(true);
				mCursor->show();
				mButton[BUTTON_NEWGAME]->show();
				mButton[BUTTON_MULTIPLAYER]->show();
				mButton[BUTTON_QUIT]->show();
				OverlayManager::getSingleton().getOverlayElement("GUI/UpdateBox")->show();
				logoCount = 6;
			}
			return;
		}
	}
	bool skipLogo()
	{
		if(mGameStateManager)
		{
			if(mGameStateManager->getGameState()==GAMESTATE_LOGO)
			{
				fadeIn(0);
				mGameStateManager->setGameState(GAMESTATE_STARTSCREEN);
				logoCount = 0;
				return true;
			}
			if(mGameStateManager->getGameState()==GAMESTATE_STARTSCREEN && logoCount<5)
			{
				/*OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenTitle")->setDimensions(0.8,0.12);
				OverlayManager::getSingleton().getOverlayElement("GUI/StartScreenTitle")->setPosition(0.5 - 0.8*0.5, 0.25 - 0.12*0.5);
				showCursorOverlay(true);
				mButton[BUTTON_NEWGAME]->show();
				mButton[BUTTON_MULTIPLAYER]->show();
				mButton[BUTTON_QUIT]->show();
				OverlayManager::getSingleton().getOverlayElement("GUI/UpdateBox")->show();*/
				logoCount = 5;
				return true;
			}
		}
		return false;
	}
	void resetLogo()
	{
		showCursorOverlay(false);
		showOptionsOverlay(false);
		showHPPanelOverlay(false);
		showTargetPanelOverlay(false);
		showChatOverlay(false);
		logoCount = 0;
		showFaderOverlay(true);
		showBackdropOverlay(true);
	}
	void updateHomeBox()
	{
		if(!mBox[GUI_HOMEBOX]->isVisible())return;
		OverlayElement *dimensionText = OverlayManager::getSingleton().getOverlayElement("GUI/DimensionText2");
		if(mInputManager->getInputBox()==dimensionText && mInputManager->getInputMode()==INPUT_TEXT)return;
		const String tCaption = StringConverter::toString(mGameStateManager->getDimension());
		if(dimensionText->getCaption()!=tCaption)dimensionText->setCaption(tCaption);
	}
	void updateFriendBox()
	{
		if(!mBox[GUI_FRIENDBOX]->isVisible())return;
		//New update, refresh list and scroll back up to top
		if(mNetworkManager->popPlayerListUpdated())
		{
			refreshFriendList();
			if(scrollerHeldID!=SCROLLER_FRIENDBOX)setScroll(SCROLLER_FRIENDBOX,0);
		}
		updateBoxScrollerCaption(GUI_FRIENDBOX,SCROLLER_FRIENDBOX,friendBoxList);
	}
	void updateBioBox()
	{
		if(!mBox[GUI_BIOBOX]->isVisible())return;
		if(mInputManager->getInputBox()!=mBoxText[GUI_BIOBOX] || mInputManager->getInputMode()!=INPUT_TEXT)updateBoxScrollerCaption(GUI_BIOBOX,SCROLLER_BIOBOX,bioList);
	}
	void updateTargetBioBox()
	{
		if(!mBox[GUI_TARGETBIOBOX]->isVisible())return;
		updateBoxScrollerCaption(GUI_TARGETBIOBOX,SCROLLER_TARGETBIOBOX,targetBioList);

		const String tBio = mNetworkManager->popTargetBioUpdated();
		if(tBio!="")refreshTargetBio(tBio);
	}
	void updateEmoteBox()
	{
		if(!mBox[GUI_EMOTEBOX]->isVisible())return;
		vector<String>::type tList;
		tList.push_back("ACTIONS:");
		if(mUnitManager->getPlayer()->isLaying()||mUnitManager->getPlayer()->isPlopped())tList.push_back("Plop Down");
		else if(mUnitManager->getPlayer()->isSitting())tList.push_back("Lay");
		else tList.push_back("Sit");
		if(mUnitManager->getPlayer()->isLeaning()||mUnitManager->getPlayer()->isRolledOver())tList.push_back("Roll over");
		else if(mUnitManager->getPlayer()->isSideLaying())tList.push_back("Lay Down");
		else if(mUnitManager->getPlayer()->isSitting())tList.push_back("Side Lay");
		else tList.push_back("Sit");
		tList.push_back(mUnitManager->getPlayer()->getIsCrouching()?"Stop Crouch":"Crouch");
		tList.push_back("Point");
		tList.push_back("Stretch");
		tList.push_back("Headswing");
		tList.push_back("Headbang");
		tList.push_back("Buttswing");
		tList.push_back("Wingwave");
		tList.push_back("Moonwalk");
		tList.push_back("Thriller");
		tList.push_back("Rofl");
		tList.push_back("Roar");
		tList.push_back("Curl");
		tList.push_back("Faint");
		tList.push_back("");

		tList.push_back("SECONDARY ACTIONS:");
		tList.push_back("Nod Head");
		tList.push_back("Shake Head");
		tList.push_back("Nod Head (Slow)");
		tList.push_back("Shake Head (Slow)");
		tList.push_back("Head Tilt");
		tList.push_back(mUnitManager->getPlayer()->isLick()?"Stop Lick":"Lick");
		tList.push_back(mUnitManager->getPlayer()->isNuzzle()?"Stop Nuzzle":"Nuzzle");
		tList.push_back("Sniff");
		tList.push_back("Tail Flick");
		tList.push_back("Laugh");
		tList.push_back("Chuckle");
		tList.push_back("");

		tList.push_back("EMOTES:");
		for(int i=0;i<(int)mDef->emoteName.size();i++)tList.push_back(mDef->emoteName[i]);
		updateBoxScrollerCaption(GUI_EMOTEBOX,SCROLLER_EMOTEBOX,tList);
	}
	void updateItemBox()
	{
		if(!mBox[GUI_ITEMBOX]->isVisible())return;
		String tItemList = "";
		for(int i=0;i<MAX_EQUIP;i++)
		{
			const String tItem = mUnitManager->getPlayer()->getEquip(i);
			if(tItem=="")tItemList += "None\n";
			else tItemList += mDef->getItemName(tItem)+"\n";
		}
		mBoxText[GUI_ITEMBOX]->setCaption(tItemList);
	}
	void updateStashBox()
	{
		if(!mBox[GUI_STASHBOX]->isVisible())return;
		const vector<const String>::type tStash = mItemManager->getStash();
		vector<String>::type tList;
		for(int i=0;i<(int)tStash.size();i++)
		{
			tList.push_back(mDef->getItemName(tStash[i]));
		}
		updateBoxScrollerCaption(GUI_STASHBOX,SCROLLER_STASHBOX,tList);
	}
	void updatePartyBox()
	{
		if(mUnitManager->popPartyChanged())
		{
			//Update Party List
			String tPartyList = "";
			const vector<const std::pair<OwnerToken,String>>::type tParty = mUnitManager->getPartyMembers();
			for(int i=0;i<(int)tParty.size();i++)
			{
				tPartyList += tParty[i].second+"\n";
			}
			mBoxText[GUI_PARTYBOX]->setCaption(tPartyList);
			if(mUnitManager->hasParty())
			{
				mButton[BUTTON_PARTYBOX1]->show();
			}
			else
			{
				mButton[BUTTON_PARTYBOX1]->hide();
			}
			//Update MiniMap as well
			const vector<MagixIndexedUnit*>::type tUnitList = mUnitManager->getUnitList();
			for(int i=0;i<(int)tUnitList.size();i++)mMiniMap->updateUnitMarker(tUnitList[i]);
		}
	}
	void updateSelectPanel(const FrameEvent &evt)
	{
		if(selectPanelTimeout>0)
		{
			selectPanelTimeout -= evt.timeSinceLastFrame;
			if(selectPanelTimeout<0)selectPanelTimeout = 0;

			//Grow panel
			const Real tRatio = (0.25-selectPanelTimeout)/0.25;
			mSelectPanel->setPosition(mSelectPanel->getLeft() + mSelectPanel->getWidth()*0.5,mSelectPanel->getTop() + mSelectPanel->getHeight()*0.5);
			mSelectPanel->setDimensions(0.15*tRatio, 0.2*tRatio);
			mSelectPanel->setPosition(mSelectPanel->getLeft() - mSelectPanel->getWidth()*0.5,mSelectPanel->getTop() - mSelectPanel->getHeight()*0.5);
			if(mSelectPanel->getLeft()+mSelectPanel->getWidth()>1)mSelectPanel->setLeft(1-mSelectPanel->getWidth());
			if(mSelectPanel->getTop()+mSelectPanel->getHeight()>1)mSelectPanel->setTop(1-mSelectPanel->getHeight());
			if(mSelectPanel->getLeft()<0)mSelectPanel->setLeft(0);
			if(mSelectPanel->getTop()<0)mSelectPanel->setTop(0);

			if(selectPanelTimeout!=0)return;

			//Show buttons
			mBox[GUI_SELECTBOX]->show();
			mBox[GUI_SELECTBOX]->setPosition(mSelectPanel->getLeft(),mSelectPanel->getTop());

			vector<unsigned short>::type tButton;
			tButton.clear();
			if(mButtonText[BUTTON_SELECTBOX1]->getCaption()=="")mButton[BUTTON_SELECTBOX1]->hide();
			else
			{
				mButton[BUTTON_SELECTBOX1]->show();
				tButton.push_back(BUTTON_SELECTBOX1);
			}
			if(mButtonText[BUTTON_SELECTBOX2]->getCaption()=="")mButton[BUTTON_SELECTBOX2]->hide();
			else
			{
				mButton[BUTTON_SELECTBOX2]->show();
				tButton.push_back(BUTTON_SELECTBOX2);
			}
			if(mButtonText[BUTTON_SELECTBOX3]->getCaption()=="")mButton[BUTTON_SELECTBOX3]->hide();
			else
			{
				mButton[BUTTON_SELECTBOX3]->show();
				tButton.push_back(BUTTON_SELECTBOX3);
			}
			if(mButtonText[BUTTON_SELECTBOX4]->getCaption()=="")mButton[BUTTON_SELECTBOX4]->hide();
			else
			{
				mButton[BUTTON_SELECTBOX4]->show();
				tButton.push_back(BUTTON_SELECTBOX4);
			}

			switch((const int)tButton.size())
			{
			case 1:
				mButton[tButton[0]]->setPosition(0.025,0);
				break;
			case 2:
				mButton[tButton[0]]->setPosition(0.025,0);
				mButton[tButton[1]]->setPosition(0.025,0.16);
				break;
			case 3:
				mButton[tButton[0]]->setPosition(0.025,0);
				mButton[tButton[1]]->setPosition(-0.05,0.12);
				mButton[tButton[2]]->setPosition(0.1,0.12);
				break;
			case 4:
				mButton[tButton[0]]->setPosition(-0.05,0.02);
				mButton[tButton[1]]->setPosition(0.1,0.02);
				mButton[tButton[2]]->setPosition(-0.05,0.14);
				mButton[tButton[3]]->setPosition(0.1,0.14);
				break;
			default:
				break;
			}

		}
	}
	void updateListSelectBox()
	{
		if(!mBox[GUI_LISTSELECTBOX]->isVisible() && !mListSelectHighlight->isVisible())return;
		if(!mListSelectTarget)showListSelectBox(false);
		if(!mListSelectTarget->isVisible())showListSelectBox(false);
	}
	void refreshFriendList()
	{
		friendBoxList.clear();
		friendBoxListButton.clear();

		//Online Friends
		friendBoxList.push_back("ONLINE FRIENDS:");
		friendBoxListButton.push_back(LISTBUTTON_NONE);
		const vector<String>::type tFriendList = mDef->loadFriendList();
		vector<String>::type tOfflineList;
		tOfflineList.clear();

		for(int i=0;i<(int)tFriendList.size();i++)
		{
			if(mNetworkManager->isPlayerOnline(tFriendList[i]))
			{
				friendBoxList.push_back(tFriendList[i]);
				friendBoxListButton.push_back(LISTBUTTON_ONLINEFRIEND);
			}
			else tOfflineList.push_back(tFriendList[i]);
		}
		if(tFriendList.size()==tOfflineList.size())
		{
			friendBoxList.push_back("None");
			friendBoxListButton.push_back(LISTBUTTON_NONE);
		}
		friendBoxList.push_back("");
		friendBoxListButton.push_back(LISTBUTTON_NONE);

		//Offline Friends
		friendBoxList.push_back("OFFLINE FRIENDS:");
		friendBoxListButton.push_back(LISTBUTTON_NONE);
		for(int i=0;i<(int)tOfflineList.size();i++)
		{
			friendBoxList.push_back(tOfflineList[i]);
			friendBoxListButton.push_back(LISTBUTTON_OFFLINEFRIEND);
		}
		if(tOfflineList.size()==0)
		{
			friendBoxList.push_back("None");
			friendBoxListButton.push_back(LISTBUTTON_NONE);
		}
		friendBoxList.push_back("");
		friendBoxListButton.push_back(LISTBUTTON_NONE);

		//Players online
		friendBoxList.push_back("ONLINE PLAYERS:");
		friendBoxListButton.push_back(LISTBUTTON_NONE);
		const vector<const String>::type tOnlineList = mNetworkManager->getPlayersOnline();
		for(int i=0;i<(int)tOnlineList.size();i++)
		{
			friendBoxList.push_back(tOnlineList[i]);
			friendBoxListButton.push_back(LISTBUTTON_NONE);
		}
		if(tOnlineList.size()==0)
		{
			friendBoxList.push_back("None");
			friendBoxListButton.push_back(LISTBUTTON_NONE);
		}
		friendBoxList.push_back("");
		friendBoxListButton.push_back(LISTBUTTON_NONE);

		//Blocked list
		friendBoxList.push_back("BLOCKED LIST:");
		friendBoxListButton.push_back(LISTBUTTON_NONE);
		const vector<String>::type tBlockedList = mDef->loadBlockedList();
		for(int i=0;i<(int)tBlockedList.size();i++)
		{
			friendBoxList.push_back(tBlockedList[i]);
			friendBoxListButton.push_back(LISTBUTTON_BLOCKED);
		}
		if(tBlockedList.size()==0)
		{
			friendBoxList.push_back("None");
			friendBoxListButton.push_back(LISTBUTTON_NONE);
		}
	}
	void refreshBio()
	{
		const String tName = mUnitManager->getPlayer()->getName();
		OverlayManager::getSingleton().getOverlayElement("GUI/BioBoxName")->setCaption(tName);
		String tBio = mDef->loadBio(tName);
		if(tBio=="")tBio = "None";
		else mInputManager->normalizeText(tBio,mBoxText[GUI_BIOBOX]);
		bioList.clear();
		String tLine = "";
		for(int i=0;i<(int)tBio.length();i++)
		{
			if(tBio[i]=='\n')
			{
				bioList.push_back(tLine);
				tLine = "";
			}
			else tLine += tBio[i];
		}
		if(tLine!="")bioList.push_back(tLine);
	}
	void refreshTargetBio(String bio, const String &name="")
	{
		if(name!="")OverlayManager::getSingleton().getOverlayElement("GUI/TargetBioBoxName")->setCaption(name);

		if(bio=="")bio = "None";
		else mInputManager->normalizeText(bio,mBoxText[GUI_TARGETBIOBOX]);
		targetBioList.clear();
		String tLine = "";
		for(int i=0;i<(int)bio.length();i++)
		{
			if(bio[i]=='\n')
			{
				targetBioList.push_back(tLine);
				tLine = "";
			}
			else tLine += bio[i];
		}
		if(tLine!="")targetBioList.push_back(tLine);
	}
	void resetCampaignScreen()
	{
		showOptionsOverlay(false);
		showGUI(false);
		showFaderOverlay(false);
		faderTimeout = 0;
		showStartScreenOverlay(false);
		showCreditsOverlay(false);
		mBackdrop->setMaterialName("GUIMat/Backdrop1");
		mBackdrop->setDimensions(1,1);
		mBackdrop->setPosition(0,0);
		showBackdropOverlay(true);
		showCampaignOverlay(true);
		mSoundManager->playMusic(MUSIC_TITLE);
	}
	void updateCharScreen(const FrameEvent &evt)
	{
		if(mGameStateManager->getGameState()==GAMESTATE_CHARSCREEN)
		{
			if(mCharScreenManager->popIsCharIDChanged())
			{
				if(mCharScreenManager->getCharID()!=mCharScreenManager->getNumChars())loadChar();
				else showBackdropOverlay(true,false);
			}
			//if true, start game transition has ended, so start game
			if(mCharScreenManager->update(evt,cursorX,cursorY))
			{
				mNetworkManager->disconnect();
				mUnitManager->createNameTag(mUnitManager->getPlayer(),mCharScreenManager->getName());
				mUnitManager->createUserTag(mUnitManager->getPlayer(),logonUsername);
				mGameStateManager->setGameState(GAMESTATE_STARTCONNECTION);
				showOptionsOverlay(false);
			}
		}
	}
	void updateChatBox(const FrameEvent &evt)
	{
		if(!mBox[GUI_TEXTOUTPUT]->isVisible() || !mBoxText[GUI_TEXTOUTPUT]->isVisible())return;
		//if there's a new line, scroll down to 1
		if(mChatManager->popHasNewLine(mChatManager->getChannel()) && scrollerHeldID!=SCROLLER_TEXTOUTPUT)setScroll(SCROLLER_TEXTOUTPUT,1);
		//if other channel has new line, do blinky
		if(doChannelBlink && (mHoverButton!=mButton[BUTTON_TEXTINPUT3]||!leftClick))
		{
			bool tHasNewLine = false;
			const vector<const unsigned char>::type tChannels = mChatManager->getOtherChannels();
			for(int i=0;i<(int)tChannels.size();i++)
				if(mChatManager->getHasNewLine(tChannels[i]))
				{
					tHasNewLine = true;
					break;
				}
			if(tHasNewLine)
			{
				buttonBlinkTimeout -= evt.timeSinceLastFrame;
				if(buttonBlinkTimeout<=0)
				{
					buttonBlinkTimeout = 1;
					mButton[BUTTON_TEXTINPUT3]->setMaterialName( (mButton[BUTTON_TEXTINPUT3]->getMaterialName()!="GUIMat/ButtonHighlight"?"GUIMat/ButtonHighlight":(mHoverButton==mButton[BUTTON_TEXTINPUT3]?"GUIMat/ButtonOver":"GUIMat/ButtonUp")) );
				}
			}
		}

		const Real tCharHeight = StringConverter::parseReal(mBoxText[GUI_TEXTOUTPUT]->getParameter("char_height"));
		if(tCharHeight==0)return;
		const Real tLines = mBox[GUI_TEXTOUTPUT]->getHeight()/tCharHeight - 1;

		mBoxText[GUI_TEXTOUTPUT]->setCaption( mChatManager->getChatBlock( Math::IFloor(tLines),mBoxText[GUI_TEXTOUTPUT]->getWidth(),tCharHeight,(1-getScroll(SCROLLER_TEXTOUTPUT)) ) );
		mBoxText[GUI_TEXTOUTPUT]->setTop((tLines - Math::IFloor(tLines))*tCharHeight + 0.008);
	}
	void updateHover()
	{
		if(mHoverWindow)
			if(!mHoverWindow->isVisible())mHoverWindow = 0;
		if(!mHoverWindow)
		{
			for(int i=0;i<MAX_BOXES;i++)
			{
				if(mBox[i])
				if(isCursorOverWindow(mBox[i]))
				{
					mHoverWindow = mBox[i];
					break;
				}
				else if(mBox[i]==mHoverWindow)mHoverWindow = 0;
			}
		}
		//Priority boxes
		else
		{
			if(isCursorOverWindow(mBox[GUI_OPTIONS]))mHoverWindow = mBox[GUI_OPTIONS];
			else if(isCursorOverWindow(mBox[GUI_SETTINGS]))mHoverWindow = mBox[GUI_SETTINGS];
			else if(isCursorOverWindow(mBox[GUI_INTERFACE]))mHoverWindow = mBox[GUI_INTERFACE];
		}
		for(int i=0;i<MAX_BUTTONS;i++)
		{
			if(mButton[i])
			if(isCursorOverButton(mButton[i]) && (mButton[i]->getParent()==mHoverWindow||!mHoverWindow))
			{
				//special "hold to click" buttons
				if(mHoverButton!=mButton[i] && leftClick && mButton[i]->getParent()==mHeldButtonWindow)
				{
					mHoverButton = mButton[i];
					mSoundManager->playGUISound(SOUND_CLICK);
					hoverButtonType = GUITYPE_BUTTON;
				}
				//Hover
				else if(!mHoverButton)
				{
					mButton[i]->setMaterialName("GUIMat/ButtonOver");
					//not held before hovering
					if(!leftClick)
					{
						mHoverButton = mButton[i];
						hoverButtonType = GUITYPE_BUTTON;
					}
				}
				//for cases where: hovered, held, unhovered, hovered again
				else if(mHoverButton==mButton[i] && leftClick)
				{
					mButton[i]->setMaterialName("GUIMat/ButtonDown");
				}
			}
			else 
			{
				//Unhover
				if(mButton[i]->getMaterialName()!="GUIMat/ButtonUp" && mButton[i]->getMaterialName()!="GUIMat/ButtonHighlight")mButton[i]->setMaterialName("GUIMat/ButtonUp");
				//unhovered and not held
				if(mHoverButton==mButton[i] && !leftClick)mHoverButton = 0;
				//special "hold to click" buttons
				if(mButton[i]==mHoverButton && mButton[i]->getParent()==mHeldButtonWindow)mHoverButton = 0;
			}
		}
		for(int i=0;i<MAX_SIZERS;i++)
		{
			if(mSizer[i])
			if(isCursorOverButton(mSizer[i]) && (mSizer[i]->getParent()==mHoverWindow||!mHoverWindow))
			{
				//Hover
				if(!mHoverButton)
				{
					mSizer[i]->setMaterialName("GUIMat/ButtonOver");
					//not held before hovering
					if(!leftClick)
					{
						mHoverButton = mSizer[i];
						hoverButtonType = GUITYPE_SIZER;
					}
				}
				//for cases where: hovered, held, unhovered, hovered again
				else if(mHoverButton==mSizer[i] && leftClick)
				{
					mSizer[i]->setMaterialName("GUIMat/ButtonDown");
				}
			}
			else 
			{
				//Unhover
				if(mSizer[i]->getMaterialName()!="GUIMat/ButtonUp")mSizer[i]->setMaterialName("GUIMat/ButtonUp");
				//unhovered and not held
				if(mHoverButton==mSizer[i] && !leftClick)mHoverButton = 0;
			}
		}
		for(int i=0;i<MAX_MOVERS;i++)
		{
			if(mMover[i])
			if(isCursorOverButton(mMover[i]) && (!mHoverWindow||mHoverWindow->getZOrder()<=mMover[i]->getZOrder()))
			{
				//Hover
				if(!mHoverButton)
				{
					mMover[i]->setMaterialName("GUIMat/ButtonOver");
					//not held before hovering
					if(!leftClick)
					{
						mHoverButton = mMover[i];
						hoverButtonType = GUITYPE_MOVER;
					}
				}
				//for cases where: hovered, held, unhovered, hovered again
				else if(mHoverButton==mMover[i] && leftClick)
				{
					mMover[i]->setMaterialName("GUIMat/ButtonDown");
				}
			}
			else 
			{
				//Unhover
				if(mMover[i]->getMaterialName()!="GUIMat/ButtonUp")mMover[i]->setMaterialName("GUIMat/ButtonUp");
				//unhovered and not held
				if(mHoverButton==mMover[i] && !leftClick)mHoverButton = 0;
			}
		}
		for(int i=0;i<MAX_SCROLLERS;i++)
		{
			if(mScroller[i])
			if((isCursorOverButton(mScroller[i])||isCursorOverButton(mScrollerMarker[i])) && (mScrollerMarker[i]->getParent()==mHoverWindow||!mHoverWindow))
			{
				//Hover
				if(!mHoverButton)
				{
					mScrollerMarker[i]->setMaterialName("GUIMat/ButtonOver");
					//not held before hovering
					if(!leftClick)
					{
						mHoverButton = mScrollerMarker[i];
						hoverButtonType = GUITYPE_SCROLLER;
					}
				}
				//for cases where: hovered, held, unhovered, hovered again
				else if(mHoverButton==mScrollerMarker[i] && leftClick)
				{
					mScrollerMarker[i]->setMaterialName("GUIMat/ButtonDown");
				}
			}
			else 
			{
				//Unhover
				if(mScrollerMarker[i]->getMaterialName()!="GUIMat/ButtonUp" && !leftClick)mScrollerMarker[i]->setMaterialName("GUIMat/ButtonUp");
				//unhovered and not held
				if(mHoverButton==mScrollerMarker[i] && !leftClick)mHoverButton = 0;
			}
		}
		for(int i=0;i<MAX_CHECKBOXES;i++)
		{
			if(mCheckBox[i])
			if(isCursorOverButton(mCheckBox[i]) && (mCheckBox[i]->getParent()==mHoverWindow||!mHoverWindow))
			{
				//Hover
				if(!mHoverButton)
				{
					//not held before hovering
					if(!leftClick)
					{
						mHoverButton = mCheckBox[i];
						hoverButtonType = GUITYPE_CHECKBOX;
					}
				}
			}
			else 
			{
				//unhovered and not held
				if(mHoverButton==mCheckBox[i] && !leftClick)mHoverButton = 0;
			}
		}
	}
	bool isCursorOverButton(OverlayElement *button, bool ignoreParent=false)
	{
		if(!button)return false;
		if(!mCursor->isVisible())return false;
		if(!button->isVisible())return false;

		if(ignoreParent)
		{
			return (cursorX >= button->getLeft() && cursorX <= button->getLeft()+button->getWidth()
				&& cursorY >= button->getTop() && cursorY <= button->getTop()+button->getHeight() );
		}

		if(!button->getParent())return false;
		if(!button->getParent()->isVisible())return false;
		return (cursorX >= (button->getLeft()+button->getParent()->getLeft()) && cursorX <= (button->getLeft()+button->getWidth()+button->getParent()->getLeft())
				&& cursorY >= (button->getTop()+button->getParent()->getTop()) && cursorY <= (button->getTop()+button->getHeight()+button->getParent()->getTop()) );
	}
	bool isCursorOverWindow(OverlayElement *window)
	{
		if(!window)return false;
		if(!mCursor->isVisible())return false;
		if(!window->isVisible())return false;
		return (cursorX >= (window->getLeft()) && cursorX <= (window->getLeft()+window->getWidth())
				&& cursorY >= (window->getTop()) && cursorY <= (window->getTop()+window->getHeight())
				&& window->isVisible() );
	}
	void setLeftClick(bool flag)
	{
		if(!leftClick && flag){mouseLeftDown();}
		else if(leftClick && !flag){mouseLeftUp();}
		leftClick = flag;
	}
	void setRightClick(bool flag)
	{
		if(!rightClick && flag){mouseRightDown();}
		else if(rightClick && !flag){mouseRightUp();}
		rightClick = flag;
	}
	void mouseLeftDown()
	{
		//hide alert box if clicked
		if(isCursorOverWindow(mAlertBox->getAlertBox()))
		{
			mAlertBox->hide();
			return;
		}
		//release input mode on left click (except for chat & chage dimension)
		if(mInputManager->getInputMode()==INPUT_TEXT
			&& mInputManager->getInputBox()!=mBoxText[GUI_TEXTINPUT]
			&& mInputManager->getInputBox()!=OverlayManager::getSingleton().getOverlayElement("GUI/DimensionText2"))
		{
			toggleInputMode(false);
		}
		//redetermine mHoverWindow
		if(mHoverWindow && (!mHoverButton || mHoverButton->getParent()!=mHoverWindow) && !isCursorOverWindow(mHoverWindow))
		{
			mHoverWindow = 0;
			updateHover();
		}
		//hide list select box
		if((mBox[GUI_LISTSELECTBOX]->isVisible() && mHoverWindow!=mBox[GUI_LISTSELECTBOX])||!mBox[GUI_LISTSELECTBOX]->isVisible())showListSelectBox(false);
		

		if(mHoverButton)
		{
			mSoundManager->playGUISound(SOUND_CLICK);
			setCurrentWindow(mHoverButton->getParent());
			if(hoverButtonType==GUITYPE_CHECKBOX)
			{
				toggleCheckBox(mHoverButton);
				return;
			}
			else mHoverButton->setMaterialName("GUIMat/ButtonDown");

			//Sizer actions
			if(mHoverButton==mSizer[SIZER_TEXTOUTPUT])	//Size chatbox
			{
				sizeWindowDrag(SIZEDIR_TOP, 0, 0.06);
				return;
			}

			//Mover actions
			mMoverWindow = getMoverWindow(mHoverButton); //check if it's a mover
			if(mMoverWindow)
			{
				doMove(true);
				return;
			}

			//Scroller actions
			short scrollerID = getScrollerID(mHoverButton); //check if it's a scroller
			if(scrollerID!=-1)doScroll(true,scrollerID);

			return;
		}

		if(mHoverWindow)
		{
			setCurrentWindow(mHoverWindow);
			//Window actions
			if(mHoverWindow==mBox[GUI_TEXTINPUT])	//chatbox
			{
				if(mInputManager->getInputMode()==INPUT_CONTROL)toggleInputMode(false);
				return;
			}
			if(mHoverWindow==mBox[GUI_CHARSCREENBOX])	//enter character name
			{
				if(isCursorOverButton(mBoxText[GUI_CHARSCREENBOX]))
					toggleInputMode(false,mBoxText[GUI_CHARSCREENBOX],"Name: ",15,false,mCharScreenManager->getName(),false);
				return;
			}
			if(mHoverWindow==mBox[GUI_LOGONBOX])	//enter logon username/password
			{
				OverlayElement *usernameText = OverlayManager::getSingleton().getOverlayElement("GUI/LogonUsernameText");
				OverlayElement *passwordText = OverlayManager::getSingleton().getOverlayElement("GUI/LogonPasswordText");
				if(isCursorOverButton(usernameText))
					toggleInputMode(false,usernameText,"Username: ",15,false,logonUsername,false);
				else if(isCursorOverButton(passwordText))
					toggleInputMode(false,passwordText,"Password: ",15,false,logonPassword,false,false,true);
				return;
			}
			if(mHoverWindow==mBox[GUI_CREATEACCOUNTBOX])	//enter new account details
			{
				const String tCaption[MAX_CREATEACCOUNTINFO] = {"Username: ",
																"Password: ",
																"Confirm Password: ",
																"Email: ",
																"Security Question: ",
																"Answer: "};
				const unsigned short tLength[MAX_CREATEACCOUNTINFO] = {15,15,15,60,120,120};
				for(int i=0;i<MAX_CREATEACCOUNTINFO;i++)
				{
					if(isCursorOverButton(mCreateAccountTextBox[i]))
						toggleInputMode(false,mCreateAccountTextBox[i],tCaption[i],tLength[i],false,createAccountInfo[i],false,false,(i==1||i==2));
				}
				return;
			}
			if(mHoverWindow==mBox[GUI_EDITACCOUNTBOX])	//edit account
			{
				const String tCaption[MAX_EDITACCOUNTINFO] = {"Username: ",
																"Old Password: ",
																"New Password: ",
																"Confirm Password: "};
				for(int i=0;i<MAX_EDITACCOUNTINFO;i++)
				{
					if(isCursorOverButton(mEditAccountTextBox[i]))
						toggleInputMode(false,mEditAccountTextBox[i],tCaption[i],15,false,editAccountInfo[i],false,false,i>0);
				}
				return;
			}
			if(mHoverWindow==mBox[GUI_BIOBOX])	//edit bio
			{
				if(isCursorOverButton(mBoxText[GUI_BIOBOX]))
				{
					String tBio = "";
					for(int i=0;i<(int)bioList.size();i++)
					{
						tBio += (i==0?"":"\n") + bioList[i];
					}
					toggleInputMode(false,mBoxText[GUI_BIOBOX],"",511,false,tBio,false,true);
				}
				return;
			}
			if(mHoverWindow==mBox[GUI_FRIENDBOX])	//friendbox options
			{
				Real tFontHeight = 0;
				short tApparentLine = 0;
				const short tButtonLine = getListButtonLine(GUI_FRIENDBOX,SCROLLER_FRIENDBOX,(int)friendBoxList.size(),tFontHeight,tApparentLine);
				if(tButtonLine==-1 || tButtonLine>=(int)friendBoxList.size() || tButtonLine>=(int)friendBoxListButton.size())return;

				if(friendBoxListButton[tButtonLine]==LISTBUTTON_NONE)return;
				//Show selection box
				listButtonData = friendBoxList[tButtonLine];
				listButtonType = friendBoxListButton[tButtonLine];
				mListSelectTarget = mHoverWindow;

				mButtonText[BUTTON_LISTSELECT1]->setCaption("");
				mButtonText[BUTTON_LISTSELECT2]->setCaption("");
				mButtonText[BUTTON_LISTSELECT3]->setCaption("");
				switch(friendBoxListButton[tButtonLine])
				{
				case LISTBUTTON_ONLINEFRIEND:
					mButtonText[BUTTON_LISTSELECT1]->setCaption("PM");
					mButtonText[BUTTON_LISTSELECT2]->setCaption("Where");
					mButtonText[BUTTON_LISTSELECT3]->setCaption("Remove");
					break;
				case LISTBUTTON_OFFLINEFRIEND:
				case LISTBUTTON_BLOCKED:
					mButtonText[BUTTON_LISTSELECT1]->setCaption("Remove");
					break;
				default:
					return;
				}
				showListSelectBox(true);

				unsigned short tNumButtons = 0;
				if(mButtonText[BUTTON_LISTSELECT1]->getCaption()=="")mButton[BUTTON_LISTSELECT1]->hide();
				else
				{
					tNumButtons += 1;
					mButton[BUTTON_LISTSELECT1]->show();
				}
				if(mButtonText[BUTTON_LISTSELECT2]->getCaption()=="")mButton[BUTTON_LISTSELECT2]->hide();
				else
				{
					tNumButtons += 1;
					mButton[BUTTON_LISTSELECT2]->show();
				}
				if(mButtonText[BUTTON_LISTSELECT3]->getCaption()=="")mButton[BUTTON_LISTSELECT3]->hide();
				else
				{
					tNumButtons += 1;
					mButton[BUTTON_LISTSELECT3]->show();
				}
				mBox[GUI_LISTSELECTBOX]->setDimensions(0.1,0.04*tNumButtons);
				mBox[GUI_LISTSELECTBOX]->setPosition(cursorX+0.02,cursorY+0.02);
				mListSelectHighlight->setDimensions(mBox[GUI_FRIENDBOX]->getWidth(),0.02);
				mListSelectHighlight->setPosition(mBox[GUI_FRIENDBOX]->getLeft(),mBox[GUI_FRIENDBOX]->getTop()+tFontHeight*tApparentLine+0.01);
				mListSelectHighlight->show();

				return;
			}
			if(mHoverWindow==mBox[GUI_EMOTEBOX])	//emotebox options
			{
				Real tFontHeight = 0;
				short tApparentLine = 0;
				const short tButtonLine = getListButtonLine(GUI_EMOTEBOX,SCROLLER_EMOTEBOX,(int)mDef->emoteName.size()+MAX_ACTIONS+5,tFontHeight,tApparentLine);
				if(tButtonLine==-1 || tButtonLine>=(int)mDef->emoteName.size()+MAX_ACTIONS+5)return;
				if(tButtonLine==0 || tButtonLine==MAX_PRIMARYACTIONS+1 || tButtonLine==MAX_PRIMARYACTIONS+2)return;
				if(tButtonLine==MAX_ACTIONS+3 || tButtonLine==MAX_ACTIONS+4)return;

				mListSelectTarget = mHoverWindow;

				//Highlight
				mListSelectHighlight->setDimensions(mBox[GUI_EMOTEBOX]->getWidth(),0.02);
				mListSelectHighlight->setPosition(mBox[GUI_EMOTEBOX]->getLeft(),mBox[GUI_EMOTEBOX]->getTop()+tFontHeight*tApparentLine+0.01);
				mListSelectHighlight->show();
				showListSelectBox(true);
				mBox[GUI_LISTSELECTBOX]->hide();

				if(tButtonLine<MAX_ACTIONS+5)
				{
					//Do action
					switch(tButtonLine)
					{
						case 1: mUnitManager->getPlayer()->doSit(); break;
						case 2: if(mUnitManager->getPlayer()->isSitting()||mUnitManager->getPlayer()->isSideLaying()||mUnitManager->getPlayer()->isLeaning())mUnitManager->getPlayer()->doSideLay();
								else mUnitManager->getPlayer()->doSit();
								break;
						case 3: mUnitManager->getPlayer()->doCrouch(!mUnitManager->getPlayer()->getIsCrouching()); break;

						case 4: mUnitManager->getPlayer()->doPoint(); break;
						case 5: mUnitManager->getPlayer()->doStretch(); break;
						case 6: mUnitManager->getPlayer()->doHeadswing(); break;
						case 7: mUnitManager->getPlayer()->doHeadbang(); break;
						case 8: mUnitManager->getPlayer()->doButtswing(); break;
						case 9: mUnitManager->getPlayer()->doWingwave(); break;
						case 10: mUnitManager->getPlayer()->doMoonwalk(); break;
						case 11: mUnitManager->getPlayer()->doThriller(); break;
						case 12: mUnitManager->getPlayer()->doRofl(); break;
						case 13: mUnitManager->getPlayer()->doRoar(); break;
						case 14: mUnitManager->getPlayer()->doCurl(); break;
						case 15: mUnitManager->getPlayer()->doFaint(); break;

						case 18: mUnitManager->getPlayer()->doNodHead(); break;
						case 19: mUnitManager->getPlayer()->doShakeHead(); break;
						case 20: mUnitManager->getPlayer()->doNodHeadSlow(); break;
						case 21: mUnitManager->getPlayer()->doShakeHeadSlow(); break;
						case 22: mUnitManager->getPlayer()->doHeadTilt(); break;
						case 23: mUnitManager->getPlayer()->toggleLick(); break;
						case 24: mUnitManager->getPlayer()->toggleNuzzle(); break;
						case 25: mUnitManager->getPlayer()->doSniff(); break;
						case 26: mUnitManager->getPlayer()->doTailFlick(); break;
						case 27: mUnitManager->getPlayer()->doLaugh(); break;
						case 28: mUnitManager->getPlayer()->doChuckle(); break;
					}
				}
				else
				{
					//Do emote
					const String tEmote = mDef->emoteName[tButtonLine-(MAX_ACTIONS+5)];
					mUnitManager->getPlayer()->setEmote(tEmote,mDef);
					mNetworkManager->sendEmote(tEmote);
					//Set hotkey
					#pragma warning(push)
					#pragma warning(disable : 4482)
					const int tHotkey = mInputManager->getHeldHotkey();
					if(tHotkey)
						if(tHotkey>=OIS::KeyCode::KC_F5 && tHotkey<=OIS::KeyCode::KC_F10)
						{
							mDef->setHotkeyF(tHotkey-OIS::KeyCode::KC_F5,tEmote);
							mChatManager->message("Hotkey F"+StringConverter::toString(tHotkey-OIS::KeyCode::KC_F5+5)+" set as '"+tEmote+"'");
						}
						else if(tHotkey>=OIS::KeyCode::KC_F11 && tHotkey<=OIS::KeyCode::KC_F12)
						{
							mDef->setHotkeyF(tHotkey-OIS::KeyCode::KC_F11+6,tEmote);
							mChatManager->message("Hotkey F"+StringConverter::toString(tHotkey-OIS::KeyCode::KC_F11+11)+" set as '"+tEmote+"'");
						}
					#pragma warning(pop)
				}

				return;
			}
			if(mHoverWindow==mBox[GUI_ITEMBOX])	//itembox options
			{
				Real tFontHeight = 0;
				short tApparentLine = 0;
				const short tButtonLine = getListButtonLine(GUI_ITEMBOX,tFontHeight,tApparentLine);
				if(tButtonLine==-1 || tButtonLine>=(int)MAX_EQUIP)return;

				mListSelectTarget = mHoverWindow;
				const String tItem = mUnitManager->getPlayer()->getEquip(tButtonLine);
				if(tItem!="")listButtonData = mDef->getItemName(tItem);
				else listButtonData = "";
				listButtonLine = tButtonLine;

				//Highlight
				mListSelectHighlight->setDimensions(mBox[GUI_ITEMBOX]->getWidth(),0.02);
				mListSelectHighlight->setPosition(mBox[GUI_ITEMBOX]->getLeft(),mBox[GUI_ITEMBOX]->getTop()+tFontHeight*tApparentLine+0.01);
				mListSelectHighlight->show();
				showListSelectBox(true);
				
				if(listButtonData!="")
				{
					mButtonText[BUTTON_LISTSELECT1]->setCaption("Stash");
					mButtonText[BUTTON_LISTSELECT2]->setCaption("Drop");
					mButton[BUTTON_LISTSELECT2]->show();
					if(mDef->isAdmin)
					{
						mButtonText[BUTTON_LISTSELECT3]->setCaption("Delete");
						mButton[BUTTON_LISTSELECT3]->show();
					}
					else mButton[BUTTON_LISTSELECT3]->hide();
					mBox[GUI_LISTSELECTBOX]->setDimensions(0.1,0.04*(mButton[BUTTON_LISTSELECT3]->isVisible()?3:2));
					mBox[GUI_LISTSELECTBOX]->setPosition(cursorX+0.02,cursorY+0.02);
				}
				else mBox[GUI_LISTSELECTBOX]->hide();

				return;
			}
			if(mHoverWindow==mBox[GUI_STASHBOX])	//stashbox options
			{
				Real tFontHeight = 0;
				short tApparentLine = 0;
				const vector<const String>::type tList = mItemManager->getStash();
				const short tButtonLine = getListButtonLine(GUI_STASHBOX,SCROLLER_STASHBOX,(int)tList.size(),tFontHeight,tApparentLine);
				if(tButtonLine==-1 || tButtonLine>=(int)tList.size())return;

				//Show selection box
				listButtonData = tList[tButtonLine];
				listButtonLine = tButtonLine;
				mListSelectTarget = mHoverWindow;

				mButtonText[BUTTON_LISTSELECT1]->setCaption("Equip");
				mButton[BUTTON_LISTSELECT2]->hide();
				mButton[BUTTON_LISTSELECT3]->hide();
				mBox[GUI_LISTSELECTBOX]->setDimensions(0.1,0.04);
				mBox[GUI_LISTSELECTBOX]->setPosition(cursorX+0.02,cursorY+0.02);

				//Highlight
				mListSelectHighlight->setDimensions(mBox[GUI_STASHBOX]->getWidth(),0.02);
				mListSelectHighlight->setPosition(mBox[GUI_STASHBOX]->getLeft(),mBox[GUI_STASHBOX]->getTop()+tFontHeight*tApparentLine+0.01);
				mListSelectHighlight->show();
				showListSelectBox(true);

				return;
			}
			//No further actions if clicked on window
			return;
		}

		if(mGameStateManager->isInGame() && !mGameStateManager->isPaused() &&!mGameStateManager->isCinematic())
		{
			//click on target HP box = attack target
			if(mUnitManager->getPlayerTarget() && isCursorOverButton(mTargetPanel,true))
			{
				MagixLiving *target = mUnitManager->getPlayerTarget();
				Skill *tSkill = mUnitManager->getPlayer()->getCurrentSkill();
				const bool tSkillTargetsAllies = (tSkill && mDef->isSkillTargetsAllies(tSkill->name));
				const bool tIsAlly = target->matchAlliance(mUnitManager->getPlayer()->getAlliance());
				if( (tSkillTargetsAllies && tIsAlly) ||
					(!tSkillTargetsAllies && !tIsAlly) )
				doAttack(target);
				else mUnitManager->getPlayer()->setTarget(target->getPosition());
				return;
			}
			doInGameLeftClick();
		}
		else if(mGameStateManager->getGameState()==GAMESTATE_CHARSCREEN)mCharScreenManager->doRotateUnit(cursorX,cursorY);
	}
	void mouseLeftUp()
	{
		if(mSelectPanel->isVisible())
		{
			if(!mHoverButton)showSelectPanel(false);
			else if(mHoverButton->getParent()!=mHeldButtonWindow)showSelectPanel(false);
		}

		if(sizeWindowDir)sizeWindowDrag(0);
		if(mMoverWindow)doMove(false);
		if(scrollerHeldID!=-1)doScroll(false);
		if(mCharScreenManager->isRotatingUnit())mCharScreenManager->endRotateUnit();

		if(mHoverButton)
		if(isCursorOverButton(mHoverButton))
		{
			if(hoverButtonType!=GUITYPE_CHECKBOX)mHoverButton->setMaterialName("GUIMat/ButtonOver");
			else return;
			
			//Button actions
			if(mHoverButton==mButton[BUTTON_QUIT] || mHoverButton==mButton[BUTTON_OPTIONQUIT])	//Quit
			{
				mGameStateManager->setTempGameState(mGameStateManager->getGameState());
				mGameStateManager->setGameState(GAMESTATE_QUIT);
				showOptionsOverlay(false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_NEWGAME])	//new game
			{
				showStartScreenOverlay(false);
				showOptionsOverlay(false);
				loadCampaigns(0);
				showCampaignOverlay(true);
				return;
			}
			if(mHoverButton==mButton[BUTTON_MULTIPLAYER])	//multiplayer game
			{
				showStartScreenOverlay(false);
				showOptionsOverlay(false);
				showLogonOverlay(true);

				logonPassword = mDef->loadPassword();
				if(logonPassword!="" && !isCheckBoxTrue(mCheckBox[CHECKBOX_LOGON]))toggleCheckBox(mCheckBox[CHECKBOX_LOGON]);
				String tPassword = "";
				for(int i=0;i<(int)logonPassword.length();i++)tPassword += "*";
				OverlayManager::getSingleton().getOverlayElement("GUI/LogonPasswordText")->setCaption("Password: "+tPassword);

				if(logonUsername=="")
					toggleInputMode(false,OverlayManager::getSingleton().getOverlayElement("GUI/LogonUsernameText"),"Username: ",15,false,logonUsername,false);
				else
					toggleInputMode(false,OverlayManager::getSingleton().getOverlayElement("GUI/LogonPasswordText"),"Password: ",15,false,logonPassword,false,false,true);
				return;
			}
			if(mHoverButton==mButton[BUTTON_LOGONBACK])	//back to startscreen
			{
				mNetworkManager->disconnect();
				showLogonOverlay(false);
				showOptionsOverlay(false);
				showStartScreenOverlay(true);
				return;
			}
			if(mHoverButton==mButton[BUTTON_LOGON1])	//logon multiplayer
			{
				logOn();
				return;
			}
			if(mHoverButton==mButton[BUTTON_NEWACCOUNT])	//new account screen
			{
				showLogonOverlay(false);
				showOptionsOverlay(false);
				showCreateAccountOverlay(true);
				resetCreateAccountFields();
				return;
			}
			if(mHoverButton==mButton[BUTTON_EDITACCOUNT])	//edit account screen
			{
				showLogonOverlay(false);
				showOptionsOverlay(false);
				showEditAccountOverlay(true);
				return;
			}
			if(mHoverButton==mButton[BUTTON_CREATEACCOUNTBACK])	//back to logon screen
			{
				showCreateAccountOverlay(false);
				showOptionsOverlay(false);
				showLogonOverlay(true);
				return;
			}
			if(mHoverButton==mButton[BUTTON_CREATEACCOUNT1])	//create new account
			{
				createAccount();
				return;
			}
			if(mHoverButton==mButton[BUTTON_EDITACCOUNTBACK])	//back to logon screen
			{
				showEditAccountOverlay(false);
				showOptionsOverlay(false);
				showLogonOverlay(true);
				return;
			}
			if(mHoverButton==mButton[BUTTON_EDITACCOUNT1])	//confirm change password
			{
				confirmPasswordChange();
				return;
			}
			if(mHoverButton==mButton[BUTTON_OPTIONRESUME])	//Options resume
			{
				toggleShowOptions();
				return;
			}
			if(mHoverButton==mButton[BUTTON_OPTIONRESTART])	//Options restart
			{
				if(mGameStateManager->isCampaign())
				{
					mGameStateManager->setGameState(GAMESTATE_QUITCAMPAIGN);
				}
				else
				{
					mNetworkManager->disconnect();
					mGameStateManager->setGameState(GAMESTATE_RETURNSTARTSCREEN);
				}
				return;
			}
			if(mHoverButton==mButton[BUTTON_OPTIONSETTINGS])	//Settings
			{
				mBox[GUI_OPTIONS]->hide();
				mBox[GUI_SETTINGS]->show();
				return;
			}
			if(mHoverButton==mButton[BUTTON_OPTIONINTERFACE])	//Interface Settings
			{
				mBox[GUI_OPTIONS]->hide();
				mBox[GUI_INTERFACE]->show();
				return;
			}
			if(mHoverButton==mButton[BUTTON_CAMPAIGNBACK])	//Campaignscreen Back
			{
				if(campaignPage==0)
				{
					showCampaignOverlay(false);
					showOptionsOverlay(false);
					showStartScreenOverlay(true);
				}
				else loadCampaigns(--campaignPage);
				return;
			}
			if(mHoverButton==mButton[BUTTON_CAMPAIGNNEXT])	//Campaignscreen Next
			{
				loadCampaigns(++campaignPage);
				return;
			}
			if(mHoverButton==mButton[BUTTON_CAMPAIGN1]||mHoverButton==mButton[BUTTON_CAMPAIGN2]||mHoverButton==mButton[BUTTON_CAMPAIGN3])	//Start Campaign
			{
				if(mHoverButton==mButton[BUTTON_CAMPAIGN1]&&(int)campaignList.size()>(campaignPage*3))mGameStateManager->setCampaign(campaignList[(campaignPage*3)]);
				else if(mHoverButton==mButton[BUTTON_CAMPAIGN2]&&(int)campaignList.size()>(1+campaignPage*3))mGameStateManager->setCampaign(campaignList[(1+campaignPage*3)]);
				else if(mHoverButton==mButton[BUTTON_CAMPAIGN3]&&(int)campaignList.size()>(2+campaignPage*3))mGameStateManager->setCampaign(campaignList[(2+campaignPage*3)]);
				mGameStateManager->setGameState(GAMESTATE_STARTCAMPAIGN);
				showCampaignOverlay(false);
				showOptionsOverlay(false);
				showBackdropOverlay(true,true);
				showFaderOverlay(true);
				fadeOut(0);
				return;
			}
			if(mHoverButton==mButton[BUTTON_CHARSCREENBACK])	//Charscreen Back
			{
				mAlertBox->hide(true);
				if(mCharScreenManager->previousPage())				//Retun to start screen
				{
					mNetworkManager->disconnect();
					showCharScreenOverlay(false);
					mGameStateManager->setGameState(GAMESTATE_RETURNSTARTSCREEN);
				}
				else
				{
					Real one=0,two=0,three=0;
					mCharScreenManager->getSliderValue(one,two,three);
					setScroll(SCROLLER_CHARSCREEN1,one);
					setScroll(SCROLLER_CHARSCREEN2,two);
					setScroll(SCROLLER_CHARSCREEN3,three);
				}
				return;
			}
			if(mHoverButton==mButton[BUTTON_CHARSCREENNEXT])	//Charscreen Next
			{
				mAlertBox->hide();
				const unsigned short tFlag = mCharScreenManager->nextPage();
				if(tFlag==CHARSCREEN_STARTGAME)					//Start scene (actual start when transition ends, see update())
				{
					//showServerOverlay(true);
					showCharScreenOverlay(false);
					mCharScreenManager->startGame();
					mAlertBox->hide(true);
					showOptionsOverlay(false);
				}
				else if(tFlag==CHARSCREEN_CREATECHAR)
				{
					showCharScreenOverlay(false);
					mBackdrop->setMaterialName("GUIMat/Backdrop1");
					showBackdropOverlay(true,true,"Creating...");
					mNetworkManager->sendCreateChar(logonUsername,mCharScreenManager->getName(),mCharScreenManager->getCharData());
				}
				else if(tFlag==CHARSCREEN_EDITCHAR)
				{
					showCharScreenOverlay(false);
					mBackdrop->setMaterialName("GUIMat/Backdrop1");
					showBackdropOverlay(true,true,"Saving...");
					mNetworkManager->sendEditChar(logonUsername,mCharScreenManager->getCharID(),mCharScreenManager->getCharData());
				}
				else
				{
					Real one=0,two=0,three=0;
					mCharScreenManager->getSliderValue(one,two,three);
					setScroll(SCROLLER_CHARSCREEN1,one);
					setScroll(SCROLLER_CHARSCREEN2,two);
					setScroll(SCROLLER_CHARSCREEN3,three);
				}
				return;
			}
			if(mHoverButton==mButton[BUTTON_CHARSCREEN_L1]||mHoverButton==mButton[BUTTON_CHARSCREEN_R1]||	//Charscreen buttons
					mHoverButton==mButton[BUTTON_CHARSCREEN_L2]||mHoverButton==mButton[BUTTON_CHARSCREEN_R2]||
					mHoverButton==mButton[BUTTON_CHARSCREEN_L3]||mHoverButton==mButton[BUTTON_CHARSCREEN_R3]||
					mHoverButton==mButton[BUTTON_CHARSCREEN_L4]||mHoverButton==mButton[BUTTON_CHARSCREEN_R4]||
					mHoverButton==mButton[BUTTON_CHARSCREENDELETE])
			{
				const unsigned short tMessage = mCharScreenManager->buttonClicked(mHoverButton);
				if(tMessage==CHARSCREEN_UPDATEGUI)
				{
					Real one=0,two=0,three=0;
					mCharScreenManager->getSliderValue(one,two,three);
					setScroll(SCROLLER_CHARSCREEN1,one);
					setScroll(SCROLLER_CHARSCREEN2,two);
					setScroll(SCROLLER_CHARSCREEN3,three);
				}
				else if(tMessage==CHARSCREEN_DELETECHAR)
				{
					mNetworkManager->sendDeleteChar(logonUsername,mCharScreenManager->getCharID());
					mCharScreenManager->deleteChar();
				}
				return;
			}
			if(mHoverButton==mButton[BUTTON_TEXTINPUT])	//say/send
			{
				toggleInputMode(false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_TEXTINPUT2])	//hide entire chatbox
			{
				if(mBoxText[GUI_TEXTINPUT]->isVisible())
				{
					showChatBox(false);
				}
				else
				{
					showChatBox(true);
				}
				return;
			}
			if(mHoverButton==mButton[BUTTON_TEXTOUTPUT])	//hide chatbox output
			{
				if(mBoxText[GUI_TEXTOUTPUT]->isVisible())
				{
					Real tWidth = mBox[GUI_TEXTOUTPUT]->getWidth();
					Real tHeight = mBox[GUI_TEXTOUTPUT]->getHeight();
					mBox[GUI_TEXTOUTPUT]->setDimensions(0.48,0.19);
					resizeScrollbarByParent(mBox[GUI_TEXTOUTPUT],tWidth,tHeight);

					mBoxText[GUI_TEXTOUTPUT]->hide();
					mScroller[SCROLLER_TEXTOUTPUT]->hide();
					mScrollerMarker[SCROLLER_TEXTOUTPUT]->hide();
					mSizer[SIZER_TEXTOUTPUT]->hide();
					mBox[GUI_TEXTOUTPUT]->setDimensions(0,0);
					mBox[GUI_TEXTOUTPUT]->setPosition(-0.1,0.9);
					mButton[BUTTON_TEXTOUTPUT]->setLeft(0.12);
					mButtonText[BUTTON_TEXTOUTPUT]->setCaption("Show");
				}
				else
				{
					mBoxText[GUI_TEXTOUTPUT]->show();
					mScroller[SCROLLER_TEXTOUTPUT]->show();
					mScrollerMarker[SCROLLER_TEXTOUTPUT]->show();
					mSizer[SIZER_TEXTOUTPUT]->show();
					mBox[GUI_TEXTOUTPUT]->setDimensions(0.48,0.19);
					mBox[GUI_TEXTOUTPUT]->setPosition(0.01,0.68);
					mButton[BUTTON_TEXTOUTPUT]->setLeft(0.40);
					mButtonText[BUTTON_TEXTOUTPUT]->setCaption("Hide");
				}
				return;
			}
			if(mHoverButton==mButton[BUTTON_SELECTBOX1]||mHoverButton==mButton[BUTTON_SELECTBOX2]||mHoverButton==mButton[BUTTON_SELECTBOX3]||mHoverButton==mButton[BUTTON_SELECTBOX4])	//Target select buttons
			{
				showSelectPanel(false);
				if(mCurrentObject)processSelectBoxClick();
				return;
			}
			if(mHoverButton==mButton[BUTTON_INTERFACE1])	//use tablet
			{
				toggleIsTablet();
				return;
			}
			if(mHoverButton==mButton[BUTTON_INTERFACE2])	//general name display
			{
				toggleDoGeneralCharname();
				return;
			}
			if(mHoverButton==mButton[BUTTON_INTERFACE3])	//local name display
			{
				toggleDoLocalUsername();
				return;
			}
			if(mHoverButton==mButton[BUTTON_INTERFACE4])	//do channel blink
			{
				toggleDoChannelBlink();
				return;
			}
			if(mHoverButton==mButton[BUTTON_INTERFACE5])	//toggle run mode
			{
				toggleRunMode();
				return;
			}
			if(mHoverButton==mButton[BUTTON_INTERFACE6])	//toggle doublejump on
			{
				toggleDoubleJumpOn();
				return;
			}
			if(mHoverButton==mButton[BUTTON_INTERFACE7])	//toggle auto lipsync
			{
				toggleAutoLipSync();
				return;
			}
			if(mHoverButton==mButton[BUTTON_INTERFACE8])	//toggle use windows cursor
			{
				toggleUseWindowsCursor();
				return;
			}
			if(mHoverButton==mButton[BUTTON_SETTINGS1])	//on shadows
			{
				toggleShadowsOn();
				return;
			}
			if(mHoverButton==mButton[BUTTON_SETTINGS2])	//use bloom
			{
				toggleIsBloom();
				return;
			}
			if(mHoverButton==mButton[BUTTON_SETTINGS3])	//paged geometry on
			{
				togglePagedGeometryOn();
				return;
			}
			if(mHoverButton==mButton[BUTTON_SETTINGSBACK])	//back to options
			{
				mBox[GUI_SETTINGS]->hide();
				mBox[GUI_OPTIONS]->show();
				return;
			}
			if(mHoverButton==mButton[BUTTON_INTERFACEBACK])	//back to options
			{
				mBox[GUI_INTERFACE]->hide();
				mBox[GUI_OPTIONS]->show();
				return;
			}
			if(mHoverButton==mButton[BUTTON_GAMEOVERCONTINUE])	//continue campaign
			{
				showCreditsOverlay(false);
				showFaderOverlay(true);
				mFader->setCustomParameter(1,Vector4(0,0,0,1));
				mGameStateManager->setGameState(GAMESTATE_CONTINUECAMPAIGN);
				return;
			}
			if(mHoverButton==mButton[BUTTON_GAMEOVERQUIT])	//quit campaign
			{
				mGameStateManager->setGameState(GAMESTATE_QUITCAMPAIGN);
				return;
			}
			if(mHoverButton==mButton[BUTTON_SERVER1] || mHoverButton==mButton[BUTTON_SERVER2])	//start connecting to server
			{
				if(mHoverButton==mButton[BUTTON_SERVER1])mGameStateManager->setServerID(1);
				else if(mHoverButton==mButton[BUTTON_SERVER2])mGameStateManager->setServerID(2);
				mCharScreenManager->startGame();
				mAlertBox->hide(true);
				showOptionsOverlay(false);
				showServerOverlay(false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_MINIMAP1])	//zoom out minimap
			{
				mMiniMap->zoomOut();
				return;
			}
			if(mHoverButton==mButton[BUTTON_MINIMAP2])	//zoom in minimap
			{
				mMiniMap->zoomIn();
				return;
			}
			if(mHoverButton==mButton[BUTTON_TEXTINPUT3])	//change chat channel
			{
				toggleChatChannel();
				return;
			}
			if(mHoverButton==mButton[BUTTON_MULTIPLAYER1])	//show/hide home box
			{
				toggleShowHomeBox();
				return;
			}
			if(mHoverButton==mButton[BUTTON_MULTIPLAYER2])	//show/hide bio box
			{
				toggleShowBioBox();
				return;
			}
			if(mHoverButton==mButton[BUTTON_MULTIPLAYER3])	//show/hide friend box
			{
				toggleShowFriendBox();
				return;
			}
			if(mHoverButton==mButton[BUTTON_MULTIPLAYER4])	//show/hide item box
			{
				toggleShowItemBox();
				return;
			}
			if(mHoverButton==mButton[BUTTON_MULTIPLAYER5])	//show/hide emote box
			{
				toggleShowEmoteBox();
				return;
			}
			if(mHoverButton==mButton[BUTTON_MULTIPLAYER6])	//show/hide party box
			{
				toggleShowPartyBox();
				return;
			}
			if(mHoverButton==mButton[BUTTON_BIOBOX])	//close bio box
			{
				showBioBox(false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_TARGETBIOBOX])	//close target bio box
			{
				showTargetBioBox(false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_FRIENDBOX])	//close friend box
			{
				showFriendBox(false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_HOMEBOX])	//close home box
			{
				showHomeBox(false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_HOMEBOX1])	//set home
			{
				commandLine = COMMAND_SETHOME;
				return;
			}
			if(mHoverButton==mButton[BUTTON_HOMEBOX2])	//go home
			{
				commandLine = COMMAND_GOHOME;
				return;
			}
			if(mHoverButton==mButton[BUTTON_HOMEBOX3])	//reset home
			{
				commandLine = COMMAND_RESETHOME;
				return;
			}
			if(mHoverButton==mButton[BUTTON_HOMEBOX4])	//change dimension
			{
				toggleInputMode(false,OverlayManager::getSingleton().getOverlayElement("GUI/DimensionText2"),"",3,false,StringConverter::toString(mGameStateManager->getDimension()),false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_EMOTEBOX])	//close emote box
			{
				showEmoteBox(false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_ITEMBOX])	//close item box
			{
				showItemBox(false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_ITEMBOX1])	//show/hide stash box
			{
				toggleShowStashBox();
				return;
			}
			if(mHoverButton==mButton[BUTTON_STASHBOX])	//close stash box
			{
				showStashBox(false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_PARTYBOX])	//close party box
			{
				showPartyBox(false);
				return;
			}
			if(mHoverButton==mButton[BUTTON_PARTYBOX1])	//leave party
			{
				mNetworkManager->sendPartyLeave();
				return;
			}
			if(mHoverButton==mButton[BUTTON_PARTYBOX2])	//accept party request
			{
				if(mUnitManager->hasParty())mNetworkManager->sendPartyAccepted(mUnitManager->getPartyInviter());
				else mNetworkManager->sendInfo(INFO_PARTYINVITEACCEPTED,mUnitManager->getPartyInviter());
				showPartyBox(false);
				showPartyBox(true);
				return;
			}
			if(mHoverButton==mButton[BUTTON_PARTYBOX3])	//deny party request
			{
				mUnitManager->setPartyInviter(0);
				showPartyBox(false);
				showPartyBox(true);
				return;
			}
			if(mHoverButton==mButton[BUTTON_LISTSELECT1]||mHoverButton==mButton[BUTTON_LISTSELECT2]||mHoverButton==mButton[BUTTON_LISTSELECT3]) //listselect buttons
			{
				const OverlayElement* tButtonText = static_cast<OverlayContainer*>(mHoverButton)->getChildIterator().getNext();
				if(!tButtonText)
				{
					showListSelectBox(false);
					return;
				}
				const String tFunction = tButtonText->getCaption();
				if(tFunction=="PM")
				{
					if(mInputManager->getInputMode()==INPUT_CONTROL)toggleInputMode(false,mBoxText[GUI_TEXTINPUT],"Say: ",256,true,"/"+listButtonData+":");
					else mInputManager->setString("/"+listButtonData+":");
				}
				else if(tFunction=="Where")
				{
					mChatManager->message(listButtonData+" is in "+mNetworkManager->getPlayerMap(listButtonData)+".");
					MagixUnit *tUnit = mUnitManager->getByUser(listButtonData);
					if(tUnit && tUnit->isIndexedUnit())mMiniMap->pingUnitMarker(static_cast<MagixIndexedUnit*>(tUnit));
				}
				else if(tFunction=="Remove")
				{
					switch(listButtonType)
					{
					case LISTBUTTON_ONLINEFRIEND:
					case LISTBUTTON_OFFLINEFRIEND:
						editFriendList(listButtonData,false);
						break;
					case LISTBUTTON_BLOCKED:
						editBlockedList(listButtonData,false);
						break;
					default:
						break;
					}
				}
				else if(tFunction=="Stash")
				{
					const String tItem = mUnitManager->getPlayer()->getEquip(listButtonLine);
					if(tItem!="" && !mItemManager->isStashFull())
					{
						if(!mGameStateManager->isCampaign())mNetworkManager->sendItemStash(listButtonLine);
					}
					else mChatManager->message("Your stash is full.");
				}
				else if(tFunction=="Drop")
				{
					mChatManager->message("Dropped "+listButtonData+".");
					unequipItem(listButtonLine);
				}
				else if(tFunction=="Delete")
				{
					mChatManager->message("Deleted "+listButtonData+".");
					unequipItem(listButtonLine,false);
				}
				else if(tFunction=="Equip")
				{
					if(!mGameStateManager->isCampaign())
					{
						const String tItem = mItemManager->getStashAt(listButtonLine);
						if(checkItemEquippable(tItem,false,false))
						{
							short tSlot = mUnitManager->getNextEmptyItemSlot(mUnitManager->getPlayer());
							mNetworkManager->sendItemUnstash(listButtonLine,tSlot);
						}
					}
				}
				showListSelectBox(false);
				return;
			}
		}
		if(mHoverButton)
		{
			//Scroller actions
			short scrollerID = getScrollerID(mHoverButton);
			if(scrollerID==SCROLLER_INTERFACE1)	//mouse sensitivity
			{
				mouseSensitivity = getScroll(SCROLLER_INTERFACE1);
				return;
			}
		}
		if(mGameStateManager->isInGame() && !mGameStateManager->isPaused() &&!mGameStateManager->isCinematic() && (mCamera->getCameraMode()==CAMERA_FREECURSOR || mCamera->getCameraMode()==CAMERA_FREE))
		{
			//click on target HP box = attack target
			if(mUnitManager->getPlayerTarget() && isCursorOverButton(mTargetPanel,true))
			{
				return;
			}
			if(!mHoverButton && !mHoverWindow)doRayPicking(true);
		}
	}
	void mouseRightDown()
	{
	}
	void mouseRightUp()
	{
		if(!mGameStateManager->isInGame() || mGameStateManager->isPaused() || mGameStateManager->isCinematic())return;
		if(mCamera->getCameraMode()!=CAMERA_FREECURSORNEW)return;
		if(!mCursor->isVisible())return;
		//right click on target HP box = attack target
		if(mUnitManager->getPlayerTarget() && isCursorOverButton(mTargetPanel,true))
		{
			MagixLiving *target = mUnitManager->getPlayerTarget();
			Skill *tSkill = mUnitManager->getPlayer()->getCurrentSkill();
			const bool tSkillTargetsAllies = (tSkill && mDef->isSkillTargetsAllies(tSkill->name));
			const bool tIsAlly = target->matchAlliance(mUnitManager->getPlayer()->getAlliance());
			if( (tSkillTargetsAllies && tIsAlly) ||
				(!tSkillTargetsAllies && !tIsAlly) )
			doAttack(target);
			else mUnitManager->getPlayer()->setTarget(target->getPosition());
			return;
		}

		doRayPicking(true);
	}
	const String getDebugText()
	{
		return mDebugText;
	}
	MagixInputManager* getInputManager()
	{
		return mInputManager;
	}
	void toggleInputMode(bool isEnter, OverlayElement *inputBox=0, String defaultCaption="Say: ", unsigned short charLimit=256, bool clearAtReturn=true, String startText="", bool allowTempStorage=true, bool allowNewLine=false, bool hideText=false)
	{
		if(mInputManager->getInputMode()==INPUT_CONTROL)
		{
			if(!inputBox)
			{
				inputBox = mBoxText[GUI_TEXTINPUT];
				String tCaption = mBoxText[GUI_TEXTINPUT]->getCaption();
				tCaption.erase(0,5);
				startText = tCaption;
			}

			mInputManager->setInputBox(inputBox,defaultCaption,charLimit,clearAtReturn,startText,allowTempStorage,allowNewLine,hideText);

			//Chat
			if(inputBox == mBoxText[GUI_TEXTINPUT])
			{
				if(!isGUIVisible)showChatOverlay(true);
				if(!mBoxText[GUI_TEXTINPUT]->isVisible())showChatBox(true);
			}
			//Bio
			else if(inputBox == mBoxText[GUI_BIOBOX])
			{
				mScroller[SCROLLER_BIOBOX]->hide();
				mScrollerMarker[SCROLLER_BIOBOX]->hide();
			}
			//Change Dimension
			else if(inputBox == OverlayManager::getSingleton().getOverlayElement("GUI/DimensionText2"))
			{
				mButtonText[BUTTON_HOMEBOX4]->setCaption("OK");
			}
		}

		if(mInputManager->toggleInputMode())
		{
			if(mInputManager->getInputBox()==mBoxText[GUI_TEXTINPUT])
			{
				String tCaption = mInputManager->getInputText();
				unsigned char tType = CHAT_GENERAL;
				if(mDef->isAdmin)tType = CHAT_ADMIN;
				else if(mDef->isMod && mDef->isModOn)tType = CHAT_MOD;
				String tParam = "";

				mChatManager->processInput(tCaption,tType,tParam);
				if(tType == CHAT_INVALID)return;
				//Command
				if(tType == CHAT_COMMAND)
				{
					mInputManager->pushTempStorage(tParam+(tParam!=tCaption?tCaption:""));
					if(tParam==COMMAND_FRIEND)editFriendList(tCaption,true);
					else if(tParam==COMMAND_UNFRIEND)editFriendList(tCaption,false);
					else if(tParam==COMMAND_BLOCK)editBlockedList(tCaption,true);
					else if(tParam==COMMAND_UNBLOCK)editBlockedList(tCaption,false);
					else if(tParam==COMMAND_KICK && (mDef->isAdmin||mDef->isMod))mNetworkManager->sendKick(tCaption);
					else if(tParam==COMMAND_ROLL)doRoll(tCaption);
					else if(tParam==COMMAND_CREATEITEM && mDef->isAdmin)checkItemEquippable(tCaption,true,true);
					else if(tParam==COMMAND_SPAWNITEM && (mDef->isAdmin))checkItemEquippable(tCaption,true,true,true);
					else if(tParam==COMMAND_GODSPEAK && mDef->isAdmin)mNetworkManager->sendGodSpeak(tCaption);
					else if(tParam==COMMAND_MODON && mDef->isMod && !mDef->isModOn)
					{
						mDef->isModOn = true;
						mChatManager->message("Mod mode on");
						//mNetworkManager->sendInfo(INFO_MODON);
						//mUnitManager->createParticleOnUnit(mUnitManager->getPlayer(),"RisingShards",-1,true);
					}
					else if(tParam==COMMAND_MODOFF && mDef->isMod && mDef->isModOn)
					{
						mDef->isModOn = false;
						mChatManager->message("Mod mode off");
						//mNetworkManager->sendInfo(INFO_MODOFF);
						//mUnitManager->destroyParticleOnUnit(mUnitManager->getPlayer(),"RisingShards");
					}
					else if(tParam==COMMAND_SPAWN && mDef->isAdmin)
					{
						const Critter tC = mDef->getCritter(tCaption);
						if(tC.type!="")
						{
							const Vector3 tPlayerPosition = mUnitManager->getPlayer()->getPosition();
							const Real tX = tPlayerPosition.x + Math::RangeRandom(-50,50);
							const Real tZ = tPlayerPosition.z + Math::RangeRandom(-50,50);
							const Real tY = tC.flying?(tPlayerPosition.y + 50):mUnitManager->getGroundHeight(tX,tZ,true,true);
							if(mGameStateManager->isCampaign())mUnitManager->createCritter(mCritterManager->getNextEmptyID(0),tCaption,Vector3(tX,tY,tZ));
							else mNetworkManager->sendCritterCreate(tCaption,Vector3(tX,tY,tZ),false);
						}
					}
					else if(tParam==COMMAND_WHERE &&(mDef->isAdmin||mDef->isMod))mNetworkManager->sendFindPlayer(tCaption);
					else if(tParam==COMMAND_WHOIS &&(mDef->isAdmin||mDef->isMod))mNetworkManager->sendFindToken(StringConverter::parseInt(tCaption));
					else if(tParam==COMMAND_RELEASEPET)
					{
						if(mUnitManager->getPlayer()->getPet()!="")
						{
							mChatManager->message("You have released your "+mUnitManager->getPlayer()->getPet()+".");
							mUnitManager->getPlayer()->setPet("");
							if(!mGameStateManager->isCampaign())mNetworkManager->sendPetUpdate();
						}
					}
					else if(tParam==COMMAND_PETFOLLOW)
					{
						if(mUnitManager->getPlayer()->getPet()!="")
							mUnitManager->getPlayer()->getPetFlags()->stay = !mUnitManager->getPlayer()->getPetFlags()->stay;
					}
					else if(tParam==COMMAND_PARTY)
					{
						MagixUnit* unit = mUnitManager->getByUser(tCaption);
						if(!unit)mChatManager->message("Cannot find "+tCaption+".",CHAT_PARTY);
						else party(unit);
					}
					else if(tParam==COMMAND_MASSBLOCK &&(mDef->isAdmin||mDef->isMod))
					{
						MagixUnit *tUnit = mUnitManager->getByUser(tCaption);
						if(tUnit && tUnit->isIndexedUnit())mNetworkManager->sendMassBlock(static_cast<MagixIndexedUnit*>(tUnit)->getIndex());
					}
					else if(tParam==COMMAND_BAN &&(mDef->isAdmin||mDef->isMod))
					{
						const vector<String>::type tPart = StringUtil::split(tCaption,";",1);
						const unsigned short tDays = mDef->isAdmin ? (tPart.size()>1?StringConverter::parseInt(tPart[1]):0) : 1;
						if(tPart.size()>0)mNetworkManager->sendKick(tPart[0],true,tDays);
					}
					else if(tParam==COMMAND_SAVECHAT)
					{
						if(tCaption=="")mChatManager->message("Usage: /savechat [filename]");
						else mChatManager->saveChatLog("./",tCaption+".txt");
					}
					else if(tParam==COMMAND_COMMANDS)
					{
						mChatManager->message("--Home Commands--");
						mChatManager->message("/sethome");
						mChatManager->message("/gohome");
						mChatManager->message("/resethome");
						mChatManager->message("--Friend/Block Commands--");
						mChatManager->message("/friend [name]");
						mChatManager->message("/unfriend [name]");
						mChatManager->message("/block [name]");
						mChatManager->message("/unblock [name]");
						mChatManager->message("--Pet Commands--");
						mChatManager->message("/releasepet");
						mChatManager->message("/petfollow");
						mChatManager->message("--Party Commands--");
						mChatManager->message("/party [name]");
						mChatManager->message("--Other Commands--");
						mChatManager->message("/position");
						mChatManager->message("/roll");
						mChatManager->message("/clearall");
						mChatManager->message("/clear");
						mChatManager->message("/lipsync [message]");
						mChatManager->message("/savechat [filename]");
						if(mDef->isAdmin)
						{
							mChatManager->message("/createitem [name]");
							mChatManager->message("/godspeak [message]");
							mChatManager->message("/ban [name];[num days] (Use as last resort)");
						}
						if(mDef->isMod)
						{
							mChatManager->message("/modon");
							mChatManager->message("/modoff");
							mChatManager->message("/ban [name] (Use as last resort)");
						}
						if(mDef->isAdmin||mDef->isMod)
						{
							mChatManager->message("/kick [name]");
							mChatManager->message("/goto [map]:[x] [y] [z]");
							mChatManager->message("/where [name]");
							mChatManager->message("/whois [playertoken]");
							mChatManager->message("/massblock [name]");
						}
					}
					else
					{
						commandLine = tParam;
						commandLineParam = tCaption;
					}
				}
				//Chat
				else
				{
					if(tType==CHAT_LIPSYNC)
					{
						mUnitManager->getPlayer()->doLipSync(tCaption);
						mInputManager->pushTempStorage(COMMAND_LIPSYNC+tCaption);
					}
					else mChatManager->say(mUnitManager,mUnitManager->getPlayer(),tCaption,tType);
					bool tLipSync = (isAutoLipSyncFlag && tType!=CHAT_PRIVATE&&tType!=CHAT_ACTION&&tType!=CHAT_LOCALACTION&&tType!=CHAT_PARTYACTION);
					if(tLipSync)mUnitManager->getPlayer()->doLipSync(tCaption);
					if(tParam!="")mInputManager->pushTempStorage("/"+tParam+":");

					//Party send
					if(tType==CHAT_PARTY||tType==CHAT_PARTYACTION||tType==CHAT_PARTYEVENT)
					{
						if(mUnitManager->hasParty())
						{
							const vector<const std::pair<OwnerToken,String>>::type tParty = mUnitManager->getPartyMembers();
							for(int i=0;i<(int)tParty.size();i++)
								mNetworkManager->sendPartyChat(tParty[i].first,tCaption,tType,tLipSync);
						}
						else mChatManager->message("No one hears you.");
					}
					//normal send
					else mNetworkManager->sendChat(tCaption,tType,tParam,tLipSync);
					
				}
			}
		}
		//Chat
		if(mInputManager->getInputBox()==mBoxText[GUI_TEXTINPUT])
		{
			if(mInputManager->getInputMode()==INPUT_CONTROL)mButtonText[BUTTON_TEXTINPUT]->setCaption("Say");
			else if(mInputManager->getInputMode()==INPUT_TEXT)mButtonText[BUTTON_TEXTINPUT]->setCaption("Send");
			return;
		}
		//Bio
		if(mInputManager->getInputBox()==mBoxText[GUI_BIOBOX] && mInputManager->getInputMode()==INPUT_CONTROL)
		{
			String tBio = mInputManager->getInputText();
			mInputManager->normalizeText(tBio,mBoxText[GUI_BIOBOX]);
			mDef->saveBio(OverlayManager::getSingleton().getOverlayElement("GUI/BioBoxName")->getCaption(),tBio);
			if(tBio=="")tBio = "None";
			bioList.clear();
			String tLine = "";
			for(int i=0;i<(int)tBio.length();i++)
			{
				if(tBio[i]=='\n')
				{
					bioList.push_back(tLine);
					tLine = "";
				}
				else tLine += tBio[i];
			}
			if(tLine!="")bioList.push_back(tLine);

			mScroller[SCROLLER_BIOBOX]->show();
			mScrollerMarker[SCROLLER_BIOBOX]->show();
			return;
		}
		//Logon Username
		if(mInputManager->getInputBox()==OverlayManager::getSingleton().getOverlayElement("GUI/LogonUsernameText") && mInputManager->getInputMode()==INPUT_CONTROL)
		{
			logonUsername = mInputManager->getInputText();
			if(isEnter)
				toggleInputMode(false,OverlayManager::getSingleton().getOverlayElement("GUI/LogonPasswordText"),"Password: ",15,false,logonPassword,false,false,true);
			return;
		}
		//Logon Password
		if(mInputManager->getInputBox()==OverlayManager::getSingleton().getOverlayElement("GUI/LogonPasswordText") && mInputManager->getInputMode()==INPUT_CONTROL)
		{
			logonPassword = mInputManager->getInputText();
			if(isEnter)logOn();
			return;
		}
		//Create Account Info
		if(mInputManager->getInputMode()==INPUT_CONTROL)
		{
			for(int i=0;i<MAX_CREATEACCOUNTINFO;i++)
				if(mInputManager->getInputBox()==mCreateAccountTextBox[i])
				{
					createAccountInfo[i] = mInputManager->getInputText();
					return;
				}
		}
		//Edit Account Info
		if(mInputManager->getInputMode()==INPUT_CONTROL)
		{
			for(int i=0;i<MAX_EDITACCOUNTINFO;i++)
				if(mInputManager->getInputBox()==mEditAccountTextBox[i])
				{
					editAccountInfo[i] = mInputManager->getInputText();
					return;
				}
		}
		//ChangeDimension
		if(mInputManager->getInputBox()==OverlayManager::getSingleton().getOverlayElement("GUI/DimensionText2") && mInputManager->getInputMode()==INPUT_CONTROL)
		{
			mButtonText[BUTTON_HOMEBOX4]->setCaption("Change");
			const unsigned char tDimension = StringConverter::parseInt(mInputManager->getInputText());
			OverlayManager::getSingleton().getOverlayElement("GUI/DimensionText2")->setCaption(StringConverter::toString(tDimension));
			if(tDimension!=mGameStateManager->getDimension())
			{
				if(tDimension>=MAXDIMS&&!mDef->isMod&&!mDef->isAdmin)
				{
					mAlertBox->showAlert("Dims above MAXDIMS is reserved for staff.",0.5,0.5,2);
				}
				else
				{
					mGameStateManager->setDimension(tDimension);
					mUnitManager->setMapChange(true,"",true,mUnitManager->getPlayer()->getPosition(),false);
				}
			}
		}
	}
	bool selectCurrentObject(SceneNode *object, const unsigned int &flags=0, bool isRightClick=false)
	{
		if(object)
		{
			//Object is item
			if(flags==ITEM_MASK)
			{
				MagixItem *tItem = mItemManager->getByObjectNode(object);
				if(!tItem)return false;

				mCurrentObject = object;
				mUnitManager->getPlayer()->setAutoTrackObject(tItem);

				if(isRightClick)mUnitManager->getPlayer()->setTarget(tItem->getPosition(),TARGET_RUNTOPICKUP);
				{
					showSelectPanel(true,mDef->getItemName(tItem->getMesh()));
					mButtonText[BUTTON_SELECTBOX1]->setCaption("Equip");
					mButtonText[BUTTON_SELECTBOX2]->setCaption("");
					mButtonText[BUTTON_SELECTBOX3]->setCaption("");
					mButtonText[BUTTON_SELECTBOX4]->setCaption("");
				}
				return true;
			}
			//Object is critter
			if(flags==CRITTER_MASK)
			{
				MagixCritter *tCritter = mCritterManager->getByObjectNode(object);
				if(!tCritter)return false;

				mCurrentObject = object;
				mUnitManager->getPlayer()->setAutoTrackObject(tCritter);

				if(!isRightClick)
				{
					if(!tCritter->getIsDead())mUnitManager->setPlayerTarget(tCritter);
					showSelectPanel(true,tCritter->getCritterType());
					//Pet commands
					if(tCritter->getIsPet() && tCritter->imTheOwner())
					{
						const PetFlags *tFlags = mUnitManager->getPlayer()->getPetFlags();
						mButtonText[BUTTON_SELECTBOX1]->setCaption("Pet");
						mButtonText[BUTTON_SELECTBOX2]->setCaption(tFlags->stay?"Follow":"Stay");
						mButtonText[BUTTON_SELECTBOX3]->setCaption(tFlags->shrink?"Grow":"Shrink");
						if(tCritter->getCritterType()=="Manta")mButtonText[BUTTON_SELECTBOX4]->setCaption(tFlags->shrink?"Devolve":"Evolve");
						else mButtonText[BUTTON_SELECTBOX4]->setCaption("");
					}
					//Critter commands
					else
					{
						if(tCritter->getIsDrawPoint())mButtonText[BUTTON_SELECTBOX1]->setCaption(!tCritter->getIsDead()?"Draw":"");
						else if(!tCritter->matchAlliance(mUnitManager->getPlayer()->getAlliance()))mButtonText[BUTTON_SELECTBOX1]->setCaption(!tCritter->getIsDead()?"Attack":"");
						else mButtonText[BUTTON_SELECTBOX1]->setCaption((tCritter->getAnim()!="Run"&&tCritter->getAnim()!="Praise"&&!tCritter->isAttacking())?(tCritter->getIsPet()?"Pet":"Praise"):"");
						mButtonText[BUTTON_SELECTBOX2]->setCaption("");
						mButtonText[BUTTON_SELECTBOX3]->setCaption("");
						mButtonText[BUTTON_SELECTBOX4]->setCaption("");
					}
				}
				else
				{
					if(tCritter->getIsDead() || tCritter->getIsDrawPoint())return false;
					Skill *tSkill = mUnitManager->getPlayer()->getCurrentSkill();
					const bool tSkillTargetsAllies = (tSkill && mDef->isSkillTargetsAllies(tSkill->name));
					const bool tIsAlly = tCritter->matchAlliance(mUnitManager->getPlayer()->getAlliance());
					if( (tSkillTargetsAllies && tIsAlly) ||
						(!tSkillTargetsAllies && !tIsAlly) )
					doAttack(tCritter);
					else return false;
				}
				return true;
			}
			//Object is not player
			if(object!=mUnitManager->getPlayer()->getObjectNode())
			{
				MagixUnit *tUnit = mUnitManager->getByObjectNode(object);
				if(!tUnit)return false;
				
				//Cannot click hidden unit
				if(tUnit->isHidden)return false;

				mCurrentObject = object;
				mUnitManager->getPlayer()->setAutoTrackObject(tUnit);

				if(!isRightClick)
				{
					if(tUnit->getHP()>0)mUnitManager->setPlayerTarget(tUnit);
					showSelectPanel(true,(tUnit->getUser()==""?tUnit->getName():tUnit->getUser()));
					const bool tIsAlly = tUnit->matchAlliance(mUnitManager->getPlayer()->getAlliance());
					if(!mGameStateManager->isCampaign() && tIsAlly)
					{
						mButtonText[BUTTON_SELECTBOX1]->setCaption( (static_cast<MagixIndexedUnit*>(tUnit)->isFriend?"Remove Friend":"Add Friend") );
						mButtonText[BUTTON_SELECTBOX2]->setCaption( (static_cast<MagixIndexedUnit*>(tUnit)->isBlocked?"Unblock":"Block") );
						mButtonText[BUTTON_SELECTBOX3]->setCaption("View Bio");
						if(mUnitManager->isPartyMember(tUnit))mButtonText[BUTTON_SELECTBOX4]->setCaption("");
						else mButtonText[BUTTON_SELECTBOX4]->setCaption( mUnitManager->hasParty()?"Invite Party":"Join Party" );
					}
					else
					{
						mButtonText[BUTTON_SELECTBOX1]->setCaption(tIsAlly?"":"Attack");
						mButtonText[BUTTON_SELECTBOX2]->setCaption("");
						mButtonText[BUTTON_SELECTBOX3]->setCaption("");
						mButtonText[BUTTON_SELECTBOX4]->setCaption("");
					}
				}
				else
				{
					if(tUnit->isIndexedUnit() && static_cast<MagixIndexedUnit*>(tUnit)->isWounded)return false;
					Skill *tSkill = mUnitManager->getPlayer()->getCurrentSkill();
					const bool tSkillTargetsAllies = (tSkill && mDef->isSkillTargetsAllies(tSkill->name));
					const bool tIsAlly = tUnit->matchAlliance(mUnitManager->getPlayer()->getAlliance());
					if( (tSkillTargetsAllies && tIsAlly) ||
						(!tSkillTargetsAllies && !tIsAlly) )
					doAttack(tUnit);
					else return false;
				}
				return true;
			}
			//Object is player
			if(isRightClick && object==mUnitManager->getPlayer()->getObjectNode() && mUnitManager->getPlayer()->getCurrentSkill() && mDef->isSkillTargetsSelf(mUnitManager->getPlayer()->getCurrentSkill()->name))
			{
				mCurrentObject = object;
				doAttack(mUnitManager->getPlayer());
				return true;
			}
		}
		else
		{
			mCurrentObject = 0;
		}
		return false;
	}
	void processSelectBoxClick()
	{
		if(!mHoverButton)return;
		const OverlayElement* tButtonText = static_cast<OverlayContainer*>(mHoverButton)->getChildIterator().getNext();
		if(!tButtonText)return;
		const String tFunction = tButtonText->getCaption();

			if(tFunction=="Add Friend"||tFunction=="Remove Friend")
			{
				MagixUnit *tUnit = mUnitManager->getByObjectNode(mCurrentObject);
				if(!tUnit)return;
				editFriendList(tUnit->getUser(),(tFunction=="Add Friend"));
			}
			else if(tFunction=="Block"||tFunction=="Unblock")
			{
				MagixUnit *tUnit = mUnitManager->getByObjectNode(mCurrentObject);
				if(!tUnit)return;
				editBlockedList(tUnit->getUser(),(tButtonText->getCaption()=="Block"));
			}
			else if(tFunction=="View Bio")
			{
				MagixUnit *tTarget = mUnitManager->getByObjectNode(mCurrentObject);
				if(!tTarget || tTarget==mUnitManager->getPlayer())return;

				if(!mGameStateManager->isCampaign())
				{
					refreshTargetBio("Loading...",tTarget->getName());
					showTargetBioBox(true);
					mNetworkManager->sendBioRequest(tTarget->getUser());
				}
			}
			else if(tFunction=="Praise")
			{
				MagixCritter *tCritter = mCritterManager->getByObjectNode(mCurrentObject);
				if(!tCritter)return;
				if(!tCritter->setAnimation("Praise"))tCritter->setAnimation("Attack1");
				if(tCritter->isInvulnerable())mUnitManager->rewardCritter(tCritter);
			}
			else if(tFunction=="Pet")
			{
				MagixCritter *tCritter = mCritterManager->getByObjectNode(mCurrentObject);
				if(!tCritter)return;
				if(!tCritter->setAnimation("Praise"))tCritter->setAnimation("Attack1");
			}
			else if(tFunction=="Stay")
			{
				MagixCritter *tCritter = mCritterManager->getByObjectNode(mCurrentObject);
				if(!tCritter)return;
				mUnitManager->getPlayer()->getPetFlags()->stay = true;
			}
			else if(tFunction=="Follow")
			{
				MagixCritter *tCritter = mCritterManager->getByObjectNode(mCurrentObject);
				if(!tCritter)return;
				mUnitManager->getPlayer()->getPetFlags()->stay = false;
			}
			else if(tFunction=="Shrink")
			{
				MagixCritter *tCritter = mCritterManager->getByObjectNode(mCurrentObject);
				if(!tCritter)return;
				mUnitManager->getPlayer()->getPetFlags()->shrink = true;
				mUnitManager->getPlayer()->getPetFlags()->evolve = false;
				mNetworkManager->sendCritterShrink(tCritter->getID(),true);
			}
			else if(tFunction=="Grow")
			{
				MagixCritter *tCritter = mCritterManager->getByObjectNode(mCurrentObject);
				if(!tCritter)return;
				mUnitManager->getPlayer()->getPetFlags()->shrink = false;
				mNetworkManager->sendCritterShrink(tCritter->getID(),false);
			}
			else if(tFunction=="Evolve")
			{
				MagixCritter *tCritter = mCritterManager->getByObjectNode(mCurrentObject);
				if(!tCritter)return;
				mUnitManager->getPlayer()->getPetFlags()->evolve = true;
				mUnitManager->getPlayer()->getPetFlags()->shrink = false;
				mNetworkManager->sendCritterShrink(tCritter->getID(),true,true);
			}
			else if(tFunction=="Devolve")
			{
				MagixCritter *tCritter = mCritterManager->getByObjectNode(mCurrentObject);
				if(!tCritter)return;
				mUnitManager->getPlayer()->getPetFlags()->evolve = false;
				mNetworkManager->sendCritterShrink(tCritter->getID(),false);
			}
			else if(tFunction=="Attack")
			{
				if(mUnitManager->getPlayer()->getIsWounded())
				{
					mChatManager->message("Cannot attack when you're wounded.");
					return;
				}
				MagixUnit *tUnit = mUnitManager->getByObjectNode(mCurrentObject);
				if(tUnit)
				{
					mUnitManager->getPlayer()->setAutoAttack(tUnit);
					return;
				}
				MagixCritter *tCritter = mCritterManager->getByObjectNode(mCurrentObject);
				if(tCritter)
				{
					mUnitManager->getPlayer()->setAutoAttack(tCritter);
					return;
				}
			}
			else if(tFunction=="Invite Party" || tFunction=="Join Party")
			{
				MagixUnit *tUnit = mUnitManager->getByObjectNode(mCurrentObject);
				party(tUnit);
			}
			else if(tFunction=="Equip")
			{
				MagixItem *tItem = mItemManager->getByObjectNode(mCurrentObject);
				if(!tItem)return;
				mUnitManager->getPlayer()->setAutoAttack(0);
				mUnitManager->getPlayer()->setTarget(tItem->getPosition(),TARGET_RUNTOPICKUP);
			}
			else if(tFunction=="Draw")
			{
				MagixCritter *tCritter = mCritterManager->getByObjectNode(mCurrentObject);
				if(!tCritter)return;
				mUnitManager->getPlayer()->setAutoAttack(0);
				mUnitManager->getPlayer()->setTarget(tCritter->getPosition(),TARGET_RUNTOPICKUP,true);
			}
			
	}
	void showSelectPanel(bool flag, const String &name="Target")
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/SelectBox")->show();
			mBoxText[GUI_SELECTBOX]->setCaption(name);
			mBox[GUI_SELECTBOX]->hide();
			mSelectPanel->setPosition(cursorX,cursorY);
			mSelectPanel->setDimensions(0,0);
			mSelectPanel->show();
			selectPanelTimeout = 0.25;
			mHeldButtonWindow = mBox[GUI_SELECTBOX];
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/SelectBox")->hide();
			mBox[GUI_SELECTBOX]->hide();
			mSelectPanel->hide();
			selectPanelTimeout = 0;
			mHeldButtonWindow = 0;
		}
	}
	MagixChatManager* getChatManager()
	{
		return mChatManager;
	}
	MagixAlertBox* getAlertBox()
	{
		return mAlertBox;
	}
	MagixCharScreenManager* getCharScreenManager()
	{
		return mCharScreenManager;
	}
	MagixMiniMap* getMiniMap()
	{
		return mMiniMap;
	}
	void sizeWindowDrag(unsigned short sizeDir, const Real &minWidth=0, const Real &minHeight=0)
	{
		sizeWindowDir = sizeDir;
		if(sizeDir!=0 && mHoverButton)
		{
			dragStartX = cursorX;
			dragStartY = cursorY;
			sizeWindowWidth = mHoverButton->getParent()->getWidth();
			sizeWindowHeight = mHoverButton->getParent()->getHeight();
			sizeWindowLeft = mHoverButton->getParent()->getLeft();
			sizeWindowTop = mHoverButton->getParent()->getTop();
			sizeWindowMinWidth = minWidth;
			sizeWindowMinHeight = minHeight;
		}
	}
	void doAttack(MagixLiving *target=0)
	{
		if(mUnitManager->getPlayer()->getIsWounded())
		{
			mChatManager->message("Cannot attack when you're wounded.");
			return;
		}
		//Skill
		if(mUnitManager->getPlayer()->isSkillMode())
		{
			Skill *tSkill = mUnitManager->getPlayer()->getCurrentSkill();
			//Run to target and cast once
			if(target && target!=mUnitManager->getPlayer())
			{
				if(tSkill->stock>0)
				{
					mUnitManager->getPlayer()->setAutoAttack(target,true);
					mUnitManager->getPlayer()->doSkill(tSkill,true);
					mUnitManager->getPlayer()->setAttackRange(mDef->getAttack(tSkill->name).attackRange);
				}
			}
			//No specified target, find a suitable target
			else
			{
				Attack tAttack = mDef->getAttack(tSkill->name);
				//Non ally attack must have proper target
				if(tAttack.singleTarget && !tAttack.hitAlly && (!mUnitManager->getPlayerTarget() || mUnitManager->getPlayerTarget()->matchAlliance(mUnitManager->getPlayer()->getAlliance())))
				{
					mChatManager->message("This skill needs a target, use E to target close units.");
					return;
				}
				//Cast once if skill can target self
				if(!mUnitManager->getPlayer()->getIsStance())mUnitManager->getPlayer()->toggleStance(mDef);
				if(tSkill->stock>0)
				{
					mUnitManager->getPlayer()->doSkill(tSkill);
				}
			}
		}
		//Normal Attack
		else 
		{
			//Chase and attack target
			if(target)
			{
				if(target->getHP()>0)mUnitManager->getPlayer()->setAutoAttack(target);
				else mUnitManager->getPlayer()->setTarget(target->getPosition());
			}
			//Strike once
			else mUnitManager->getPlayer()->doAttack(mDef);
		}
	}
	void doInGameLeftClick()
	{
		//Attack
		if(mCamera->getCameraMode()==CAMERA_LOCKED)doAttack();
		//Raypick
		else doRayPicking(false);
	}
	void doRayPicking(bool isRightClick)
	{
		const Ray mouseRay = mCamera->getCamera()->getCameraToViewportRay(cursorX,cursorY);
        mRayQuery->setRay(mouseRay);
		mRayQuery->setSortByDistance(true);
		mRayQuery->setQueryMask(UNIT_MASK|ITEM_MASK|CRITTER_MASK);
		RaySceneQueryResult &result = mRayQuery->execute();
        RaySceneQueryResult::iterator i = result.begin();

		selectCurrentObject(0);

        for( i = result.begin(); i != result.end(); i++ )
		{
			//is movable
			if(i->movable && i->movable->getName().substr(0, 5) != "tile[")
			{
				if(selectCurrentObject(i->movable->getParentSceneNode(),i->movable->getQueryFlags(),isRightClick))return;
			}
			//is world fragment
			else if(i->worldFragment)
			{
				if(!isRightClick)
				{
					if(mUnitManager->getPlayer()->getAutoTrackObject())mUnitManager->getPlayer()->setAutoTrackObject(0);
					mUnitManager->setPlayerTarget(0);
					if(mCamera->getCameraMode()==CAMERA_FREECURSORNEW)
					{
						if(mUnitManager->getPlayer()->getAutoAttackTarget())
						{
							mUnitManager->getPlayer()->setAutoAttack(0);
							mUnitManager->getPlayer()->setTarget(mUnitManager->getPlayer()->getPosition());
						}
					}

				}
				else
				{
					mUnitManager->getPlayer()->setAutoAttack(0);
					mUnitManager->getPlayer()->setTarget(i->worldFragment->singleIntersection);
				}
				return;
			}
        }
		//Clicked on nothing
		if(!isRightClick)
		{
			if(mUnitManager->getPlayer()->getAutoTrackObject())mUnitManager->getPlayer()->setAutoTrackObject(0);
			mUnitManager->setPlayerTarget(0);
		}
		mUnitManager->getPlayer()->setAutoAttack(0);
		mUnitManager->getPlayer()->setTarget(mUnitManager->getPlayer()->getPosition());
	}
	void doScroll(bool flag, short iID=0)
	{
		if(!flag)
		{
			scrollerHeldID = -1;
			return;
		}
		if(!mScroller[iID] || !mScrollerMarker[iID])return;
		scrollerHeldID = iID;

		if(isScrollerHorz[iID])
		{
			mScrollerMarker[iID]->setLeft(cursorX - mScroller[iID]->getParent()->getLeft());
			if(mScrollerMarker[iID]->getLeft()<mScroller[iID]->getLeft())mScrollerMarker[iID]->setLeft(mScroller[iID]->getLeft());
			if(mScrollerMarker[iID]->getLeft()>mScroller[iID]->getLeft()+mScroller[iID]->getWidth())mScrollerMarker[iID]->setLeft(mScroller[iID]->getLeft()+mScroller[iID]->getWidth());
			mScrollerMarker[iID]->setLeft(mScrollerMarker[iID]->getLeft()-mScrollerMarker[iID]->getWidth()/2);
		}
		else
		{
			mScrollerMarker[iID]->setTop(cursorY - mScroller[iID]->getParent()->getTop());
			if(mScrollerMarker[iID]->getTop()<mScroller[iID]->getTop())mScrollerMarker[iID]->setTop(mScroller[iID]->getTop());
			if(mScrollerMarker[iID]->getTop()>mScroller[iID]->getTop()+mScroller[iID]->getHeight())mScrollerMarker[iID]->setTop(mScroller[iID]->getTop()+mScroller[iID]->getHeight());
			mScrollerMarker[iID]->setTop(mScrollerMarker[iID]->getTop()-mScrollerMarker[iID]->getHeight()/2);
		}
	}
	void doMove(bool flag)
	{
		if(flag && mMoverWindow)
		{
			dragStartX = cursorX - mMoverWindow->getLeft();
			dragStartY = cursorY - mMoverWindow->getTop();
		}
		else
		{
			dragStartX = 0;
			dragStartY = 0;
			mMoverWindow = 0;
		}
	}
	const Real getScroll(unsigned short iID)
	{
		if(!mScroller[iID] || !mScrollerMarker[iID])return 0;

		if(isScrollerHorz[iID])
		{
			return (mScrollerMarker[iID]->getLeft() + mScrollerMarker[iID]->getWidth()/2 - mScroller[iID]->getLeft())/mScroller[iID]->getWidth();
		}
		else
		{
			return (mScrollerMarker[iID]->getTop() + mScrollerMarker[iID]->getHeight()/2 - mScroller[iID]->getTop())/mScroller[iID]->getHeight();
		}
	}
	void setScroll(unsigned short iID, Real ratio)
	{
		if(!mScroller[iID] || !mScrollerMarker[iID])return;

		if(isScrollerHorz[iID])
		{
			mScrollerMarker[iID]->setLeft(mScroller[iID]->getLeft() + mScroller[iID]->getWidth()*ratio - mScrollerMarker[iID]->getWidth()/2);
		}
		else
		{
			mScrollerMarker[iID]->setTop(mScroller[iID]->getTop() + mScroller[iID]->getHeight()*ratio - mScrollerMarker[iID]->getHeight()/2);
		}
	}
	void resizeScrollbarByParent(OverlayElement *parent, const Real &dwidth, const Real &dheight)
	{
		for(int i=0;i<MAX_SCROLLERS;i++)
		{
			if(mScroller[i])
			if(mScroller[i]->getParent()==parent)
			{
				Real tRatio = getScroll(i);
				if(isScrollerHorz[i])
				{
					mScroller[i]->setWidth(mScroller[i]->getWidth()+(parent->getWidth()-dwidth));
				}
				else
				{
					mScroller[i]->setHeight(mScroller[i]->getHeight()+(parent->getHeight()-dheight));
				}
				setScroll(i,tRatio);
				return;
			}
		}
	}
	const short getScrollerID(OverlayElement *scroller)
	{
		for(int i=0;i<MAX_SCROLLERS;i++)
			if(mScrollerMarker[i]&& mScrollerMarker[i]==scroller)return i;
		return -1;
	}
	void updateBoxScrollerCaption(const unsigned short &boxID, const unsigned short &scrollerID, const vector<String>::type &captionList)
	{
		if(boxID>=MAX_BOXES)return;
		if(!mBox[boxID] || !mBoxText[boxID])return;
		
		const Real tCharHeight = StringConverter::parseReal(mBoxText[boxID]->getParameter("char_height"));
		if(tCharHeight==0)return;
		const Real tLines = (mBox[boxID]->getHeight()-mBoxText[boxID]->getTop())/tCharHeight - 1;

		int start = getScroll(scrollerID)* (int(captionList.size()) - Math::IFloor(tLines));
		if(start<0)start = 0;
		String tCaption = "";
		unsigned short tCaptionLines = 0;
		for(int i=start;i<int(captionList.size());i++)
		{
			tCaption += (i==start?"":"\n");
			tCaption += captionList[i];
			tCaptionLines += 1;
			if(tCaptionLines>=tLines)break;
		}
		mBoxText[boxID]->setCaption(tCaption);
	}
	const short getListButtonLine(const unsigned short &boxID, const unsigned short &scrollerID, const unsigned short &listLines, Real &fontHeight, short &apparentLine)
	{
		if(boxID>=MAX_BOXES)return -1;
		if(!mBox[boxID] || !mBoxText[boxID])return -1;
		
		const Real tCharHeight = StringConverter::parseReal(mBoxText[boxID]->getParameter("char_height"));
		if(tCharHeight==0)return -1;
		else fontHeight = tCharHeight;
		const Real tLines = (mBox[boxID]->getHeight()-mBoxText[boxID]->getTop())/tCharHeight - 1;

		int start = getScroll(scrollerID)* (listLines - Math::IFloor(tLines));
		if(start<0)start = 0;
		
		const Real tY = cursorY - mBox[boxID]->getTop();
		apparentLine = tY/tCharHeight;
		return short(apparentLine + start);
	}
	const short getListButtonLine(const unsigned short &boxID, Real &fontHeight, short &apparentLine)
	{
		if(boxID>=MAX_BOXES)return -1;
		if(!mBox[boxID] || !mBoxText[boxID])return -1;
		
		const Real tCharHeight = StringConverter::parseReal(mBoxText[boxID]->getParameter("char_height"));
		if(tCharHeight==0)return -1;
		else fontHeight = tCharHeight;
		const Real tLines = (mBox[boxID]->getHeight()-mBoxText[boxID]->getTop())/tCharHeight - 1;
		
		const Real tY = cursorY - mBox[boxID]->getTop();
		apparentLine = tY/tCharHeight;
		return short(apparentLine);
	}
	OverlayElement* getMoverWindow(OverlayElement *mover)
	{
		for(int i=0;i<MAX_MOVERS;i++)
			if(mMover[i]&& mMover[i]==mover)return mMover[i]->getParent();
		return 0;
	}
	void toggleShowOptions()
	{
		if(!mBox[GUI_OPTIONS]->isVisible() && !mBox[GUI_SETTINGS]->isVisible() && !mBox[GUI_INTERFACE]->isVisible())
		{
			showOptionsOverlay(true);
			if(mGameStateManager->isCampaign() && !mGameStateManager->isPaused())mGameStateManager->togglePause();
			if(mGameStateManager->isInGame())
			{
				mButton[BUTTON_OPTIONRESTART]->show();
				mButton[BUTTON_OPTIONQUIT]->setTop(0.35);
				mBox[GUI_OPTIONS]->setTop(0.25);
				mBox[GUI_OPTIONS]->setHeight(0.45);
			}
			else
			{
				mButton[BUTTON_OPTIONRESTART]->hide();
				mButton[BUTTON_OPTIONQUIT]->setTop(0.275);
				mBox[GUI_OPTIONS]->setTop(0.2875);
				mBox[GUI_OPTIONS]->setHeight(0.375);
			}
		}
		else
		{
			showOptionsOverlay(false);
			if(mGameStateManager->isCampaign() && mGameStateManager->isPaused())mGameStateManager->togglePause();
		}
	}
	bool isOptionsVisible()
	{
		return (mBox[GUI_OPTIONS]->isVisible()||mBox[GUI_SETTINGS]->isVisible()||mBox[GUI_INTERFACE]->isVisible());
	}
	void showChatBox(bool flag, bool showInput=true)
	{
		if(!flag)
		{
			mBox[GUI_TEXTOUTPUT]->hide();

			mBox[GUI_TEXTINPUT]->setDimensions(0,0);
			mBox[GUI_TEXTINPUT]->setPosition(-0.1,0.9);
			mBoxText[GUI_TEXTINPUT]->hide();
			mButton[BUTTON_TEXTINPUT]->hide();
			mButton[BUTTON_TEXTINPUT3]->hide();
			mButton[BUTTON_TEXTINPUT2]->setLeft(0.12);
			mButtonText[BUTTON_TEXTINPUT2]->setCaption("Show");

			if(mInputManager->getInputMode()==INPUT_TEXT)
			{
				if(mInputManager->getInputText().length()>1)mInputManager->setClearAtReturn(false);
				mInputManager->toggleInputMode();
				mButtonText[BUTTON_TEXTINPUT]->setCaption("Say");
			}
		}
		else
		{
			mBox[GUI_TEXTOUTPUT]->show();

			if(showInput)
			{
				mBox[GUI_TEXTINPUT]->setDimensions(0.48,0.07);
				mBox[GUI_TEXTINPUT]->setPosition(0.01,0.9);
				mBoxText[GUI_TEXTINPUT]->show();
				mButton[BUTTON_TEXTINPUT]->show();
				mButton[BUTTON_TEXTINPUT3]->show();
				mButton[BUTTON_TEXTINPUT2]->setLeft(0.4);
				mButtonText[BUTTON_TEXTINPUT2]->setCaption("Hide");
			}
			else
			{
				mBox[GUI_TEXTINPUT]->hide();
			}
		}
	}
	void initializeSettings()
	{
		bool tTablet=false,tShadowsOn=false,tBloom=false,tGeneralCharname=false,tLocalUsername=false,tChannelBlink=true,tRunMode=true,tDoubleJumpOn=true,tAutoLipSync=false;
		Real tMusicVol=1,tGuiVol=1,tSoundVol=1;
		mDef->loadSettings(tTablet,mouseSensitivity,tMusicVol,tGuiVol,tSoundVol,tShadowsOn,tBloom,logonUsername,tGeneralCharname,tLocalUsername,tChannelBlink,tRunMode,tDoubleJumpOn,tAutoLipSync,useWindowsCursor);

		if(tTablet)toggleIsTablet();
		toggleShadowsOn();
		if(!tShadowsOn)toggleShadowsOn();
		if(tBloom)toggleIsBloom();
		mButtonText[BUTTON_SETTINGS3]->setCaption((mDef->pagedGeometryOn?"On":"Off"));
		mSoundManager->setMusicVolume(tMusicVol);
		setScroll(SCROLLER_SETTINGS1,tMusicVol);
		mSoundManager->setGUISoundVolume(tGuiVol);
		setScroll(SCROLLER_SETTINGS2,tGuiVol);
		mSoundManager->setSoundVolume(tSoundVol);
		setScroll(SCROLLER_SETTINGS3,tSoundVol);
		setScroll(SCROLLER_SETTINGS4,mDef->viewDistance);

		setScroll(SCROLLER_INTERFACE1,mouseSensitivity);
		if(tGeneralCharname)toggleDoGeneralCharname();
		if(tLocalUsername)toggleDoLocalUsername();
		if(!tChannelBlink)toggleDoChannelBlink();
		if(!tRunMode)toggleRunMode();
		if(!tDoubleJumpOn)toggleDoubleJumpOn();
		if(tAutoLipSync)toggleAutoLipSync();

		if(useWindowsCursor)toggleUseWindowsCursor(true);

		OverlayManager::getSingleton().getOverlayElement("GUI/LogonUsernameText")->setCaption("Username: "+logonUsername);
	}
	void saveSettings()
	{
		mDef->saveSettings(isTabletFlag, mouseSensitivity,
						mSoundManager->getMusicVolume(),
						mSoundManager->getGUISoundVolume(),
						mSoundManager->getSoundVolume(),
						isShadowsOnFlag,
						isBloomFlag,
						logonUsername,
						mChatManager->doGeneralCharname,
						mChatManager->doLocalUsername,
						doChannelBlink,
						mUnitManager->getPlayer()->defaultControlModeIsAbsolute,
						isDoubleJumpOnFlag,
						isAutoLipSyncFlag,
						(mButtonText[BUTTON_INTERFACE8]->getCaption()=="On"));
	}
	void toggleIsTablet(bool showAlert=false)
	{
		isTabletFlag = !isTabletFlag;
		mButtonText[BUTTON_INTERFACE1]->setCaption((isTabletFlag?"On":"Off"));
		if(showAlert)mAlertBox->showAlert( (isTabletFlag?"Switched to Tablet Mode (Hotkey INSERT)":"Switched to Mouse Mode (Hotkey INSERT)") ,0.5,0.5);
	}
	bool isTablet()
	{
		return isTabletFlag;
	}
	void toggleShadowsOn()
	{
		isShadowsOnFlag = !isShadowsOnFlag;
		mButtonText[BUTTON_SETTINGS1]->setCaption((isShadowsOnFlag?"On":"Off"));
		mSceneMgr->setShadowTechnique((isShadowsOnFlag?SHADOWTYPE_TEXTURE_MODULATIVE:SHADOWTYPE_NONE));
		if(isShadowsOnFlag)
		{
			mSceneMgr->setShadowFarDistance(350);
			mSceneMgr->setShadowTextureSize(1024);
			mSceneMgr->setShadowColour(ColourValue(0.5,0.5,0.5,1));
		}
	}
	bool isShadowsOn()
	{
		return isShadowsOnFlag;
	}
	void toggleIsBloom()
	{
		isBloomFlag = !isBloomFlag;
		mButtonText[BUTTON_SETTINGS2]->setCaption((isBloomFlag?"On":"Off"));
		if(CompositorManager::getSingleton().hasCompositorChain(mWindow->getViewport(0)))
			CompositorManager::getSingleton().setCompositorEnabled(mWindow->getViewport(0), "Bloom", isBloomFlag);
	}
	void toggleMotionBlur()
	{
		motionBlurFlag = !motionBlurFlag;
		if(CompositorManager::getSingleton().hasCompositorChain(mWindow->getViewport(0)))
			CompositorManager::getSingleton().setCompositorEnabled(mWindow->getViewport(0), "MotionBlur", motionBlurFlag);
	}
	void togglePagedGeometryOn()
	{
		mDef->pagedGeometryOn = !mDef->pagedGeometryOn;
		mButtonText[BUTTON_SETTINGS3]->setCaption((mDef->pagedGeometryOn?"On":"Off"));
	}
	void toggleUnderwaterShader()
	{
		underwaterShaderFlag = !underwaterShaderFlag;
		showPseudoShaderOverlay(underwaterShaderFlag);
		if(underwaterShaderFlag)mPseudoShader->setMaterialName("PseudoShader/Underwater");
	}
	void toggleDoChannelBlink()
	{
		doChannelBlink = !doChannelBlink;
		mButtonText[BUTTON_INTERFACE4]->setCaption((doChannelBlink?"On":"Off"));
	}
	void toggleDoGeneralCharname()
	{
		mChatManager->doGeneralCharname = !mChatManager->doGeneralCharname;
		mButtonText[BUTTON_INTERFACE2]->setCaption((mChatManager->doGeneralCharname?"Character":"Username"));
		if(mChatManager->getChannel()==1)mMiniMap->setMapTagUseCharName(mChatManager->doGeneralCharname);
	}
	void toggleDoLocalUsername()
	{
		mChatManager->doLocalUsername = !mChatManager->doLocalUsername;
		mButtonText[BUTTON_INTERFACE3]->setCaption((mChatManager->doLocalUsername?"Username":"Character"));
		if(mChatManager->getChannel()==0||mChatManager->getChannel()==2)mMiniMap->setMapTagUseCharName(!mChatManager->doLocalUsername);
	}
	void toggleRunMode()
	{
		mUnitManager->getPlayer()->toggleControlMode();
		mButtonText[BUTTON_INTERFACE5]->setCaption((mUnitManager->getPlayer()->defaultControlModeIsAbsolute?"Absolute":"Relative"));
	}
	void toggleDoubleJumpOn()
	{
		isDoubleJumpOnFlag = !isDoubleJumpOnFlag;
		mButtonText[BUTTON_INTERFACE6]->setCaption((isDoubleJumpOnFlag?"On":"Off"));
	}
	void toggleAutoLipSync()
	{
		isAutoLipSyncFlag = !isAutoLipSyncFlag;
		mButtonText[BUTTON_INTERFACE7]->setCaption((isAutoLipSyncFlag?"On":"Off"));
	}
	void toggleUseWindowsCursor(bool dontShowAlert=false)
	{
		const bool tOn = (mButtonText[BUTTON_INTERFACE8]->getCaption()=="On");
		mButtonText[BUTTON_INTERFACE8]->setCaption((!tOn?"On":"Off"));
		if(!dontShowAlert)mAlertBox->showAlert("Restart the game for this option to take effect",0.5,0.5,1.5);
	}
	bool getUseWindowsCursor()
	{
		return useWindowsCursor;
	}
	bool isBloom()
	{
		return isBloomFlag;
	}
	bool isMotionBlur()
	{
		return motionBlurFlag;
	}
	const Real getMouseSensitivity()
	{
		return mouseSensitivity;
	}
	void fadeIn(const Real &duration, const ColourValue &colour=ColourValue(0,0,0))
	{
		if(duration==0)
		{
			showFaderOverlay(false);
			faderTimeout = 0;
		}
		else
		{
			faderTimeout = duration;
			faderTimeoutMax = duration;
			faderColour = colour;
		}
	}
	void fadeOut(const Real &duration, const ColourValue &colour=ColourValue(0,0,0))
	{
		if(duration==0)
		{
			if(!mFader->isVisible())showFaderOverlay(true);
			mFader->setCustomParameter(1,Vector4(colour.r,colour.g,colour.b,1));
			#pragma warning(push)
			#pragma warning(disable : 4482)
			if(mFader->getTechnique()->getName()=="2")
			{
				mFader->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
																									LayerBlendSource::LBS_MANUAL,
																									LayerBlendSource::LBS_CURRENT,
																									ColourValue(colour.r,colour.g,colour.b));
				mFader->getTechnique()->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LayerBlendOperationEx::LBX_SOURCE1,
																								LayerBlendSource::LBS_MANUAL,
																								LayerBlendSource::LBS_CURRENT,
																								1);
			}
			#pragma warning(pop)
			faderTimeout = 0;
		}
		else
		{
			faderTimeout = -duration;
			faderTimeoutMax = duration;
			faderColour = colour;
		}
	}
	void showCinematicBars(bool flag, bool skipTransition=false)
	{
		if(flag)
		{
			if(skipTransition)
			{
				showBackdropOverlay(false);
				cinematicBarsTimeout = 0;
			}
			else
			{
				mBackdrop->setMaterialName("GUIMat/Backdrop2");
				mBackdrop->setDimensions(1.5,1.5);
				mBackdrop->setPosition(-0.25,-0.25);
				showBackdropOverlay(true);
				cinematicBarsTimeout = 2;
			}
		}
		else 
		{
			if(skipTransition)
			{
				showBackdropOverlay(false);
				cinematicBarsTimeout = 0;
			}
			else
			{
				mBackdrop->setMaterialName("GUIMat/Backdrop2");
				mBackdrop->setDimensions(1.5,1);
				mBackdrop->setPosition(-0.25,0);
				showBackdropOverlay(true);
				cinematicBarsTimeout = -2;
			}
		}
	}
	void showCinematicCaption(const Real &duration, const String &caption)
	{
		OverlayManager::getSingleton().getOverlayElement("GUI/CinematicCaption")->setCaption(caption);
		OverlayManager::getSingleton().getOverlayElement("GUI/CinematicCaption")->show();
		cinematicCaptionTimeout = duration;
	}
	void showCreditsCaption(const String &caption)
	{
		if(!OverlayManager::getSingleton().getOverlayElement("GUI/Credits")->isVisible())
			OverlayManager::getSingleton().getOverlayElement("GUI/Credits")->show();
		OverlayElement *tCaption = OverlayManager::getSingleton().getOverlayElement("GUI/CreditsCaption");
		tCaption->setCaption(caption);
		vector<String>::type tLines = StringUtil::split(caption,"\n");
		tCaption->setTop(0.5-StringConverter::parseReal(tCaption->getParameter("char_height"))*int(tLines.size())/2);
	}
	void updateSkillText(const Skill *skill)
	{
		if(skill)mSkillText->setCaption(skill->name+" x"+StringConverter::toString(skill->stock));
		else mSkillText->setCaption("Attack");
	}
	const String getSkillText()
	{
		String tText = mSkillText->getCaption();
		if(StringUtil::match(tText,"* x*"))tText.erase(tText.find_first_of(" x"));
		return tText;
	}
	void showPickupText(const String &caption)
	{
		mPickupText->setCaption(caption);
		pickupTextTimeout = 2;
	}
	void toggleShowGUI()
	{
		if(mUnitManager->getShouldNameTagsBeVisible() && isGUIVisible)
		{
			mUnitManager->setShouldNameTagsBeVisible(false);
			return;
		}
		if(!mUnitManager->getShouldNameTagsBeVisible() && isGUIVisible)
		{
			showGUI(false);
			return;
		}
		if(!mUnitManager->getShouldNameTagsBeVisible() && !isGUIVisible)
		{
			if(!mGameStateManager->isCampaign())mUnitManager->setShouldNameTagsBeVisible(true);
			else
			{
				showGUI(true);
			}
			return;
		}
		if(mUnitManager->getShouldNameTagsBeVisible() && !isGUIVisible)
		{
			showGUI(true);
			return;
		}
		
	}
	void showGUI(bool flag)
	{
		if(!flag)
		{
			wasGUIVisible = isGUIVisible;
			wasChatBoxVisible = mBox[GUI_TEXTOUTPUT]->isVisible();
		}
		isGUIVisible = flag;
		if(!mGameStateManager->isCampaign())
		{
			showChatOverlay(isGUIVisible,true,(isGUIVisible&&wasChatBoxVisible));
			showMultiplayerOverlay(isGUIVisible);
		}
		if(!flag || mUnitManager->getPlayerTarget())showTargetPanelOverlay(isGUIVisible);
		showHPPanelOverlay(isGUIVisible);
		mMiniMap->show(isGUIVisible);
	}
	bool getWasGUIVisible()
	{
		return wasGUIVisible;
	}
	const String popCommand()
	{
		const String tCommand = commandLine;
		commandLine = "";
		return tCommand;
	}
	const String popCommandParam()
	{
		const String tParam = commandLineParam;
		commandLineParam = "";
		return tParam;
	}
	void setChatChannel(const unsigned char &channel)
	{
		mChatManager->setChannel(channel);
		if(mChatManager->getChannel()==0)
		{
			mButtonText[BUTTON_TEXTINPUT3]->setCaption("Local");
			mBoxText[GUI_TEXTOUTPUT]->setParameter("colour_top",CHAT_LOCAL_COLOUR_TOP);
			mBoxText[GUI_TEXTOUTPUT]->setParameter("colour_bottom",CHAT_LOCAL_COLOUR_BOTTOM);
			mBoxText[GUI_TEXTINPUT]->setParameter("colour_top",CHAT_LOCAL_COLOUR_TOP);
			mBoxText[GUI_TEXTINPUT]->setParameter("colour_bottom",CHAT_LOCAL_COLOUR_BOTTOM);

			mMiniMap->setMapTagUseCharName(!mChatManager->doLocalUsername);
		}
		else if(mChatManager->getChannel()==1)
		{
			mButtonText[BUTTON_TEXTINPUT3]->setCaption("General");
			mBoxText[GUI_TEXTOUTPUT]->setParameter("colour_top",CHAT_GENERAL_COLOUR_TOP);
			mBoxText[GUI_TEXTOUTPUT]->setParameter("colour_bottom",CHAT_GENERAL_COLOUR_BOTTOM);
			mBoxText[GUI_TEXTINPUT]->setParameter("colour_top",CHAT_GENERAL_COLOUR_TOP);
			mBoxText[GUI_TEXTINPUT]->setParameter("colour_bottom",CHAT_GENERAL_COLOUR_BOTTOM);

			mMiniMap->setMapTagUseCharName(mChatManager->doGeneralCharname);
		}
		else
		{
			mButtonText[BUTTON_TEXTINPUT3]->setCaption("Party");
			mBoxText[GUI_TEXTOUTPUT]->setParameter("colour_top",CHAT_PARTY_COLOUR_TOP);
			mBoxText[GUI_TEXTOUTPUT]->setParameter("colour_bottom",CHAT_PARTY_COLOUR_BOTTOM);
			mBoxText[GUI_TEXTINPUT]->setParameter("colour_top",CHAT_PARTY_COLOUR_TOP);
			mBoxText[GUI_TEXTINPUT]->setParameter("colour_bottom",CHAT_PARTY_COLOUR_BOTTOM);

			mMiniMap->setMapTagUseCharName(!mChatManager->doLocalUsername);
		}
		if(mButton[BUTTON_TEXTINPUT3]->getMaterialName()=="GUIMat/ButtonHighlight")mButton[BUTTON_TEXTINPUT3]->setMaterialName("GUIMat/ButtonUp");
	}
	void toggleChatChannel()
	{
		mChatManager->toggleChannel();
		setChatChannel(mChatManager->getChannel());
	}
	void toggleShowHomeBox()
	{
		showHomeBox(!mBox[GUI_HOMEBOX]->isVisible());
	}
	void toggleShowBioBox()
	{
		showBioBox(!mBox[GUI_BIOBOX]->isVisible());
	}
	void toggleShowFriendBox()
	{
		showFriendBox(!mBox[GUI_FRIENDBOX]->isVisible());
	}
	void toggleShowMiniMap()
	{
		mMiniMap->flipVisibility();
	}
	void toggleShowEmoteBox()
	{
		showEmoteBox(!mBox[GUI_EMOTEBOX]->isVisible());
	}
	void toggleShowItemBox()
	{
		showItemBox(!mBox[GUI_ITEMBOX]->isVisible());
	}
	void toggleShowStashBox()
	{
		showStashBox(!mBox[GUI_STASHBOX]->isVisible());
	}
	void toggleShowPartyBox()
	{
		showPartyBox(!mBox[GUI_PARTYBOX]->isVisible());
	}
	void editFriendList(const String &name, bool add)
	{
		if(name=="")return;
		MagixUnit *tUnit = mUnitManager->getByUser(name);
		if(tUnit == mUnitManager->getPlayer())tUnit = 0;

		mDef->editFriendList(name,add);
		refreshFriendList();
		if(tUnit)static_cast<MagixIndexedUnit*>(tUnit)->isFriend = add;
		mChatManager->message( (add?"Added ":"Removed ")+name+" as Friend.");
	}
	void editBlockedList(const String &name, bool add)
	{
		if(name=="")return;
		//Disallow unblocking one not in the blocklist
		if(!add && !mDef->isInBlockedList(name))return;

		MagixUnit *tUnit = mUnitManager->getByUser(name);
		if(tUnit == mUnitManager->getPlayer())tUnit = 0;

		mDef->editBlockedList(name,add);
		refreshFriendList();
		if(tUnit && !static_cast<MagixIndexedUnit*>(tUnit)->isAdmin && !static_cast<MagixIndexedUnit*>(tUnit)->isMod)
		{
			static_cast<MagixIndexedUnit*>(tUnit)->isBlocked = add;
			tUnit->isHidden = add;
			mNetworkManager->sendInfo((add?INFO_YOURBLOCKED:INFO_YOURUNBLOCKED),OwnerToken(static_cast<MagixIndexedUnit*>(tUnit)->getIndex()));
		}
		mChatManager->message( (add?"Added ":"Removed ") +name+(add?" to":" from")+" Blocked List.");
	}
	void setCurrentWindow(OverlayElement *window)
	{
		if(!window)return;
		if(window->getZOrder()!=48700)return;
		if(getParentOverlay(mCurrentWindow))getParentOverlay(mCurrentWindow)->setZOrder(487);
		mCurrentWindow = window;
		if(getParentOverlay(mCurrentWindow))getParentOverlay(mCurrentWindow)->setZOrder(488);
		mHoverWindow = mCurrentWindow;
	}
	Overlay* getParentOverlay(OverlayElement *element)
	{
		if(!element)return 0;
		if(element==mBox[GUI_TEXTINPUT]||element==mBox[GUI_TEXTOUTPUT])return OverlayManager::getSingleton().getByName("GUIOverlay/Chat");
		String tName = element->getName();
		if(tName.length()<4)return 0;
		tName.erase(0,3);
		return OverlayManager::getSingleton().getByName("GUIOverlay"+tName);
	}
	void logOn()
	{
		if(logonUsername=="")
		{
			mAlertBox->showAlert("Enter username",0.5,0.5,1.5);
			return;
		}
		if(logonPassword=="")
		{
			mAlertBox->showAlert("Enter password",0.5,0.5,1.5);
			return;
		}
		isCreateAccount = false;
		isEditAccount = false;
		showLogonOverlay(false);
		showBackdropOverlay(true,true,"Connecting...");
		mNetworkManager->connectMainServer();
	}
	void createAccount()
	{
		if(createAccountInfo[0].length()==0)
		{
			mAlertBox->showAlert("Enter username",0.5,0.5,1.5);
			return;
		}
		if(createAccountInfo[1].length()==0)
		{
			mAlertBox->showAlert("Enter password",0.5,0.5,1.5);
			return;
		}
		if(createAccountInfo[2].length()==0)
		{
			mAlertBox->showAlert("Enter password confirmation",0.5,0.5,1.5);
			return;
		}
		if(createAccountInfo[1]!=createAccountInfo[2])
		{
			mAlertBox->showAlert("Confirm Password mismatch!",0.5,0.5,1.5);
			return;
		}
		if(createAccountInfo[0][0]==' ')
		{
			mAlertBox->showAlert("Username may not start with a space",0.5,0.5,1.5);
			return;
		}
		for(int i=0;i<(int)createAccountInfo[0].length();i++)
		{
			if(createAccountInfo[0][i]=='/'||createAccountInfo[0][i]=='\\'||createAccountInfo[0][i]==':'||
				createAccountInfo[0][i]=='*'||createAccountInfo[0][i]=='?'||createAccountInfo[0][i]=='\"'||
				createAccountInfo[0][i]=='<'||createAccountInfo[0][i]=='>'||createAccountInfo[0][i]=='|'||
				createAccountInfo[0][i]=='\t'||createAccountInfo[0][i]=='\n')
			{
				mAlertBox->showAlert("Username may not contain /\\:*?\"<>|",0.5,0.5,1.5);
				return;
			}
		}
		isCreateAccount = true;
		isEditAccount = false;
		showCreateAccountOverlay(false);
		showBackdropOverlay(true,true,"Connecting...");
		mNetworkManager->connectMainServer();
	}
	void resetCreateAccountFields()
	{
		const String tCaption[MAX_CREATEACCOUNTINFO] = {"Username: ",
														"Password: ",
														"Confirm Password: ",
														"Email: ",
														"Security Question: ",
														"Answer: "};
		for(int i=0;i<MAX_CREATEACCOUNTINFO;i++)
		{
			createAccountInfo[i] = (i==3?"@":"");
			mCreateAccountTextBox[i]->setCaption(tCaption[i]+createAccountInfo[i]);
		}
	}
	void confirmPasswordChange()
	{
		if(editAccountInfo[0].length()==0)
		{
			mAlertBox->showAlert("Enter username",0.5,0.5,1.5);
			return;
		}
		if(editAccountInfo[1].length()==0)
		{
			mAlertBox->showAlert("Enter old password",0.5,0.5,1.5);
			return;
		}
		if(editAccountInfo[2].length()==0)
		{
			mAlertBox->showAlert("Enter new password",0.5,0.5,1.5);
			return;
		}
		if(editAccountInfo[3].length()==0)
		{
			mAlertBox->showAlert("Enter new password confirmation",0.5,0.5,1.5);
			return;
		}
		if(editAccountInfo[2]!=editAccountInfo[3])
		{
			mAlertBox->showAlert("Confirm Password mismatch!",0.5,0.5,1.5);
			return;
		}
		isEditAccount = true;
		isCreateAccount = false;
		showEditAccountOverlay(false);
		showBackdropOverlay(true,true,"Connecting...");
		mNetworkManager->connectMainServer();
	}
	const String getUsername()
	{
		return logonUsername;
	}
	void loadChar()
	{
		if(mCharScreenManager->getNumChars()!=0)
		{
			showBackdropOverlay(true,true,"Loading...");
			mNetworkManager->sendLoadChar(logonUsername,mCharScreenManager->getCharID());
		}
		else
		{
			showBackdropOverlay(true,false);
			mAlertBox->showAlert("Click here to create\na new character",0.70,0.85,5);
		}
	}
	void setBackdrop(const String &name, bool resetSize)
	{
		mBackdrop->setMaterialName(name);
		if(resetSize)
		{
			mBackdrop->setDimensions(1,1);
			mBackdrop->setPosition(0,0);
		}
	}
	void doRoll(const String &input)
	{
		const vector<String>::type tInput = StringUtil::split(input," ",2);
		if(tInput.size()!=2)
		{
			mChatManager->message("Usage: /roll [dice] [sides]");
			return;
		}
		const short tDice = StringConverter::parseInt(tInput[0]);
		const short tSides = StringConverter::parseInt(tInput[1])+1;
		if(Math::Abs(tDice)>100 || Math::Abs(tSides)>101)
		{
			mChatManager->message("Max dice 100, sides 100.");
			return;
		}
		short tTotal = 0;
		for(short i=0; i<tDice; i++)tTotal += (short)Math::RangeRandom(1,tSides);

		const String tCaption = "rolled "+tInput[0]+" of "+tInput[1]+" sides and got a "+StringConverter::toString(tTotal);
		mChatManager->say(mUnitManager,mUnitManager->getPlayer(),tCaption,(mChatManager->getChannel()==0?CHAT_LOCALEVENT:(mChatManager->getChannel()==2?CHAT_PARTYEVENT:CHAT_EVENT)));
		mNetworkManager->sendChat(tCaption,(mChatManager->getChannel()==0?CHAT_LOCALEVENT:(mChatManager->getChannel()==2?CHAT_PARTYEVENT:CHAT_EVENT)));
	}
	void pickupItem(MagixItem *item)
	{
		//Check if item is pickable, if yes, send pickup request
		if(checkItemEquippable(item->getMesh()))mNetworkManager->sendItemPickup(item->getID());
	}
	bool checkItemEquippable(const String &meshName, bool isCreate=false,bool doEquip=false,bool ground=false)
	{
		if(isCreate && mDef->getItemIsPrivate(meshName))
		{
			mChatManager->message("Cannot create private items.");
			return false;
		}
		if(!mDef->itemMeshExists(meshName))
		{
			mChatManager->message("Item doesn't exist.");
			return false;
		}

		if(ground)
		{
			mChatManager->message("Spawned item "+mDef->getItemName(meshName)+".");
			mNetworkManager->sendItemDrop(meshName,mUnitManager->getPlayer()->getPosition().x,mUnitManager->getPlayer()->getPosition().z);
			return true;
		}

		short tSlot = mUnitManager->getNextEmptyItemSlot(mUnitManager->getPlayer());
		if(tSlot==-1)
		{
			mChatManager->message("Cannot equip anymore items.");
			return false;
		}
		else if(doEquip)
		{
			equipItem(meshName);
		}
		return true;
	}
	void equipItem(const String &meshName)
	{
		short tSlot = mUnitManager->getNextEmptyItemSlot(mUnitManager->getPlayer());
		if(tSlot!=-1)
		{
			if(!mGameStateManager->isCampaign())mNetworkManager->sendItemEquip(meshName,tSlot);
			else
			{
				mChatManager->message("Equipped "+mDef->getItemName(meshName)+".");
				mUnitManager->equipItem(mUnitManager->getPlayer(),meshName,tSlot);
			}
		}
	}
	void unequipItem(const unsigned short &iID, bool doDrop=true)
	{
		const String tEquip = mUnitManager->getPlayer()->getEquip(iID);
		if(tEquip=="")return;
		
		if(!mGameStateManager->isCampaign())
		{
			mNetworkManager->sendItemUnequip(iID);
			mUnitManager->unequipItem(mUnitManager->getPlayer(),iID);
			if(doDrop)mNetworkManager->sendItemDrop(tEquip,mUnitManager->getPlayer()->getPosition().x,mUnitManager->getPlayer()->getPosition().z);
		}
		else
		{
			mUnitManager->unequipItem(mUnitManager->getPlayer(),iID);
			if(doDrop)mUnitManager->createItem(mItemManager->getNextEmptyID(mUnitManager->getPlayer()->getIndex()*100),tEquip,mUnitManager->getPlayer()->getPosition().x,mUnitManager->getPlayer()->getPosition().z);
		}
	}
	bool isDoubleJumpOn()
	{
		return isDoubleJumpOnFlag;
	}
	void loadCampaigns(const unsigned short &page)
	{
		campaignList.clear();
		mDef->loadCampaignList(campaignList);
		for(int i=0;i<3;i++)
		{
			if((int)campaignList.size()>(i+campaignPage*3))
			{
				mButtonText[BUTTON_CAMPAIGN1+i]->setCaption(campaignList[(i+campaignPage*3)]);
				mButton[BUTTON_CAMPAIGN1+i]->show();
			}
			else mButton[BUTTON_CAMPAIGN1+i]->hide();
		}
		if((((int)campaignList.size()-1)/3)>campaignPage)mButton[BUTTON_CAMPAIGNNEXT]->show();
		else mButton[BUTTON_CAMPAIGNNEXT]->hide();
		OverlayManager::getSingleton().getOverlayElement("GUI/CampaignText2")->setCaption("Page "+StringConverter::toString(campaignPage+1));
	}
	void toggleCheckBox(OverlayElement *checkBox)
	{
		if(!checkBox)return;
		if(checkBox->getMaterialName()!="GUIMat/CheckBoxTrue" && checkBox->getMaterialName()!="GUIMat/CheckBoxFalse")return;
		if(isCheckBoxTrue(checkBox))checkBox->setMaterialName("GUIMat/CheckBoxFalse");
		else checkBox->setMaterialName("GUIMat/CheckBoxTrue");
	}
	bool isCheckBoxTrue(OverlayElement *checkBox)
	{
		if(!checkBox)return false;
		return (checkBox->getMaterialName()=="GUIMat/CheckBoxTrue");
	}
	void party(MagixUnit *unit)
	{
		if(!unit || !unit->isIndexedUnit())return;
		if(mUnitManager->hasParty())
		{
			if(mUnitManager->isPartyFull())
			{
				mChatManager->message("Party is full.");
				return;
			}
			mNetworkManager->sendInfo(INFO_PARTYINVITE,static_cast<MagixIndexedUnit*>(unit)->getIndex());
			mChatManager->message("Inviting "+unit->getUser()+" to join party.",CHAT_PARTY);
		}
		else
		{
			mNetworkManager->sendInfo(INFO_PARTYREQUEST,static_cast<MagixIndexedUnit*>(unit)->getIndex());
			mChatManager->message("Requesting to join "+unit->getUser()+"'s party.",CHAT_PARTY);
		}
	}
};

#endif
