#ifndef __MagixCharScreenManager_h_
#define __MagixCharScreenManager_h_

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
	void initialize(MagixExternalDefinitions *def, MagixEffectsManager *effectsMgr, MagixAlertBox *alertBox, MagixUnit *unit, SceneManager *sceneMgr, OverlayElement *name, OverlayElement *backdrop)
	{
		mDef = def;
		mEffectsManager = effectsMgr;
		mAlertBox = alertBox;
		mUnit = unit;
		mSceneMgr = sceneMgr;
		mName = name;
		mBackdrop = backdrop;
		mColourBox = OverlayManager::getSingleton().getOverlayElement("GUI/CharScreenColourBox");
	}
	void initializeText(const unsigned short &i, OverlayElement *text)
	{
		mText[i] = text;
	}
	void initializeBackNextButtons(OverlayElement *back, OverlayElement *next, OverlayElement *del, OverlayElement *backText, OverlayElement *nextText, OverlayElement *delText)
	{
		mBackButton = back;
		mNextButton = next;
		mDeleteButton = del;
		mBackButtonText = backText;
		mNextButtonText = nextText;
		mDeleteButtonText = delText;
	}
	void initializeLRButtons(const unsigned short &i, OverlayElement *lButton, OverlayElement *rButton)
	{
		mLButton[i] = lButton;
		mRButton[i] = rButton;
	}
	void initializeSliders(const unsigned short &i, OverlayElement *slider, OverlayElement *sliderMarker)
	{
		mSlider[i] = slider;
		mSliderMarker[i] = sliderMarker;
	}
	void reset(bool load=false)
	{
		startGameCount = 0;
		isRotateUnit = false;
		dYaw = Degree(0);
		camZoom = 0;
		page = PAGE_SELECT;
		charID = 0;
		mName->setCaption("Name: ");
		//isMale = true;
		maneID = 0;
		headID = 0;
		tailID = 0;
		wingID = 0;
		tuftID = 0;
		bodyMarkID = 0;
		headMarkID = 0;
		tailMarkID = 0;
		colourPartID = 0;
		colourVal[0] = ColourValue(0.45,0.35,0.25); //Pelt
		colourVal[1] = ColourValue(0.65,0.55,0.35); //Underfur
		colourVal[2] = ColourValue(0.2,0.15,0); //Mane
		colourVal[3] = ColourValue(0.02,0.02,0.02); //Nose
		colourVal[4] = ColourValue(0.25,0.15,0.05); //Above eyes
		colourVal[5] = ColourValue(0.55,0.45,0.25); //Below eyes
		colourVal[6] = ColourValue(0.25,0.25,0.05); //Ears
		colourVal[7] = ColourValue(0.2,0.15,0); //Tailtip
		colourVal[8] = ColourValue(0.55,0.3,0.15); //Eyes
		colourVal[9] = ColourValue(1,1,1); //Wings
		colourVal[10] = ColourValue(0,0,0); //Markings
		//if(load)loadChar(0);
		isCharIDChanged = false;
		isEditMode = false;
		doReequip = false;
		deleteConfirmation = "";
		refreshPage();
	}
	const unsigned short buttonClicked(OverlayElement *button)
	{
		bool updateGUI = false;
		if(button==mDeleteButton)
		{
			if(page==PAGE_SELECT)
			{
				page = PAGE_EDIT;
				refreshPage();
				positionUnitForEdit();
				return 0;
			}
			if(page==PAGE_EDIT)
			{
				if(deleteConfirmation=="")
				{
					deleteConfirmation = getName();
					setName("");
					mName->setPosition(0.05,0.5);
					mName->show();
					mAlertBox->showAlert("Enter character name to confirm delete",mName->getLeft()+0.25,mName->getTop()+mName->getHeight()+0.05);
					return 0;
				}
				if(deleteConfirmation!=getName())
				{
					mAlertBox->showAlert("Character name mismatch",mName->getLeft()+0.25,mName->getTop()+mName->getHeight()+0.05);
					return 0;
				}
				const short tID = charID;
				reset();
				charID = tID;
				return CHARSCREEN_DELETECHAR;
			}
			/*if(page==PAGE_CREATENEW)
			{
				page = PAGE_PRESET1;
				createDefaultUnit();
				loadPreset();
			}*/

			return (updateGUI?CHARSCREEN_UPDATEGUI:0);
		}

		unsigned short buttonNo = 0;
		bool buttonIsRight = false;

		for(int i=0;i<MAX_CHARSCREENTEXT;i++)
		{
			if(button==mLButton[i])
			{
				buttonNo = i;
				buttonIsRight = false;
				break;
			}
			if(button==mRButton[i])
			{
				buttonNo = i;
				buttonIsRight = true;
				break;
			}
		}

		if(page==PAGE_SELECT)
		{
			if(numChars!=0)
			{
				if(buttonIsRight)charID += 1;
				else charID -= 1;
				if(charID<0)charID = numChars;
				if(charID>numChars)charID = 0;
				//loadChar(charID);
				//refreshPage();
				isCharIDChanged = true;
				if(charID!=numChars)
				{
					mNextButton->hide();
					mDeleteButton->hide();
				}
				else 
				{
					updateChar("");
					refreshPage();
				}
			}
		}
		else if(page==PAGE_CUSTOM1||page==PAGE_PRESET1)
		{
			changeTrait(buttonNo,buttonIsRight);
		}
		else if(page==PAGE_CUSTOM2||page==PAGE_PRESET2)
		{
			changeTrait(buttonNo+4,buttonIsRight);
		}
		else if(page==PAGE_CUSTOM3)
		{
			do
			{
				if(buttonIsRight)colourPartID += 1;
				else colourPartID -= 1;
				if(colourPartID<0)colourPartID = MAX_COLOURS-1;
				if(colourPartID>=MAX_COLOURS)colourPartID = 0;
			}while(colourPartID==9 && wingID==0  || colourPartID==2 && maneID==0&&tuftID==0 || colourPartID==10 && bodyMarkID==0&&headMarkID==0&&tailMarkID==0);
			refreshPage();
			updateGUI = true;
		}

		return (updateGUI?CHARSCREEN_UPDATEGUI:0);;
	}
	void changeTrait(const unsigned short &iID, bool increment)
	{
		//head
		if(iID==0)
		{
			headID += (increment?1:-1);
			if(headID<0)headID = mDef->maxHeads-1;
			if(headID>=mDef->maxHeads)headID = 0;
			applyRestrictions("Head",increment);
			mUnit->replaceHeadMesh(mSceneMgr,meshName("Head",headID),(page==PAGE_PRESET1||page==PAGE_PRESET2));
			doReequip = true;
		}
		//mane
		else if(iID==1)
		{
			maneID += (increment?1:-1);
			if(maneID<0)maneID = mDef->maxManes-1;
			if(maneID>=mDef->maxManes)maneID = 0;
			applyRestrictions("Mane",increment);
			mUnit->replaceManeMesh(mSceneMgr,meshName("Mane",maneID),(page==PAGE_PRESET1||page==PAGE_PRESET2));
		}
		//tail
		else if(iID==2)
		{
			tailID += (increment?1:-1);
			if(tailID<0)tailID = mDef->maxTails-1;
			if(tailID>=mDef->maxTails)tailID = 0;
			mUnit->replaceTailMesh(mSceneMgr,meshName("Tail",tailID),(page==PAGE_PRESET1||page==PAGE_PRESET2));
		}
		//wings
		else if(iID==3)
		{
			wingID += (increment?1:-1);
			if(wingID<0)wingID = mDef->maxWings-1;
			if(wingID>=mDef->maxWings)wingID = 0;
			mUnit->replaceWingMesh(mSceneMgr,meshName("Wing",wingID),(page==PAGE_PRESET1||page==PAGE_PRESET2));
		}
		//tuft
		else if(iID==4)
		{
			tuftID += (increment?1:-1);
			if(tuftID<0)tuftID = mDef->maxTufts-1;
			if(tuftID>=mDef->maxTufts)tuftID = 0;
			applyRestrictions("Tuft",increment);
			mUnit->replaceTuftMesh(mSceneMgr,meshName("Tuft",tuftID),(page==PAGE_PRESET1||page==PAGE_PRESET2));
		}
		//body mark
		else if(iID==5)
		{
			bodyMarkID += (increment?1:-1);
			if(bodyMarkID<0)bodyMarkID = mDef->maxBodyMarks-1;
			if(bodyMarkID>=mDef->maxBodyMarks)bodyMarkID = 0;
			mUnit->setMarkings(bodyMarkID,headMarkID,tailMarkID);
		}
		//head mark
		else if(iID==6)
		{
			headMarkID += (increment?1:-1);
			if(headMarkID<0)headMarkID = mDef->maxHeadMarks-1;
			if(headMarkID>=mDef->maxHeadMarks)headMarkID = 0;
			mUnit->setMarkings(bodyMarkID,headMarkID,tailMarkID);
		}
		//tail mark
		else if(iID==7)
		{
			tailMarkID += (increment?1:-1);
			if(tailMarkID<0)tailMarkID = mDef->maxTailMarks-1;
			if(tailMarkID>=mDef->maxTailMarks)tailMarkID = 0;
			mUnit->setMarkings(bodyMarkID,headMarkID,tailMarkID);
		}
		refreshPage();
	}
	const String meshName(const String &type, const unsigned short &iID)
	{
		if(type=="Mane")return mDef->maneMesh[iID];
		if(type=="Head")return mDef->headMesh[iID];
		if(type=="Tail")return mDef->tailMesh[iID];
		if(type=="Wing")return mDef->wingMesh[iID];
		if(type=="Tuft")return mDef->tuftMesh[iID];
		if(type=="BodyMark")return (iID==0?"No Markings":"BodyMark"+StringConverter::toString(iID));
		if(type=="HeadMark")return (iID==0?"No Markings":"HeadMark"+StringConverter::toString(iID));
		if(type=="TailMark")return (iID==0?"No Markings":"TailMark"+StringConverter::toString(iID));

		return "";
	}
	const String partName(const unsigned short &iID)
	{
		const String tPart[MAX_COLOURS] = {"Pelt","Underfur","Mane","Nose","Above Eyes","Below Eyes","Ears","Tailtip","Eyes","Wings","Markings"};
		return tPart[iID];
	}
	void applyRestrictions(const String &partChanged, bool increment)
	{
		if(partChanged=="Head")
		{
			//back to maneless
			maneID = 0;
			mUnit->replaceManeMesh(mSceneMgr,meshName("Mane",maneID));
			return;
		}
		if(partChanged=="Mane")
		{
			while(mDef->isRestricted(headID,maneID))
			{
				maneID += (increment?1:-1);
				if(maneID<0)maneID = mDef->maxManes-1;
				if(maneID>=mDef->maxManes)maneID = 0;
			}
			return;
		}
		if(partChanged=="Tuft")
		{
			while(mDef->isRestrictedTuft(tuftID))
			{
				tuftID += (increment?1:-1);
				if(tuftID<0)tuftID = mDef->maxTufts-1;
				if(tuftID>=mDef->maxTufts)tuftID = 0;
			}
			return;
		}
	}
	void refreshPage()
	{
		//Char select page
		if(page==PAGE_SELECT)
		{
			if(mName->isVisible())mName->hide();
			if(!mText[0]->isVisible())mText[0]->show();
			mText[0]->setCaption(getName());
			mText[0]->setPosition(0.45,0.02);

			for(int i=1;i<MAX_CHARSCREENTEXT;i++)
			{
				if(mLButton[i]->isVisible())mLButton[i]->hide();
				if(mRButton[i]->isVisible())mRButton[i]->hide();
				if(mText[i]->isVisible())mText[i]->hide();
			}
			if(!mLButton[0]->isVisible())mLButton[0]->show();
			if(!mRButton[0]->isVisible())mRButton[0]->show();
			mLButton[0]->setPosition(0.01,0.05);
			mRButton[0]->setPosition(0.85,0.05);
			for(int i=0;i<3;i++)
			{
				if(mSlider[i]->isVisible())mSlider[i]->hide();
				if(mSliderMarker[i]->isVisible())mSliderMarker[i]->hide();
			}
			if(mColourBox->isVisible())mColourBox->hide();
			if(!mBackdrop->isVisible())mBackdrop->show();
			mBackdrop->setMaterialName("GUIMat/Backdrop2");
			mName->getParent()->setMaterialName("GUIMat/None");
			mBackButton->setPosition(0.01,0.85);
			if(getName()!="")
			{
				if(!mNextButton->isVisible())mNextButton->show();
				mNextButtonText->setCaption("Start");
				mNextButton->setPosition(0.38,0.85);
				if(!mDeleteButton->isVisible())mDeleteButton->show();
				mDeleteButton->setPosition(0.85-mDeleteButton->getWidth()+mRButton[0]->getWidth(),0.85);
				mDeleteButtonText->setCaption("Edit");
			}
			else
			{
				if(charID==numChars)
				{
					if(!mNextButton->isVisible())mNextButton->show();
					mNextButtonText->setCaption("New");
					mNextButton->setPosition(0.85-mNextButton->getWidth()+mRButton[0]->getWidth(),0.85);
				}
				else if(mNextButton->isVisible())mNextButton->hide();
				if(mDeleteButton->isVisible())mDeleteButton->hide();
			}
			return;
		}
		//Create new/method selection page
		/*if(page==PAGE_CREATENEW)
		{
			if(mName->isVisible())mName->hide();
			if(!mText[0]->isVisible())mText[0]->show();
			mText[0]->setPosition(0.25,0.05);
			mText[0]->setCaption("Create new\ncharacter");
			for(int i=0;i<MAX_CHARSCREENTEXT;i++)
			{
				if(mLButton[i]->isVisible())mLButton[i]->hide();
				if(mRButton[i]->isVisible())mRButton[i]->hide();
				if(i!=0 && mText[i]->isVisible())mText[i]->hide();
			}
			for(int i=0;i<3;i++)
			{
				if(mSlider[i]->isVisible())mSlider[i]->hide();
				if(mSliderMarker[i]->isVisible())mSliderMarker[i]->hide();
			}
			if(mColourBox->isVisible())mColourBox->hide();
			if(mBackdrop->isVisible())mBackdrop->hide();
			mName->getParent()->setMaterialName("GUIMat/StartScreen");
			mBackButton->setPosition(0.05,0.6);
			mNextButton->setPosition(0.18,0.30);
			mNextButtonText->setCaption("Custom");
			if(!mDeleteButton->isVisible())mDeleteButton->show();
			mDeleteButton->setPosition(0.18,0.45);
			mDeleteButtonText->setCaption("Preset");
			return;
		}*/
		//Mesh page 1
		if(page==PAGE_CUSTOM1)
		{
			if(!mName->isVisible())mName->show();
			mName->setPosition(0.05,0.1);
			mText[0]->setPosition(0.25,0.2);
			mText[0]->setCaption(meshName("Head",headID));
			mText[1]->setCaption(meshName("Mane",maneID));
			mText[2]->setCaption(meshName("Tail",tailID));
			mText[3]->setCaption(meshName("Wing",wingID));

			for(int i=0;i<MAX_CHARSCREENTEXT;i++)
			{
				if(!mLButton[i]->isVisible())mLButton[i]->show();
				if(!mRButton[i]->isVisible())mRButton[i]->show();
				if(!mText[i]->isVisible())mText[i]->show();
				mText[i]->setLeft(0.25);
			}
			mLButton[0]->setPosition(0.05,0.2);
			mRButton[0]->setPosition(0.4,0.2);
			for(int i=0;i<3;i++)
			{
				if(mSlider[i]->isVisible())mSlider[i]->hide();
				if(mSliderMarker[i]->isVisible())mSliderMarker[i]->hide();
			}
			if(mColourBox->isVisible())mColourBox->hide();
			mUnit->setColours(colourVal[0],colourVal[1],colourVal[2],colourVal[3],colourVal[4],colourVal[5],colourVal[6],colourVal[7],colourVal[8],colourVal[9],colourVal[10]);
			if(mBackdrop->isVisible())mBackdrop->hide();
			mName->getParent()->setMaterialName("GUIMat/StartScreen");
			mBackButton->setPosition(0.05,0.6);
			mNextButton->setPosition(0.3,0.6);
			mNextButtonText->setCaption("Next");
			if(mDeleteButton->isVisible())mDeleteButton->hide();
			return;
		}
		//Mesh page 2
		if(page==PAGE_CUSTOM2)
		{
			if(mName->isVisible())mName->hide();
			mText[0]->setPosition(0.25,0.2);
			mText[0]->setCaption(meshName("Tuft",tuftID));
			mText[1]->setCaption(meshName("BodyMark",bodyMarkID));
			mText[2]->setCaption(meshName("HeadMark",headMarkID));
			mText[3]->setCaption(meshName("TailMark",tailMarkID));

			for(int i=0;i<MAX_CHARSCREENTEXT;i++)
			{
				if(!mLButton[i]->isVisible())mLButton[i]->show();
				if(!mRButton[i]->isVisible())mRButton[i]->show();
				if(!mText[i]->isVisible())mText[i]->show();
				mText[i]->setLeft(0.25);
			}
			mLButton[0]->setPosition(0.05,0.2);
			mRButton[0]->setPosition(0.4,0.2);
			for(int i=0;i<3;i++)
			{
				if(mSlider[i]->isVisible())mSlider[i]->hide();
				if(mSliderMarker[i]->isVisible())mSliderMarker[i]->hide();
			}
			if(mColourBox->isVisible())mColourBox->hide();
			mUnit->setColours(colourVal[0],colourVal[1],colourVal[2],colourVal[3],colourVal[4],colourVal[5],colourVal[6],colourVal[7],colourVal[8],colourVal[9],colourVal[10]);
			if(mBackdrop->isVisible())mBackdrop->hide();
			mName->getParent()->setMaterialName("GUIMat/StartScreen");
			mBackButton->setPosition(0.05,0.6);
			mNextButton->setPosition(0.3,0.6);
			mNextButtonText->setCaption("Next");
			if(mDeleteButton->isVisible())mDeleteButton->hide();
			return;
		}
		//Colour page
		if(page==PAGE_CUSTOM3)
		{
			if(mName->isVisible())mName->hide();
			if(!mText[0]->isVisible())mText[0]->show();
			mText[0]->setCaption(partName(colourPartID));
			mText[0]->setPosition(0.25,0.1);
			mText[1]->setCaption("R: "+StringConverter::toString(int(colourVal[colourPartID].r*255)));
			mText[2]->setCaption("G: "+StringConverter::toString(int(colourVal[colourPartID].g*255)));
			mText[3]->setCaption("B: "+StringConverter::toString(int(colourVal[colourPartID].b*255)));

			for(int i=1;i<MAX_CHARSCREENTEXT;i++)
			{
				if(mLButton[i]->isVisible())mLButton[i]->hide();
				if(mRButton[i]->isVisible())mRButton[i]->hide();
				if(!mText[i]->isVisible())mText[i]->show();
				mText[i]->setLeft(0.4);
			}
			if(!mLButton[0]->isVisible())mLButton[0]->show();
			if(!mRButton[0]->isVisible())mRButton[0]->show();
			mLButton[0]->setPosition(0.05,0.1);
			mRButton[0]->setPosition(0.4,0.1);
			for(int i=0;i<3;i++)
			{
				if(!mSlider[i]->isVisible())mSlider[i]->show();
				if(!mSliderMarker[i]->isVisible())mSliderMarker[i]->show();
			}
			if(!mColourBox->isVisible())mColourBox->show();
			mColourBox->setCustomParameter(1,Vector4(colourVal[colourPartID].r,colourVal[colourPartID].g,colourVal[colourPartID].b,1));
			#pragma warning(push)
			#pragma warning(disable : 4482)
			if(mColourBox->getTechnique()->getName()=="2")
				mColourBox->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
																										LayerBlendSource::LBS_MANUAL,
																										LayerBlendSource::LBS_CURRENT,
																										ColourValue(colourVal[colourPartID].r,colourVal[colourPartID].g,colourVal[colourPartID].b));
			#pragma warning(pop)
			mUnit->setColours(colourVal[0],colourVal[1],colourVal[2],colourVal[3],colourVal[4],colourVal[5],colourVal[6],colourVal[7],colourVal[8],colourVal[9],colourVal[10]);
			if(mBackdrop->isVisible())mBackdrop->hide();
			mName->getParent()->setMaterialName("GUIMat/StartScreen");
			mBackButton->setPosition(0.05,0.6);
			mNextButton->setPosition(0.3,0.6);
			mNextButtonText->setCaption("Next");
			return;
		}
		//Traits page
		if(page==PAGE_CUSTOM4)
		{
			mText[0]->setCaption("Size");
			mText[0]->setPosition(0.25,0.1);
			mText[1]->setCaption(StringConverter::toString(mUnit->getObjectNode()->getScale().x,3));
			mText[1]->setLeft(0.4);

			for(int i=0;i<MAX_CHARSCREENTEXT;i++)
			{
				mLButton[i]->hide();
				mRButton[i]->hide();
				if(i>1)mText[i]->hide();
			}
			for(int i=1;i<3;i++)
			{
				mSlider[i]->hide();
				mSliderMarker[i]->hide();
			}
			mColourBox->hide();
			if(mBackdrop->isVisible())mBackdrop->hide();
			mName->getParent()->setMaterialName("GUIMat/StartScreen");
			mBackButton->setPosition(0.05,0.6);
			mNextButton->setPosition(0.3,0.6);
			mNextButtonText->setCaption("Finish");
			return;
		}
		//Preset page
		/*if(page==PAGE_PRESET1)
		{
			if(!mName->isVisible())mName->show();
			if(!mText[0]->isVisible())mText[0]->show();
			mText[0]->setPosition(0.25,0.35);

			for(int i=1;i<MAX_CHARSCREENTEXT;i++)
			{
				if(mLButton[i]->isVisible())mLButton[i]->hide();
				if(mRButton[i]->isVisible())mRButton[i]->hide();
				if(mText[i]->isVisible())mText[i]->hide();
			}
			if(!mLButton[0]->isVisible())mLButton[0]->show();
			if(!mRButton[0]->isVisible())mRButton[0]->show();
			mLButton[0]->setPosition(0.05,0.35);
			mRButton[0]->setPosition(0.4,0.35);
			for(int i=0;i<3;i++)
			{
				if(mSlider[i]->isVisible())mSlider[i]->hide();
				if(mSliderMarker[i]->isVisible())mSliderMarker[i]->hide();
			}
			if(mColourBox->isVisible())mColourBox->hide();
			if(mBackdrop->isVisible())mBackdrop->hide();
			mName->getParent()->setMaterialName("GUIMat/StartScreen");
			mBackButton->setPosition(0.05,0.6);
			mNextButton->setPosition(0.3,0.6);
			mNextButtonText->setCaption("Finish");
			if(mDeleteButton->isVisible())mDeleteButton->hide();
			return;
		}*/
		//Edit Page
		if(page==PAGE_EDIT)
		{
			if(mName->isVisible())mName->hide();
			if(!mText[0]->isVisible())mText[0]->show();
			mText[0]->setPosition(0.25,0.05);
			mText[0]->setCaption("Edit Character");
			for(int i=0;i<MAX_CHARSCREENTEXT;i++)
			{
				if(mLButton[i]->isVisible())mLButton[i]->hide();
				if(mRButton[i]->isVisible())mRButton[i]->hide();
				if(i!=0 && mText[i]->isVisible())mText[i]->hide();
			}
			for(int i=0;i<3;i++)
			{
				if(mSlider[i]->isVisible())mSlider[i]->hide();
				if(mSliderMarker[i]->isVisible())mSliderMarker[i]->hide();
			}
			if(mColourBox->isVisible())mColourBox->hide();
			if(mBackdrop->isVisible())mBackdrop->hide();
			mName->getParent()->setMaterialName("GUIMat/StartScreen");
			mBackButton->setPosition(0.05,0.6);
			mNextButton->setPosition(0.18,0.30);
			mNextButtonText->setCaption("Edit");
			if(!mDeleteButton->isVisible())mDeleteButton->show();
			mDeleteButton->setPosition(0.18,0.45);
			mDeleteButtonText->setCaption("Delete");
			return;
		}
		//Preset page (edit)
		if(page==PAGE_PRESET1)
		{
			if(!mName->isVisible())mName->show();
			mName->setPosition(0.05,0.1);
			if(!mText[0]->isVisible())mText[0]->show();
			/*mText[0]->setCaption("Size");
			mText[0]->setPosition(0.25,0.2);
			mText[1]->setCaption(StringConverter::toString(mUnit->getObjectNode()->getScale().x,3));
			mText[1]->setLeft(0.4);

			for(int i=0;i<MAX_CHARSCREENTEXT;i++)
			{
				if(mLButton[i]->isVisible())mLButton[i]->hide();
				if(mRButton[i]->isVisible())mRButton[i]->hide();
				if(i>1 && mText[i]->isVisible())mText[i]->hide();
			}
			mText[1]->show();
			for(int i=1;i<3;i++)
			{
				if(mSlider[i]->isVisible())mSlider[i]->hide();
				if(mSliderMarker[i]->isVisible())mSliderMarker[i]->hide();
			}
			mSlider[0]->show();
			mSliderMarker[0]->show();*/
			mText[0]->setPosition(0.25,0.2);
			mText[0]->setCaption(meshName("Head",headID));
			mText[1]->setCaption(meshName("Mane",maneID));
			mText[2]->setCaption(meshName("Tail",tailID));
			mText[3]->setCaption(meshName("Wing",wingID));

			for(int i=0;i<MAX_CHARSCREENTEXT;i++)
			{
				if(!mLButton[i]->isVisible())mLButton[i]->show();
				if(!mRButton[i]->isVisible())mRButton[i]->show();
				if(!mText[i]->isVisible())mText[i]->show();
				mText[i]->setLeft(0.25);
			}
			mLButton[0]->setPosition(0.05,0.2);
			mRButton[0]->setPosition(0.4,0.2);
			for(int i=0;i<3;i++)
			{
				if(mSlider[i]->isVisible())mSlider[i]->hide();
				if(mSliderMarker[i]->isVisible())mSliderMarker[i]->hide();
			}
			if(mColourBox->isVisible())mColourBox->hide();
			if(mBackdrop->isVisible())mBackdrop->hide();
			mName->getParent()->setMaterialName("GUIMat/StartScreen");
			mBackButton->setPosition(0.05,0.6);
			mNextButton->setPosition(0.3,0.6);
			mNextButtonText->setCaption("Next");
			if(mDeleteButton->isVisible())mDeleteButton->hide();
			return;
		}
		//Preset page2 (edit)
		if(page==PAGE_PRESET2)
		{
			if(mName->isVisible())mName->hide();
			mText[0]->setPosition(0.25,0.2);
			mText[0]->setCaption(meshName("Tuft",tuftID));
			mText[1]->setCaption("Size");
			mText[1]->setLeft(0.25);
			mText[2]->setCaption(StringConverter::toString(mUnit->getObjectNode()->getScale().x,3));
			mText[2]->setLeft(0.4);

			for(int i=1;i<MAX_CHARSCREENTEXT;i++)
			{
				mLButton[i]->hide();
				mRButton[i]->hide();
				if(i>2)mText[i]->hide();
			}
			mSlider[1]->show();
			mSliderMarker[1]->show();
			for(int i=0;i!=1&&i<3;i++)
			{
				mSlider[i]->hide();
				mSliderMarker[i]->hide();
			}
			if(mColourBox->isVisible())mColourBox->hide();
			if(mBackdrop->isVisible())mBackdrop->hide();
			mName->getParent()->setMaterialName("GUIMat/StartScreen");
			mBackButton->setPosition(0.05,0.6);
			mNextButton->setPosition(0.3,0.6);
			mNextButtonText->setCaption("Finish");
			if(mDeleteButton->isVisible())mDeleteButton->hide();
			return;
		}
	}
	bool previousPage()
	{
		//back to startscreen
		if(page==PAGE_SELECT)return true;
		//back to char select page
		if(page==PAGE_CUSTOM1 || page==PAGE_PRESET1 || page==PAGE_EDIT)
		{
			if(mUnit->getObjectNode())
			{
				mEffectsManager->destroyParticleByObjectNode(mUnit->getObjectNode());
				mEffectsManager->destroyItemEffectByOwnerNode(mUnit->getObjectNode());
				mUnit->destroyUnit(mSceneMgr);
			}
			if(!isEditMode && page!=PAGE_EDIT)
			{
				reset();
				charID = numChars;
			}
			else
			{
				page = 0;
				const short tID = charID;
				reset();
				charID = tID;
				isCharIDChanged = true;
			}
			positionUnitForSelect();
			refreshPage();
			return false;
		}
		page -= 1;
		refreshPage();
		return false;
	}
	const unsigned short nextPage()
	{
		//start game
		if(page==PAGE_SELECT && getName()!="")
		{
			//startGameCount = 3;
			return CHARSCREEN_STARTGAME;
		}
		//next to custom char page1
		if(page==PAGE_SELECT)
		{
			createDefaultUnit();
			isEditMode = false;
		}
		//Name not valid, pop an alert
		if((page==PAGE_CUSTOM1 || page==PAGE_PRESET1) && (!validateName() || getName().length()==0 || getName()[0]==' '))
		{
			String tAlert = "Must not include : ; | * ? < > / \\";
			if(getName().length()==0)tAlert = "Enter a name";
			else if(getName()[0]==' ')tAlert = "Must not start with a space";
			mAlertBox->showAlert(tAlert,mName->getLeft()+0.25,mName->getTop()+mName->getHeight()+0.05);
			return 0;
		}
		//create char and return to char select page
		if(page==PAGE_CUSTOM4/* || page==PAGE_PRESET1*/)
		{
			/*createChar(charID);
			mUnit->destroyUnit(mSceneMgr);
			mSceneMgr->getCamera("PlayerCam")->setOrientation(Quaternion(Degree(-40),Vector3::UNIT_Y));
			mSceneMgr->getCamera("PlayerCam")->setPosition(3200,120,2770);
			short tCharID = charID;
			reset();
			page = 0;
			charID = tCharID;
			loadChar(charID);
			refreshPage();*/
			return (isEditMode?CHARSCREEN_EDITCHAR:CHARSCREEN_CREATECHAR);
		}
		//edit char
		if(page==PAGE_EDIT)
		{
			isEditMode = true;
			if(deleteConfirmation!="")setName(deleteConfirmation);
			if(mUnit->getMatGroupName()=="CustomMat")
			{
				page = PAGE_CUSTOM1;
				updateCharData();
			}
			else
			{
				page = PAGE_PRESET1;
				updateCharData(true);
			}
			refreshPage();
			return 0;
		}
		//finish edit char (preset)
		if(page==PAGE_PRESET2)
		{
			return CHARSCREEN_EDITCHAR;
		}
		page += 1;
		refreshPage();
		return 0;
	}
	void updateSliders(const Real &one, const Real &two, const Real &three)
	{
		if(page==PAGE_CUSTOM3)
		{
			colourVal[colourPartID].r = one;
			colourVal[colourPartID].g = two;
			colourVal[colourPartID].b = three;
		}
		else if(page==PAGE_CUSTOM4 || page==PAGE_PRESET2)
		{
			const Real tScale = 0.25 + (page==PAGE_PRESET2?two:one)*1.25;
			mUnit->getObjectNode()->setScale(tScale,tScale,tScale);
		}
		refreshPage();
	}
	void getSliderValue(Real &one, Real &two, Real &three)
	{
		if(page==PAGE_CUSTOM3)
		{
			one = colourVal[colourPartID].r;
			two = colourVal[colourPartID].g;
			three = colourVal[colourPartID].b;
		}
		else if(page==PAGE_CUSTOM4)
		{
			one = (mUnit->getObjectNode()->getScale().x-0.25)/1.25;
		}
		else if(page==PAGE_PRESET2)
		{
			two = (mUnit->getObjectNode()->getScale().x-0.25)/1.25;
		}
	}
	const String getName()
	{
		String tName = mName->getCaption();
		if(tName.length()>=6)tName.erase(0,6);
		return tName;
	}
	void setName(const String &name)
	{
		mName->setCaption("Name: " + name);
	}
	bool validateName()
	{
		const String tName = getName();
		for(int i=0;i<int(tName.length());i++)
		{
			if(tName[i]=='/'||tName[i]=='\\'||tName[i]==':'||tName[i]==';'||
				tName[i]=='*'||tName[i]=='?'||tName[i]=='\"'||
				tName[i]=='<'||tName[i]=='>'||tName[i]=='|'||
				tName[i]=='\t'||tName[i]=='\n')
				return false;
		}
		return true;
	}
	void doRotateUnit(Real x, Real y)
	{
		rotateStartX = x;
		rotateStartY = y;
		isRotateUnit = true;
	}
	void endRotateUnit()
	{
		isRotateUnit = false;
	}
	void stopRotateUnit()
	{
		endRotateUnit();
		dYaw = Degree(0);
	}
	bool isRotatingUnit()
	{
		return isRotateUnit;
	}
	void startGame()
	{
		startGameCount = 3;
	}
	bool update(const FrameEvent &evt, const Real &x, const Real &y)
	{
		//First page, stop any rotation and check if game started
		if(page==PAGE_SELECT)
		{
			stopRotateUnit();
			if(startGameCount>0)
			{
				startGameCount -= evt.timeSinceLastFrame;
				mBackdrop->setDimensions(1+(3-startGameCount)/3*0.4,1+(3-startGameCount)/3*0.6);
				mBackdrop->setPosition(-(mBackdrop->getWidth()-1)/2,-(mBackdrop->getHeight()-1)/2);
				const Vector3 tVect = mUnit->getPosition(true)+mUnit->getObjectNode()->getOrientation()*Vector3(0,1,4)-Vector3(3200,120,2770);
				mSceneMgr->getCamera("PlayerCam")->setPosition(Vector3(3200,120,2770)+tVect*(3-startGameCount)/3*0.25);
				if(startGameCount<=0)
				{
					startGameCount = 0;
					mBackdrop->setMaterialName("GUIMat/Backdrop1");
					mBackdrop->setDimensions(1,1);
					mBackdrop->setPosition(0,0);
					return true;
				}
			}
			return false;
		}
		//Do rotation/zoom otherwise
		if(isRotateUnit)
		{
			dYaw = Degree((x-rotateStartX)*180);
			camZoom += y - rotateStartY;
			rotateStartX = x;
			rotateStartY = y;
			if(Math::Abs(dYaw)>Degree(15))dYaw = Degree(15*dYaw<Degree(0)?-1:1);
			if(camZoom<0)camZoom = 0;
			if(camZoom>0.75)camZoom = 0.75;
		}
		
		if(mUnit->getObjectNode())
		{
			//Rotate unit
			mUnit->yaw(dYaw);
			mUnit->getObjectNode()->yaw(dYaw);
			//Camera zoom
			const Vector3 tVect = mUnit->getPosition(true)+mUnit->getObjectNode()->getOrientation()*Vector3(0,1,4)-Vector3(3200,120,2770);
			mSceneMgr->getCamera("PlayerCam")->setPosition(Vector3(3200,120,2770)+tVect*camZoom);
		}
		return false;
	}
	/*short loadNumChars()
	{
		using namespace std;

		short tNumChars = 0;
		ifstream inFile;
		inFile.open(FILENAME_CHARACTERS, ifstream::in);
		while(inFile.good() && !inFile.eof())
		{
			char tBuffer[1024];
			inFile.getline(tBuffer,1024);
			tNumChars += 1;
		}
		inFile.close();

		return tNumChars;
	}*/
	/*void loadChar(short iID)
	{
		using namespace std;

		char tBuffer[1024]="";
		short tNumChars = -1;
		ifstream inFile;
		inFile.open(FILENAME_CHARACTERS, ifstream::in);

		if(!inFile.good())mAlertBox->showAlert("Click here to create\na new character",0.70,0.85,5);

		while(inFile.good() && !inFile.eof() && tNumChars<iID)
		{
			strcpy(tBuffer,"");
			inFile.getline(tBuffer,1024);
			tNumChars += 1;
		}
		inFile.close();

		String tData = tBuffer;
		decrypt(tData);

		vector<String>::type tString = StringUtil::split(tData,"|\n",2);
		//Loaded successfully
		if(tString.size()>0)
		{
			vector<String>::type tParam = StringUtil::split(tString[0],";");
			if(tParam.size()==7 || tParam.size()==8)
			{
				setName(tParam[0]);
				mUnit->createUnit(1,mSceneMgr,tParam[1],tParam[2],tParam[3],tParam[4],tParam[5],tParam[6]);
				if(tParam.size()==8)
				{
					const float tScale = StringConverter::parseReal(tParam[7]);
					if(tScale>0 && tScale<10)mUnit->getObjectNode()->setScale(tScale,tScale,tScale);
				}
				if(tParam[6]=="CustomMat" && tString.size()==2)mUnit->setColours(tString[1]);
				const Real tDist = (mUnit->getObjectNode()->getScale().x-1)*-11;
				mUnit->setPosition(3215 - (tDist<0?tDist:0)*0.6, 113 + (tDist<0?tDist:0), 2750 + (tDist<0?tDist:0)*0.6);
				mUnit->resetTarget();
				mUnit->setGroundHeight(113 + (tDist<0?tDist:0));
				mUnit->yaw(Degree(-80));
				mUnit->getObjectNode()->yaw(Degree(-80));
				mUnit->setFreeLook(false,false);
				mUnit->update(FrameEvent(),mDef);

				return;
			}
			//bad data, delete it
			if(iID<(loadNumChars()-1))deleteChar(iID);
		}
		//no data
		setName("");
		mUnit->destroyUnit(mSceneMgr);
	}*/
	void deleteChar()
	{
		updateChar("");
		if(numChars>0)numChars--;
		charID = numChars;
		positionUnitForSelect();
		refreshPage();
	}
	const String getCharData()
	{
		//char name
		String tBuffer = getName();
		StringUtil::trim(tBuffer);
		tBuffer += ";";
		//char meshstring
		tBuffer += mUnit->getMeshString() + ";";
		//char matgroupname
		tBuffer += mUnit->getMatGroupName() + ";";
		//char meshstring2
		tBuffer += mUnit->getMeshString2() + ";";
		//markings
		if(mUnit->getMatGroupName() == "CustomMat")
		{
			short tBodyMark,tHeadMark,tTailMark;
			mUnit->getMarkings(tBodyMark,tHeadMark,tTailMark);
			tBuffer += StringConverter::toString(tBodyMark) + ";";
			tBuffer += StringConverter::toString(tHeadMark) + ";";
			tBuffer += StringConverter::toString(tTailMark);
		}
		//char scale
		if(mUnit->getObjectNode()->getScale().x != 1)
		{
			tBuffer += ";" + StringConverter::toString(mUnit->getObjectNode()->getScale().x,3);
		}
		//char colours
		if(mUnit->getMatGroupName() == "CustomMat")
		{
			tBuffer += "|" + mUnit->getColourString();
		}
		
		return tBuffer;
	}
	void updateCharData(bool isPreset=false)
	{
		const vector<String>::type tMesh = StringUtil::split(mUnit->getMeshString(),";",5);
		if(tMesh.size()==5)
		{
			for(int i=0;i<mDef->maxHeads;i++)
				if(mDef->headMesh[i]==tMesh[1])
				{
					headID = i;
					break;
				}
			for(int i=0;i<mDef->maxManes;i++)
				if(mDef->maneMesh[i]==tMesh[2])
				{
					maneID = i;
					break;
				}
			for(int i=0;i<mDef->maxTails;i++)
				if(mDef->tailMesh[i]==tMesh[3])
				{
					tailID = i;
					break;
				}
			for(int i=0;i<mDef->maxWings;i++)
				if(mDef->wingMesh[i]==tMesh[4])
				{
					wingID = i;
					break;
				}
		}
		const vector<String>::type tMesh2 = StringUtil::split(mUnit->getMeshString2(),";",1);
		if(tMesh2.size()==1)
		{
			for(int i=0;i<mDef->maxTufts;i++)
				if(mDef->tuftMesh[i]==tMesh2[0])
				{
					tuftID = i;
					break;
				}
		}
		if(!isPreset)
		{
			mUnit->getMarkings(bodyMarkID,headMarkID,tailMarkID);
			const vector<String>::type tColour = StringUtil::split(mUnit->getColourString(),";",MAX_COLOURS);
			for(int i=0;i<(int)tColour.size();i++)
					colourVal[i] = StringConverter::parseColourValue(tColour[i]);
		}
	}
	void updateChar(const String &data)
	{
		if(mUnit->getObjectNode())
		{
			mEffectsManager->destroyParticleByObjectNode(mUnit->getObjectNode());
			mEffectsManager->destroyItemEffectByOwnerNode(mUnit->getObjectNode());
			mUnit->destroyUnit(mSceneMgr);
		}
		if(mUnit->getType()==OBJECT_PLAYER)
		{
			static_cast<MagixPlayer*>(mUnit)->reset();
		}
		
		const vector<String>::type tString = StringUtil::split(data,"|\n",2);
		//Loaded successfully
		if(tString.size()>0)
		{
			const vector<String>::type tParam = StringUtil::split(tString[0],";");
			if(tParam.size()>=7 && tParam.size()<=12)
			{
				setName(tParam[0]);

				float tScale = 0;
				bool tHasNewMeshes = false;
				if(tParam.size()==8)tScale = StringConverter::parseReal(tParam[7]);
				else if(tParam.size()==9)tScale = StringConverter::parseReal(tParam[8]);
				else if(tParam.size()==12)tScale = StringConverter::parseReal(tParam[11]);
				if(tParam.size()>8 || tParam.size()==8&&tScale<0.0000005)tHasNewMeshes = true;

				mUnit->createUnit(1,mSceneMgr,tParam[1],tParam[2],tParam[3],tParam[4],tParam[5],tParam[6],(tHasNewMeshes?tParam[7]:mDef->tuftMesh[0]));
				
				if(tScale>0.0000005 && tScale<10)mUnit->getObjectNode()->setScale(tScale,tScale,tScale);
				if(tParam[6]=="CustomMat")
				{
					if(tHasNewMeshes)mUnit->setMarkings(StringConverter::parseInt(tParam[8]),StringConverter::parseInt(tParam[9]),StringConverter::parseInt(tParam[10]));
					if(tString.size()==2)mUnit->setColours(tString[1]);
				}
				positionUnitForSelect();

				return;
			}
		}
		//no data
		setName("");
	}
	void createCharSuccess()
	{
		const String tName = getName();
		//mUnit->destroyUnit(mSceneMgr);
		reset();
		charID = numChars;
		numChars++;
		//updateChar(tData);
		setName(tName);
		positionUnitForSelect();
		if(mUnit)mUnit->setHP(500);
		refreshPage();
	}
	void editCharSuccess()
	{
		const String tName = getName();
		const short tID = charID;
		reset();
		charID = tID;
		setName(tName);
		positionUnitForSelect();
		refreshPage();
	}
	/*void createChar(short iID)
	{
		using namespace std;

		ofstream outFile;
		outFile.open(FILENAME_CHARACTERS, ios::app);

		//char name
		String tBuffer = getName();
		StringUtil::trim(tBuffer);
		tBuffer += ";";
		//char meshstring
		tBuffer += mUnit->getMeshString() + ";";
		//char matgroupname
		tBuffer += mUnit->getMatGroupName();
		//char scale
		if(mUnit->getObjectNode()->getScale().x != 1)
		{
			tBuffer += ";" + StringConverter::toString(mUnit->getObjectNode()->getScale().x);
		}
		//char colours
		if(mUnit->getMatGroupName() == "CustomMat")
		{
			tBuffer += "|" + mUnit->getColourString();
		}
		encrypt(tBuffer);
		outFile.write(tBuffer.c_str(),int(tBuffer.length()));
		//endline
		tBuffer = "\n";
		outFile.write(tBuffer.c_str(),int(tBuffer.length()));

		outFile.close();
	}
	void deleteChar(short iID)
	{
		using namespace std;

		short tCharID = 0;
		long tSize = 0;
		char *tBuffer;

		//read, read, read
		ifstream inFile;
		inFile.open(FILENAME_CHARACTERS, ifstream::in);

		inFile.seekg(0,ifstream::end);
		tSize = inFile.tellg();
		inFile.seekg(0);

		tBuffer = new char[tSize];
		strcpy(tBuffer,"");

		while(inFile.good() && !inFile.eof())
		{
			char tMiniBuffer[1024]="";
			inFile.getline(tMiniBuffer,1024);
			if(tCharID != iID)
			{
				strcat(tBuffer,tMiniBuffer);
				strcat(tBuffer,"\n");
			}
			tCharID += 1;
		}
		inFile.close();

		//write, write, write
		ofstream outFile;
		outFile.open(FILENAME_CHARACTERS, ofstream::out);
		outFile.write(tBuffer,int(strlen(tBuffer)-1));
		outFile.close();

		delete[] tBuffer;

		setName("");
		mUnit->destroyUnit(mSceneMgr);
		charID = loadNumChars()-1;
		refreshPage();
	}*/
	void createDefaultUnit()
	{
		mUnit->createUnit(1,mSceneMgr,"Body",meshName("Head",headID),meshName("Mane",maneID),meshName("Tail",tailID),meshName("Wing",wingID),"KovMat",meshName("Tuft",tuftID));
		positionUnitForEdit();
	}
	void positionUnitForEdit()
	{
		mSceneMgr->getCamera("PlayerCam")->setOrientation(Quaternion(Degree(-80),Vector3::UNIT_Y));
		if(!mUnit->getObjectNode())return;
		mUnit->setPosition(3235,113,2774);
		mUnit->resetTarget();
		mUnit->setGroundHeight(113);
		mUnit->yaw(Degree(-120));
		mUnit->getObjectNode()->yaw(Degree(-120));
		mUnit->setFreeLook(false,false);
	}
	void positionUnitForSelect()
	{
		mSceneMgr->getCamera("PlayerCam")->setOrientation(Quaternion(Degree(-40),Vector3::UNIT_Y));
		mSceneMgr->getCamera("PlayerCam")->setPosition(3200,120,2770);
		if(!mUnit->getObjectNode())return;
		const Real tDist = (mUnit->getObjectNode()->getScale().x-1)*-11;
		mUnit->setPosition(3215 - (tDist<0?tDist:0)*0.6, 113 + (tDist<0?tDist:0), 2750 + (tDist<0?tDist:0)*0.6);
		mUnit->resetTarget();
		mUnit->setGroundHeight(113 + (tDist<0?tDist:0));
		mUnit->yaw(Degree(-80));
		mUnit->getObjectNode()->yaw(Degree(-80));
		mUnit->setFreeLook(false,false);
		mUnit->update(FrameEvent(),mDef);
	}
	/*void encrypt(String &input)
	{
		for(int i=0;i<(int)input.length();i++)
		{
			input[i] = input[i]+10;
		}
	}
	void decrypt(String &input)
	{
		for(int i=0;i<(int)input.length();i++)
		{
			input[i] = input[i]-10;
		}
	}*/

	void setNumChars(const short &tNum)
	{
		numChars = tNum;
	}
	const short getNumChars()
	{
		return numChars;
	}
	bool popIsCharIDChanged()
	{
		const bool charIDChanged = isCharIDChanged;
		isCharIDChanged = false;
		return charIDChanged;
	}
	bool popDoReequip()
	{
		const bool reequip = doReequip;
		doReequip = false;
		return reequip;
	}
	const short getCharID()
	{
		return charID;
	}
};

#endif