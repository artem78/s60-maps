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
#include "Defs.h"
#include <aknappui.h> 
#include "S60Maps.pan"
#include "S60MapsAppUi.h"

// Constants
const TInt KMovementRepeaterInterval = 200000;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CS60MapsAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CS60MapsAppView* CS60MapsAppView::NewL(const TRect& aRect,
		const TCoordinate &aInitialPosition, TZoom aInitialZoom,
		//TZoom aMinZoom, TZoom aMaxZoom,
		TTileProvider* aTileProvider)
	{
	CS60MapsAppView* self = CS60MapsAppView::NewLC(aRect, aInitialPosition, aInitialZoom,
			/*aMinZoom, aMaxZoom,*/ aTileProvider);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CS60MapsAppView* CS60MapsAppView::NewLC(const TRect& aRect,
		const TCoordinate &aInitialPosition, TZoom aInitialZoom,
		//TZoom aMinZoom, TZoom aMaxZoom,
		TTileProvider* aTileProvider)
	{
	CS60MapsAppView* self = new (ELeave) CS60MapsAppView(aInitialZoom);
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aInitialPosition, /*aMinZoom, aMaxZoom,*/ aTileProvider);
	return self;
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CS60MapsAppView::ConstructL(const TRect& aRect, const TCoordinate &aInitialPosition,
		//TZoom aMinZoom, TZoom aMaxZoom,
		TTileProvider* aTileProvider)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CEikonEnv::Static()->AppUi());
	
	//SetZoomBounds(aMinZoom, aMaxZoom);
	//SetZoomBounds(aTileProvider->MinZoomLevel(), aTileProvider->MaxZoomLevel());
//	SetTileProviderL(aTileProvider);
	
	// Create layers
	iLayers = RPointerArray<CMapLayerBase>(10);
	iLayers.Append(CTiledMapLayer::NewL(this, aTileProvider));
#ifdef DEBUG_SHOW_TILE_BORDER_AND_XYZ
	iLayers.Append(new (ELeave) CTileBorderAndXYZLayer(this));
#endif
	iLayers.Append(new (ELeave) CUserPositionLayer(this));
	iLayers.Append(CScaleBarLayer::NewL(this));
	iLayers.Append(CLandmarksLayer::NewL(this, appUi->LandmarkDb()));
#ifdef DEBUG_SHOW_ADDITIONAL_INFO
	iLayers.Append(new (ELeave) CMapLayerDebugInfo(this));
#endif
	iLayers.Append(new (ELeave) CCrosshairLayer(this));
	
	SetTileProviderL(aTileProvider);

	// Periodic timer for repeating the movement at holding (touch interface)
	iMovementRepeater = CPeriodic::NewL(0); // neutral priority
	
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Set initial displayed position
	Move(aInitialPosition);
	
	// Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::CS60MapsAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CS60MapsAppView::CS60MapsAppView(TZoom aInitialZoom) :
	iZoom(aInitialZoom),
	iIsSoftkeysShown(EFalse)
	// Position will be set later in ConstructL
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
	iLayers.ResetAndDestroy();

	iMovementRepeater->Cancel();
	delete iMovementRepeater;
	iMovementRepeater = NULL;
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
	TInt i;
	for (i = 0; i < iLayers.Count(); i++)
		{
		//Window().BeginRedraw();
		gc.Reset();
		iLayers[i]->Draw(gc);
		//Window().EndRedraw();
		}
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
void CS60MapsAppView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	const TInt KSwipingThreshold = 100; // px
	const TInt KTouchingThreshold = 15; // px

	if (aPointerEvent.iType == TPointerEvent::EButton1Down)
		{
		iMovementRepeater->Cancel();
		iPointerDownPosition = aPointerEvent.iPosition;
		// Request drag events
		Window().PointerFilter(EPointerFilterDrag, 0);

		/*
		 * +---------------------+ 
		 * |                     |
		 * |       MoveUp        | 1/4
		 * |                     |
		 * +----------+----------+ 
		 * |          |          |
		 * |          |          |
		 * |  Move    |   Move   | 2/4
		 * |  Left    |   Right  |
		 * |          |          |
		 * |          |          |
		 * +---1/2----+---1/2----+
		 * |                     |
		 * |      MoveDown       | 1/4
		 * |                     |
		 * +---------------------+
		 */
		TRect upRect(Rect());
		upRect.SetHeight(Size().iHeight / 4);
		TRect downRect(upRect);
		downRect.Move(0, Size().iHeight * 3 / 4);
		TRect leftRect(upRect);
		leftRect.SetWidth(Size().iWidth / 2);
		leftRect.SetHeight(Size().iHeight / 2);
		leftRect.Move(0, Size().iHeight / 4);
		TRect rightRect(leftRect);
		rightRect.Move(Size().iWidth / 2, 0);
		
		if (upRect.Contains(aPointerEvent.iPosition))
			{
			iMovement = EMoveUp;
			}
		else if (downRect.Contains(aPointerEvent.iPosition))
			{
			iMovement = EMoveDown;
			}
		else if (leftRect.Contains(aPointerEvent.iPosition))
			{
			iMovement = EMoveLeft;
			}
		else if (rightRect.Contains(aPointerEvent.iPosition))
			{
			iMovement = EMoveRight;
			}
		else
			{
			iMovement = EMoveNone;
			}

		iMovementRepeater->Start(
				KMovementRepeaterInterval * 2,
				KMovementRepeaterInterval,
				TCallBack(MovementRepeaterCallback, this));
		}
	else if (aPointerEvent.iType == TPointerEvent::EDrag)
		{
		TPoint posDelta = aPointerEvent.iPosition - iPointerDownPosition;
		if (Max(Abs(posDelta.iX), Abs(posDelta.iY)) > KTouchingThreshold)
			{
			iMovementRepeater->Cancel();
			iMovement = EMoveNone;
			}
		}
	else if (aPointerEvent.iType == TPointerEvent::EButton1Up)
		{
		iMovementRepeater->Cancel();
		// Cancel the request for drag events
		Window().PointerFilter(EPointerFilterDrag, EPointerFilterDrag);

		TPoint posDelta = aPointerEvent.iPosition - iPointerDownPosition;
		if (Abs(posDelta.iX) > KSwipingThreshold)
			{
			// swiping left/right -> zoom out/in
			if (posDelta.iX < 0)
				{
				ZoomOut();
				}
			else
				{
				ZoomIn();
				}
			}
		else if (Abs(posDelta.iY) > KSwipingThreshold)
			{
			// swiping up/down -> show/hide softkeys
			if (posDelta.iY < 0)
				{
				SetRect(iAvkonAppUi->ClientRect());
				iIsSoftkeysShown = ETrue;
				}
			else
				{
				SetRect(iAvkonAppUi->ApplicationRect());
				iIsSoftkeysShown = EFalse;
				}
			}
		else
			{
			// touching
			ExecuteMovement();
			}
		}

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

TKeyResponse CS60MapsAppView::OfferKeyEventL(const TKeyEvent &aKeyEvent,
		TEventCode aType)
	{
	if (aType == EEventKey /*EEventKeyDown*/)
		{
		switch (aKeyEvent.iScanCode)
			{
			case EStdKeyUpArrow:
			case '2':
			//case EStdKeyNkp2:
				{
				SetFollowUser(EFalse);
				MoveUp();
				return EKeyWasConsumed;
				//break;
				}
				
			case EStdKeyDownArrow:
			case '8':
			//case EStdKeyNkp8:
				{
				SetFollowUser(EFalse);
				MoveDown();
				return EKeyWasConsumed;
				//break;
				}
				
			case EStdKeyLeftArrow:
			case '4':
			//case EStdKeyNkp4:
				{
				SetFollowUser(EFalse);
				MoveLeft();
				return EKeyWasConsumed;
				//break;
				}
				
			case EStdKeyRightArrow:
			case '6':
			//case EStdKeyNkp6:
				{
				SetFollowUser(EFalse);
				MoveRight();
				return EKeyWasConsumed;
				//break;
				}
				
			case '3':
			//case EStdKeyNkp3:
				{
				ZoomIn();
				return EKeyWasConsumed;
				//break;
				}
				
			case '1':
			//case EStdKeyNkp1:
				{
				ZoomOut();
				return EKeyWasConsumed;
				//break;
				}
			}
		}
	
	return EKeyWasNotConsumed;
	}

void CS60MapsAppView::Move(const TPoint &aPoint, TBool aSavePos)
	{
	// Check that position has changed
	if (iTopLeftPosition != aPoint)
		{
		iTopLeftPosition = aPoint;	
		if (aSavePos)
			{
			TPoint center = aPoint + Rect().Center();
			iCenterPosition = MapMath::ProjectionPointToGeoCoords(center, iZoom); // Store new position
			}
		
		TReal tmp;
		Math::Pow(tmp, 2, iZoom);
		TInt maxXY = KTileSize * (int) tmp - 1;
		//TRect mapRect;
		//mapRect.SetSize(TSize(maxXY, maxXY));
		
		TRect viewRect;
		viewRect.SetRect(Rect().iTl, Rect().iBr);
		viewRect.Move(iTopLeftPosition);
		
		// Correct longitude when it goes out of bounds
		if (viewRect.iTl.iY < 0)
			iTopLeftPosition.iY = 0;
		else if (viewRect.iBr.iY > maxXY)
			iTopLeftPosition.iY = maxXY - viewRect.Height() + 1;
		
		// Correct latitude when it goes out of bounds
		if (viewRect.iTl.iX < 0)
			iTopLeftPosition.iX = 0;
		else if (viewRect.iBr.iX > maxXY)
			iTopLeftPosition.iX = maxXY - viewRect.Width() + 1;
		
		
		DrawNow();
		}
	}

void CS60MapsAppView::Move(const TCoordinate &aPos)
	{
	/*__ASSERT_ALWAYS*/
	__ASSERT_DEBUG(Math::IsFinite(aPos.Latitude()) && Math::IsFinite(aPos.Longitude()),
			Panic(ES60MapsInvalidPosition)); // Possible may be NaN
	
	iCenterPosition = aPos; // Store new position
	TPoint point = MapMath::GeoCoordsToProjectionPoint(aPos, iZoom);
	// Convert from center to top left
	point.iX -= Rect().Width() / 2;
	point.iY -= Rect().Height() / 2;
	Move(point, EFalse);
	}

void CS60MapsAppView::Move(const TCoordinate &aPos, TZoom aZoom)
	{
	// FixMe: Redrawing called twice
	Move(aPos);
	SetZoom(aZoom);
	}

void CS60MapsAppView::Move(TReal64 aLat, TReal64 aLon)
	{
	Move(aLat, aLon, iZoom);
	}

void CS60MapsAppView::Move(TReal64 aLat, TReal64 aLon, TZoom aZoom)
	{
	TCoordinate coord(aLat, aLon);
	Move(coord, aZoom);
	}

void CS60MapsAppView::SetZoomBounds(TZoom aMinZoom, TZoom aMaxZoom)
	{
	// Checks
	if (aMinZoom <= aMaxZoom)
		{
		iMinZoom = Max(aMinZoom, KMinZoomLevel);
		iMaxZoom = Min(aMaxZoom, KMaxZoomLevel);
		}
	
	// ToDo: Return error when checks failed
	
	// Set zoom to maximum/minimum available value if it went out of bounds
	if (iZoom < iMinZoom)
		SetZoom(iMinZoom);
	else if (iZoom > iMaxZoom)
		SetZoom(iMaxZoom);
	}

void CS60MapsAppView::SetZoom(TZoom aZoom)
	{
	// ToDo: Return error code KErrArgument or panic if zoom out of bounds
	if (aZoom >= iMinZoom and aZoom <= iMaxZoom)
		{
		if (iZoom != aZoom)
			{
			iZoom = aZoom;
			Move(iCenterPosition);
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

void CS60MapsAppView::MoveUp(TUint aPixels)
	{
	TPoint point = iTopLeftPosition;
	point.iY -= aPixels;
	Move(point);
	}

void CS60MapsAppView::MoveDown(TUint aPixels)
	{
	TPoint point = iTopLeftPosition;
	point.iY += aPixels;
	Move(point);
	}

void CS60MapsAppView::MoveLeft(TUint aPixels)
	{
	TPoint point = iTopLeftPosition;
	point.iX -= aPixels;
	Move(point);
	}

void CS60MapsAppView::MoveRight(TUint aPixels)
	{
	TPoint point = iTopLeftPosition;
	point.iX += aPixels;
	Move(point);
	}

TZoom CS60MapsAppView::GetZoom() const
	{
	return iZoom;
	}

TCoordinate CS60MapsAppView::GetCenterCoordinate() const
	{
	TPoint point = iTopLeftPosition;
	point.iX += Rect().Width() / 2;
	point.iY += Rect().Height() / 2;
	return MapMath::ProjectionPointToGeoCoords(point, iZoom);
	}

TBool CS60MapsAppView::CheckCoordVisibility(const TCoordinate &aCoord) const
	{	
	TPoint projectionPoint = MapMath::GeoCoordsToProjectionPoint(aCoord, iZoom);
	TPoint screenPoint = ProjectionCoordsToScreenCoords(projectionPoint);
	return CheckPointVisibility(screenPoint);
	}

TBool CS60MapsAppView::CheckPointVisibility(const TPoint &aPoint) const
	{
	TRect screenRect = Rect();
	screenRect.Resize(1, 1);
	return screenRect.Contains(aPoint);
	}

TPoint CS60MapsAppView::GeoCoordsToScreenCoords(const TCoordinate &aCoord) const
	{
	TPoint point = MapMath::GeoCoordsToProjectionPoint(aCoord, iZoom);
	return ProjectionCoordsToScreenCoords(point);
	}

TCoordinate CS60MapsAppView::ScreenCoordsToGeoCoords(const TPoint &aPoint) const
	{	
	TPoint point = ScreenCoordsToProjectionCoords(aPoint);
	return MapMath::ProjectionPointToGeoCoords(point, iZoom);
	}

TPoint CS60MapsAppView::ProjectionCoordsToScreenCoords(const TPoint &aPoint) const
	{
	return aPoint - iTopLeftPosition;
	}

TPoint CS60MapsAppView::ScreenCoordsToProjectionCoords(const TPoint &aPoint) const
	{
	return aPoint + iTopLeftPosition;
	}

void CS60MapsAppView::Bounds(TCoordinate &aTopLeftCoord, TCoordinate &aBottomRightCoord) const
	{
	aTopLeftCoord = ScreenCoordsToGeoCoords(Rect().iTl);
	aBottomRightCoord = ScreenCoordsToGeoCoords(Rect().iBr - TPoint(1, 1));
	}

void CS60MapsAppView::Bounds(TTile &aTopLeftTile, TTile &aBottomRightTile) const
	{
	TPoint topLeftProjection = ScreenCoordsToProjectionCoords(Rect().iTl);
	TPoint bottomRightProjection = ScreenCoordsToProjectionCoords(Rect().iBr - TPoint(1, 1));
	aTopLeftTile = MapMath::ProjectionPointToTile(topLeftProjection, GetZoom());
	aBottomRightTile = MapMath::ProjectionPointToTile(bottomRightProjection, GetZoom());
	}

void CS60MapsAppView::Bounds(TTileReal &aTopLeftTile, TTileReal &aBottomRightTile) const
	{
	TCoordinate topLeftCoord, bottomRightCoord;
	Bounds(topLeftCoord, bottomRightCoord);
	aTopLeftTile = MapMath::GeoCoordsToTileReal(topLeftCoord, GetZoom());
	aBottomRightTile = MapMath::GeoCoordsToTileReal(bottomRightCoord, GetZoom());
	}

void CS60MapsAppView::UpdateUserPosition()
	{
	if (iIsFollowUser && iIsUserPositionRecieved)
		Move(iUserPosition);
	else
		DrawNow();
	}

void CS60MapsAppView::SetUserPosition(const TCoordinateEx& aPos)
	{
	iUserPosition = aPos;
	ShowUserPosition();
	}

TInt CS60MapsAppView::UserPosition(TCoordinateEx& aPos)
	{
	if (!iIsUserPositionRecieved)
		return KErrNotFound;
	
	aPos = iUserPosition;
	return KErrNone;
	}

void CS60MapsAppView::ShowUserPosition()
	{
	iIsUserPositionRecieved = ETrue;
	UpdateUserPosition();
	}

void CS60MapsAppView::HideUserPosition()
	{
	iIsUserPositionRecieved = EFalse;
	UpdateUserPosition();
	}

void CS60MapsAppView::SetFollowUser(TBool anEnabled)
	{
	iIsFollowUser = anEnabled;
	
	if (iIsFollowUser && iIsUserPositionRecieved)
		{
		const TZoom minZoom = 16;
		if (GetZoom() < minZoom)
			SetZoom(minZoom);
		}
	UpdateUserPosition();
	}

TInt CS60MapsAppView::MovementRepeaterCallback(TAny* aObject)
	{
	((CS60MapsAppView*)aObject)->ExecuteMovement();
	return 1;
	}

void CS60MapsAppView::ExecuteMovement()
	{
	switch(iMovement)
		{
		case EMoveUp:
			{
			SetFollowUser(EFalse);
			MoveUp();
			break;
			}
		case EMoveDown:
			{
			SetFollowUser(EFalse);
			MoveDown();
			break;
			}
		case EMoveLeft:
			{
			SetFollowUser(EFalse);
			MoveLeft();
			break;
			}
		case EMoveRight:
			{
			SetFollowUser(EFalse);
			MoveRight();
			break;
			}
		case EMoveNone:
			break;
		}
	}

void CS60MapsAppView::SetTileProviderL(TTileProvider* aTileProvider)
	{
	static_cast<CTiledMapLayer*>(iLayers[0 /*tiled map*/])->SetTileProviderL(aTileProvider);
	SetZoomBounds(aTileProvider->iMinZoomLevel, aTileProvider->iMaxZoomLevel);
	}

// End of File
