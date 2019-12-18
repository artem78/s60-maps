/*
 ============================================================================
 Name		: S60MapsAppView.h
 Author	  : artem78
 Copyright   : 
 Description : Declares view class for application.
 ============================================================================
 */

#ifndef __S60MAPSAPPVIEW_h__
#define __S60MAPSAPPVIEW_h__

// INCLUDES
#include <coecntrl.h>

#include <lbsposition.h>
#include "MapMath.h"
#include "Map.h"
#include "Defs.h"
#include <s32strm.h>

// Constants
const TUint KMapDefaultMoveStep = 20; // In pixels

// CLASS DECLARATION
class CS60MapsAppView : public CCoeControl
	{
public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a CS60MapsAppView object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of CS60MapsAppView.
	 */
	static CS60MapsAppView* NewL(const TRect& aRect,
			const TCoordinate &aInitialPosition, TZoom aInitialZoom);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Create a CS60MapsAppView object, which will draw itself
	 * to aRect.
	 * @param aRect Rectangle this view will be drawn to.
	 * @return A pointer to the created instance of CS60MapsAppView.
	 */
	static CS60MapsAppView* NewLC(const TRect& aRect,
			const TCoordinate &aInitialPosition, TZoom aInitialZoom);

	/**
	 * ~CS60MapsAppView
	 * Virtual Destructor.
	 */
	virtual ~CS60MapsAppView();
	
	void ExternalizeL(RWriteStream &aStream) const;
	void InternalizeL(RReadStream &aStream); 

public:
	// Functions from base classes

	/**
	 * From CCoeControl, Draw
	 * Draw this CS60MapsAppView to the screen.
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
	//virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	
	/*virtual*/ TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent,
			TEventCode aType);

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 * Perform the second phase construction of a
	 * CS60MapsAppView object.
	 * @param aRect The rectangle this view will be drawn to.
	 */
	void ConstructL(const TRect& aRect, const TCoordinate &aInitialPosition);

	/**
	 * CS60MapsAppView.
	 * C++ default constructor.
	 */
	CS60MapsAppView(TZoom aInitialZoom);


// Custom properties and methods
private:
	TPoint iTopLeftPosition; // Mercators coordinates of control`s top left corner in pixels
							 // Note: Do not directly change this value! Use Move() instead.
	TZoom iZoom; // Zoom level from KMinZoomLevel to KMaxZoomLevel
				 // Note: Do not directly change this value! Use SetZoom() instead.
	TFixedArray<CMapLayerBase*, 2> iLayers;
	
	void Move(const TPoint &aPoint);
	void Move(const TCoordinate &aPos);
	void Move(const TCoordinate &aPos, TZoom aZoom);
	void Move(TReal64 aLat, TReal64 aLon);
	void Move(TReal64 aLat, TReal64 aLon, TZoom aZoom);
	void SetZoom(TZoom aZoom);
	void ZoomIn();
	void ZoomOut();
	void MoveUp(	TUint aPixels = KMapDefaultMoveStep);
	void MoveDown(	TUint aPixels = KMapDefaultMoveStep);
	void MoveLeft(	TUint aPixels = KMapDefaultMoveStep);
	void MoveRight(	TUint aPixels = KMapDefaultMoveStep);
	/*inline*/ TPoint ProjectionCoordsToScreenCoords(const TPoint &aPoint) const;
	/*inline*/ TPoint ScreenCoordsToProjectionCoords(const TPoint &aPoint) const;
	
public:
	/*inline*/ TZoom GetZoom() const;
	TCoordinate GetCenterCoordinate() const;
	TBool CheckCoordVisibility(const TCoordinate &aCoord) const;
	TBool CheckPointVisibility(const TPoint &aPoint) const;
	TPoint GeoCoordsToScreenCoords(const TCoordinate &aCoord) const;
	TCoordinate ScreenCoordsToGeoCoords(const TPoint &aPoint) const;

	};
	
#endif // __S60MAPSAPPVIEW_h__
// End of File
