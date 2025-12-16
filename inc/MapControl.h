/*
 ============================================================================
 Name		: MapControl.h
 Author	  : artem78
 Copyright   : 
 Description : Declares map control
 ============================================================================
 */

#ifndef __MAPCONTROL_h__
#define __MAPCONTROL_h__

// INCLUDES
#include <coecntrl.h>

#include <lbsposition.h>
#include "MapMath.h"
#include "Map.h"
#include "Defs.h"
#include <s32strm.h>
#include "Utils.h"

// Constants
const TUint KMapDefaultMoveStep = 20; // In pixels

// Hard restrictions for MapView zoom level
const TZoom KMinZoomLevel = /*0*/ 1;
const TZoom KMaxZoomLevel = 19;

// CLASS DECLARATION

class CCoeControlWithDelayedDraw : public CCoeControl
	{ // ToDo: Write instruction how to use this
private:
	TInt iCounter; // Nesting level of sections with disabled redrawings
	TBool iIsDrawNeeded; // Flag indicates if redrawing is needed after redrawings fully enabled
	
	enum TPanic
		{
		ENegativeCounter = 1,		// iCounter < 0
		ENonZeroCounterInDestructor	// iCounter != 0
		};
	
	void RaisePanic(TPanic aPanicCode);

protected:
	void EnableDraw();
	void DisableDraw();
	void DrawDelayed();
	
public:
	virtual ~CCoeControlWithDelayedDraw();
	};


class CMapControl : public CCoeControlWithDelayedDraw
	{
public:
	enum TS60MapsMovement
		{
		EMoveNone,
		EMoveUp,
		EMoveDown,
		EMoveLeft,
		EMoveRight
		};

public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a CMapControl object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of CMapControl.
	 */
	static CMapControl* NewL(const TRect& aRect,
			const TCoordinate &aInitialPosition, TZoom aInitialZoom,
			//TZoom aMinZoom = KMinZoomLevel, TZoom aMaxZoom = KMaxZoomLevel
			TTileProvider* aTileProvider);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Create a CMapControl object, which will draw itself
	 * to aRect.
	 * @param aRect Rectangle this view will be drawn to.
	 * @return A pointer to the created instance of CMapControl.
	 */
	static CMapControl* NewLC(const TRect& aRect,
			const TCoordinate &aInitialPosition, TZoom aInitialZoom,
			//TZoom aMinZoom = KMinZoomLevel, TZoom aMaxZoom = KMaxZoomLevel,
			TTileProvider* aTileProvider);

	/**
	 * ~CMapControl
	 * Virtual Destructor.
	 */
	virtual ~CMapControl();


public:
	// Functions from base classes

	/**
	 * From CCoeControl, Draw
	 * Draw this CMapControl to the screen.
	 * @param aRect the rectangle of this view that needs updating
	 */
	void Draw(const TRect& aRect) const;

	/**
	 * From CoeControl, SizeChanged.
	 * Called by framework when the view size is changed.
	 */
	virtual void SizeChanged();

	/**
	 * From CoeControl, HandlePointerEventL.
	 * Called by framework when a pointer touch event occurs.
	 * Note: although this method is compatible with earlier SDKs, 
	 * it will not be called in SDKs without Touch support.
	 * @param aPointerEvent the information about this event
	 */
	/*virtual*/ void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	
	/*virtual*/ TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent,
			TEventCode aType);

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 * Perform the second phase construction of a
	 * CMapControl object.
	 * @param aRect The rectangle this view will be drawn to.
	 */
	void ConstructL(const TRect& aRect, const TCoordinate &aInitialPosition,
			/*TZoom aMinZoom, TZoom aMaxZoom,*/
			TTileProvider* aTileProvider);

	/**
	 * CMapControl.
	 * C++ default constructor.
	 */
	CMapControl(TZoom aInitialZoom);


// Custom properties and methods
private:
	TPoint iTopLeftPosition; // Mercators coordinates of control`s top left corner in pixels
							 // Note: Do not directly change this value! Use Move() instead.
	TZoom iZoom; // Zoom level from KMinZoomLevel to KMaxZoomLevel
				 // Note: Do not directly change this value! Use SetZoom() instead.
	TZoom iMinZoom; // Minimum zoom level
	TZoom iMaxZoom; // Maximum zoom level
	TCoordinate iCenterPosition; // Similar to iTopLeftPosition, but used for
				// more accurate moving to position when zoom changed
				// ToDo: Any ideas how to make it without additional property? 
	RPointerArray<CMapLayerBase> iLayers;
	
	TCoordinateEx iUserPosition;
	TBool iIsUserPositionRecieved; // Todo: Redundant flag - iUserPosition with lat=NaN and lon=NaN can indicates unknown position 
	TBool iIsFollowUser;
	TBool iIsUserPositionVisiblePrev; // Todo: Redundant?

	/*
	 * iPointerDownPosition
	 * is used to distinguish touchings
	 * from swipes.
	 */
	TPoint iPointerDownPosition;
	/*
	 * iMovement, iMovementRepeater
	 * are used to repeat the movement
	 * at long touching (holding).
	 */
	TS60MapsMovement iMovement;
	CPeriodic* iMovementRepeater;
	
	TBool iIsSoftkeysShown;
	TBool iIsCrosshairVisible;
	CPeriodic* /*iUserInactivityTimer*/ iCrosshairAutoHideTimer;
	CFont* iDefaultFont;
	CFont* iSmallFont;
	
	void Move(const TPoint &aPoint, TBool savePos = ETrue); // Used by all another Move methods
public:
	void Move(const TCoordinate &aPos);
	void Move(const TCoordinate &aPos, TZoom aZoom);
	void MoveAndZoomIn(const TCoordinate &aPos);
	void MoveAndZoomToBounds(const TBounds &aBounds);

	static TInt MovementRepeaterCallback(TAny* aObject);
	void ExecuteMovement();

	void SetZoomBounds(TZoom aMinZoom, TZoom aMaxZoom);
private:
	void SetZoom(TZoom aZoom);
public:
	void ZoomIn();
	void ZoomOut();
private:
	void MoveUp(	TUint aPixels = KMapDefaultMoveStep);
	void MoveDown(	TUint aPixels = KMapDefaultMoveStep);
	void MoveLeft(	TUint aPixels = KMapDefaultMoveStep);
	void MoveRight(	TUint aPixels = KMapDefaultMoveStep);
	/*inline*/ TPoint ProjectionCoordsToScreenCoords(const TPoint &aPoint) const;
	
	void UpdateUserPosition();
	inline TBool IsUserPositionVisible() // Note: Location marker size ignored (i.e. like a point)
			{ return iIsUserPositionRecieved && CheckCoordVisibility(iUserPosition); };
	void ShowCrosshair();
	void HideCrosshair();
	inline TBool IsCrosshairVisible() const
			{ return iIsCrosshairVisible; };
	static TInt CrosshairAutoHideCallback(TAny* anObj);
	void ShowCrosshairForAShortTime();
	
public:
	/*inline*/ TZoom GetZoom() const;
	TCoordinate GetCenterCoordinate() const;
	TBool CheckCoordVisibility(const TCoordinate &aCoord, TInt aGoingBeyondToleranceInPx = 0) const;
	TBool CheckPointVisibility(const TPoint &aPoint, TInt aGoingBeyondTolerance/*InPx*/ = 0) const;
	TPoint GeoCoordsToScreenCoords(const TCoordinate &aCoord) const;
	TCoordinate ScreenCoordsToGeoCoords(const TPoint &aPoint) const;
	void Bounds(TBounds &aCoordRect) const;
	void Bounds(TTile &aTopLeftTile, TTile &aBottomRightTile) const;
	
	void SetUserPosition(const TCoordinateEx& aPos);
	TInt UserPosition(TCoordinateEx& aPos);
	void ShowUserPosition();
	void HideUserPosition();
	void SetFollowUser(TBool anEnabled = ETrue);
	inline TBool IsFollowingUser()
		{ return iIsFollowUser; };
	void SetTileProviderL(TTileProvider* aTileProvider);
	inline TBool IsSoftkeysShown()
		{ return iIsSoftkeysShown; };
	void HandleLanguageChangedL();
	void ReloadVisibleAreaL();
	inline const CFont* DefaultFont() const
		{ return iDefaultFont; };
	inline const CFont* SmallFont() const // for copyright string
		{ return iSmallFont; };
	void NotifyLandmarksUpdated();
	/*inline*/ TPoint ScreenCoordsToProjectionCoords(const TPoint &aPoint) const;
	
private:
	TZoom PreferredZoomForBounds(const TBounds &aBounds) const;

	};
	
#endif // __MAPCONTROL_h__
// End of File
