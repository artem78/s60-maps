/*
 ============================================================================
 Name		: S60MapsAppView.cpp
 Author	  : artem78
 Copyright   : 
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include "S60MapsAppView.h"
#include <e32math.h>

// Constants
const /*TUInt8*/ TInt KMinZoomLevel = 0;
const /*TUInt8*/ TInt KMaxZoomLevel = 19;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CS60MapsAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CS60MapsAppView* CS60MapsAppView::NewL(const TRect& aRect,
		const TCoordinate &aInitialPosition, /*TUint8*/ TInt aInitialZoom)
	{
	CS60MapsAppView* self = CS60MapsAppView::NewLC(aRect, aInitialPosition, aInitialZoom);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CS60MapsAppView* CS60MapsAppView::NewLC(const TRect& aRect,
		const TCoordinate &aInitialPosition, /*TUint8*/ TInt aInitialZoom)
	{
	CS60MapsAppView* self = new (ELeave) CS60MapsAppView(aInitialPosition, aInitialZoom);
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CS60MapsAppView::ConstructL(const TRect& aRect)
	{
	// Create layers
	iLayers[0] = new (ELeave) CMapLayerStub(this);
	
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::CS60MapsAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CS60MapsAppView::CS60MapsAppView(const TCoordinate &aInitialPosition,
		/*TUint8*/ TInt aInitialZoom) :
	iPosition(aInitialPosition),
	iZoom(aInitialZoom)
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::~CS60MapsAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CS60MapsAppView::~CS60MapsAppView()
	{
	// Destroy all layers
	iLayers.DeleteAll();
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CS60MapsAppView::Draw(const TRect& /*aRect*/) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	// Gets the control's extent
	TRect drawRect(Rect());

	// Clears the screen
	gc.Clear(drawRect);
	
	// Draw layers
	// ToDo: Reset styles to default for each layers
	TInt i;
	for (i = 0; i < iLayers.Count(); i++)
		iLayers[i]->Draw(gc);
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CS60MapsAppView::SizeChanged()
	{
	DrawNow();
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
/*void CS60MapsAppView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}*/

TKeyResponse CS60MapsAppView::OfferKeyEventL(const TKeyEvent &aKeyEvent,
		TEventCode aType)
	{
	if (aType == EEventKey /*EEventKeyDown*/)
		{
		switch (aKeyEvent.iScanCode)
			{
			case /*EKeyUpArrow*/ EStdKeyUpArrow:
			case 50: // ToDo: Replace number to constant
				{
				MoveUp();
				return EKeyWasConsumed;
				//break;
				}
				
			case /*EKeyDownArrow*/ EStdKeyDownArrow:
			case 56: // ToDo: Replace number to constant
				{
				MoveDown();
				return EKeyWasConsumed;
				//break;
				}
				
			case /*EKeyLeftArrow*/ EStdKeyLeftArrow:
			case 52: // ToDo: Replace number to constant
				{
				MoveLeft();
				return EKeyWasConsumed;
				//break;
				}
				
			case /*EKeyRightArrow*/ EStdKeyRightArrow:
			case 54: // ToDo: Replace number to constant
				{
				MoveRight();
				return EKeyWasConsumed;
				//break;
				}
				
			case EStdKeyIncVolume:
			case 51: // ToDo: Replace number to constant
				{
				ZoomIn();
				return EKeyWasConsumed;
				//break;
				}
				
			case EStdKeyDecVolume:
			case 49: // ToDo: Replace number to constant
				{
				ZoomOut();
				return EKeyWasConsumed;
				//break;
				}
			}
		}
	
	return EKeyWasNotConsumed;
	}

void CS60MapsAppView::Move(const TCoordinate &aPos)
	{
	if (iPosition.Latitude() != aPos.Latitude() ||
			iPosition.Longitude() != aPos.Longitude()) // Altitude not needed
		{
		iPosition = aPos;
		DrawNow();
		}
	}

void CS60MapsAppView::Move(const TCoordinate &aPos, /*TUint8*/ TInt aZoom)
	{
	// FixMe: Redrawing called twice
	Move(aPos);
	SetZoom(aZoom);
	}

void CS60MapsAppView::Move(TReal64 aLat, TReal64 aLon)
	{
	Move(aLat, aLon, iZoom);
	}

void CS60MapsAppView::Move(TReal64 aLat, TReal64 aLon, /*TUint8*/ TInt aZoom)
	{
	TCoordinate coord(aLat, aLon);
	Move(coord, aZoom);
	}

void CS60MapsAppView::SetZoom(/*TUint8*/ TInt aZoom)
	{
	// ToDo: Return error code KErrArgument or panic if zoom out of bounds
	if (aZoom >= KMinZoomLevel and aZoom <= KMaxZoomLevel)
		{
		if (iZoom != aZoom)
			{
			iZoom = aZoom;
			DrawNow();
			}
		}
	}

void CS60MapsAppView::ZoomIn()
	{
	if (iZoom < KMaxZoomLevel)
		SetZoom(iZoom + 1);
	}

void CS60MapsAppView::ZoomOut()
	{
	if (iZoom > KMinZoomLevel)
		SetZoom(iZoom - 1);
	}

// ToDo: Reduce repetiotion of code
void CS60MapsAppView::MoveUp(TUint aPixels)
	{
	TReal64 horizontalAngle;
	TReal64 verticalAngle;
	MapMath::PixelsToDegrees(iPosition.Latitude(), iZoom, aPixels,
			horizontalAngle, verticalAngle);
	Move(iPosition.Latitude() + verticalAngle,
			iPosition.Longitude());
	}

void CS60MapsAppView::MoveDown(TUint aPixels)
	{
	TReal64 horizontalAngle;
	TReal64 verticalAngle;
	MapMath::PixelsToDegrees(iPosition.Latitude(), iZoom, aPixels,
			horizontalAngle, verticalAngle);
	Move(iPosition.Latitude() - verticalAngle,
			iPosition.Longitude());
	}

void CS60MapsAppView::MoveLeft(TUint aPixels)
	{
	TReal64 horizontalAngle;
	TReal64 verticalAngle;
	MapMath::PixelsToDegrees(iPosition.Latitude(), iZoom, aPixels,
			horizontalAngle, verticalAngle);
	Move(iPosition.Latitude(),
			iPosition.Longitude() - horizontalAngle);
	}

void CS60MapsAppView::MoveRight(TUint aPixels)
	{
	TReal64 horizontalAngle;
	TReal64 verticalAngle;
	MapMath::PixelsToDegrees(iPosition.Latitude(), iZoom, aPixels,
			horizontalAngle, verticalAngle);
	Move(iPosition.Latitude(),
			iPosition.Longitude() + horizontalAngle);
	}

/*TUint8*/ TInt CS60MapsAppView::GetZoom()
	{
	return iZoom;
	}

TCoordinate CS60MapsAppView::GetCenterCoordinate()
	{
	return iPosition;
	}

TBool CS60MapsAppView::CheckPointVisibility(const TCoordinate &aCoord)
	{
	TRect rect = Rect();
	TCoordinate topLeftCoord = ScreenCoordsToGeoCoords(rect.iTl);
	TCoordinate bottomRightCoord = ScreenCoordsToGeoCoords(rect.iBr);
	if (aCoord.Latitude() < topLeftCoord.Latitude() &&
		aCoord.Latitude() > bottomRightCoord.Latitude() &&
		aCoord.Longitude() > topLeftCoord.Longitude() &&
		aCoord.Longitude() < bottomRightCoord.Longitude())
		
		return ETrue;
	else
		return EFalse;
	}

TPoint CS60MapsAppView::GeoCoordsToScreenCoords(const TCoordinate &aCoord)
	{
	// ToDo: Check screen coords bounds
	
	TTileReal centerTileReal = MapMath::GeoCoordsToTileReal(iPosition, iZoom);
	TTileReal tileReal = MapMath::GeoCoordsToTileReal(aCoord, iZoom);
	
	TRect rect = Rect();
	
	TPoint point;
	TReal x = rect.Width() / 2.0 + (tileReal.iX - centerTileReal.iX) * 256.0; // ToDo: Use rect.Center()
	Math::Round(x, x, 0);
	point.iX = x;
	TReal y = rect.Height() / 2.0 + (tileReal.iY - centerTileReal.iY) * 256.0; // ToDo: Use rect.Center()
	Math::Round(y, y, 0);
	point.iY = y;
	
	return point;
	}

TCoordinate CS60MapsAppView::ScreenCoordsToGeoCoords(const TPoint &aPoint)
	{
	TRect rect = Rect();
	TTileReal centralTileReal = MapMath::GeoCoordsToTileReal(iPosition, iZoom);
	TTileReal tileReal;
	tileReal.iX = (rect.Width() / 2.0 - centralTileReal.iX * 256.0 - aPoint.iX) / -256.0; // ToDo: Use rect.Center()
	tileReal.iY = (rect.Height() / 2.0 - centralTileReal.iY * 256.0 - aPoint.iY) / -256.0; // ToDo: Use rect.Center()
	
	return MapMath::TileToGeoCoords(tileReal, iZoom);
	}

// End of File
