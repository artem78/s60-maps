/*
 ============================================================================
 Name		: MapControlLayers.h
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : 
 ============================================================================
 */

#ifndef MAPCONTROLLAYERS_H
#define MAPCONTROLLAYERS_H

#include <e32base.h>
#include <fbs.h>
#include <epos_cposlandmarkdatabase.h>
#include <akniconutils.h> // For CAknIcon
#include <lbssatellite.h>
#include "Utils.h"
#include "Search.h"
#include "Map.h"


// Forward declaration
class CMapControl;


// Classes

class CMapLayerBase : public CBase
	{
protected:
	CMapControl* iMapView;
public:
	CMapLayerBase(/*const*/ CMapControl* aMapView);
	virtual void Draw(CWindowGc &aGc) = 0;
	};


#ifdef DEBUG_SHOW_ADDITIONAL_INFO
// Debug layer with additional info
class CMapLayerDebugInfo : public CMapLayerBase
	{
	// Constructor/destructor
private:
	CMapLayerDebugInfo(/*const*/ CMapControl* aMapView);
	void ConstructL();
	
public:
	static CMapLayerDebugInfo* NewL(CMapControl* aMapView);
	static CMapLayerDebugInfo* NewLC(CMapControl* aMapView);
	~CMapLayerDebugInfo();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// Own
private:
	TInt iRedrawingsCount;

	void DrawInfoL(CWindowGc &aGc);
	};
#endif


// Class for drawing map tiles
class CTiledMapLayer : public CMapLayerBase, public MTileBitmapManagerObserver
	{
// Base methods
public:
	~CTiledMapLayer();
	static CTiledMapLayer* NewL(CMapControl* aMapView, TTileProvider* aTileProvider);
	static CTiledMapLayer* NewLC(CMapControl* aMapView, TTileProvider* aTileProvider);

private:
	CTiledMapLayer(CMapControl* aMapView);
	void ConstructL(TTileProvider* aTileProvider);
	
// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
// From MTileBitmapManagerObserver
public:
	void OnTileLoaded();
	void OnTileLoadingFailed();
	
// Custom properties and methods
private:
	CTileBitmapManager* iBitmapMgr;
	TTileProvider* iTileProvider;
	void VisibleTiles(RArray<TTile> &aTiles); // Return list of visible tiles
	void DrawTile(CWindowGc &aGc, const TTile &aTile, const CFbsBitmap *aBitmap);
	void DrawError(CWindowGc &aGc, const TTile &aTile, const TDesC &aErrMsg);
	void DrawCopyrightText(CWindowGc &aGc);
	
public:
	void SetTileProviderL(TTileProvider* aTileProvider);
	void ReloadVisibleAreaL();
	};


// Displays user location
class CUserPositionLayer : public CMapLayerBase
	{
// From CMapLayerBase
public:
	CUserPositionLayer(/*const*/ CMapControl* aMapView);
	void Draw(CWindowGc &aGc);
	
// Own methods
private:
	void DrawAccuracyCircle(CWindowGc &aGc, const TPoint &aScreenPos, TSize aSize);
	void DrawDirectionMarkL(CWindowGc &aGc, const TPoint &aScreenPos, TReal aRotation);
	void DrawRoundMark(CWindowGc &aGc, const TPoint &aScreenPos);
	};


#ifdef DEBUG_SHOW_TILE_BORDER_AND_XYZ
// Debug layer for drawing tile`s border and x/y/z values.
// May be used as stub.
class CTileBorderAndXYZLayer : public CMapLayerBase
	{
// From CMapLayerBase
public:
	CTileBorderAndXYZLayer(CMapControl* aMapView);
	void Draw(CWindowGc &aGc);

// Custom properties and methods
private:
	void VisibleTiles(RArray<TTile> &aTiles); // Return list of visible tiles
	void DrawTile(CWindowGc &aGc, const TTile &aTile);
	
	};
#endif


class CScaleBarLayer : public CMapLayerBase
	{
	// Constructor / destructor
public:
	~CScaleBarLayer();
	static CScaleBarLayer* NewL(CMapControl* aMapView);
	static CScaleBarLayer* NewLC(CMapControl* aMapView);

private:
	CScaleBarLayer(CMapControl* aMapView);
	void ConstructL();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// Own
private:
	HBufC* iMetersUnit;
	HBufC* iKilometersUnit;
	
	void GetOptimalLength(TInt &optimalLength, TReal32 &optimalDistance);
	
public:
	void ReloadStringsFromResourceL();
	
	};


class CLandmarksLayer : public CMapLayerBase
	{
	// Constructor / destructor
public:
	~CLandmarksLayer();
	static CLandmarksLayer* NewL(CMapControl* aMapView, CPosLandmarkDatabase* aLmDb);
	static CLandmarksLayer* NewLC(CMapControl* aMapView, CPosLandmarkDatabase* aLmDb);

private:
	CLandmarksLayer(CMapControl* aMapView, CPosLandmarkDatabase* aLmDb);
	void ConstructL();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// Own
private:
	CPosLandmarkDatabase* iLandmarksDb; // Not owned
	CAknIcon* iIcon;
	
	TBounds iCachedArea; // Area for which landmarks are loaded
	CArrayPtr<CPosLandmark>* iCachedLandmarks; // May be NULL if no landmarks
	TBool iReloadNeeded; // Used for indication if landmarks may be changed outside (for ex. created/deleted/renamed)
	TZoom iZoom;
	/*RRegion*/ RRegionBuf<20> iNameRegion;
#ifdef __WINSCW__
	// For debug only
	TInt iVisibleIconsCount;
#endif
	
	void ReloadLandmarksListL(); // ToDo: Is moving to another class needed?
	void DrawL(CWindowGc &aGc);
	void DrawLandmarks(CWindowGc &aGc);
	void DrawLandmarkIcon(CWindowGc &aGc, const CPosLandmark* aLandmark);
	void DrawLandmarkName(CWindowGc &aGc, const CPosLandmark* aLandmark);
	
public:
	inline void NotifyLandmarksUpdated() { iReloadNeeded = ETrue; };
	};


class CCrosshairLayer : public CMapLayerBase
	{
	// Constructor / Destructor
public:
	CCrosshairLayer(CMapControl* aMapView);
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	};


class CSignalIndicatorLayer : public CMapLayerBase
	{	
	// Constructor / Destructor
public:
	static CSignalIndicatorLayer* NewL(CMapControl* aMapView);
	static CSignalIndicatorLayer* NewLC(CMapControl* aMapView);
	~CSignalIndicatorLayer();

private:
	CSignalIndicatorLayer(CMapControl* aMapView);
	void ConstructL();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// New
private:
	enum TSignalStrength
		{
		ESignalNone,
		ESignalVeryLow,
		ESignalLow,
		ESignalMedium,
		ESignalGood,
		ESignalVeryGood,
		ESignalHigh
		};
	
	// Constants for bars settings ("K" prefix used instead of "E" for enums)
	enum {
		KBarWidth		= 4,
		KStartBarHeight	= 5,
		KBarBorderWidth	= 1,
		KBarsSpacing	= 2,
		KBarHeightIncremement	= 3,
		KBarsCount		= ESignalHigh - ESignalVeryLow + 1,
		KBarsTotalWidth		= KBarsCount * KBarWidth + (KBarsCount - 1) * KBarsSpacing,
		KBarsTotalHeight	= KStartBarHeight + (KBarsCount - 1) * KBarHeightIncremement
	};

	CAknIcon* iSatelliteIcon;
	
	void DrawBarsV1(CWindowGc &aGc, TSignalStrength aBarsCount);
	TRect DrawBarsV2(CWindowGc &aGc, const TPoint &aTopRight, const TPositionSatelliteInfo &aSatInfo);
	void DrawSatelliteIcon(CWindowGc &aGc, const TPoint &aPos);
	
	/* Converts signal strength value to real number in range [0,1], where 0 = no signal and 1 = maximum signal */
	static TReal32 SignalStrengthToReal(TInt aSignalStrength);
	
	};


class CSearchResultsLayer : public CMapLayerBase
	{
	// Constructor / destructor
public:
	~CSearchResultsLayer();
	static CSearchResultsLayer* NewL(CMapControl* aMapView);
	static CSearchResultsLayer* NewLC(CMapControl* aMapView);

private:
	CSearchResultsLayer(CMapControl* aMapView);
	void ConstructL();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// Own
private:
	CAknIcon* iIcon;
	CAknIcon* iIconSelected;
	
	void DrawIcon(CWindowGc &aGc, const TSearchResultItem &aSearchResult, TBool aSelected = EFalse);
	void DrawTextWithBackgroundL(CWindowGc &aGc, const TSearchResultItem &aSearchResult);
	void DrawBackgroundBoxL(CWindowGc &aGc, const TRect &aRect, const TPoint &aArrowTopPoint);
	void DrawMultiLineText(CWindowGc &aGc, CArrayFix<TPtrC>* aLines, const CFont* aFont,
			TInt aLineHeight, const TRect &aFirstLineRect); // With horizontal alignment centered
	void IconRect(const TSearchResultItem &aSearchResult, TRect &aRect);
	};


class CTrack;

class CRouteLayer : public CMapLayerBase
	{
	// Constructor / destructor
public:
	~CRouteLayer();
	static CRouteLayer* NewL(CMapControl* aMapView);
	static CRouteLayer* NewLC(CMapControl* aMapView);

private:
	CRouteLayer(CMapControl* aMapView);
	void ConstructL();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// New members
private:
	void DrawL(CWindowGc &aGc);
	};



#endif // MAPCONTROLLAYERS_H
