#pragma once

#include "MagixExternalDefinitions.h"
#include "MagixUnitManager.h"
#include "MagixCollisionManager.h"
#include "MagixCamera.h"

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
	MagixMiniMap();
	~MagixMiniMap();
	void reset();
	void initialize(MagixExternalDefinitions *def, MagixUnitManager *unitMgr, MagixCollisionManager *collisionMgr, MagixCamera *camera);
	void show(bool flag);
	void flipVisibility();
	void update(bool cursorVisible, const Real &cursorX, const Real &cursorY, const FrameEvent &evt);
	bool mapPosition(const Vector2 &center, const Vector2 &input, Vector2 &output);
	void zoomIn();
	void zoomOut();
	bool isInMarker(OverlayElement *marker, const Real &x, const Real &y);
	void setMapTagUseCharName(bool flag);
	void updatePortalMap();
	void updateUnitMarker(MagixIndexedUnit *unit);
	void pingUnitMarker(MagixIndexedUnit *unit);
};

