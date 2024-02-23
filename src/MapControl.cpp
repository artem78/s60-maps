/*
 ============================================================================
 Name		: MapControl.cpp
 Author	  : artem78
 Copyright   : 
 Description : Map control implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include "MapControl.h"
#include <e32math.h>
#include "Defs.h"
#include <aknappui.h> 
#include "S60Maps.pan"
#include "S60MapsAppUi.h"
#include "S60Maps.hrh"
#include "MapView.h"

// Constants
const TInt KMovementRepeaterInterval = 200000;

// Map layer ids
enum TMapLayerId
	{
	// Main layers
	ETiledMapLayerId,
#ifdef DEBUG_SHOW_TILE_BORDER_AND_XYZ	
	ETileBorderAndXYZLayerId,
#endif	
	ELandmarksLayerId,
	EUserPositionLayerId,
	EScaleBarLayerId,
#ifdef DEBUG_SHOW_ADDITIONAL_INFO
	EDebugInfoLayerId,
#endif	
	ECrosshairLayerId,
	ESignalIndicatorLayerId
	};

// ============================ MEMBER FUNCTIONS ===============================

// CCoeControlWithDelayedDraw

void CCoeControlWithDelayedDraw::EnableDraw()
	{
	iCounter--;
	__ASSERT_DEBUG(iCounter >= 0, RaisePanic(ENegativeCounter));
	
	DEBUG(_L("iCounter=%d iIsDrawNeeded=%d"), iCounter, iIsDrawNeeded);
	
	if (iCounter == 0)
		{
		if (iIsDrawNeeded)
			{
			DrawNow();
			}
			
		iIsDrawNeeded = EFalse;
		}
	};

void CCoeControlWithDelayedDraw::DisableDraw()
	{
	iCounter++;
	
	DEBUG(_L("iCounter=%d iIsDrawNeeded=%d"), iCounter, iIsDrawNeeded);
	};

void CCoeControlWithDelayedDraw::DrawDelayed()
	{
	iIsDrawNeeded = ETrue;
	
	DEBUG(_L("iCounter=%d iIsDrawNeeded=%d"), iCounter, iIsDrawNeeded);
	};

void CCoeControlWithDelayedDraw::RaisePanic(TPanic aPanicCode)
	{
	_LIT(KPanicCategory, "Delayed Draw");
	User::Panic(KPanicCategory, aPanicCode);
	}

CCoeControlWithDelayedDraw::~CCoeControlWithDelayedDraw()
	{
	__ASSERT_DEBUG(iCounter == 0, RaisePanic(ENonZeroCounterInDestructor));
	}


// CMapControl

// -----------------------------------------------------------------------------
// CMapControl::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMapControl* CMapControl::NewL(const TRect& aRect,
		const TCoordinate &aInitialPosition, TZoom aInitialZoom,
		//TZoom aMinZoom, TZoom aMaxZoom,
		TTileProvider* aTileProvider)
	{
	CMapControl* self = CMapControl::NewLC(aRect, aInitialPosition, aInitialZoom,
			/*aMinZoom, aMaxZoom,*/ aTileProvider);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CMapControl::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMapControl* CMapControl::NewLC(const TRect& aRect,
		const TCoordinate &aInitialPosition, TZoom aInitialZoom,
		//TZoom aMinZoom, TZoom aMaxZoom,
		TTileProvider* aTileProvider)
	{
	CMapControl* self = new (ELeave) CMapControl(aInitialZoom);
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aInitialPosition, /*aMinZoom, aMaxZoom,*/ aTileProvider);
	return self;
	}

// -----------------------------------------------------------------------------
// CMapControl::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMapControl::ConstructL(const TRect& aRect, const TCoordinate &aInitialPosition,
		//TZoom aMinZoom, TZoom aMaxZoom,
		TTileProvider* aTileProvider)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CEikonEnv::Static()->AppUi());
	
	//SetZoomBounds(aMinZoom, aMaxZoom);
	//SetZoomBounds(aTileProvider->MinZoomLevel(), aTileProvider->MaxZoomLevel());
//	SetTileProviderL(aTileProvider);
	
	// Prepare default font
	_LIT(KFontName, /*"OpenSans"*/ "Series 60 Sans");
	const TInt KFontHeightInTwips = /*9*/ 10 * 12; // Twip = 1/12 point
	TFontSpec fontSpec(KFontName, KFontHeightInTwips);
	fontSpec.iTypeface.SetIsSerif(EFalse);
	fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	CGraphicsDevice* screenDevice = CCoeEnv::Static()->ScreenDevice();
	TInt r = screenDevice->/*GetNearestFontToMaxHeightInTwips*/ GetNearestFontInTwips(iDefaultFont, fontSpec);
	User::LeaveIfError(r);
	
	// Create layers
	iLayers = RPointerArray<CMapLayerBase>(10);
	iLayers.Append(CTiledMapLayer::NewL(this, aTileProvider));
#ifdef DEBUG_SHOW_TILE_BORDER_AND_XYZ
	iLayers.Append(new (ELeave) CTileBorderAndXYZLayer(this));
#endif
	iLayers.Append(CLandmarksLayer::NewL(this, appUi->LandmarkDb()));
	iLayers.Append(new (ELeave) CUserPositionLayer(this));
	iLayers.Append(CScaleBarLayer::NewL(this));
#ifdef DEBUG_SHOW_ADDITIONAL_INFO
	iLayers.Append(CMapLayerDebugInfo::NewL(this));
#endif
	iLayers.Append(new (ELeave) CCrosshairLayer(this));
	iLayers.Append(CSignalIndicatorLayer::NewL(this));
	
	SetTileProviderL(aTileProvider);

	// Periodic timer for repeating the movement at holding (touch interface)
	iMovementRepeater = CPeriodic::NewL(0); // neutral priority
	
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Set initial displayed position
	Move(aInitialPosition);

	iCrosshairAutoHideTimer = CPeriodic::New(CActive::EPriorityStandard);
	
	// Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// CMapControl::CMapControl()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMapControl::CMapControl(TZoom aInitialZoom) :
	iZoom(aInitialZoom),
	iIsSoftkeysShown(EFalse)
	// Position will be set later in ConstructL
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CMapControl::~CMapControl()
// Destructor.
// -----------------------------------------------------------------------------
//
CMapControl::~CMapControl()
	{
	iCrosshairAutoHideTimer->Cancel();
	delete iCrosshairAutoHideTimer;
	
	// Destroy all layers
	iLayers.ResetAndDestroy();

	iMovementRepeater->Cancel();
	delete iMovementRepeater;
	iMovementRepeater = NULL;
	
	CCoeEnv::Static()->ScreenDevice()->ReleaseFont(iDefaultFont);
	}

// -----------------------------------------------------------------------------
// CMapControl::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CMapControl::Draw(const TRect& /*aRect*/) const
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
		if (i == ECrosshairLayerId && !IsCrosshairVisible())
			continue; // Skip drawing crosshair if hidden
		
		//Window().BeginRedraw();
		gc.Reset();
		iLayers[i]->Draw(gc);
		//Window().EndRedraw();
		}
	}

// -----------------------------------------------------------------------------
// CMapControl::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CMapControl::SizeChanged()
	{
	DrawNow();
	}

// -----------------------------------------------------------------------------
// CMapControl::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CMapControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
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
			TCoordinate coord = GetCenterCoordinate();
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
			Move(coord);
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

TKeyResponse CMapControl::OfferKeyEventL(const TKeyEvent &aKeyEvent,
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
				DisableDraw();
				SetFollowUser(EFalse);
				MoveUp();
				EnableDraw();
				return EKeyWasConsumed;
				//break;
				}
				
			case EStdKeyDownArrow:
			case '8':
			//case EStdKeyNkp8:
				{
				DisableDraw();
				SetFollowUser(EFalse);
				MoveDown();
				EnableDraw();
				return EKeyWasConsumed;
				//break;
				}
				
			case EStdKeyLeftArrow:
			case '4':
			//case EStdKeyNkp4:
				{
				DisableDraw();
				SetFollowUser(EFalse);
				MoveLeft();
				EnableDraw();
				return EKeyWasConsumed;
				//break;
				}
				
			case EStdKeyRightArrow:
			case '6':
			//case EStdKeyNkp6:
				{
				DisableDraw();
				SetFollowUser(EFalse);
				MoveRight();
				EnableDraw();
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
				
			case EStdKeyBackspace: // "C" key
				{
				static_cast<CS60MapsAppUi*>(iAvkonAppUi)->MapView()->ProcessCommandL(EDeleteLandmark);
				return EKeyWasConsumed;
				}
				
			//case EStdKeyDevice3: // Central key
			case '5': // https://github.com/artem78/s60-maps/issues/51
				{
				static_cast<CS60MapsAppUi*>(iAvkonAppUi)->MapView()->ProcessCommandL(ECreateOrRenameLandmark);
				return EKeyWasConsumed;
				}
			}
		}
	
	return EKeyWasNotConsumed;
	}

void CMapControl::Move(const TPoint &aPoint, TBool aSavePos)
	{
	DisableDraw();
	
	TPoint oldPoint = iTopLeftPosition;
	
	// Check that position has changed
	if (oldPoint != aPoint)
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
		
		if (iTopLeftPosition != oldPoint) // Check if position really changed
			DrawDelayed();
		}
	
	EnableDraw();
	}

void CMapControl::Move(const TCoordinate &aPos)
	{
	DisableDraw();
	
	/*__ASSERT_ALWAYS*/
	__ASSERT_DEBUG(Math::IsFinite(aPos.Latitude()) && Math::IsFinite(aPos.Longitude()),
			Panic(ES60MapsInvalidPosition)); // Possible may be NaN
	
	iCenterPosition = aPos; // Store new position
	TPoint point = MapMath::GeoCoordsToProjectionPoint(aPos, iZoom);
	// Convert from center to top left
	point.iX -= Rect().Width() / 2;
	point.iY -= Rect().Height() / 2;
	Move(point, EFalse);
	
	EnableDraw();
	}

void CMapControl::Move(const TCoordinate &aPos, TZoom aZoom)
	{
	DisableDraw();
	
	Move(aPos);
	SetZoom(aZoom);
	
	EnableDraw();
	}

void CMapControl::Move(TReal64 aLat, TReal64 aLon)
	{
	DisableDraw();
	
	Move(aLat, aLon, iZoom);
	
	EnableDraw();
	}

void CMapControl::Move(TReal64 aLat, TReal64 aLon, TZoom aZoom)
	{
	DisableDraw();
	
	TCoordinate coord(aLat, aLon);
	Move(coord, aZoom);
	
	EnableDraw();
	}

void CMapControl::MoveAndZoomIn(const TCoordinate &aPos)
	{
	DisableDraw();
	
	const TZoom minZoom = 16;
	if (GetZoom() < minZoom)
		{
		SetZoom(minZoom);
		}
	Move(aPos);
	
	EnableDraw();
	}

void CMapControl::SetZoomBounds(TZoom aMinZoom, TZoom aMaxZoom)
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

void CMapControl::SetZoom(TZoom aZoom)
	{
	DisableDraw();
	
	// ToDo: Return error code KErrArgument or panic if zoom out of bounds
	if (aZoom >= iMinZoom and aZoom <= iMaxZoom)
		{
		if (iZoom != aZoom)
			{
			iZoom = aZoom;
			Move(iCenterPosition);
			DrawDelayed();
			}
		}
	
	EnableDraw();
	}

void CMapControl::ZoomIn()
	{
	if (iZoom < KMaxZoomLevel)
		{
		DisableDraw();
		SetZoom(iZoom + 1);
		ShowCrosshairForAShortTime();
		EnableDraw();
		}
	}

void CMapControl::ZoomOut()
	{
	if (iZoom > KMinZoomLevel)
		{
		DisableDraw();
		SetZoom(iZoom - 1);
		ShowCrosshairForAShortTime();
		EnableDraw();
		}
	}

void CMapControl::MoveUp(TUint aPixels)
	{
	TPoint point = iTopLeftPosition;
	point.iY -= aPixels;
	DisableDraw();
	Move(point);
	ShowCrosshairForAShortTime();
	EnableDraw();
	}

void CMapControl::MoveDown(TUint aPixels)
	{
	TPoint point = iTopLeftPosition;
	point.iY += aPixels;
	DisableDraw();
	Move(point);
	ShowCrosshairForAShortTime();
	EnableDraw();
	}

void CMapControl::MoveLeft(TUint aPixels)
	{
	TPoint point = iTopLeftPosition;
	point.iX -= aPixels;
	DisableDraw();
	Move(point);
	ShowCrosshairForAShortTime();
	EnableDraw();
	}

void CMapControl::MoveRight(TUint aPixels)
	{
	TPoint point = iTopLeftPosition;
	point.iX += aPixels;
	DisableDraw();
	Move(point);
	ShowCrosshairForAShortTime();
	EnableDraw();
	}

TZoom CMapControl::GetZoom() const
	{
	return iZoom;
	}

TCoordinate CMapControl::GetCenterCoordinate() const
	{
	TPoint point = iTopLeftPosition;
	point.iX += Rect().Width() / 2;
	point.iY += Rect().Height() / 2;
	return MapMath::ProjectionPointToGeoCoords(point, iZoom);
	}

TBool CMapControl::CheckCoordVisibility(const TCoordinate &aCoord) const
	{	
	TPoint projectionPoint = MapMath::GeoCoordsToProjectionPoint(aCoord, iZoom);
	TPoint screenPoint = ProjectionCoordsToScreenCoords(projectionPoint);
	return CheckPointVisibility(screenPoint);
	}

TBool CMapControl::CheckPointVisibility(const TPoint &aPoint) const
	{
	TRect screenRect = Rect();
	screenRect.Resize(1, 1);
	return screenRect.Contains(aPoint);
	}

TPoint CMapControl::GeoCoordsToScreenCoords(const TCoordinate &aCoord) const
	{
	TPoint point = MapMath::GeoCoordsToProjectionPoint(aCoord, iZoom);
	return ProjectionCoordsToScreenCoords(point);
	}

TCoordinate CMapControl::ScreenCoordsToGeoCoords(const TPoint &aPoint) const
	{	
	TPoint point = ScreenCoordsToProjectionCoords(aPoint);
	return MapMath::ProjectionPointToGeoCoords(point, iZoom);
	}

TPoint CMapControl::ProjectionCoordsToScreenCoords(const TPoint &aPoint) const
	{
	return aPoint - iTopLeftPosition;
	}

TPoint CMapControl::ScreenCoordsToProjectionCoords(const TPoint &aPoint) const
	{
	return aPoint + iTopLeftPosition;
	}

void CMapControl::Bounds(TCoordinate &aTopLeftCoord, TCoordinate &aBottomRightCoord) const
	{
	aTopLeftCoord = ScreenCoordsToGeoCoords(Rect().iTl);
	aBottomRightCoord = ScreenCoordsToGeoCoords(Rect().iBr - TPoint(1, 1));
	}

void CMapControl::Bounds(TCoordRect &aCoordRect) const
	{
	Bounds(aCoordRect.iTlCoord, aCoordRect.iBrCoord);
	}

void CMapControl::Bounds(TTile &aTopLeftTile, TTile &aBottomRightTile) const
	{
	TPoint topLeftProjection = ScreenCoordsToProjectionCoords(Rect().iTl);
	TPoint bottomRightProjection = ScreenCoordsToProjectionCoords(Rect().iBr - TPoint(1, 1));
	aTopLeftTile = MapMath::ProjectionPointToTile(topLeftProjection, GetZoom());
	aBottomRightTile = MapMath::ProjectionPointToTile(bottomRightProjection, GetZoom());
	}

void CMapControl::UpdateUserPosition()
	{
	DisableDraw();
	
	TBool isUserPositionVisibleNow = IsUserPositionVisible(); 
	
	if (iIsFollowUser && iIsUserPositionRecieved)
		Move(iUserPosition);
	else
		{
		if (isUserPositionVisibleNow || iIsUserPositionVisiblePrev) // Checks if user location mark visible on the screen or just disappeared
			{
			DrawDelayed();
			}
		}
	
	iIsUserPositionVisiblePrev = isUserPositionVisibleNow;
	
	EnableDraw();
	}

void CMapControl::SetUserPosition(const TCoordinateEx& aPos)
	{
	iUserPosition = aPos;
	ShowUserPosition();
	}

TInt CMapControl::UserPosition(TCoordinateEx& aPos)
	{
	if (!iIsUserPositionRecieved)
		return KErrNotFound;
	
	aPos = iUserPosition;
	return KErrNone;
	}

void CMapControl::ShowUserPosition()
	{
	iIsUserPositionRecieved = ETrue;
	UpdateUserPosition();
	}

void CMapControl::HideUserPosition()
	{
	iIsUserPositionRecieved = EFalse;
	UpdateUserPosition();
	}

void CMapControl::SetFollowUser(TBool anEnabled)
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

TInt CMapControl::MovementRepeaterCallback(TAny* aObject)
	{
	((CMapControl*)aObject)->ExecuteMovement();
	return 1;
	}

void CMapControl::ExecuteMovement()
	{
	switch(iMovement)
		{
		case EMoveUp:
			{
			DisableDraw();
			SetFollowUser(EFalse);
			MoveUp();
			EnableDraw();
			break;
			}
		case EMoveDown:
			{
			DisableDraw();
			SetFollowUser(EFalse);
			MoveDown();
			EnableDraw();
			break;
			}
		case EMoveLeft:
			{
			DisableDraw();
			SetFollowUser(EFalse);
			MoveLeft();
			EnableDraw();
			break;
			}
		case EMoveRight:
			{
			DisableDraw();
			SetFollowUser(EFalse);
			MoveRight();
			EnableDraw();
			break;
			}
		case EMoveNone:
			break;
		}
	}

void CMapControl::SetTileProviderL(TTileProvider* aTileProvider)
	{
	static_cast<CTiledMapLayer*>(iLayers[ETiledMapLayerId])->SetTileProviderL(aTileProvider);
	SetZoomBounds(aTileProvider->iMinZoomLevel, aTileProvider->iMaxZoomLevel);
	}

void CMapControl::ShowCrosshair()
	{
	if (iIsCrosshairVisible)
		return;
	
	iIsCrosshairVisible = ETrue;
	DisableDraw();
	DrawDelayed();
	EnableDraw();
	}

void CMapControl::HideCrosshair()
	{
	if (!IsCrosshairVisible())
		return;
	
	iIsCrosshairVisible = EFalse;
	DisableDraw();
	DrawDelayed();
	EnableDraw();
	}

TInt CMapControl::CrosshairAutoHideCallback(TAny* anObj)
	{
	CMapControl* obj = static_cast<CMapControl*>(anObj);
	obj->HideCrosshair();
	return EFalse;
	}

void CMapControl::ShowCrosshairForAShortTime()
	{
		ShowCrosshair();
		if (iCrosshairAutoHideTimer->IsActive())
			iCrosshairAutoHideTimer->Cancel();
		TCallBack callback(CrosshairAutoHideCallback, this);
		iCrosshairAutoHideTimer->Start((TInt)(1.5 * KSecond), 0, callback);
	}

void CMapControl::HandleLanguageChangedL()
	{
	static_cast<CScaleBarLayer*>(iLayers[EScaleBarLayerId])->ReloadStringsFromResourceL();
	}

void CMapControl::ReloadVisibleAreaL()
	{
	static_cast<CTiledMapLayer*>(iLayers[ETiledMapLayerId])->ReloadVisibleAreaL();
	}

void CMapControl::NotifyLandmarksUpdated()
	{
	static_cast<CLandmarksLayer*>(iLayers[ELandmarksLayerId])->NotifyLandmarksUpdated();
	}

// End of File
