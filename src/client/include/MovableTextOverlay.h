#ifndef __MovableTextOverlay_H__
#define __MovableTextOverlay_H__

#include "Ogre.h"
#include "OgreFont.h"
#include "OgreFontManager.h"

using namespace Ogre;

class MovableTextOverlayAttributes
{
public:
	MovableTextOverlayAttributes(const Ogre::String & name, const Ogre::Camera *cam,
						 const Ogre::String & fontName = "Tahoma", const int &charHeight = 16, const Ogre::ColourValue & color = Ogre::ColourValue::White, const Ogre::String & materialName = "");
	~MovableTextOverlayAttributes();

	void setFontName(const Ogre::String & fontName);
	void setMaterialName(const Ogre::String & materialName);
	void setColor(const Ogre::ColourValue & color);
	void setCharacterHeight(const unsigned int &height);

	const Ogre::String& getName() const {return mName;}
	const Ogre::Camera* getCamera() const {return mpCam;}
	const Ogre::Font* getFont() const {return mpFont;}
	const Ogre::String& getFontName() const {return mFontName;}
	const Ogre::String& getMaterialName() const {return mMaterialName;}
	const Ogre::ColourValue& getColor() const {return mColor;}
	const Ogre::Real getCharacterHeight() const {return mCharHeight;}

	const Ogre::String mName;
	const Ogre::Camera *mpCam;

	Ogre::Font* mpFont;
	Ogre::String mFontName;
	Ogre::String mMaterialName;
	Ogre::ColourValue mColor;
	Ogre::Real mCharHeight;
};

class MovableTextOverlay {
public:
	MovableTextOverlay(const Ogre::String & name, const Ogre::String & caption,
						const Ogre::MovableObject *mov, MovableTextOverlayAttributes *attrs, const Real &tLeft=0.0, const Real &tTop=0.0, const Real &yOffset=0);

	virtual ~MovableTextOverlay();

	void setCaption(const Ogre::String & caption);
	void setUpdateFrequency(Ogre::Real updateFrequency) {mUpdateFrequency = updateFrequency;}
	void setAttributes(MovableTextOverlayAttributes *attrs)
	{
		mAttrs = attrs;
		_updateOverlayAttrs();
	}

	const Ogre::String&	getName() const {return mName;}
	const Ogre::String&	getCaption() const {return mCaption;}
	const Ogre::Real getUpdateFrequency() const {return mUpdateFrequency;}
	const bool isOnScreen() const {return mOnScreen;}
	const bool isEnabled() const {return mEnabled;}
	const MovableTextOverlayAttributes* getAttributes() const {return mAttrs;}

	void enable(bool enable);
	void update(const Real &timeSincelastFrame, bool placeAtTop, const Real &borderSizeOffset=0, bool chaseUpdate=false);

	// Needed for RectLayoutManager.
	int getPixelsTop() {return Ogre::OverlayManager::getSingleton().getViewportHeight() * (mpOvContainer->getTop());}
	int getPixelsBottom() {return Ogre::OverlayManager::getSingleton().getViewportHeight() * (mpOvContainer->getTop() + mpOvContainer->getHeight());}
	int getPixelsLeft() {return Ogre::OverlayManager::getSingleton().getViewportWidth() * mpOvContainer->getLeft();}
	int getPixelsRight() {return Ogre::OverlayManager::getSingleton().getViewportWidth() * (mpOvContainer->getLeft() + mpOvContainer->getWidth());}

	void setPixelsTop(int px) {mpOvContainer->setTop((Ogre::Real)px / Ogre::OverlayManager::getSingleton().getViewportHeight());}
	void setPixelsLeft(int px) {mpOvContainer->setLeft((Ogre::Real)px / Ogre::OverlayManager::getSingleton().getViewportWidth());}
	// end

protected:
	void _computeTextSize();
	void _updateOverlayAttrs();
	void _getMinMaxEdgesOfAABBIn2D(Ogre::Real& MinX, Ogre::Real& MinY, Ogre::Real& MaxX, Ogre::Real& MaxY, bool getTop);
	void _getScreenCoordinates(const Ogre::Vector3& position, Ogre::Real& x, Ogre::Real& y);

	const Ogre::String mName;
	const Ogre::MovableObject* mpMov;

	Ogre::Overlay* mpOv;
	Ogre::OverlayContainer* mpOvContainer;
	Ogre::OverlayElement* mpOvText;
	
	// true if mpOvContainer is visible, false otherwise
	bool mEnabled;

	// true if mTextWidth needs to be recalculated
	bool mNeedUpdate;

	// Text width in pixels
	Ogre::Real mTextWidth;
	unsigned short mNoOfLines;

	// the Text
	Ogre::String mCaption;

	// true if the upper vertices projections of the MovableObject are on screen
	bool mOnScreen;

	// the update frequency in seconds
	// mpOvContainer coordinates get updated each mUpdateFrequency seconds.
	Ogre::Real mUpdateFrequency;

	// the Font/Material/Color text attributes
	MovableTextOverlayAttributes *mAttrs;
	Real mYOffset;
};

MovableTextOverlay::MovableTextOverlay(const Ogre::String & name, const Ogre::String & caption,
						const Ogre::MovableObject *mov, MovableTextOverlayAttributes *attrs, const Real &tLeft, const Real &tTop, const Real &yOffset)
: mpMov(mov)
, mpOv(NULL)
, mpOvContainer(NULL)
, mpOvText(NULL)
, mAttrs(attrs)
, mName(name)
, mCaption("")
, mUpdateFrequency(0.01)
, mNeedUpdate(TRUE)
, mOnScreen(FALSE)
, mEnabled(FALSE)
, mYOffset(yOffset/OverlayManager::getSingleton().getViewportHeight())
{
	if (name == "")
        Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Trying to create MovableTextOverlay without name", "MovableTextOverlay::MovableTextOverlay");

    if (caption == "")
        Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Trying to create MovableTextOverlay without caption", "MovableTextOverlay::MovableTextOverlay");

	if (mAttrs == NULL)
		Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Trying to create MovableTextOverlay without Attributes", "MovableTextOverlay::MovableTextOverlay");
/*
    if(Ogre::OverlayManager::getSingleton().getByName(name + "_Ov")) 
    { 
        Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Trying to create MovableTextOverlay with a duplicate name", "MovableTextOverlay::MovableTextOverlay");
    }
*/
	// create an overlay that we can use for later
	mpOv = Ogre::OverlayManager::getSingleton().create(name+"_Ov");
	mpOv->hide();
	mpOvContainer = static_cast<Ogre::OverlayContainer*>(Ogre::OverlayManager::getSingleton().createOverlayElement(
              "Panel", name+"_OvC"));
	mpOvContainer->setPosition(tLeft, tTop);
	mpOvContainer->setDimensions(0.0, 0.0);

	mpOv->add2D(mpOvContainer);

	mpOvText = Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", name+"_OvTxt");
	mpOvContainer->addChild(mpOvText);

	mpOvText->setMetricsMode(Ogre::GMM_RELATIVE);
	mpOvText->setDimensions(1.0, 1.0);
	mpOvText->setMetricsMode(Ogre::GMM_PIXELS);
	mpOvText->setPosition(0, 0);

	mpOv->setZOrder(450);

	_updateOverlayAttrs();

	setCaption(caption);
}

MovableTextOverlay::~MovableTextOverlay()
{
	// overlay cleanup -- Ogre would clean this up at app exit but if your app 
	// tends to create and delete these objects often it's a good idea to do it here.
	const Ogre::String mmName = mName;
	mpOv->hide();
	Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
	mpOvContainer->removeChild(mmName+"_OvTxt");
	//mpOv->remove2D(mpOvContainer);
	//Ogre::Overlay* mpOv;
	//Ogre::OverlayContainer* mpOvContainer;
	//Ogre::OverlayElement* mpOvText;
	if(mpOv)
		overlayManager->destroy(mpOv);
	if(mpOvText) 
		overlayManager->destroyOverlayElement(mpOvText);
	if(mpOvContainer)
		overlayManager->destroyOverlayElement(mpOvContainer);
}

void MovableTextOverlay::setCaption(const Ogre::String & caption)
{
    if (caption != mCaption)
    {
        mCaption = caption;
		mpOvText->setCaption(mCaption);
        mNeedUpdate = true;
    }
}

void MovableTextOverlay::_computeTextSize()
{
	//compute text width/height
	const Font *pFont = mAttrs->getFont();

	vector<String>::type tText = StringUtil::split(mCaption,"\n");
	Real tLongestWidth = 0;

	for(int i=0; i<int(tText.size()); i++)
	{
		String tLine = tText[i];
		Real tTextWidth = 0;
		for(Ogre::String::iterator i = tLine.begin(); i < tLine.end();i++)
		{   
			if (*i == 0x0020)
				tTextWidth += pFont->getGlyphAspectRatio(0x0030);
			else
			{
				tTextWidth += pFont->getGlyphAspectRatio(*i);
			}
		}
		if(tTextWidth>tLongestWidth)tLongestWidth = tTextWidth;
	}

	mTextWidth = tLongestWidth;
	mTextWidth *= mAttrs->getCharacterHeight();
	mNoOfLines = int(tText.size());
}

void MovableTextOverlay::_getMinMaxEdgesOfAABBIn2D(Ogre::Real& MinX, Ogre::Real& MinY, Ogre::Real& MaxX, Ogre::Real& MaxY, bool getTop)
{
	const Ogre::Camera* mpCam = mAttrs->getCamera();

	MinX = 0;
	MinY = 0;
	MaxX = 0;
	MaxY = 0;

	Ogre::Real X[4];// the 2D dots of the AABB in screencoordinates
	Ogre::Real Y[4];

	if (!mpMov->isInScene())
	   return;

	const Ogre::AxisAlignedBox &AABB = mpMov->getWorldBoundingBox(true);// the AABB of the target
	const Ogre::Vector3 CornersOfTopAABB[4] =
	{getTop ? AABB.getCorner(AxisAlignedBox::FAR_LEFT_TOP) : AABB.getCorner(AxisAlignedBox::FAR_LEFT_BOTTOM),
	getTop ? AABB.getCorner(AxisAlignedBox::FAR_RIGHT_TOP) : AABB.getCorner(AxisAlignedBox::FAR_RIGHT_BOTTOM),
	getTop ? AABB.getCorner(AxisAlignedBox::NEAR_LEFT_TOP) : AABB.getCorner(AxisAlignedBox::NEAR_LEFT_BOTTOM),
	getTop ? AABB.getCorner(AxisAlignedBox::NEAR_RIGHT_TOP) : AABB.getCorner(AxisAlignedBox::NEAR_RIGHT_BOTTOM)};

	Ogre::Vector3 CameraPlainNormal = mpCam->getDerivedOrientation().zAxis();//The normal vector of the plaine.this points directly infront of the cam

	Ogre::Plane CameraPlain = Plane(CameraPlainNormal,mpCam->getDerivedPosition());//the plaine that devides the space bevor and behin the cam

	for (int i = 0; i < 4; i++)
	{
	  X[i] = 0;
	  Y[i] = 0;
	  
	  _getScreenCoordinates(CornersOfTopAABB[i],X[i],Y[i]);// transfor into 2d dots

	  
	  if (CameraPlain.getSide(CornersOfTopAABB[i]) == Plane::NEGATIVE_SIDE)
	  {
	     
		 if (i == 0)// accept the first set of values, no matter how bad it might be.
		 {
			MinX = X[i];
			MinY = Y[i];
			MaxX = X[i];
			MaxY = Y[i];
		 }
		 else// now compare if you get "better" values
		 {
			if (MinX > X[i])// get the x minimum
			{
			   MinX = X[i];
			}
			if (MinY > Y[i])// get the y minimum
			{
			   MinY = Y[i];
			}
			if (MaxX < X[i])// get the x maximum
			{
			   MaxX = X[i];
			}
			if (MaxY < Y[i])// get the y maximum
			{
			   MaxY = Y[i];
			}
		 }
	  }
	  else
	  {
		MinX = 0;
		MinY = 0;
		MaxX = 0;
		MaxY = 0;
		break;
	  }
	}
} 

void MovableTextOverlay::_getScreenCoordinates(const Ogre::Vector3& position, Ogre::Real& x, Ogre::Real& y)
{
	const Ogre::Camera* mpCam = mAttrs->getCamera();
	Vector3 hcsPosition = mpCam->getProjectionMatrix() * (mpCam->getViewMatrix() * position);

	x = 1.0f - ((hcsPosition.x * 0.5f) + 0.5f);// 0 <= x <= 1 // left := 0,right := 1
	y = ((hcsPosition.y * 0.5f) + 0.5f);// 0 <= y <= 1 // bottom := 0,top := 1
}

void MovableTextOverlay::enable(bool enable)
{
	if (mEnabled == enable)
		return;

	mEnabled = enable;
	if (mEnabled)
		mpOv->show();
	else
		mpOv->hide();
}

void MovableTextOverlay::update(const Real &timeSincelastFrame, bool placeAtTop, const Real &borderSizeOffset, bool chaseUpdate)
{
	static Real timeTillUpdate = 0;

	timeTillUpdate -= timeSincelastFrame;
	if (timeTillUpdate > 0)
		return;
	timeTillUpdate = mUpdateFrequency;

	Ogre::Real min_x, max_x, min_y, max_y;
	_getMinMaxEdgesOfAABBIn2D(min_x, min_y, max_x, max_y, placeAtTop);

	if (mNeedUpdate)
	{
		_computeTextSize();
		mNeedUpdate = false;
	}

	Real relTextWidth = mTextWidth / OverlayManager::getSingleton().getViewportWidth();
	Real relTextHeight = mAttrs->getCharacterHeight()*mNoOfLines / OverlayManager::getSingleton().getViewportHeight();

	Real tX = 1-(min_x + max_x + relTextWidth)/2;
	Real tY = 1-max_y-(placeAtTop?relTextHeight+borderSizeOffset:0) + mYOffset;
	//container "chases" towards update position
	if(chaseUpdate)
	{
		tX = (tX - mpOvContainer->getLeft())*0.08;
		tY = (tY - mpOvContainer->getTop())*0.08;
		mpOvContainer->setPosition(mpOvContainer->getLeft()+tX, mpOvContainer->getTop()+tY);
	}
	else mpOvContainer->setPosition(tX, tY);

	mpOvContainer->setDimensions(relTextWidth+borderSizeOffset, relTextHeight+borderSizeOffset);
	if(borderSizeOffset!=0)
	{
		mpOvText->setPosition(0,borderSizeOffset*600);
	}

	if ((min_x+relTextWidth>0.0) && (max_x-relTextWidth<1.0) && (min_y+relTextHeight>0.0) && (max_y-relTextHeight<1.0)
		&& !(min_x==0&&max_x==0&&min_y==0&&max_y==0) )
	   mOnScreen = true;
	else
	   mOnScreen = false;
}

void MovableTextOverlay::_updateOverlayAttrs()
{
	const String &newMatName = mAttrs->getMaterialName();
	const String &oldMatName = mpOvContainer->getMaterialName();
	if (oldMatName != newMatName)
	{
		if (oldMatName.length())
			mpOvContainer->getMaterial()->unload();

		if (newMatName.length())
			mpOvContainer->setMaterialName(newMatName);

	}

	mpOvText->setColour(mAttrs->getColor());

	mpOvText->setParameter("font_name", mAttrs->getFontName());
	mpOvText->setParameter("char_height", Ogre::StringConverter::toString(mAttrs->getCharacterHeight()));
	mpOvText->setParameter("alignment", "center");
	mpOvText->setParameter("horz_align", "center");
	mpOvText->setParameter("vert_align", "top");
}


MovableTextOverlayAttributes::MovableTextOverlayAttributes(const Ogre::String & name, const Ogre::Camera *cam,
						 const Ogre::String & fontName, const int &charHeight, const Ogre::ColourValue & color, const Ogre::String & materialName)
: mpCam(cam)
, mpFont(NULL)
, mName(name)
, mFontName("")
, mMaterialName("")
, mCharHeight(charHeight)
, mColor(ColourValue::ZERO)
{
	if (fontName.length() == 0)
        Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Invalid font name", "MovableTextOverlayAttributes::MovableTextOverlayAttributes");

	setFontName(fontName);
	setMaterialName(materialName);
	setColor(color);
}

MovableTextOverlayAttributes::~MovableTextOverlayAttributes()
{
	setFontName("");
	setMaterialName("");
}

void MovableTextOverlayAttributes::setFontName(const Ogre::String & fontName)
{
    if (mFontName != fontName || !mpFont)
    {
		if (mpFont)
		{
			mpFont->unload();
			mpFont = NULL;
		}

		mFontName = fontName;
		if (mFontName.length())
		{
			mpFont = dynamic_cast<Ogre::Font*>(Ogre::FontManager::getSingleton().getByName(mFontName).getPointer());
			if (!mpFont)
				Ogre::Exception(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Could not find font " + fontName, "MovableTextOverlay::setFontName");
			mpFont->load();
		}
    }
}

void MovableTextOverlayAttributes::setMaterialName(const Ogre::String & materialName)
{
	if (mMaterialName != materialName)
	{
		if (mMaterialName.length())
			Ogre::MaterialManager::getSingletonPtr()->getByName(mMaterialName).getPointer()->unload();

		mMaterialName = materialName;
		if (mMaterialName.length())
		{
			Ogre::Material *mpMaterial = dynamic_cast<Ogre::Material*>(Ogre::MaterialManager::getSingletonPtr()->getByName(mMaterialName).getPointer());
			if (!mpMaterial)
				Ogre::Exception(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Could not find material " + materialName, "MovableTextOverlay::setMaterialName");
			mpMaterial->load();
		}
	}
}

void MovableTextOverlayAttributes::setColor(const Ogre::ColourValue & color)
{
        mColor = color;
}

void MovableTextOverlayAttributes::setCharacterHeight(const unsigned int &height)
{
        mCharHeight = height;
}


#endif