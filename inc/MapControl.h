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

// Constants
const TUint KMapDefaultMoveStep = 20; // In pixels

// Hard restrictions for MapView zoom level
const TZoom KMinZoomLevel = /*0*/ 1;
const TZoom KMaxZoomLevel = 19;

// CLASS DECLARATION
class CMapControl : public CCoeControl
	{
public:
	enum S60MapsMovement
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
#ifdef DEBUG_SHOW_TILE_BORDER_AND_XYZ
	#ifdef DEBUG_SHOW_ADDITIONAL_INFO
	TFixedArray<CMapLayerBase*, 4> iLayers;
	#else
	TFixedArray<CMapLayerBase*, 3> iLayers;
	#endif
#else
	#ifdef DEBUG_SHOW_ADDITIONAL_INFO
	TFixedArray<CMapLayerBase*, 3> iLayers;
	#else
	TFixedArray<CMapLayerBase*, 2> iLayers;
	#endif
#endif
	
	TCoordinateEx iUserPosition;
	TBool iIsUserPositionRecieved;
	TBool iIsFollowUser;

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
	S60MapsMovement iMovement;
	CPeriodic* iMovementRepeater;
	
	void Move(const TPoint &aPoint, TBool savePos = ETrue); // Used by all another Move methods
public:
	void Move(const TCoordinate &aPos);
	void Move(const TCoordinate &aPos, TZoom aZoom);
	void Move(TReal64 aLat, TReal64 aLon);
	void Move(TReal64 aLat, TReal64 aLon, TZoom aZoom);

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
	/*inline*/ TPoint ScreenCoordsToProjectionCoords(const TPoint &aPoint) const;
	
	void UpdateUserPosition();
	
public:
	/*inline*/ TZoom GetZoom() const;
	TCoordinate GetCenterCoordinate() const;
	TBool CheckCoordVisibility(const TCoordinate &aCoord) const;
	TBool CheckPointVisibility(const TPoint &aPoint) const;
	TPoint GeoCoordsToScreenCoords(const TCoordinate &aCoord) const;
	TCoordinate ScreenCoordsToGeoCoords(const TPoint &aPoint) const;
	void Bounds(TCoordinate &aTopLeftCoord, TCoordinate &aBottomRightCoord) const;
	void Bounds(TTile &aTopLeftTile, TTile &aBottomRightTile) const;
	void Bounds(TTileReal &aTopLeftTile, TTileReal &aBottomRightTile) const;
	
	void SetUserPosition(const TCoordinateEx& aPos);
	TInt UserPosition(TCoordinateEx& aPos);
	void ShowUserPosition();
	void HideUserPosition();
	void SetFollowUser(TBool anEnabled = ETrue);
	void SetTileProviderL(TTileProvider* aTileProvider);

	};
	
#endif // __MAPCONTROL_h__
// End of File