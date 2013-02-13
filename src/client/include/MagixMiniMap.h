#ifndef __MagixMiniMap_h_
#define __MagixMiniMap_h_

class MagixMiniMap
{
protected:
	MagixExternalDefinitions *mDef;
	MagixUnitManager *mUnitManager;
	MagixCollisionManager *mCollisionManager;
	MagixCamera *mCamera;
	OverlayContainer *mMap;
	OverlayElement *mMapCone;
	OverlayElement *mMapArrow;
	OverlayElement *mMapTag;
	vector<bool>::type hasPortalMarker;
	vector<bool>::type hasGateMarker;
	vector<const std::pair<Vector2,String>>::type portalMap;
	vector<const std::pair<Vector2,String>>::type gateMap;
	Real zoomRatio;
	bool mapTagUseCharName;
	vector<const unsigned char>::type pingUnitMarkers;
	unsigned char pingCounter;
	Real pingTimer;
public:
	MagixMiniMap()
	{
		mDef = 0;
		mUnitManager = 0;
		mCollisionManager = 0;
		mMap = 0;
		mMapCone = 0;
		mMapArrow = 0;
		mMapTag = 0;
		hasPortalMarker.clear();
		hasGateMarker.clear();
		portalMap.clear();
		gateMap.clear();
		zoomRatio = 1;
		mapTagUseCharName = false;
		pingUnitMarkers.clear();
		pingCounter = 0;
		pingTimer = 0;
	}
	~MagixMiniMap()
	{
		Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
		overlayManager->destroy("GUIOverlay/MiniMap2"); // must destroy manually
	}
	void reset()
	{
		pingUnitMarkers.clear();
		pingCounter = 0;
		pingTimer = 0;
	}
	void initialize(MagixExternalDefinitions *def, MagixUnitManager *unitMgr, MagixCollisionManager *collisionMgr, MagixCamera *camera)
	{
		mDef = def;
		mUnitManager = unitMgr;
		mCollisionManager = collisionMgr;
		mCamera = camera;

		mMap = static_cast<OverlayContainer*>(OverlayManager::getSingleton().getOverlayElement("GUI/MiniMap"));
		mMapCone = OverlayManager::getSingleton().getOverlayElement("GUI/MiniMapCone");
		mMapArrow = OverlayManager::getSingleton().getOverlayElement("GUI/MiniMapArrow");
		mMapTag = OverlayManager::getSingleton().getOverlayElement("GUI/MiniMapTag");
	}
	void show(bool flag)
	{
		if(flag)
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/MiniMap")->show();
			OverlayManager::getSingleton().getByName("GUIOverlay/MiniMap2")->show();
			mMap->show();
		}
		else
		{
			OverlayManager::getSingleton().getByName("GUIOverlay/MiniMap")->hide();
			OverlayManager::getSingleton().getByName("GUIOverlay/MiniMap2")->hide();
			mMap->hide();
		}
	}
	void flipVisibility()
	{
		show(!mMap->isVisible());
	}
	void update(bool cursorVisible, const Real &cursorX, const Real &cursorY, const FrameEvent &evt)
	{
		if(!mMap->isVisible())return;
		if(!mUnitManager->getPlayer()->getObjectNode())return;
		const Radian tCamYaw = mCamera->getYaw()+Degree(180);
		const Radian tPlayerYaw = mUnitManager->getPlayer()->getObjectNode()->getOrientation().getYaw();

		if(mMapTag->isVisible())mMapTag->hide();

		//Rotations
		mMapCone->getTechnique()->getPass(0)->getTextureUnitState(0)->setTextureRotate(tCamYaw);
		mMapArrow->getTechnique()->getPass(0)->getTextureUnitState(0)->setTextureRotate(tPlayerYaw);

		Vector2 tCenter = Vector2(mUnitManager->getPlayer()->getPosition().x,mUnitManager->getPlayer()->getPosition().z);
		//Player
		if(!mCamera->getCamera()->isAttached())
		{
			const Vector2 tCamPosition = Vector2(mCamera->getCamera()->getPosition().x,mCamera->getCamera()->getPosition().z);
			
			Vector2 tMarkerPosition;
			if(mapPosition(tCamPosition,tCenter,tMarkerPosition))
			{
				if(!mMapArrow->isVisible())mMapArrow->show();
				mMapArrow->setPosition(tMarkerPosition.x,tMarkerPosition.y);
			}
			else mMapArrow->hide();

			tCenter = tCamPosition;
		}
		else 
		{
			mMapArrow->setPosition(0.06,0.08);
			if(!mMapArrow->isVisible())mMapArrow->show();
		}

		//Portals
		for(int i=0;i<(int)portalMap.size();i++)
		{
			bool doMarker = false;
			Vector2 tMarkerPosition;
			if(mapPosition(tCenter,portalMap[i].first,tMarkerPosition))
			{
				doMarker = true;
				OverlayElement *tPortalMarker = 0;
				if(!hasPortalMarker[i])
				{
					tPortalMarker = OverlayManager::getSingleton().createOverlayElement("Panel", "PortalMarker"+StringConverter::toString(i));
					tPortalMarker->setMaterialName("GUIMat/MiniMapDotTeal");
					tPortalMarker->setDimensions(0.03,0.04);
					mMap->addChild(tPortalMarker);
					hasPortalMarker[i] = true;
				}
				else tPortalMarker = OverlayManager::getSingleton().getOverlayElement("PortalMarker"+StringConverter::toString(i));
				tPortalMarker->setPosition(tMarkerPosition.x,tMarkerPosition.y);

				//Check cursor position, show portal name
				if(cursorVisible && isInMarker(tPortalMarker,cursorX,cursorY))
				{
					mMapTag->setCaption(portalMap[i].second);
					mMapTag->setPosition(cursorX,cursorY);
					if(!mMapTag->isVisible())mMapTag->show();
				}
			}
			if(!doMarker && hasPortalMarker[i])
			{
				OverlayManager::getSingleton().destroyOverlayElement("PortalMarker"+StringConverter::toString(i));
				hasPortalMarker[i] = false;
			}

		}

		//Gates
		for(int i=0;i<(int)gateMap.size();i++)
		{
			bool doMarker = false;
			Vector2 tMarkerPosition;
			if(mapPosition(tCenter,gateMap[i].first,tMarkerPosition))
			{
				doMarker = true;
				OverlayElement *tGateMarker = 0;
				if(!hasGateMarker[i])
				{
					tGateMarker = OverlayManager::getSingleton().createOverlayElement("Panel", "GateMarker"+StringConverter::toString(i));
					tGateMarker->setMaterialName("GUIMat/MiniMapDotTeal");
					tGateMarker->setDimensions(0.03,0.04);
					mMap->addChild(tGateMarker);
					hasGateMarker[i] = true;
				}
				else tGateMarker = OverlayManager::getSingleton().getOverlayElement("GateMarker"+StringConverter::toString(i));
				tGateMarker->setPosition(tMarkerPosition.x,tMarkerPosition.y);

				//Check cursor position, show gate name
				if(cursorVisible && isInMarker(tGateMarker,cursorX,cursorY))
				{
					mMapTag->setCaption(mDef->getMapName(gateMap[i].second));
					mMapTag->setPosition(cursorX,cursorY);
					if(!mMapTag->isVisible())mMapTag->show();
				}
			}
			if(!doMarker && hasGateMarker[i])
			{
				OverlayManager::getSingleton().destroyOverlayElement("GateMarker"+StringConverter::toString(i));
				hasGateMarker[i] = false;
			}

		}

		//Units
		const vector<MagixIndexedUnit*>::type tUnitList = mUnitManager->getUnitList();

		for(int i=0;i<(int)tUnitList.size();i++)
		{
			const Vector2 tUnitPosition = Vector2(tUnitList[i]->getPosition().x,tUnitList[i]->getPosition().z);
			Vector2 tMarkerPosition;
			if(!tUnitList[i]->isHidden && mapPosition(tCenter,tUnitPosition,tMarkerPosition))
			{
				OverlayElement *tUnitMarker = 0;
				if(!tUnitList[i]->hasMarker)
				{
					tUnitMarker = OverlayManager::getSingleton().createOverlayElement("Panel", "UnitMarker"+StringConverter::toString(tUnitList[i]->getUnitID()));
					tUnitMarker->setDimensions(0.03,0.04);
					mMap->addChild(tUnitMarker);
					tUnitList[i]->hasMarker = true;
					updateUnitMarker(tUnitList[i]);
				}
				else tUnitMarker = OverlayManager::getSingleton().getOverlayElement("UnitMarker"+StringConverter::toString(tUnitList[i]->getUnitID()));
				tUnitMarker->setPosition(tMarkerPosition.x,tMarkerPosition.y);

				//Check cursor position, show unit name
				if(cursorVisible && isInMarker(tUnitMarker,cursorX,cursorY))
				{
					mMapTag->setCaption((!mapTagUseCharName&&tUnitList[i]->getUser()!=""?tUnitList[i]->getUser():tUnitList[i]->getName()));
					mMapTag->setPosition(cursorX,cursorY);
					if(!mMapTag->isVisible())mMapTag->show();
				}
			}
			else if(tUnitList[i]->hasMarker)
			{
				OverlayManager::getSingleton().destroyOverlayElement("UnitMarker"+StringConverter::toString(tUnitList[i]->getUnitID()));
				tUnitList[i]->hasMarker = false;
			}
		}

		//Pinged Units
		if(pingCounter>0)
		{
			pingTimer -= evt.timeSinceLastFrame;
			if(pingTimer<=0)
			{
				for(int i=0;i<(int)pingUnitMarkers.size();i++)
				{
					OverlayElement *tUnitMarker = OverlayManager::getSingleton().getOverlayElement("UnitMarker"+StringConverter::toString(pingUnitMarkers[i]));
					if(!tUnitMarker)continue;
					if(pingCounter%2==0)tUnitMarker->setMaterialName("GUIMat/MiniMapDotRed");
					else tUnitMarker->setMaterialName("GUIMat/MiniMapDotYellow");
				}
				pingTimer = 0.25;
				pingCounter--;
			}
			if(pingCounter==0)
			{
				pingTimer = 0;
				pingUnitMarkers.clear();
			}
		}
	}
	bool mapPosition(const Vector2 &center, const Vector2 &input, Vector2 &output)
	{
		output = Vector2((center.x-input.x)*0.00006*zoomRatio+0.06,(center.y-input.y)*0.00008*zoomRatio+0.08);
		if(output.x<-0.015 || output.x>0.135 || output.y<-0.02 || output.y>0.18)return false;
		return true;
	}
	void zoomIn()
	{
		if(zoomRatio>=2)return;
		zoomRatio *= 2;
		mMapCone->setDimensions(0.15*zoomRatio, 0.2*zoomRatio);
		mMapCone->setPosition(mMap->getWidth()*0.5 - mMapCone->getWidth()*0.5,mMap->getHeight()*0.5 - mMapCone->getHeight()*0.5);
	}
	void zoomOut()
	{
		if(zoomRatio<=0.03125)return;
		zoomRatio *= 0.5;
		mMapCone->setDimensions(0.15*zoomRatio, 0.2*zoomRatio);
		mMapCone->setPosition(mMap->getWidth()*0.5 - mMapCone->getWidth()*0.5,mMap->getHeight()*0.5 - mMapCone->getHeight()*0.5);
	}
	bool isInMarker(OverlayElement *marker, const Real &x, const Real &y)
	{
		if(!mMap->contains(x,y))return false;
		const Real tX = mMap->getLeft() + marker->getLeft();
		const Real tY = mMap->getTop() + marker->getTop();
		const Real tQuartWidth = marker->getWidth()/4;
		const Real tQuartHeight = marker->getHeight()/4;

		if(x >= tX+tQuartWidth && x <= tX+marker->getWidth()-tQuartWidth
			&& y >= tY+tQuartHeight && y <= tY+marker->getHeight()-tQuartHeight)
			return true;
		return false;
	}
	void setMapTagUseCharName(bool flag)
	{
		mapTagUseCharName = flag;
	}
	void updatePortalMap()
	{
		for(int i=0;i<(int)hasPortalMarker.size();i++)
			if(hasPortalMarker[i])
				OverlayManager::getSingleton().destroyOverlayElement("PortalMarker"+StringConverter::toString(i));
		for(int i=0;i<(int)hasGateMarker.size();i++)
			if(hasGateMarker[i])
				OverlayManager::getSingleton().destroyOverlayElement("GateMarker"+StringConverter::toString(i));
		mCollisionManager->getPortalMap(portalMap);
		mCollisionManager->getGateMap(gateMap);
		hasPortalMarker.clear();
		hasGateMarker.clear();
		for(int i=0;i<(int)portalMap.size();i++)hasPortalMarker.push_back(false);
		for(int i=0;i<(int)gateMap.size();i++)hasGateMarker.push_back(false);
	}
	void updateUnitMarker(MagixIndexedUnit *unit)
	{
		if(!unit)return;
		if(!unit->hasMarker)return;
		OverlayElement *tUnitMarker = OverlayManager::getSingleton().getOverlayElement("UnitMarker"+StringConverter::toString(unit->getUnitID()));
		if(!tUnitMarker)return;
		if(mUnitManager->isPartyMember(unit))tUnitMarker->setMaterialName("GUIMat/MiniMapDotGreen");
		else tUnitMarker->setMaterialName("GUIMat/MiniMapDotYellow");
	}
	void pingUnitMarker(MagixIndexedUnit *unit)
	{
		if(!unit)return;
		if(!unit->hasMarker)return;
		OverlayElement *tUnitMarker = OverlayManager::getSingleton().getOverlayElement("UnitMarker"+StringConverter::toString(unit->getUnitID()));
		if(!tUnitMarker)return;
		pingUnitMarkers.push_back(unit->getUnitID());
		pingCounter = 20;
	}
};

#endif
