/*
 * Map.cpp
 *
 *  Created on: 14.08.2019
 *      Author: artem78
 */

#include "Map.h"
#include "MapControl.h"
#include <eikenv.h>
#include <gdi.h>
#include <imageconversion.h>
#include <e32math.h>
#include <bitstd.h>
#include "Logger.h"
#include "S60Maps.pan"
#include "S60MapsAppUi.h"
#include "S60MapsApplication.h"
#include <bautils.h>
#include "Defs.h"
#include <S60Maps_0xED689B88.rsg>
#include <epos_cposlandmarksearch.h>
#include <epos_cposlmareacriteria.h>
#include "icons.mbg"
#include "LBSSatelliteExtended.h"
#include "MapView.h"
#include <aknutils.h> 


CMapLayerBase::CMapLayerBase(/*const*/ CMapControl* aMapView) :
		iMapView(aMapView)
	{
	}


#ifdef DEBUG_SHOW_ADDITIONAL_INFO

// CMapLayerDebugInfo

CMapLayerDebugInfo::CMapLayerDebugInfo(/*const*/ CMapControl* aMapView) :
	CMapLayerBase(aMapView),
	iRedrawingsCount(0)
	{
	}

CMapLayerDebugInfo* CMapLayerDebugInfo::NewLC(CMapControl* aMapView)
	{
	CMapLayerDebugInfo* self = new (ELeave) CMapLayerDebugInfo(aMapView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMapLayerDebugInfo* CMapLayerDebugInfo::NewL(CMapControl* aMapView)
	{
	CMapLayerDebugInfo* self = CMapLayerDebugInfo::NewLC(aMapView);
	CleanupStack::Pop(); // self;
	return self;
	}

void CMapLayerDebugInfo::ConstructL()
	{
	}

CMapLayerDebugInfo::~CMapLayerDebugInfo()
	{
	}

void CMapLayerDebugInfo::Draw(CWindowGc &aGc)
	{
	TRAP_IGNORE(DrawInfoL(aGc));
	};

void CMapLayerDebugInfo::DrawInfoL(CWindowGc &aGc)
	{
	_LIT(KRedrawingsText, "Redrawings: %d");
	_LIT(KLatText, "Lat: %f");
	_LIT(KLonText, "Lon: %f");
	_LIT(KZoomText, "Zoom: %d");
	_LIT(KGdopText, "GDOP: %.1f");
	
	iRedrawingsCount++;
	TCoordinate center = iMapView->GetCenterCoordinate();
	
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	const TPositionSatelliteInfoExtended* satInfo =
			static_cast<const TPositionSatelliteInfoExtended*>(appUi->SatelliteInfo());
	
	TReal gdop = KNaN;
	if (appUi->PositioningState() == EPositionRecieved && satInfo)
		{
		gdop = satInfo->GeometricDoP();
		}
	
	// Prepare strings
	/*TBuf<100> buff;
		_LIT(KFmt, "Redrawings: %d\rLat: %f\rLon: %f\rZoom: %d");
		buff.Format(KFmt, iRedrawingsCount, center.Latitude(), center.Longitude(), (TInt) iMapView->GetZoom());*/
	
	CDesCArrayFlat* strings = new (ELeave) CDesCArrayFlat(5);
	CleanupStack::PushL(strings);
	
	TBuf<32> buff;
	buff.Format(KRedrawingsText, iRedrawingsCount);
	strings->AppendL(buff);
	buff.Format(KLatText, center.Latitude());
	strings->AppendL(buff);
	buff.Format(KLonText, center.Longitude());
	strings->AppendL(buff);
	buff.Format(KZoomText, (TInt) iMapView->GetZoom());
	strings->AppendL(buff);
	buff.Format(KGdopText, gdop);
	strings->AppendL(buff);
	
	// Draw
	aGc.Reset();
	aGc.SetPenColor(KRgbDarkBlue);
	
	aGc.UseFont(iMapView->DefaultFont());
	TRect area = iMapView->Rect();
	area.Shrink(4, 4);
	TInt baselineOffset = 0;
	for (TInt i = 0; i < strings->Count(); i++)
		{
		baselineOffset += iMapView->DefaultFont()->AscentInPixels() + 5;
		aGc.DrawText((*strings)[i], area, baselineOffset, CGraphicsContext::ELeft);
		}
	aGc.DiscardFont();
	
	CleanupStack::PopAndDestroy(strings);
	}

#endif


// MTileBitmapManagerObserver
void MTileBitmapManagerObserver::OnTileLoadingFailed(const TTile &/*aTile*/, TInt /*aErrCode*/)
	{
	// No any action by default
	}


// CTiledMapLayer

CTiledMapLayer::CTiledMapLayer(CMapControl* aMapView) :
	CMapLayerBase(aMapView)
	{
	// No implementation required
	}

CTiledMapLayer::~CTiledMapLayer()
	{
	delete iBitmapMgr;
	}

CTiledMapLayer* CTiledMapLayer::NewL(CMapControl* aMapView, TTileProvider* aTileProvider)
	{
	CTiledMapLayer* self = CTiledMapLayer::NewLC(aMapView, aTileProvider);
	CleanupStack::Pop(); // self;
	return self;
	}

CTiledMapLayer* CTiledMapLayer::NewLC(CMapControl* aMapView, TTileProvider* aTileProvider)
	{
	CTiledMapLayer* self = new (ELeave) CTiledMapLayer(aMapView);
	CleanupStack::PushL(self);
	self->ConstructL(aTileProvider);
	return self;
	}

void CTiledMapLayer::ConstructL(TTileProvider* aTileProvider)
	{
	SetTileProviderL(aTileProvider);
	}

void CTiledMapLayer::Draw(CWindowGc &aGc)
	{
	DEBUG(_L("Begin layer drawing"));
	
	RArray<TTile> tiles(10);
	VisibleTiles(tiles);
	for (TInt idx = 0; idx < tiles.Count(); idx++)
		{
		CFbsBitmap* bitmap;
		TInt err = iBitmapMgr->GetTileBitmap(tiles[idx], bitmap);
		switch (err)
			{
			case KErrNone:
				{
				DrawTile(aGc, tiles[idx], bitmap);
				break;
				}
				
			case KErrNotFound:
				{
				iBitmapMgr->AddToLoading(tiles[idx]);
				TInt err = iBitmapMgr->GetTileBitmap(tiles[idx], bitmap);
				if (KErrNone == err)
					DrawTile(aGc, tiles[idx], bitmap);
				break;
				}
				
			/*default:
				break;*/
			}
		
		}
	
	tiles.Close();
	
	DrawCopyrightText(aGc);
	
	DEBUG(_L("End layer drawing"));
	}

void CTiledMapLayer::VisibleTiles(RArray<TTile> &aTiles)
	{
	TTile topLeftTile, bottomRightTile;
	iMapView->Bounds(topLeftTile, bottomRightTile);
	TUint x, y;
	for (y = topLeftTile.iY; y <= bottomRightTile.iY; y++)
		{
		for (x = topLeftTile.iX; x <= bottomRightTile.iX; x++)
			{
			TTile tile;
			tile.iX = x;
			tile.iY = y;
			tile.iZ = iMapView->GetZoom();
			aTiles.Append/*L*/(tile); // ToDo: Check error code
			}
		}
	aTiles.Compress();
	}

void CTiledMapLayer::DrawTile(CWindowGc &aGc, const TTile &aTile, const CFbsBitmap *aBitmap)
	{
	TCoordinate coord = MapMath::TileToGeoCoords(aTile);
	TPoint point = iMapView->GeoCoordsToScreenCoords(coord);
	TRect destRect;
	destRect.iTl = point;
	destRect.SetSize(TSize(KTileSize, KTileSize));
	TRect screenRect = iMapView->Rect();
	if (!screenRect.Intersects(destRect)) // Check if tile is visible
		return;
	
	destRect.Intersection(screenRect);
	
	TRect srcRect = destRect;
	srcRect.Move(-point);

	
	aGc.DrawBitmap(destRect, aBitmap, srcRect);
	}

void CTiledMapLayer::OnTileLoaded(const TTile &/*aTile*/, const CFbsBitmap */*aBitmap*/)
	{
	//iMapView->DrawDeferred();
	iMapView->DrawNow();
	}

void CTiledMapLayer::SetTileProviderL(TTileProvider* aTileProvider)
	{
	iTileProvider = aTileProvider;
	
	//iMapView->SetZoomBounds(iTileProvider->MinZoomLevel(), iTileProvider->MaxZoomLevel());
	
	TFileName cacheDir;
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	CS60MapsApplication* app = static_cast<CS60MapsApplication*>(appUi->Application());
	app->CacheDir(cacheDir);
	cacheDir.Append(iTileProvider->iId);
	cacheDir.Append(KPathDelimiter);
	
	RFs fs = iMapView->ControlEnv()->FsSession();
	
	// Create cache dir (if not exists)
	TInt r = fs.MkDirAll(cacheDir);
	if (r != KErrAlreadyExists)
		User::LeaveIfError(r);
	
	if (iBitmapMgr == NULL)
		// Create bitmap manager if not exist yet
		iBitmapMgr = CTileBitmapManager::NewL(this, fs, iTileProvider, cacheDir);
	else
		// Set new tile provider and cache dir for bitmap manager
		iBitmapMgr->ChangeTileProvider(iTileProvider, cacheDir);
	
	//iMapView->DrawNow();
	}

void CTiledMapLayer::ReloadVisibleAreaL()
	{
	RArray<TTile> tiles(10);
	VisibleTiles(tiles);
	for (TInt idx = 0; idx < tiles.Count(); idx++)
		{
		iBitmapMgr->AddToLoading(tiles[idx], ETrue);
		}
	
	tiles.Close();
	}

void CTiledMapLayer::DrawCopyrightText(CWindowGc &aGc)
	{
	if (!iTileProvider->iCopyrightTextShort.Length())
		{ // no copyright info provided
		return;
		}
	
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	
	RBuf copyrightText;
	TInt r = copyrightText.Create(iTileProvider->iCopyrightTextShort.Length() + 10);
	if (r == KErrNone)
		{
		copyrightText.Append('(');
		copyrightText.Append('c' /*'C'*/);
		copyrightText.Append(')');
		copyrightText.Append(' ');
		copyrightText.Append(iTileProvider->iCopyrightTextShort);
		
		const TInt KMargin = /*14*/ 8;
		TRect textRect;
		textRect = iMapView->Rect();
		textRect.Shrink(KMargin, 0);
		TInt textBaseline = textRect.Height() - KMargin;
		aGc.UseFont(iMapView->SmallFont());
		static_cast<CWindowGcEx*>(&aGc)->DrawOutlinedText(copyrightText, textRect, textBaseline,
				CGraphicsContext::ERight, 0, KRgbBlack, KRgbWhite, ETrue);
		aGc.DiscardFont();
		
		copyrightText.Close();
		}
	}


// CUserPositionLayer

CUserPositionLayer::CUserPositionLayer(/*const*/ CMapControl* aMapView) :
		CMapLayerBase(aMapView)
	{
	
	}

void CUserPositionLayer::Draw(CWindowGc &aGc)
	{
	TCoordinateEx pos;
	TInt r = iMapView->UserPosition(pos);
	if (r == KErrNone && iMapView->CheckCoordVisibility(pos))
		{
		TPoint screenPoint = iMapView->GeoCoordsToScreenCoords(pos);
		
		if (!Math::IsNaN(pos.HorAccuracy()) && pos.HorAccuracy() > 0)
			{
			// Draw accuracy circle
			TSize circleSize;
			MapMath::MetersToPixels(pos.Latitude(), iMapView->GetZoom(), pos.HorAccuracy(),
					circleSize.iWidth, circleSize.iHeight);
			circleSize.iHeight = circleSize.iWidth; // Real height may a little differ
						// due to projection limitations, but I ignore this
			DrawAccuracyCircle(aGc, screenPoint, circleSize);
			}
		
		// ToDo: Do not draw direction mark when speed is too low (about < 3 kph)
		if (!Math::IsNaN(pos.Course()))
			{ // Draw direction mark
			TRAP_IGNORE(DrawDirectionMarkL(aGc, screenPoint, pos.Course()));
			}
		else
			{
			// If there is no course information available, draw circle mark
			DrawRoundMark(aGc, screenPoint);
			}
		}
	}

void CUserPositionLayer::DrawAccuracyCircle(CWindowGc &aGc, const TPoint &aScreenPos, TSize aSize)
	{
	const TInt KAlpha = 0x50;
	TRgb fillColor   = 0xAD8B58;
	TRgb strokeColor = 0x4F3612;
	fillColor.SetAlpha(KAlpha);
	strokeColor.SetAlpha(KAlpha);
	
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(fillColor);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetPenColor(strokeColor);
	aGc.SetPenSize(TSize(1, 1));

	TRect rect(TSize(0, 0));
	rect.Move(aScreenPos);
	//rect.Grow(aSize);
	rect.Grow(aSize.iWidth / 2, aSize.iHeight / 2);
	aGc.DrawEllipse(rect);
	}

void CUserPositionLayer::DrawDirectionMarkL(CWindowGc &aGc, const TPoint &aScreenPos, TReal aRotation)
	{
	// Points
	CArrayFix<TPoint>* points = new CArrayFixFlat<TPoint>(3);
	CleanupStack::PushL(points);
	
	points->AppendL(TPoint(-6, -5));
	//points->AppendL(TPoint(0, 0));
	points->AppendL(TPoint(6, -5));
	points->AppendL(TPoint(0, 13));
	
	// Rotation
	//TReal rad = aRotation * KDegToRad; // Rotation in radians
	TReal rad = (aRotation + 180.0) * KDegToRad; // Rotation in radians
	for (TInt i = 0; i < points->Count(); i++)
		{
		TReal x = points->At(i).iX;
		TReal y = points->At(i).iY;
		TReal c, s;
		TInt ret;
		ret = Math::Cos(c, rad);
		User::LeaveIfError(ret);
		ret = Math::Sin(s, rad);
		User::LeaveIfError(ret);
		TReal newX = x * c - y * s;
		TReal newY = x * s + y * c;
		ret = Math::Round(newX, newX, 0);
		User::LeaveIfError(ret);
		ret = Math::Round(newY, newY, 0);
		User::LeaveIfError(ret);
		points->At(i).iX = static_cast<TInt>(newX);
		points->At(i).iY = static_cast<TInt>(newY);
		}
	
	// Add drawing offset
	for (TInt i = 0; i < points->Count(); i++)
		points->At(i) += aScreenPos;
	
	// Draw white outline
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetPenSize(TSize(3, 3));
	aGc.SetPenColor(KRgbWhite);
	aGc.DrawPolygon(points);
	
	// Draw arrow
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbRed);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetPenSize(TSize(1, 1));
	aGc.SetPenColor(KRgbBlack);
	//aGc.DrawPolyLine(points);
	aGc.DrawPolygon(points);
	
	// Cleaning
	CleanupStack::PopAndDestroy(points);
	}

void CUserPositionLayer::DrawRoundMark(CWindowGc &aGc, const TPoint &aScreenPos)
	{
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbRed);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetPenSize(TSize(1, 1));
	aGc.SetPenColor(KRgbBlack);
	
	TSize size(10, 10);
	TRect rect(aScreenPos, size);
	rect.Move(-size.iWidth / 2, -size.iHeight / 2); // Correct offset from figure center
	aGc.DrawEllipse(rect);
	}



#ifdef DEBUG_SHOW_TILE_BORDER_AND_XYZ

// CTileBorderAndNumbersLayer

CTileBorderAndXYZLayer::CTileBorderAndXYZLayer(CMapControl* aMapView) :
		CMapLayerBase(aMapView)
	{
	}
	
void CTileBorderAndXYZLayer::Draw(CWindowGc &aGc)
	{
	RArray<TTile> tiles(10);
	VisibleTiles(tiles);
	for (TInt idx = 0; idx < tiles.Count(); idx++)
		DrawTile(aGc, tiles[idx]);
	
	tiles.Close();
	}

void CTileBorderAndXYZLayer::VisibleTiles(RArray<TTile> &aTiles)
	{
	TTile topLeftTile, bottomRightTile;
	iMapView->Bounds(topLeftTile, bottomRightTile);
	TUint x, y;
	for (y = topLeftTile.iY; y <= bottomRightTile.iY; y++)
		{
		for (x = topLeftTile.iX; x <= bottomRightTile.iX; x++)
			{
			TTile tile;
			tile.iX = x;
			tile.iY = y;
			tile.iZ = iMapView->GetZoom();
			aTiles.Append/*L*/(tile); // ToDo: Check error code
			}
		}
	aTiles.Compress();
	}

void CTileBorderAndXYZLayer::DrawTile(CWindowGc &aGc, const TTile &aTile)
	{
	// Calculate tile position
	TCoordinate coord = MapMath::TileToGeoCoords(aTile);
	TPoint point = iMapView->GeoCoordsToScreenCoords(coord);
	TRect rect(TSize(KTileSize, KTileSize));
	rect.Move(point);
	
	// Draw tile boundary
	aGc.SetPenColor(KRgbDarkBlue);
	aGc.SetPenStyle(CGraphicsContext::EDottedPen);
	aGc.SetPenSize(TSize(1, 1));
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc.DrawRect(rect);
	
	// Draw tile numbers
	_LIT(KTextFormat, "x=%d y=%d z=%d");
	TBuf<30> buff;
	buff.Format(KTextFormat, aTile.iX, aTile.iY, aTile.iZ);
	const CFont* font = CEikonEnv::Static()->SymbolFont();
	aGc.UseFont(font);
	TInt baseline = rect.Height() / 2 + font->AscentInPixels() / 2;
	aGc.DrawText(buff, rect, baseline, CGraphicsContext::ECenter);
	aGc.DiscardFont();
	}
#endif


// CScaleBarLayer

CScaleBarLayer::CScaleBarLayer(CMapControl* aMapView):
		CMapLayerBase(aMapView)
	{
	}

CScaleBarLayer::~CScaleBarLayer()
	{
	delete iKilometersUnit;
	delete iMetersUnit;
	}

CScaleBarLayer* CScaleBarLayer::NewLC(CMapControl* aMapView)
	{
	CScaleBarLayer* self = new (ELeave) CScaleBarLayer(aMapView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CScaleBarLayer* CScaleBarLayer::NewL(CMapControl* aMapView)
	{
	CScaleBarLayer* self = CScaleBarLayer::NewLC(aMapView);
	CleanupStack::Pop(); // self;
	return self;
	}

void CScaleBarLayer::ConstructL()
	{
	// Read strings from resources
	ReloadStringsFromResourceL();
	}

void CScaleBarLayer::Draw(CWindowGc &aGc)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	
	if (!appUi->Settings()->iIsScaleBarVisible) // Check display or not
		return;
	
	const TInt KBarLeftMargin    = 14;
	const TInt KBarBottomMargin  = KBarLeftMargin;
	const TInt KBarHeight        = 3 + 2;
	const TInt KTextBottomMargin = 6;
	
	TInt barWidth;
	TReal32 horDist;
	GetOptimalLength(barWidth, horDist);
	
	// Draw scale line
	TPoint barStartPoint(KBarLeftMargin, iMapView->Rect().iBr.iY - KBarBottomMargin);
	TPoint barEndPoint(barStartPoint);
	barStartPoint.iY -= KBarHeight;
	barEndPoint.iX += barWidth;
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbBlack);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetPenSize(TSize(1, 1));
	aGc.SetPenColor(KRgbWhite);
	aGc.DrawRect(TRect(barStartPoint, barEndPoint));
	aGc.Reset();
	
	// Getting text
	HBufC* unit;
	TRealFormat realFmt = TRealFormat();
	//realFmt.iType = KRealFormatNoExponent; // Don`t work as expected
	realFmt.iType = KRealFormatFixed;
	//realFmt.iPlaces = ...; // Will be set later
	realFmt.iWidth = KDefaultRealWidth;
	realFmt.iTriad = ' ';
	realFmt.iTriLen = 1;
	if (horDist < /*1000*/ 999.5) // Less than 1 km
		{
		// Round value to meters
		unit = iMetersUnit;
		realFmt.iPlaces = 0;
		}
	else // More than 1 km
		{
		// Round value to kilometers
		unit = iKilometersUnit;
		horDist /= 1000.0;
		if (horDist > 10) // If more than 10 km don`t show fractional part
			realFmt.iPlaces = 0;
		else
			realFmt.iPlaces = 1;
		}
	TBuf<32> text;
	////////////
	/*DEBUG(_L("realFmt.iPlaces  = %d"), realFmt.iPlaces);
	DEBUG(_L("realFmt.iPoint   = %c (code=%d)"), realFmt.iPoint, realFmt.iPoint);
	DEBUG(_L("realFmt.iTriLen  = %d"), realFmt.iTriLen);
	DEBUG(_L("realFmt.iTriad   = %c (code=%d)"), realFmt.iTriad, realFmt.iTriad);
	DEBUG(_L("realFmt.iType    = %d"), realFmt.iType);
	DEBUG(_L("realFmt.iWidth   = %d"), realFmt.iWidth);*/
	///////////
	/*TInt r =*/ text.Num(horDist, realFmt);
	//DEBUG(_L("r = %d"), r);
	if (text.Length() >= 2 && text[text.Length() - 2] == realFmt.iPoint && text[text.Length() - 1] == '0')
		text.SetLength(text.Length() - 2); // Remove fractional part if equals to zero
	text.Append(' ');
	text.Append(*unit);
	DEBUG(_L("%f => %S"), horDist, &text);
	
	// Draw text
	TRect textRect(barStartPoint, barEndPoint);
	textRect.iBr.iY = textRect.iTl.iY; 
	textRect.iTl.iY -= 30;
	textRect.Move(0, -KTextBottomMargin);
	TInt baselineOffset = textRect.Height() /*- font->AscentInPixels()*/; 
	aGc.UseFont(iMapView->DefaultFont());
	//aGc.DrawText(text, startPoint);
	static_cast<CWindowGcEx*>(&aGc)->DrawOutlinedText(text, textRect, baselineOffset,
			CGraphicsContext::ECenter);
	aGc.DiscardFont();
	}

void CScaleBarLayer::GetOptimalLength(TInt &optimalLength, TReal32 &optimalDistance)
	{
	const TInt KMinLengthPx = 50;
	const TInt KMaxLengthPx = 150;
	
	TReal32 minLengtsMetersReal, maxLengthMetersReal, unused;
	MapMath::PixelsToMeters(iMapView->GetCenterCoordinate().Latitude(),
				iMapView->GetZoom(), KMinLengthPx, minLengtsMetersReal, unused);
	MapMath::PixelsToMeters(iMapView->GetCenterCoordinate().Latitude(),
				iMapView->GetZoom(), KMaxLengthPx, maxLengthMetersReal, unused);
	
	DEBUG(_L("min=%.2fm   max=%.2fm"), minLengtsMetersReal, maxLengthMetersReal);
	
	TInt minLengthMeters = (TInt) (minLengtsMetersReal + 0.5);
	TInt maxLengthMeters = (TInt) (maxLengthMetersReal + 0.5);
	DEBUG(_L("rounded min=%dm   rounded max=%dm"), minLengthMeters, maxLengthMeters);
	
	TInt digits = MathUtils::Digits(maxLengthMeters);
	DEBUG(_L("digits=%d"), digits);
	
	optimalLength = KMaxLengthPx;
	while (--digits >= 0)
		{
		TInt roundedLength = MathUtils::IntFloor(maxLengthMeters, digits);
		DEBUG(_L("rounded length=%dm"), roundedLength);
		
		if (roundedLength >= minLengthMeters && roundedLength <= maxLengthMeters)
			{
			TInt unusedInt;
			MapMath::MetersToPixels(iMapView->GetCenterCoordinate().Latitude(),
				iMapView->GetZoom(), roundedLength, optimalLength, unusedInt);
			
			optimalDistance = roundedLength;
			
			break;
			}
		}
	}

void CScaleBarLayer::ReloadStringsFromResourceL()
	{
	// Free previous loaded strings
	delete iMetersUnit;
	delete iKilometersUnit;
	
	iMetersUnit = NULL;
	iKilometersUnit = NULL;
	
	iMetersUnit = CCoeEnv::Static()->AllocReadResourceL(R_METERS_UNIT_SHORT);
	iKilometersUnit = CCoeEnv::Static()->AllocReadResourceL(R_KILOMETERS_UNIT_SHORT);
	
	DEBUG(_L("Loaded strings: %S, %S"), &*iMetersUnit, &*iKilometersUnit);
	}


// CLandmarksLayer

CLandmarksLayer::CLandmarksLayer(CMapControl* aMapView, CPosLandmarkDatabase* aLmDb):
		CMapLayerBase(aMapView),
		iLandmarksDb(aLmDb),
		iReloadNeeded(EFalse),
		iZoom(-1)
	{
	}

CLandmarksLayer::~CLandmarksLayer()
	{
	iNameRegion.Close();
	//iNameRegion.Destroy();
	if (iCachedLandmarks)
		{
		iCachedLandmarks->ResetAndDestroy();
		}
	delete iCachedLandmarks;
	ReleaseLandmarkResources();
	
	delete iIcon;
	}

CLandmarksLayer* CLandmarksLayer::NewLC(CMapControl* aMapView, CPosLandmarkDatabase* aLmDb)
	{
	CLandmarksLayer* self = new (ELeave) CLandmarksLayer(aMapView, aLmDb);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CLandmarksLayer* CLandmarksLayer::NewL(CMapControl* aMapView, CPosLandmarkDatabase* aLmDb)
	{
	CLandmarksLayer* self = CLandmarksLayer::NewLC(aMapView, aLmDb);
	CleanupStack::Pop(); // self;
	return self;
	}

void CLandmarksLayer::ConstructL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	CS60MapsApplication* app = static_cast<CS60MapsApplication*>(appUi->Application());
	
	iIcon = app->LoadIconL(EMbmIconsStar, EMbmIconsStar_mask);
	
	iCachedArea.SetCoords(TCoordinate(0, 0), TCoordinate(0, 0));
	}

void CLandmarksLayer::Draw(CWindowGc &aGc)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CEikonEnv::Static()->AppUi());
	
	if (!appUi->Settings()->GetLandmarksVisibility()) // Check display or not
		return;
	
	TRAPD(r, DrawL(aGc));
	if (r != KErrNone)
		{
		DEBUG(_L("Leave with code %d"), r);
		}
	}

void CLandmarksLayer::ReloadLandmarksListL()
	{
	const TInt KMaxVisibleLandmarksLimit = 100;
	
	
	if (iCachedLandmarks)
		{
		iCachedLandmarks->ResetAndDestroy();
		}
	delete iCachedLandmarks;
	iCachedLandmarks = NULL;
	
	DEBUG(_L("Start landmarks queing"));
	
	CPosLandmarkSearch* landmarkSearch = CPosLandmarkSearch::NewL(*iLandmarksDb);
	CleanupStack::PushL(landmarkSearch);
	landmarkSearch->SetMaxNumOfMatches(KMaxVisibleLandmarksLimit); // Set display amount limit
	CPosLmAreaCriteria* areaCriteria = CPosLmAreaCriteria::NewLC(
			iCachedArea.iBrCoord.Latitude(),
			iCachedArea.iTlCoord.Latitude(),
			iCachedArea.iTlCoord.Longitude(),
			iCachedArea.iBrCoord.Longitude());
	//TReal/*64*/ a,b,c,d;
	//areaCriteria->GetSearchArea(a,b,c,d);
	//DEBUG(_L("areaCriteria =   S Lat=%f    N Lat=%f   W Lon=%f   E Lon=%f"), a,b,c,d);
	CPosLmOperation* landmarkOp = landmarkSearch->StartLandmarkSearchL(*areaCriteria, EFalse);
	ExecuteAndDeleteLD(landmarkOp);
	//   landmarkOp->NextStep(...)
	TInt landmarksCount = landmarkSearch->NumOfMatches();
	//DEBUG(_L("Visible %d landmarks"), landmarksCount);
	if (landmarksCount)
		{
		CPosLmItemIterator* landmarkIter = landmarkSearch->MatchIteratorL();
		CleanupStack::PushL(landmarkIter);
		
		RArray<TPosLmItemId> landmarkIds(landmarksCount);
		CleanupClosePushL(landmarkIds);
		landmarkIter->GetItemIdsL(landmarkIds, 0, landmarksCount);
		
		CPosLmOperation* landmarkOp2 = iLandmarksDb->PreparePartialLandmarksL(landmarkIds);
		CleanupStack::PushL(landmarkOp2);
		landmarkOp2->ExecuteL();
		iCachedLandmarks = iLandmarksDb->TakePreparedPartialLandmarksL(landmarkOp2);
	
		CleanupStack::PopAndDestroy(3, landmarkIter);
		}
		
	CleanupStack::PopAndDestroy(2, landmarkSearch);
	
	iReloadNeeded = EFalse;
	iZoom = iMapView->GetZoom();
	
	DEBUG(_L("End landmarks queing (found %d items)"), landmarksCount);
	}

void CLandmarksLayer::DrawL(CWindowGc &aGc)
	{
	TBounds viewCoordRect;
	
	iMapView->Bounds(viewCoordRect);
	if (iReloadNeeded || !iCachedArea.Contains(viewCoordRect) || iMapView->GetZoom() > iZoom + 3)
		{ /* Update cached landmarks list only when:
				- view goes outside the cached area
				- force update triggered
				- zoomed in more then 3 levels */
		
		iZoom = iMapView->GetZoom();
		TRect largeRect = iMapView->Rect();
		const TInt KGrowDelta = KMapDefaultMoveStep * /*5*/ 10;
		largeRect.Grow(KGrowDelta, KGrowDelta);
		/*TInt KGrowDeltaX = iMapView->Rect().Width() / 2;
		TInt KGrowDeltaY = iMapView->Rect().Height() / 2;
		largeRect.Grow(KGrowDeltaX, KGrowDeltaY);*/
		iCachedArea.iTlCoord = iMapView->ScreenCoordsToGeoCoords(largeRect.iTl);
		iCachedArea.iBrCoord = iMapView->ScreenCoordsToGeoCoords(largeRect.iBr);
		
		// Fixing coordinates when going beyond bounds
		TPoint projTl, projBr;
		projTl = iMapView->ScreenCoordsToProjectionCoords(largeRect.iTl);
		projBr = iMapView->ScreenCoordsToProjectionCoords(largeRect.iBr);
		//DEBUG(_L("projTl=%d,%d  projBr=%d,%d"), projTl.iX, projTl.iY, projBr.iX, projBr.iY);
		TInt maxProjXY = MapMath::MaxProjectionCoordXY(iMapView->GetZoom());
		if (projTl.iX < KMinProjectionCoordXY)
			{
			// w lon = -180
			iCachedArea.iTlCoord.SetCoordinate(iCachedArea.iTlCoord.Latitude(), KMinLongitudeMapBound);
			//DEBUG(_L("w lon overflow fixed"));
			}
		else if (projTl.iX > maxProjXY)
			{
			//w lon < 180
			iCachedArea.iTlCoord.SetCoordinate(iCachedArea.iTlCoord.Latitude(), KMaxLongitudeMapBound - 0.0001);
			//DEBUG(_L("w lon overflow fixed"));
			}
		if (projTl.iY < KMinProjectionCoordXY)
			{
			// n lat = +90 (85...)
			iCachedArea.iTlCoord.SetCoordinate(KMaxLatitudeMapBound, iCachedArea.iTlCoord.Longitude());
			//DEBUG(_L("n lat overflow fixed"));
			}
		if (projBr.iX > maxProjXY)
			{
			// e lon = 180
			iCachedArea.iBrCoord.SetCoordinate(iCachedArea.iBrCoord.Latitude(), KMaxLongitudeMapBound - 0.0001);
			//DEBUG(_L("e lon overflow fixed"));
			}
		if (projBr.iY > maxProjXY)
			{
			// s lon = -90 (85...)
			iCachedArea.iBrCoord.SetCoordinate(KMinLatitudeMapBound, iCachedArea.iBrCoord.Longitude());
			//DEBUG(_L("s lon overflow fixed"));
			}
		
		//DEBUG(_L("iCachedArea =   S Lat=%f    N Lat=%f   W Lon=%f   E Lon=%f"), iCachedArea.iBrCoord.Latitude(),
		//		iCachedArea.iTlCoord.Latitude() ,iCachedArea.iTlCoord.Longitude()
		//		,iCachedArea.iBrCoord.Longitude());
		//TBuf<128> dbgMsg;
		///*dbgMsg.Format(_L("S Lat=%f    N Lat=%f   W Lon=%f   E Lon=%f"), iCachedArea.iBrCoord.Latitude(),
		//		iCachedArea.iTlCoord.Latitude() ,iCachedArea.iTlCoord.Longitude()
		//		,iCachedArea.iBrCoord.Longitude());*/
		//MiscUtils::DbgMsg(dbgMsg);
		
		ReloadLandmarksListL();
		
		DEBUG(_L("Landmarks list updated"));
		//MiscUtils::DbgMsg(_L("Landmarks list updated"));
		}
	else
		{
		DEBUG(_L("Skip landmarks list update"));
		}
	
	DrawLandmarks(aGc);
	}

void CLandmarksLayer::DrawLandmarks(CWindowGc &aGc)
	{
	if (!iCachedLandmarks || !iCachedLandmarks->Count())
		{
		DEBUG(_L("No landmarks to draw"));
		return;
		}
	
	DEBUG(_L("Landmarks redrawing started"));
	
	// Draw landmark icons
#ifdef __WINSCW__
	iVisibleIconsCount = 0;
#endif
	for (TInt i = iCachedLandmarks->Count() - 1; i >= 0; i--)
		{// reverse loop needed to proper display order
		CPosLandmark* landmark = (*iCachedLandmarks)[i];
		if (!landmark) continue;
		DrawLandmarkIcon(aGc, landmark);
		}
	
	// Draw landmark names (they will be always above icons)
	
	//const TRgb KPenColor(59, 120, 162);
	/*const TRgb KPenColor(21, 63, 92);
	aGc.SetPenColor(KPenColor); // For drawing text*/
	aGc.UseFont(iMapView->DefaultFont());
	
	iNameRegion.Clear();
	for (TInt i = 0; i < iCachedLandmarks->Count(); i++)
		{
		if (iNameRegion.CheckError())
			{
			/* Skip futher landmark names processiong if any error in iNameRegion
			 * (for example: internal buffer overflow)
			 * */
			break;
			}
		
		CPosLandmark* landmark = (*iCachedLandmarks)[i];
		if (!landmark) continue;
		DrawLandmarkName(aGc, landmark);
		}

#ifdef __WINSCW__
	DEBUG(_L("Visible landmark icons: %d, visible names: %d, total cached landmarks: %d"),
			iVisibleIconsCount, iNameRegion.Count(), iCachedLandmarks->Count());
#endif
	
	aGc.DiscardFont();
	
	DEBUG(_L("Landmarks redrawing ended"));	
	}


void CLandmarksLayer::DrawLandmarkIcon(CWindowGc &aGc, const CPosLandmark* aLandmark)
	{
	// Get landmark coordinates
	TLocality landmarkPos;
	if (aLandmark->GetPosition(landmarkPos) != KErrNone)
		{
		return; // no coordinates
		}
	
	// Calculate icon position on the screen
	TSize iconSize = iIcon->Bitmap()->SizeInPixels();
	TPoint landmarkPoint = iMapView->GeoCoordsToScreenCoords(landmarkPos);
	TRect dstRect(landmarkPoint, TSize(0, 0));
	dstRect.Grow(iconSize.iWidth / 2, iconSize.iHeight / 2);
	
	if (iMapView->Rect().Intersects(dstRect))
		{ // Draw landmark icon only if it intersects with control
		TRect srcRect(TPoint(0, 0), iconSize);
		aGc.DrawBitmapMasked(dstRect, iIcon->Bitmap(), srcRect, iIcon->Mask(), 0);
#ifdef __WINSCW__
		iVisibleIconsCount++;
#endif
		}
	}

void CLandmarksLayer::DrawLandmarkName(CWindowGc &aGc, const CPosLandmark* aLandmark)
	{
	// Get landmark coordinates and name
	TPtrC landmarkName;
	if (aLandmark->GetLandmarkName(landmarkName) != KErrNone || !landmarkName.Length())
		{
		return; // empty name
		}
	
	TLocality landmarkPos;
	if (aLandmark->GetPosition(landmarkPos) != KErrNone)
		{
		return; // no coordinates
		}
	
	// Trim landmark name if so long
	const TInt KMaxLandmarkNameLength = 32;
	TBuf<KMaxLandmarkNameLength> landmarkNameTrimmed;
	if (landmarkName.Length() > KMaxLandmarkNameLength)
		{
		_LIT(KElipsisStr, "...");
		landmarkNameTrimmed = landmarkName.Left(landmarkNameTrimmed.MaxLength() - KElipsisStr().Length());
		landmarkNameTrimmed += KElipsisStr;
		landmarkName.Set(landmarkNameTrimmed);
		}
	
	// Calculate label position on the screen
	const TInt KLabelMargin = 5;
	TPoint landmarkPoint = iMapView->GeoCoordsToScreenCoords(landmarkPos);
	TPoint labelPoint(landmarkPoint);
	TSize iconSize = iIcon->Bitmap()->SizeInPixels();
	labelPoint.iX += iconSize.iWidth / 2 + KLabelMargin;
	labelPoint.iY += /*iMapView->DefaultFont()->HeightInPixels()*/ iMapView->DefaultFont()->AscentInPixels() / 2;
	TRect nameRect;
	nameRect.SetWidth(iMapView->DefaultFont()->TextWidthInPixels(landmarkName));
	nameRect.SetHeight(iMapView->DefaultFont()->HeightInPixels());
	nameRect.Move(labelPoint);
	
	if (iMapView->Rect().Intersects(nameRect) && !iNameRegion.Intersects(nameRect))
		{ /* Draw landmark name only if it on visible part of the control
			 and it doesn't overllap any of previous drawned names */
		const TRgb KTextColor(21, 63, 92);
		static_cast<CWindowGcEx*>(&aGc)->DrawOutlinedText(landmarkName, labelPoint, KTextColor);
		iNameRegion.AddRect(nameRect);
		}
	}


// CCrosshairLayer

CCrosshairLayer::CCrosshairLayer(CMapControl* aMapView) :
		CMapLayerBase(aMapView)
	  {	
	  }

void CCrosshairLayer::Draw(CWindowGc &aGc)
	{
	const TInt KLineHalfLength = 10; // in px
	TPoint center = iMapView->Rect().Center();
	
	TPoint start1 = center;
	start1.iX -= (KLineHalfLength + 1);
	TPoint end1 = center;
	end1.iX += KLineHalfLength + 2;
	
	TPoint start2 = center;
	start2.iY -= (KLineHalfLength + 1);
	TPoint end2 = center;
	end2.iY += KLineHalfLength + 2;
	
	TRect rect1(start1 - TPoint(1, 1), end1 + TPoint(1, 2));
	TRect rect2(start2 - TPoint(1, 1), end2 + TPoint(2, 1));
	
	// Draw white outline
	aGc.SetPenStyle(CGraphicsContext::ENullPen);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbWhite);
	aGc.DrawRect(rect1);
	aGc.DrawRect(rect2);
	
	// Draw main crosshair lines
	aGc.SetPenColor(KRgbBlack);
	aGc.SetPenSize(TSize(1, 1));
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc.DrawLine(start1, end1);
	aGc.DrawLine(start2, end2);
	}


// CSignalIndicatorLayer

CSignalIndicatorLayer* CSignalIndicatorLayer::NewLC(CMapControl* aMapView)
	{
	CSignalIndicatorLayer* self = new (ELeave) CSignalIndicatorLayer(aMapView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSignalIndicatorLayer* CSignalIndicatorLayer::NewL(CMapControl* aMapView)
	{
	CSignalIndicatorLayer* self = CSignalIndicatorLayer::NewLC(aMapView);
	CleanupStack::Pop(); // self;
	return self;
	}

CSignalIndicatorLayer::CSignalIndicatorLayer(CMapControl* aMapView) :
		CMapLayerBase(aMapView)
	{
	}

void CSignalIndicatorLayer::ConstructL()
	{
	// Load icon
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	CS60MapsApplication* app = static_cast<CS60MapsApplication*>(appUi->Application());
	
	iSatelliteIcon = app->LoadIconL(EMbmIconsSatellite, EMbmIconsSatellite_mask);
	}

CSignalIndicatorLayer::~CSignalIndicatorLayer()
	{
	delete iSatelliteIcon;
	}

void CSignalIndicatorLayer::Draw(CWindowGc &aGc)
	{
	const TInt KSpacing = 6;
	
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	
	if (!appUi->Settings()->iIsSignalIndicatorVisible) // Check display or not
		return;
	
	const TPositionSatelliteInfoExtended* satInfo =
			static_cast<const TPositionSatelliteInfoExtended*>(appUi->SatelliteInfo());
	
	if (!satInfo) return;
	
	TPoint satIconPoint;
	switch (appUi->Settings()->iSignalIndicatorType)
		{
		case CSettings::ESignalIndicatorGeneralType:
			{
			TReal gdop = appUi->IsPositionRecieved() ? satInfo->GeometricDoP() : KNaN;
			TSignalStrength signalStrength = ESignalNone;
			// According to: https://en.wikipedia.org/wiki/Dilution_of_precision_(navigation)#Interpretation
			if (!Math::IsFinite(gdop))
				{
				signalStrength = ESignalNone;
				}
			else if (gdop < 1)
				{
				signalStrength = ESignalHigh;
				}
			else if (gdop < 2)
				{
				signalStrength = ESignalVeryGood;
				}
			else if (gdop < 5)
				{
				signalStrength = ESignalGood;
				}
			else if (gdop < 10)
				{
				signalStrength = ESignalMedium;
				}
			else if (gdop < 20)
				{
				signalStrength = ESignalLow;
				}
			else if (gdop < 50)
				{
				signalStrength = ESignalVeryLow;
				}
			
			_LIT(KFmt, "%d/%d");
			TBuf<64> buff;
			buff.Format(KFmt, satInfo->NumSatellitesUsed(), satInfo->NumOfVisibleSatellites());
			//DEBUG(buff);
			
			const CFont* font = iMapView->DefaultFont();
			
			TRect textArea = iMapView->Rect();
			textArea.Shrink(14, 14);
			textArea.iBr.iX -= KBarsTotalWidth + KSpacing;
			textArea.iBr.iY = 14 + KBarsTotalHeight;
			TReal tmp = (textArea.Height() + font->AscentInPixels()) / 2.0;
			Math::Round(tmp, tmp, 0);
			TInt baselineOffset = static_cast<TInt>(tmp);
			
			aGc.UseFont(font);
			static_cast<CWindowGcEx*>(&aGc)->DrawOutlinedText(buff, textArea, baselineOffset,
					CGraphicsContext::ERight);
			aGc.DiscardFont();
			
			DrawBarsV1(aGc, signalStrength);
			
			satIconPoint.iX = iMapView->Rect().iBr.iX - (14 + KBarsTotalWidth + KSpacing * 2 + font->TextWidthInPixels(buff)
					+ iSatelliteIcon->Bitmap()->SizeInPixels().iWidth);
			satIconPoint.iY = iMapView->Rect().iTl.iY + 14 + KBarsTotalHeight - iSatelliteIcon->Bitmap()->SizeInPixels().iHeight; 
			}
			break;
			
		case CSettings::ESignalIndicatorPerSatelliteType:
			{
			TRect barsRect = DrawBarsV2(aGc, TPoint(iMapView->Rect().iBr.iX - 14, iMapView->Rect().iTl.iY + 14), *satInfo);
			
			satIconPoint.iX = barsRect.iTl.iX - KSpacing - iSatelliteIcon->Bitmap()->SizeInPixels().iWidth;
			satIconPoint.iY = barsRect.iBr.iY - iSatelliteIcon->Bitmap()->SizeInPixels().iHeight;
			}
			break;
		}
	
	DrawSatelliteIcon(aGc, satIconPoint);
	}

void CSignalIndicatorLayer::DrawBarsV1(CWindowGc &aGc, TSignalStrength aSignalStrength)
	{
	__ASSERT_DEBUG(aSignalStrength >= ESignalNone, Panic(ES60MapsInvaidSignalValuePanic));
	__ASSERT_DEBUG(aSignalStrength <= ESignalHigh, Panic(ES60MapsInvaidSignalValuePanic));
	
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetPenSize(TSize(KBarBorderWidth, KBarBorderWidth));
	
	TRgb brushColor, penColor;
	
	// Set color for active bars
	switch (aSignalStrength)
		{
		case ESignalVeryLow:
			{
			brushColor = TRgb(192,0,0);
			penColor = TRgb(58,0,0);
			break;
			}
			
		case ESignalLow:
		case ESignalMedium:
			{
			brushColor = TRgb(251,193,0);
			penColor = TRgb(75,58,0);
			break;
			}
			
		case ESignalGood:
		case ESignalVeryGood:
		case ESignalHigh:
			{
			brushColor = TRgb(144,209,75);
			penColor = TRgb(43,63,22);
			break;
			}
		}
	
	TRect barRect(TPoint(iMapView->Rect().iBr.iX - (14 + KBarsTotalWidth),
			iMapView->Rect().iTl.iY + 14 + KBarsTotalHeight - KStartBarHeight),
			TSize(KBarWidth, KStartBarHeight));
	for (TInt i = ESignalVeryLow; i <= ESignalHigh; i++)
		{
		if (i == aSignalStrength + 1)
			{ // Change color for inactive bars
			brushColor = KRgbWhite;
			penColor = KRgbGray;
			}
		
		// Draw white outline
		TRect outlineRect(barRect);
		outlineRect.Grow(1, 1);
		aGc.SetBrushColor(KRgbWhite);
		aGc.SetPenColor(KRgbWhite);
		aGc.DrawRect(outlineRect);
		
		// Draw bar itself
		aGc.SetBrushColor(brushColor);
		aGc.SetPenColor(penColor);
		aGc.DrawRect(barRect);
		barRect.SetHeight(barRect.Height() + KBarHeightIncremement);
		barRect.Move(KBarWidth + KBarsSpacing, -KBarHeightIncremement);
		}
	}

TRect CSignalIndicatorLayer::DrawBarsV2(CWindowGc &aGc, const TPoint &aTopRight, const TPositionSatelliteInfo &aSatInfo)
	{
	const TInt KBarMaxHeight = KBarsTotalHeight;
	
	const TRgb KUnusedSatColor = /*TRgb(150, 150, 150)*/ KRgbWhite;
	const TRgb KUnusedSatBorderColor = /*TRgb(20, 20, 20)*/ KRgbGray;
	const TRgb KUsedSatColor = TRgb(144, 209, 75);
	const TRgb KUsedSatBorderColor = TRgb(43, 63, 22);
	TRgb backgroundColor = KUnusedSatBorderColor /*KRgbDarkGray*/;
	backgroundColor.SetAlpha(150);

	aGc.SetPenSize(TSize(KBarBorderWidth, KBarBorderWidth));
	
	TRectEx barMaxRect(aTopRight.iX - KBarWidth, aTopRight.iY, aTopRight.iX, aTopRight.iY + KBarMaxHeight);
	
	for (TInt i = aSatInfo.NumSatellitesInView() - 1; i >= 0; i--)
		{
		TSatelliteData satData;
		TReal32 signalStrength = 0;
		if (aSatInfo.GetSatelliteData(i, satData) == KErrNone)
			{
			signalStrength = SignalStrengthToReal(satData.SignalStrength());
			DEBUG(_L("sat=%d signal=%d signal real=%.2f"), i, satData.SignalStrength(), signalStrength);
			}
		
		// Draw white outline and background
		TRect outlineRect(barMaxRect);
		outlineRect.Grow(1, 1);
		aGc.SetPenStyle(CGraphicsContext::ESolidPen);
		aGc.SetPenColor(KRgbWhite);
		aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc.SetBrushColor(backgroundColor);
		aGc.DrawRect(outlineRect);
		
		// Draw fill
		aGc.SetPenStyle(CGraphicsContext::ENullPen);
		aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc.SetBrushColor(satData.IsUsed() ? KUsedSatColor : KUnusedSatColor);
		TRect barRect = barMaxRect;
		//TInt barHeight = Max(3, KBarMaxHeight * signalStrength);
		TInt barHeight = (TInt) (KBarMaxHeight * signalStrength + 0.5);
		barRect.iTl.iY += KBarMaxHeight - barHeight;
		aGc.DrawRect(barRect);
		
		// Draw border
		aGc.SetPenStyle(CGraphicsContext::ESolidPen);
		aGc.SetPenColor(satData.IsUsed() ? KUsedSatBorderColor : KUnusedSatBorderColor);
		aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
		aGc.DrawRect(barMaxRect);
		
		
		if (i != 0)
			{
			barMaxRect.Move(-(KBarWidth + KBarsSpacing), 0);
			}
		}
	
	return TRect(barMaxRect.iTl, TPoint(aTopRight.iX, barMaxRect.iBr.iY));
	}

void CSignalIndicatorLayer::DrawSatelliteIcon(CWindowGc &aGc, const TPoint &aPos)
	{
	TSize iconSize = iSatelliteIcon->Bitmap()->SizeInPixels();
	TRect dstRect(aPos, iconSize);
	TRect srcRect(TPoint(0, 0), iconSize);
	aGc.DrawBitmapMasked(dstRect, iSatelliteIcon->Bitmap(), srcRect, iSatelliteIcon->Mask(), 0);
	}

TReal32 CSignalIndicatorLayer::SignalStrengthToReal(TInt aSignalStrength)
	{
	const TInt KMaxSignalStrength = 40; // Taken from here: https://github.com/SymbianSource/oss.FCL.sf.mw.locationsrv/blob/282094c09b81e1848755ad40e31052da0bcac81b/locationsystemui/locationsysui/locblidsatelliteinfo/src/satellitecontrol.cpp#L1193
	return Min(Max(aSignalStrength / TReal32(KMaxSignalStrength), 0.0), 1.0); // KMaxSignalStrength => 1, 0 => 0
	}


// CSearchResultsLayer

CSearchResultsLayer::CSearchResultsLayer(CMapControl* aMapView):
		CMapLayerBase(aMapView)
	{
	}

CSearchResultsLayer::~CSearchResultsLayer()
	{
	delete iIconSelected;
	delete iIcon;
	}

CSearchResultsLayer* CSearchResultsLayer::NewLC(CMapControl* aMapView)
	{
	CSearchResultsLayer* self = new (ELeave) CSearchResultsLayer(aMapView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSearchResultsLayer* CSearchResultsLayer::NewL(CMapControl* aMapView)
	{
	CSearchResultsLayer* self = CSearchResultsLayer::NewLC(aMapView);
	CleanupStack::Pop(); // self;
	return self;
	}

void CSearchResultsLayer::ConstructL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	CS60MapsApplication* app = static_cast<CS60MapsApplication*>(appUi->Application());
	
	iIcon = app->LoadIconL(EMbmIconsLocation, EMbmIconsLocation_mask);
	iIconSelected = app->LoadIconL(EMbmIconsLocation_selected, EMbmIconsLocation_mask);
	}

void CSearchResultsLayer::Draw(CWindowGc &aGc)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CEikonEnv::Static()->AppUi());
	
	// Check if any items to display
	CSearch* search = appUi->MapView()->Search();
	if (!/*iMapView->Search()*/ search)
		return;
	
	const CArrayFix/*Seg*/<TSearchResultItem>* searchResArr = /*iMapView->Search()*/search->Results();
	if (!searchResArr || !searchResArr->Count())
		return;
	
	/*TRAPD(r, DrawL(aGc));
	if (r != KErrNone)
		{
		DEBUG(_L("Leave with code %d"), r);
		}*/
	
	//////
	/*CArrayFixSeg<TSearchResultItem>* searchResArr = new (ELeave) CArrayFixSeg<TSearchResultItem>(10);
	TSearchResultItem item;
	
	item.iName = _L("Result 1");
	item.iCoord.SetCoordinate(39.9153, -75.2896);
	searchResArr->AppendL(item);
	
	item.iName = _L("Result 2");
	item.iCoord.SetCoordinate(39.8768, -75.3288);
	searchResArr->AppendL(item);
	
	item.iName = _L("Result 3");
	item.iCoord.SetCoordinate(39.9366, -75.3466);
	searchResArr->AppendL(item);
	
	item.iName = _L("Result 4");
	item.iCoord.SetCoordinate(39.94969, -75.34055);
	searchResArr->AppendL(item);*/
	//////
	
	/*aGc.SetPenColor(KRgbRed);
	aGc.SetPenSize(TSize(5,5));
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	*/
	/*aGc.SetBrushColor(KRgbRed);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);*/
	//aGc.UseFont(iMapView->DefaultFont());
	
	TInt nearestItemIdx = -1;
	TReal32 distance, minDistance = 99999999;
	TCoordinate screenCenterCoord = iMapView->GetCenterCoordinate();
	TSize iconSize = iIcon->Bitmap()->SizeInPixels();
	TSearchResultItem item;
	for (TInt i = 0; i < searchResArr->Count(); i++)
		{
		item = (*searchResArr)[i];
		
		// Calculate nearest to the screen center landmark index
		distance = 99999999;
		if (screenCenterCoord.Distance(item.iCoord, distance) == KErrNone)
			{
			if (distance < minDistance)
				{
				nearestItemIdx = i;
				minDistance = distance;
				}
			}
		
		
		// Calculate icon position on the screen
		//TSize iconSize = iIcon->Bitmap()->SizeInPixels();
		TPoint resultPoint = iMapView->GeoCoordsToScreenCoords(item.iCoord);
		TRect dstRect(resultPoint, iconSize);
		dstRect.Move(-iconSize.iWidth / 2, -iconSize.iHeight);
		
		TRect srcRect(TPoint(0, 0), iconSize);
		
		// Draw icon
		aGc.DrawBitmapMasked(dstRect, iIcon->Bitmap(), srcRect, iIcon->Mask(), 0);
		}
	
	
	// Draw text
	if (nearestItemIdx > -1)
		{
		item = (*searchResArr)[nearestItemIdx];
		TPoint resultPoint = iMapView->GeoCoordsToScreenCoords(item.iCoord);
		TPoint screenCenterPoint = iMapView->GeoCoordsToScreenCoords(iMapView->GetCenterCoordinate());		
		
		/*TRect popupArea = TRect(resultPoint, TSize(0, 0));
		const TInt areaSize = 30;
		popupArea.Grow(areaSize / 2, areaSize / 2);*/
		 
		TRect popupArea(resultPoint, iconSize);
		popupArea.Move(-iconSize.iWidth / 2, -iconSize.iHeight);
		const TInt areaIndent = 10;
		popupArea.Grow(areaIndent, areaIndent);
		////////////
		/*aGc.SetPenStyle(CGraphicsContext::EDashedPen);
		aGc.DrawRect(popupArea);*/
		/////////
		if (popupArea.Contains(screenCenterPoint))
			{
			// Selected icon
			TRect dstRect(resultPoint, iconSize);
			dstRect.Move(-iconSize.iWidth / 2, -iconSize.iHeight);
			
			TRect srcRect(TPoint(0, 0), iconSize);
			
			// Draw icon
			aGc.DrawBitmapMasked(dstRect, iIconSelected->Bitmap(), srcRect, iIconSelected->Mask(), 0);
			
			TRAP_IGNORE(DrawTextWithBackgroundL(aGc, item));
			}
		}
	//aGc.DiscardFont();
	
	/*//////
	//delete searchResArr;
	//////*/
	}

void CSearchResultsLayer::DrawTextWithBackgroundL(CWindowGc &aGc,
		const TSearchResultItem &aSearchResult)
	{
	//const CFont* font = iMapView->SmallFont();
	const CFont* font = iMapView->DefaultFont();
	TInt baselineOffset = /*font->BaselineOffsetInPixels()*/ /*font->AscentInPixels()*/ font->FontMaxAscent();
	const TRgb KTextColor(0x4040cd);
	aGc.SetPenColor(KTextColor);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	TRgb bgColor(KRgbWhite);
	bgColor.SetAlpha(170);
	aGc.SetBrushColor(bgColor);
	// Skip leading TAB (used for propper display in list)
	TPtrC name(aSearchResult.iName.Right(aSearchResult.iName.Length() - 1));
	TPoint resultPoint = iMapView->GeoCoordsToScreenCoords(aSearchResult.iCoord);
	TRect textRect(iMapView->Rect());
	textRect.iTl.iY = resultPoint.iY + 20;
	textRect.SetHeight(50);
	textRect.Shrink(/*15*/25, 0);
	//////////
	//aGc.DrawRect(textRect);
	/////////			
	CArrayFix<TPtrC>* lines = new (ELeave) CArrayFixFlat<TPtrC>(10);
	CleanupStack::PushL(lines);
	AknTextUtils::WrapToArrayL(name, textRect.Width(), *iMapView->DefaultFont(), *lines);
	TInt maxLineWidth = 0;
	for (TInt i = 0; i < lines->Count(); i++)
		{
		maxLineWidth = Max(maxLineWidth, font->TextWidthInPixels((*lines)[i]));
		}
	const TInt nextLineDelta = font->HeightInPixels() + 3;
	TRect r2 /*= textRect*/;
	r2.iTl.iX = iMapView->Rect().Center().iX - maxLineWidth / 2;
	r2.iTl.iY = textRect.iTl.iY;
	r2.SetWidth(maxLineWidth);
	r2.SetHeight(lines->Count() * nextLineDelta);
	r2.Grow(/*3*/ 12, 3);
	CArrayFix<TPoint>* points = new (ELeave) CArrayFixFlat<TPoint>(7);
	CleanupStack::PushL(points);
	const TInt d = 7;
	points->AppendL(TPoint(r2.iBr.iX, r2.iTl.iY));
	points->AppendL(TPoint(r2.iBr.iX, r2.iBr.iY));
	points->AppendL(TPoint(r2.iTl.iX, r2.iBr.iY));
	points->AppendL(TPoint(r2.iTl.iX, r2.iTl.iY));
	points->AppendL(TPoint(r2.Center().iX - d, r2.iTl.iY));
	points->AppendL(resultPoint);
	points->AppendL(TPoint(r2.Center().iX + d, r2.iTl.iY));
	aGc.DrawPolygon(points);
	CleanupStack::PopAndDestroy(points);
	
	
	
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush); // Should be reset before text will be drawn
	
	
	/////////////////////////
	aGc.UseFont(font);
	for (TInt i = 0; i < lines->Count(); i++)
		{
		/*static_cast<CWindowGcEx*>(&aGc)->DrawOutlinedText*/ aGc.DrawText((*lines)[i], textRect,
				baselineOffset, CGraphicsContext::ECenter, 0/*, KTextColor*/);
		textRect.Move(0, nextLineDelta);
		}
	aGc.DiscardFont();
	/////////////////////////
	CleanupStack::PopAndDestroy(lines);
	}


// CTileBitmapSaver

_LIT(KSaverThreadName, "TileSaverThread");

CTileBitmapSaver::CTileBitmapSaver(CTileBitmapManager* aMgr) :
		iMgr(aMgr),
		iThreadId(0),
		iItemsInQueue(0)
	{
	}

CTileBitmapSaver::~CTileBitmapSaver()
	{
	// Stop and destroy running thread
	RThread thr;
	if (thr.Open(iThreadId) == KErrNone)
		{
		// Clear queue
		TSaverQueryItem item;
		while (iQueue.Receive(item) != KErrUnderflow)
			{};
			
		// Add stop-item to queue
		TSaverQueryItem stopItem;
		stopItem.iShouldStop = ETrue;
		iQueue.Send(stopItem);
		
		// Wait until thread will be closed
		TRequestStatus status;
		thr.Logon(status);
		User::WaitForRequest(status);
		DEBUG(_L("Saver thread stopped"));
		thr.Close();
		}
	else
		{
		ERROR(_L("Saver thread is not running!"));
		}	
	
	
	// Free other resources
	iQueue.Close();
	}

CTileBitmapSaver* CTileBitmapSaver::NewLC(CTileBitmapManager* aMgr)
	{
	CTileBitmapSaver* self = new (ELeave) CTileBitmapSaver(aMgr);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTileBitmapSaver* CTileBitmapSaver::NewL(CTileBitmapManager* aMgr)
	{
	CTileBitmapSaver* self = CTileBitmapSaver::NewLC(aMgr);
	CleanupStack::Pop(); // self;
	return self;
	}

void CTileBitmapSaver::ConstructL()
	{
	// Some initializations
	User::LeaveIfError(iQueue.CreateLocal(50)); // ToDo: Move number to constant
	
	// Prepare and start new thread	
	RThread thr;
	User::LeaveIfError(thr.Create(
			KSaverThreadName,
			ThreadFunction,
			KDefaultStackSize,
			NULL,
			this
			));
	thr.SetPriority(/*EPriorityMuchLess*/ EPriorityLess);
	thr.Resume();
	DEBUG(_L("Saver thread started"));
	iThreadId = thr.Id(); // Remember thread ID for future use
	thr.Close();
	}

void CTileBitmapSaver::AppendL(const TTile &aTile, CFbsBitmap *aBitmap)
	{
	TSaverQueryItem item;
	item.iBitmap = aBitmap;
	item.iTile = aTile;
	item.iShouldStop = EFalse;
	
	TInt r = iQueue.Send(item);
	if (r == KErrNone) // No errors
		{
		iItemsInQueue++;
		DEBUG(_L("%S appended to saving queue"), &aTile.AsDes());
		DEBUG(_L("Now %d items in saving queue"), iItemsInQueue);
		}
	else // Any error
		{
		if (r == KErrOverflow)
			{
			ERROR(_L("Saving queue is full!"));
			}
		
		ERROR(_L("Error appending %S to saving queue (code: %d)"),
						&aTile.AsDes(), r);
		//User::Leave(r);
		}
	}

TInt CTileBitmapSaver::ThreadFunction(TAny* anArg)
	{	
	// Initializations for new thread
	CTrapCleanup* cleanup = CTrapCleanup::New();
	RFs fs;
	TInt res = fs.Connect();
	if (res == KErrNone)
		{
		RFbsSession fbsSess;
		res = fbsSess.Connect(fs);
		if (res == KErrNone)
			{
			CTileBitmapSaver* saver = static_cast<CTileBitmapSaver*>(anArg);
			
			while (ETrue)
				{
				TSaverQueryItem item;
				saver->iQueue.ReceiveBlocking(item);
				//iItemsInQueue--;
				
				if (item.iShouldStop)
					{
					res = KErrNone;
					DEBUG(_L("Stop signal recieved. Going to exit."))
					break; // Going to exit
					}
				
				DEBUG(_L("Start saving %S"), &item.iTile.AsDes());
				saver->iItemsInQueue--;
				DEBUG(_L("Now %d items in saving queue"), saver->iItemsInQueue);
				TRAPD(r, saver->SaveL(item, fs));
				if (r != KErrNone)
					{
					ERROR(_L("Saving of %S failed with code %d"),
							&item.iTile.AsDes(), r);
					}
				}
			
			fbsSess.Disconnect();
			//fbsSess.Close();
			}
		
		fs.Close();
		}
	
	DEBUG(_L("Thread finished"));
	
	delete cleanup;
	
	return /*KErrNone*/ res;
	}

void CTileBitmapSaver::SaveL(const TSaverQueryItem &anItem, RFs &aFs)
	{
	TFileName tileFileName;
	iMgr->TileFileName(anItem.iTile, tileFileName);
	BaflUtils::EnsurePathExistsL(aFs, tileFileName);
	
	RFile file;
	User::LeaveIfError(file.Replace(aFs, tileFileName, EFileWrite));
	CleanupClosePushL(file);

	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Duplicate(anItem.iBitmap->Handle())); // Can`t use bitmap from another thread directly,
																	 // therefore duplicate it
	User::LeaveIfError(bitmap->Save(file));
	
	DEBUG(_L("Bitmap for %S sucessfully saved to file \"%S\""),
					&anItem.iTile.AsDes(), &tileFileName);
	
	CleanupStack::PopAndDestroy(2, &file);
	}

// CTileBitmapManager

CTileBitmapManager::CTileBitmapManager(MTileBitmapManagerObserver *aObserver,
		RFs aFs, TTileProvider* aTileProvider, TInt aLimit) :
		CActive(EPriorityStandard),
		iObserver(aObserver),
		iLimit(aLimit),
		iState(/*TProcessingState::*/EIdle),
		iFs(aFs),
		iTileProvider(aTileProvider)
	{
	// No implementation required
	}

CTileBitmapManager::~CTileBitmapManager()
	{
	delete iSaver;
	// cancel()
	delete iFileMapper;
	delete iImgDecoder;
	iItemsLoadingQueue.Close();
	iItems.ResetAndDestroy();
	iItems.Close();
	delete iHTTPClient;
	}

CTileBitmapManager* CTileBitmapManager::NewLC(MTileBitmapManagerObserver *aObserver,
		RFs aFs, TTileProvider* aTileProvider, const TDesC &aCacheDir, TInt aLimit)
	{
	CTileBitmapManager* self = new (ELeave) CTileBitmapManager(aObserver, aFs, aTileProvider, aLimit);
	CleanupStack::PushL(self);
	self->ConstructL(aCacheDir);
	return self;
	}

CTileBitmapManager* CTileBitmapManager::NewL(MTileBitmapManagerObserver *aObserver,
		RFs aFs, TTileProvider* aTileProvider, const TDesC &aCacheDir, TInt aLimit)
	{
	CTileBitmapManager* self = CTileBitmapManager::NewLC(aObserver, aFs, aTileProvider, aCacheDir, aLimit);
	CleanupStack::Pop(); // self;
	return self;
	}

void CTileBitmapManager::ConstructL(const TDesC &aCacheDir)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
#ifdef __WINSCW__
	// Add some delay for network services have been started on the emulator,
	// otherwise CEcmtServer: 3 panic will be raised.
	User::After(10 * KSecond);
#endif
	iHTTPClient = CHTTPClient2::NewL(this, appUi->iSockServ, appUi->iConn);
	_LIT8(KAllowedTypes, "image/png, image/jpeg"); // PNG and JPG supported
	iHTTPClient->SetHeaderL(HTTP::EAccept, KAllowedTypes);
	_LIT8(KKeepAlive, "Keep-Alive");
	iHTTPClient->SetHeaderL(HTTP::EConnection, KKeepAlive); // Not mandatory for HTTP 1.1
	
	iItems = RPointerArray<CTileBitmapManagerItem>(iLimit);
	iItemsLoadingQueue = RArray<TTile>(20); // ToDo: Move 20 to constant
	
	iImgDecoder = CBufferedImageDecoder::NewL(iFs);
	
	iFileMapper = CFileTreeMapper::NewL(aCacheDir, 2, 1, ETrue);
	iSaver = CTileBitmapSaver::NewL(this);
	// ToDo: Start saver thread only when needed (at first downloaded tile)
	
	CActiveScheduler::Add(this);
	}

TInt CTileBitmapManager::GetTileBitmap(const TTile &aTile, CFbsBitmap* &aBitmap)
	{
	CTileBitmapManagerItem* item = Find(aTile);
	DEBUG(_L("tile=%S, item=%x"), &aTile.AsDes(), item);
	
	if (item == NULL)
		return KErrNotFound;
	DEBUG(_L("tile=%S, isready=%d, bitmap=%x"), &aTile.AsDes(), item->IsReady(), item->Bitmap());
	
	if (!item->IsReady())
		return KErrNotReady;
	
	aBitmap = item->Bitmap();
	item->iLastAccessTime.HomeTime(); // update access time
	return KErrNone;
	}

void CTileBitmapManager::AddToLoading(const TTile &aTile, TBool aForce)
	{
	CTileBitmapManagerItem* item = Find(aTile);
	
	if (aForce)
		{
		DEBUG(_L("Try to reload %S"), &aTile.AsDes());
		
		// Remove tile from disk and bitmap cache
		DeleteTileFile(aTile);
		Delete(aTile);
		
		Append(aTile);
		}
	else
		{
		if (item == NULL)
			Append(aTile);
		}
	}

/*TInt*/ void CTileBitmapManager::Append/*L*/(const TTile &aTile)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	
	if (iItems.Count() >= iLimit)
		{
		// Delete most rarely used item
		TInt idx(0);
		TTime minTime;
		minTime.HomeTime();
		for (TInt i = 0; i < iItems.Count(); i++)
			{
			if (iItems[i]->iLastAccessTime < minTime)
				{
				idx = i;
				minTime = iItems[i]->iLastAccessTime;
				}
			}
		
#ifdef _DEBUG
		TBuf<32> timeDes;
		iItems[idx]->iLastAccessTime.FormatL(timeDes, _L("%H:%T:%S"));
		DEBUG(_L("Delete most rarely used bitmap of %S from cache with idx=%d and last acess time=%S"),
				&iItems[idx]->Tile().AsDes(), idx, &timeDes);
#endif
		delete iItems[idx];
		iItems.Remove(idx);
		}
	
	// Add new one
	CTileBitmapManagerItem* item = CTileBitmapManagerItem::NewL(aTile/*, iObserver*/);
	iItems.Append(item);
	
	if (iState == EIdle)
		{
		// Try to find on disk first
		if (appUi->Settings()->iUseDiskCache && IsTileFileExists(aTile))
			{
			item->CreateBitmapIfNotExistL();
			TRAPD(r, LoadBitmapL(aTile, item->Bitmap()));
			if (r == KErrNone)
				{
				item->SetReady();
				}
			else // If read error, try to download
				{
				ERROR(_L("Error while reading %S from file (code: %d)"),
						&aTile.AsDes(), r);
				
				StartDownloadTileL(aTile);
				}
			}
		else
			{
			DEBUG(_L("Tile %S not found in cache dir"), &aTile.AsDes());
			// Start download now
			StartDownloadTileL(aTile);
			}
		}
	else
		{
		// Add to loading queue
		// ToDo: Check array is not full
		iItemsLoadingQueue.Append(aTile);
		DEBUG(_L("Tile %S appended to download queue"), &aTile.AsDes());
		DEBUG(_L("Total %d tiles in download queue"), iItemsLoadingQueue.Count());
		}
	DEBUG(_L("Now %d items in bitmap cache"), iItems.Count());
	}

CTileBitmapManagerItem* CTileBitmapManager::Find(const TTile &aTile) const
	{
	if (!iItems.Count())
		return NULL;
	
	for (TInt idx = iItems.Count() - 1; idx >= 0; idx--) // Needed items more often
											// located at the end of array (newest)
		{
		if (iItems[idx]->Tile() == aTile)
			{
			//iItems[idx]->iLastAccessTime.HomeTime();
			return iItems[idx];
			}
		}
	
	return NULL;
	}

void CTileBitmapManager::StartDownloadTileL(const TTile &aTile)
	{
	if (iIsOfflineMode)
		return;
	
	if (iState != /*TProcessingState::*/EIdle)
		return;
	
	iState = /*TProcessingState::*/EDownloading;
	iLoadingTile = aTile;
	
	RBuf8 tileUrl;
	const TInt KReserveLength = 30; // For variables substitution
	tileUrl.CreateL(iTileProvider->iTileUrlTemplate.Length() + KReserveLength);
	tileUrl.CleanupClosePushL();
	iTileProvider->TileUrl(tileUrl, aTile);
	
	iHTTPClient->GetL(tileUrl);
	// SetActive()
	DEBUG(_L8("Started download tile %S from url %S"), &aTile.AsDes8(), &tileUrl);
	
	CleanupStack::PopAndDestroy(&tileUrl);
	}

void CTileBitmapManager::DoCancel()
	{
	DEBUG(_L("Cancel"));
//	iHTTPClient->CancelRequest();
	iImgDecoder->Cancel();
	}

void CTileBitmapManager::RunL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	
	DEBUG(_L("CTileBitmapManager::RunL"));
	if (iStatus.Int() == KErrNone)
		{
		/*CFbsBitmap* bitmap;
		TInt r = GetTileBitmap(iLoadingTile, bitmap);
		__ASSERT_DEBUG(r == KErrNone, User::Leave(KErrNotFound));
		CTileBitmapManagerItem* item = Find(iLoadingTile);
		__ASSERT_DEBUG(item != NULL, User::Leave(KErrNotFound));*/
		CTileBitmapManagerItem* item = Find(iLoadingTile);
		__ASSERT_DEBUG(item != NULL, Panic(ES60MapsTileBitmapManagerItemNotFoundPanic));
		__ASSERT_DEBUG(item->Bitmap() != NULL, Panic(ES60MapsTileBitmapIsNullPanic));
		
		item->SetReady();
		
		INFO(_L("Tile %S downloaded and decoded"), &iLoadingTile.AsDes());
		iObserver->OnTileLoaded(iLoadingTile, item->Bitmap());
		
		if (appUi->Settings()->iUseDiskCache)
			{
			SaveBitmapInBackgroundL(iLoadingTile, item->Bitmap());
			}
		}
	else
		{
		ERROR(_L("Image decoding error: %d"), iStatus.Int());
		iObserver->OnTileLoadingFailed(iLoadingTile, iStatus.Int());
		}
	
	
	iImgDecoder->Reset();
	iState = /*TProcessingState::*/EIdle;
	
	// Start download next tile in queue
	if (iItemsLoadingQueue.Count())
		{
		TTile tile = iItemsLoadingQueue[0]; 
		iItemsLoadingQueue.Remove(0);
		
		StartDownloadTileL(tile);
		}
	}

/*TInt CTileBitmapManager::RunError(TInt aError)
	{
	return aError;
	}*/

void CTileBitmapManager::OnHTTPResponseDataChunkRecieved(
		const RHTTPTransaction aTransaction, const TDesC8 &aDataChunk,
		TInt /*anOverallDataSize*/, TBool /*anIsLastChunk*/)
	{
	DEBUG(_L("HTTP chunk recieved"));
	
	// Checking that mime-type is PNG or JPG
	// (If any error (for example: 404 Not Found) chunk may contains
	// HTML/text data instead correct image. In this case, 
	// we need to skip any processing.)
	RStringPool strP = aTransaction.Session().StringPool();
	RHTTPHeaders respHeaders = aTransaction.Response().GetHeaderCollection();
	RStringF fieldName = strP.StringF(HTTP::EContentType, RHTTPSession::GetTable());
	THTTPHdrVal fieldVal;
	TInt r = respHeaders.GetField(fieldName, 0, fieldVal);
	__ASSERT_DEBUG(r == KErrNone, Panic(ES60MapsNoRequiredHeaderInResponse)); // Unlikely if response don`t contains Content-Type header
	if (r != KErrNone)
		return;
	
	_LIT8(KPNGMimeType, "image/png");
	RStringF pngMimeType = strP.OpenFStringL(KPNGMimeType);
	_LIT8(KJPEGMimeType, "image/jpeg");
	RStringF jpegMimeType = strP.OpenFStringL(KJPEGMimeType);
	if (fieldVal.StrF() != pngMimeType and fieldVal.StrF() != jpegMimeType)
		{
		pngMimeType.Close();
		jpegMimeType.Close();
		return; // Skip other types exept PNG or JPG
		}
	pngMimeType.Close();
	jpegMimeType.Close();
	
	
	// Append data to decoder`s buffer
	iImgDecoder->AppendDataL(aDataChunk);
	
	if (!iImgDecoder->ValidDecoder())
		iImgDecoder->ContinueOpenL();
	
	if (!iImgDecoder->ValidDecoder())
		return; // Next function will leave if decoder not created
	
	if (!iImgDecoder->IsImageHeaderProcessingComplete())
		iImgDecoder->ContinueProcessingHeaderL();
	
	//iImgDecoder->AppendDataL(aDataChunk);
	//iImgDecoder->ContinueConvert();
	}

void CTileBitmapManager::OnHTTPResponse(const RHTTPTransaction /*aTransaction*/)
	{
	DEBUG(_L("HTTP response success"));
	
	iState = /*TProcessingState::*/EDecoding;
	
	// Start convert PNG/JPG to CFbsBitmap
	//CFbsBitmap* bitmap;
//	TInt r = GetTileBitmap(iLoadingTile, bitmap);
//	__ASSERT_DEBUG(r != KErrNotFound, User::Leave(KErrNotFound));
	CTileBitmapManagerItem* item = Find(iLoadingTile);
	__ASSERT_DEBUG(item != NULL, Panic(ES60MapsTileBitmapManagerItemNotFoundPanic));
	item->CreateBitmapIfNotExistL();
	__ASSERT_DEBUG(item->Bitmap() != NULL, Panic(ES60MapsTileBitmapIsNullPanic));
	
	//iImgDecoder->ContinueOpenL();
	DEBUG(_L("Tile %S succesfully downloaded, starting decode"), &iLoadingTile.AsDes());
	iImgDecoder->Convert(&this->iStatus, /**bitmap*/ *item->Bitmap(), 0);
	//iImgDecoder->ContinueConvert(&this->iStatus);
	SetActive();
	}

void CTileBitmapManager::OnHTTPError(TInt aError,
		const RHTTPTransaction /*aTransaction*/)
	{
	//ERROR(_L("HTTP error: %d"), aError);
	ERROR(_L("Failed to download tile %S, error: %d"), &iLoadingTile.AsDes(), aError);
	iObserver->OnTileLoadingFailed(iLoadingTile, aError);
	
	iImgDecoder->Reset();
	iState = /*TProcessingState::*/EIdle;
	
	
	switch (aError)
		{
		case KErrCancel:
		case KErrNotReady:
			{
			// If access point not provided switch to offline mode
			
			// ToDo: This code may be thrown in other cases. Try to find better way
			// to determine if IAP have been choosed or not.
			
			// FixMe: Access point choosing dialog appears several times in a row
			// (in my case: 2 in emulator, 5-6 on the phone) and only after that
			// we can catch cancel in this callback
			// https://github.com/artem78/s60-maps/issues/4
			iIsOfflineMode = ETrue;
			INFO(_L("Switched to Offline Mode"));
			iItemsLoadingQueue.Reset(); // Clear queue of loading tiles
			
			break;
			}
		
		case KErrAbort: // Request aborted
			{
			INFO(_L("HTTP request cancelled"));
			// No any further action
			
			break;
			}
			
		default:
			{
			// Start download next tile in queue
			if (iItemsLoadingQueue.Count())
				{
				TTile tile = iItemsLoadingQueue[0]; 
				iItemsLoadingQueue.Remove(0);
				
				StartDownloadTileL(tile);
				}

			break;
			}
		}
	}

void CTileBitmapManager::OnHTTPHeadersRecieved(
		const RHTTPTransaction aTransaction)
	{
	DEBUG(_L("HTTP headers recieved"));
	
	RStringPool strP = aTransaction.Session().StringPool();
	RHTTPHeaders respHeaders = aTransaction.Response().GetHeaderCollection();
	RStringF fieldName = strP.StringF(HTTP::EContentType, RHTTPSession::GetTable());
	THTTPHdrVal fieldVal;
	TInt r = respHeaders.GetField(fieldName, 0, fieldVal);
	__ASSERT_DEBUG(r == KErrNone, Panic(ES60MapsNoRequiredHeaderInResponse)); // Unlikely if response don`t contains Content-Type header
	if (r != KErrNone)
		return;
	/*TBuf<32> tmp;
	tmp.Copy(fieldVal.StrF().DesC());
	DEBUG(_L("mime=%S"), &tmp);*/
	
	
	iImgDecoder->Reset();
	iImgDecoder->OpenL(KNullDesC8, fieldVal.StrF().DesC());
	}

void CTileBitmapManager::SaveBitmapInBackgroundL(const TTile &aTile, /*const*/ CFbsBitmap *aBitmap)
	{
	iSaver->AppendL(aTile, aBitmap);
	}

void CTileBitmapManager::LoadBitmapL(const TTile &aTile, CFbsBitmap *aBitmap)
	{	
	TFileName tileFileName;
	TileFileName(aTile, tileFileName);
	
	RFile file;
	User::LeaveIfError(file.Open(iFs, tileFileName, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(file);
	User::LeaveIfError(aBitmap->Load(file));	
	CleanupStack::PopAndDestroy(&file);
	INFO(_L("Bitmap for %S sucessfully loaded from file \"%S\""), &aTile.AsDes(), &tileFileName);
	}

TBool CTileBitmapManager::IsTileFileExists(const TTile &aTile) /*const*/
	{
	TFileName tileFileName;
	TileFileName(aTile, tileFileName);
	return BaflUtils::FileExists(iFs, tileFileName);
	}

void CTileBitmapManager::TileFileName(const TTile &aTile, TFileName &aFileName) const
	{
	_LIT(KMBMExtension, ".mbm");
	
	/*TFileName*/ TBuf<32> originalFileName;
	originalFileName.AppendNum(aTile.iZ);
	originalFileName.Append('_');
	originalFileName.AppendNum(aTile.iX);
	originalFileName.Append('_');
	originalFileName.AppendNum(aTile.iY);
	originalFileName.Append(KMBMExtension);
	
	iFileMapper->GetFilePath(originalFileName, aFileName);
	}

void CTileBitmapManager::ChangeTileProvider(TTileProvider* aTileProvider,
		const TDesC &aCacheDir)
	{
	// FixMe: On the program startup this method may be called twice with same tile provider 
	if (iTileProvider->iId == aTileProvider->iId)
		return; // Nothing changed

	INFO(_L("Changing of tile provider from %S to %S"), &iTileProvider->iTitle, &aTileProvider->iTitle);
	
	Cancel();	
	iHTTPClient->CancelRequest();
	iItemsLoadingQueue.Reset(); // Should already be cleared by Cancel() call at previous line
	iItems.ResetAndDestroy();
	iTileProvider = aTileProvider;
	iFileMapper->SetBaseDir(aCacheDir);
	}

void CTileBitmapManager::DeleteTileFile(const TTile &aTile)
	{
	TFileName tileFileName;
	TileFileName(aTile, tileFileName);
	/*TInt r =*/ iFs.Delete(tileFileName);
	/*if (r != KErrNone)
		{
		
		}*/
	}

void CTileBitmapManager::Delete(const TTile &aTile)
	{
	for (TInt idx = 0; idx < iItems.Count(); idx++)
		{
		if (iItems[idx]->Tile() == aTile)
			{
			delete iItems[idx];
			iItems.Remove(idx);
			return; // Hope no duplicates
			}
		}
	}

// CTileBitmapManagerItem

CTileBitmapManagerItem::~CTileBitmapManagerItem()
	{
	// FixMe: Bitmap pointer maybe still used outside when deleting
	delete iBitmap; // iBitmap already may be NULL
	
	if (iBitmap != NULL)
		DEBUG(_L("Bitmap of %S destroyed"), &iTile.AsDes());
	
	DEBUG(_L("Bitmap manager item of %S destroyed"), &iTile.AsDes());
	}

CTileBitmapManagerItem* CTileBitmapManagerItem::NewL(const TTile &aTile)
	{
	CTileBitmapManagerItem* self = CTileBitmapManagerItem::NewLC(aTile);
	CleanupStack::Pop(); // self;
	return self;
	}

CTileBitmapManagerItem* CTileBitmapManagerItem::NewLC(const TTile &aTile)
	{
	CTileBitmapManagerItem* self = new (ELeave) CTileBitmapManagerItem(aTile);
	CleanupStack::PushL(self);
	self->ConstructL();
	DEBUG(_L("Bitmap manager item of %S created"), &self->iTile.AsDes());
	return self;
	}

CTileBitmapManagerItem::CTileBitmapManagerItem(const TTile &aTile) :
		iTile(aTile)
	{
	// No implementation required
	}

void CTileBitmapManagerItem::ConstructL()
	{
	iLastAccessTime.HomeTime();
	}

void CTileBitmapManagerItem::CreateBitmapIfNotExistL()
	{
	if (iBitmap == NULL)
		iBitmap = new (ELeave) CFbsBitmap();
	
	if (iBitmap->Handle() == 0)
		{
		TSize size(KTileSize, KTileSize);
		TDisplayMode mode = EColor16M;
		User::LeaveIfError(iBitmap->Create(size, mode));
		}
	}

// TTileProvider

TTileProvider::TTileProvider(const TDesC& anId, const TDesC& aTitle,
		const TDesC8& anUrlTemplate, TZoom aMinZoom, TZoom aMaxZoom,
		const TDesC& aCopyrightTextShort, const TDesC& aCopyrightText,
		const TDesC& aCopyrightUrl)
	{
	iId.Copy(anId);
	iTitle.Copy(aTitle);
	iTileUrlTemplate.Copy(anUrlTemplate);
	iMinZoomLevel = aMinZoom;
	iMaxZoomLevel = aMaxZoom;
	iCopyrightTextShort = aCopyrightTextShort;
	if (aCopyrightText.Length() == 0 && aCopyrightTextShort.Length() > 0)
		{
		iCopyrightText = aCopyrightTextShort;
		}
	else
		{
		iCopyrightText = aCopyrightText;
		}
	iCopyrightUrl = aCopyrightUrl;
	}

void TTileProvider::TileUrl(TDes8 &aUrl, const TTile &aTile)
	{
	aUrl.Zero();
	TLex8 lex(iTileUrlTemplate);

	do
		{
		/*TInt*/ TUint32 value;
		TChar randChar;
		
		if (ParseVariable(lex, aTile, value) == KErrNone)
			{
			// Try to parse variable
			aUrl.AppendNum(value);
			}
		else if (ParseRandCharRange(lex, randChar) == KErrNone)
			{
			// Try to parse random char range
			aUrl.Append(randChar);
			}
		else
			{
			// Copy char to destination string
			aUrl.Append(lex.Get());
			}
			
		} while (!lex.Eos());
	}

TInt TTileProvider::ParseRandCharRange(TLex8 &aLex, TChar &aReturnedChar)
	{
	const TChar KStartChar = '{';
	const TChar KEndChar = '}';
	
	TLexMark8 startMark;
	aLex.Mark(startMark);
	
	if (aLex.Eos() || aLex.Peek() != KStartChar)
		{
		aLex.UnGetToMark(startMark);
		return KErrNotFound;
		}
	aLex.Inc();
	
	// Process start char
	if (aLex.Eos() || !aLex.Peek().IsAlphaDigit())
		{
		aLex.UnGetToMark(startMark);
		return KErrNotFound;
		}
	TChar ch1 = aLex.Get();
	
	if (aLex.Eos() || aLex.Peek() != '-')
		{
		aLex.UnGetToMark(startMark);
		return KErrNotFound;
		}
	aLex.Inc();
	
	// Process end char
	if (aLex.Eos() || !aLex.Peek().IsAlphaDigit())
		{
		aLex.UnGetToMark(startMark);
		return KErrNotFound;
		}
	TChar ch2 = aLex.Get();
	
	if (aLex.Eos() || aLex.Peek() != KEndChar)
		{
		aLex.UnGetToMark(startMark);
		return KErrNotFound;
		}
	aLex.Inc();
	
	// Check that start char is smaller than end char
	if (!(ch1 < ch2))
		{
		aLex.UnGetToMark(startMark);
		return KErrGeneral;
		}
	
	// Get random char from range
//	aReturnedChar = ch1 + (Math::Random() % (ch2 - ch1 + 1));
	aReturnedChar = TUint32(ch1) + (Math::Random() % (TUint32(ch2) - TUint32(ch1) + 1));
	return KErrNone;
	}

TInt TTileProvider::ParseVariable(TLex8 &aLex, const TTile aTile, /*TInt32*/ TUint32 &aReturnedVal)
	{
	const TChar KStartChar = '{';
	const TChar KEndChar = '}';
	const TChar KVariablePrefixChar = '$';
	
	TLexMark8 startMark;
	aLex.Mark(startMark);
	
	if (aLex.Eos() || aLex.Peek() != KStartChar)
		{
		aLex.UnGetToMark(startMark);
		return KErrNotFound;
		}
	aLex.Inc();
	
	if (aLex.Eos() || aLex.Peek() != KVariablePrefixChar)
		{
		aLex.UnGetToMark(startMark);
		return KErrNotFound;
		}
	aLex.Inc();
	
	if (aLex.Eos())
		{
		aLex.UnGetToMark(startMark);
		return KErrNotFound;
		}
	
	TChar variable = aLex.Get();
	/*TInt32*/ TUint32 value = KNaN;
	switch (variable)
		{
		case 'x':
			value = aTile.iX;
			break;
			
		case 'y':
			value = aTile.iY;
			break;
							
		case 'z':
			value = aTile.iZ;
			break;
			
		default:
			aLex.UnGetToMark(startMark);
			return KErrNotFound;
			break;
		}
	
	if (aLex.Eos() || aLex.Peek() != KEndChar)
		{
		aLex.UnGetToMark(startMark);
		return KErrNotFound;
		}
	aLex.Inc();
	
	aReturnedVal = value;
	
	return KErrNone;	
	}



// TCoordinateEx

TCoordinateEx::TCoordinateEx() /*:
		iLatitude(KNaN),
		iLongitude(KNaN),
		iAltitude(KNaN),
		iCourse(KNaN)*/
	{
	iLatitude  = KNaN;
	iLongitude = KNaN;
	iAltitude  = KNaN;
	iCourse    = KNaN;
	iHorAccuracy = KNaN;
	}

TCoordinateEx::TCoordinateEx(const TCoordinateEx &aCoordEx) /*:
		iLatitude(aCoordEx.Latitude()),
		iLongitude(aCoordEx.Longitude()),
		iAltitude(aCoordEx.Altitude()),
		iCourse(aCoordEx.Course())*/
	{
	iLatitude  = aCoordEx.Latitude();
	iLongitude = aCoordEx.Longitude();
	iAltitude  = aCoordEx.Altitude();
	iCourse    = aCoordEx.Course();
	iHorAccuracy = aCoordEx.HorAccuracy();
	}

TCoordinateEx::TCoordinateEx(const TCoordinate &aCoord) /*:
		iLatitude(aCoord.Latitude()),
		iLongitude(aCoord.Longitude()),
		iAltitude(aCoord.Altitude()),
		iCourse(KNaN)*/
	{
	iLatitude  = aCoord.Latitude();
	iLongitude = aCoord.Longitude();
	iAltitude  = aCoord.Altitude();
	iCourse    = KNaN;
	iHorAccuracy = KNaN;
	}
