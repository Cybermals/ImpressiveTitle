#pragma once

#include "GameConfig.h"
#include "MagixExternalDefinitions.h"
#include "MagixGameStateManager.h"
#include "MagixUnitManager.h"
#include "MagixInputManager.h"
#include "MagixNetworkManager.h"
//#include "MagixCritterManager.h"
#include "MagixCamera.h"
#include "MagixChatManager.h"
#include "MagixAlertBox.h"
#include "MagixCharScreenManager.h"
#include "MagixSoundManager.h"
#include "MagixMiniMap.h"
#include "MagixConst.h"

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
	inline std::wstring atow(const std::string& str)
	{
		std::wstringstream wstrm;
		wstrm << str.c_str();
		return wstrm.str();
	}
	MagixGUI();
	~MagixGUI();
	void initialize(SceneManager *sceneMgr, RenderWindow *window, MagixExternalDefinitions *def, MagixGameStateManager *gameStateMgr, MagixItemManager *itemManager, 
		MagixCritterManager *critterMgr, MagixUnitManager *unitManager, MagixNetworkManager *networkManager, MagixCamera *camera, MagixSoundManager *soundMgr, 
		MagixEffectsManager *effectsMgr, MagixCollisionManager *collisionMgr);

	void showFaderOverlay(bool flag);

	void showPseudoShaderOverlay(bool flag);
	void showCursorOverlay(bool flag);
	void showBackdropOverlay(bool flag, bool showLoadingText = false, const String &loadingCaption = "Loading...");
	void showHPPanelOverlay(bool flag);
	void showTargetPanelOverlay(bool flag);
	void showOptionsOverlay(bool flag);
	void showStartScreenOverlay(bool flag);
	void showCampaignOverlay(bool flag);
	void showLogonOverlay(bool flag);
	void showCreateAccountOverlay(bool flag);
	void showEditAccountOverlay(bool flag);
	void showCharScreenOverlay(bool flag);
	void showServerOverlay(bool flag);
	void showMultiplayerOverlay(bool flag);
	void showHomeBox(bool flag);
	void showFriendBox(bool flag);
	void showEmoteBox(bool flag);
	void showItemBox(bool flag);
	void showStashBox(bool flag);
	void showPartyBox(bool flag, bool showAcceptDenyButtons = false);
	void showBioBox(bool flag);
	void showTargetBioBox(bool flag);
	void showListSelectBox(bool flag);
	void showChatOverlay(bool flag, bool showChat = true, bool bShowChatBox = false);
	void showCreditsOverlay(bool flag, bool showCreditsCaption = true, bool showGameOverButtons = false);
	void scrollMouse(int x, int y, int winWidth, int winHeight, bool setPosition = false);
	void showMouse(bool flag);
	const Real getMouseX();
	const Real getMouseY();
	void reset();
	void update(const FrameEvent &evt);
	void updatePlayer(const FrameEvent &evt);
	void setWounded(bool flag);
	void updateNetworkMessage();
	void updateCompositors();
	void updateTargetIndicator();
	void updateFader(const FrameEvent &evt);
	void updateCinematicBars(const FrameEvent &evt);
	void updateCinematicCaption(const FrameEvent &evt);
	void updateHPPanel(const FrameEvent &evt);
	void updateTargetPanel(const FrameEvent &evt);
	void updateLogo(const FrameEvent &evt);
	bool skipLogo();
	void resetLogo();
	void updateHomeBox();
	void updateFriendBox();
	void updateBioBox();
	void updateTargetBioBox();
	void updateEmoteBox();
	void updateItemBox();
	void updateStashBox();
	void updatePartyBox();
	void updateSelectPanel(const FrameEvent &evt);
	void updateListSelectBox();
	void refreshFriendList();
	void refreshBio();
	void refreshTargetBio(String bio, const String &name = "");
	void resetCampaignScreen();
	void updateCharScreen(const FrameEvent &evt);
	void updateChatBox(const FrameEvent &evt);
	void updateHover();
	bool isCursorOverButton(OverlayElement *button, bool ignoreParent = false);
	bool isCursorOverWindow(OverlayElement *window);
	void setLeftClick(bool flag);
	void setRightClick(bool flag);
	void mouseLeftDown();
	void mouseLeftUp();
	void mouseRightDown();
	void mouseRightUp();
	const String getDebugText();
	MagixInputManager* getInputManager();
	void toggleInputMode(bool isEnter, OverlayElement *inputBox = 0, String defaultCaption = "Say: ", unsigned short charLimit = 256, bool clearAtReturn = true, String startText = "", bool allowTempStorage = true, bool allowNewLine = false, bool hideText = false);
	bool selectCurrentObject(SceneNode *object, const unsigned int &flags = 0, bool isRightClick = false);
	void processSelectBoxClick();
	void showSelectPanel(bool flag, const String &name = "Target");
	MagixChatManager* getChatManager();
	MagixAlertBox* getAlertBox();
	MagixCharScreenManager* getCharScreenManager();
	MagixMiniMap* getMiniMap();
	void sizeWindowDrag(unsigned short sizeDir, const Real &minWidth = 0, const Real &minHeight = 0);
	void doAttack(MagixLiving *target = 0);
	void doInGameLeftClick();
	void doRayPicking(bool isRightClick);
	void doScroll(bool flag, short iID = 0);
	void doMove(bool flag);
	const Real getScroll(unsigned short iID);
	void setScroll(unsigned short iID, Real ratio);
	void resizeScrollbarByParent(OverlayElement *parent, const Real &dwidth, const Real &dheight);
	const short getScrollerID(OverlayElement *scroller);
	void updateBoxScrollerCaption(const unsigned short &boxID, const unsigned short &scrollerID, const vector<String>::type &captionList);
	const short getListButtonLine(const unsigned short &boxID, const unsigned short &scrollerID, const unsigned short &listLines, Real &fontHeight, short &apparentLine);
	const short getListButtonLine(const unsigned short &boxID, Real &fontHeight, short &apparentLine);
	OverlayElement* getMoverWindow(OverlayElement *mover);
	void toggleShowOptions();
	bool isOptionsVisible();
	void showChatBox(bool flag, bool showInput = true);
	void initializeSettings();
	void saveSettings();
	void toggleIsTablet(bool showAlert = false);
	bool isTablet();
	void toggleShadowsOn();
	bool isShadowsOn();
	void toggleIsBloom();
	void toggleMotionBlur();
	void togglePagedGeometryOn();
	void toggleUnderwaterShader();
	void toggleDoChannelBlink();
	void toggleDoGeneralCharname();
	void toggleDoLocalUsername();
	void toggleRunMode();
	void toggleDoubleJumpOn();
	void toggleAutoLipSync();
	void toggleUseWindowsCursor(bool dontShowAlert = false);
	bool getUseWindowsCursor();
	bool isBloom();
	bool isMotionBlur();
	const Real getMouseSensitivity();
	void fadeIn(const Real &duration, const ColourValue &colour = ColourValue(0, 0, 0));
	void fadeOut(const Real &duration, const ColourValue &colour = ColourValue(0, 0, 0));
	void showCinematicBars(bool flag, bool skipTransition = false);
	void showCinematicCaption(const Real &duration, const String &caption);
	void showCreditsCaption(const String &caption);
	void updateSkillText(const Skill *skill);
	const String getSkillText();
	void showPickupText(const String &caption);
	void toggleShowGUI();
	void showGUI(bool flag);
	bool getWasGUIVisible();
	const String popCommand();
	const String popCommandParam();
	void setChatChannel(const unsigned char &channel);
	void toggleChatChannel();
	void toggleShowHomeBox();
	void toggleShowBioBox();
	void toggleShowFriendBox();
	void toggleShowMiniMap();
	void toggleShowEmoteBox();
	void toggleShowItemBox();
	void toggleShowStashBox();
	void toggleShowPartyBox();
	void editFriendList(const String &name, bool add);
	void editBlockedList(const String &name, bool add);
	void setCurrentWindow(OverlayElement *window);
	Overlay* getParentOverlay(OverlayElement *element);
	void logOn();
	void createAccount();
	void resetCreateAccountFields();
	void confirmPasswordChange();
	const String getUsername();
	void loadChar();
	void setBackdrop(const String &name, bool resetSize);
	void doRoll(const String &input);
	void pickupItem(MagixItem *item);
	bool checkItemEquippable(const String &meshName, bool isCreate = false, bool doEquip = false, bool ground = false);
	void equipItem(const String &meshName);
	void unequipItem(const unsigned short &iID, bool doDrop = true);
	bool isDoubleJumpOn();
	void loadCampaigns(const unsigned short &page);
	void toggleCheckBox(OverlayElement *checkBox);
	bool isCheckBoxTrue(OverlayElement *checkBox);
	void party(MagixUnit *unit);
};
