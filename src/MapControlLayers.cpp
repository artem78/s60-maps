/*
 ============================================================================
 Name		: MapControlLayers.cpp
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description :
 ============================================================================
 */

#include "MapControlLayers.h"
#include "MapControl.h"
#include <eikenv.h>
#include <gdi.h>
#include <e32math.h>
#include "Logger.h"
#include "S60Maps.pan"
#include "S60MapsAppUi.h"
#include "S60MapsApplication.h"
#include "Defs.h"
#include <S60Maps_0xED689B88.rsg>
#include <epos_cposlandmarksearch.h>
#include <epos_cposlmareacriteria.h>
#include "icons.mbg"
#include "LBSSatelliteExtended.h"
#include "MapView.h"
#include <aknutils.h> 


// CMapLayerBase
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
	if (appUi->IsPositioningAvailable() && satInfo)
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
		if (iBitmapMgr->HasError(tiles[idx]))
			{
			_LIT(KFmt, "Error: %S");
			const HBufC* err = iBitmapMgr->ErrMsg(tiles[idx]);
			if (err == NULL)
				continue;
			
			RBuf msg;
			TInt r = msg.Create(KFmt().Length() + err->Length() + 20);
			if (r == KErrNone)
				{
				msg.Format(KFmt, err);	
				DrawError(aGc, tiles[idx], msg);
				msg.Close();
				}
			continue;
			}
		
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
	//DEBUG(_L("VisibleTiles:   tl=%S  br=%S"), &topLeftTile.AsDes(), &bottomRightTile.AsDes());
	TUint x, y, maxXY;
	maxXY = MapMath::MaxTileXY(iMapView->GetZoom());
	for (y = topLeftTile.iY; y <= Min(bottomRightTile.iY, maxXY); y++)
		{
		for (x = topLeftTile.iX; x <= Min(bottomRightTile.iX, maxXY); x++)
			{
			TTile tile;
			tile.iX = x;
			tile.iY = y;
			tile.iZ = iMapView->GetZoom();
			aTiles.Append/*L*/(tile); // ToDo: Check error code
			//DEBUG(_L("visible %S"), &tile.AsDes());
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

void CTiledMapLayer::DrawError(CWindowGc &aGc, const TTile &aTile, const TDesC &aErrMsg)
	{
	TCoordinate coord = MapMath::TileToGeoCoords(aTile);
	TPoint point = iMapView->GeoCoordsToScreenCoords(coord);
	TRect destRect;
	destRect.iTl = point;
	destRect.SetSize(TSize(KTileSize, KTileSize));
	TRect screenRect = iMapView->Rect();
	if (!screenRect.Intersects(destRect)) // Check if tile is visible
		return;
	
	// Draw box
	aGc.SetPenColor(KRgbRed);
	//aGc.SetPenSize(TSize(/*1,1*/ 2,2));
	aGc.SetBrushStyle(CGraphicsContext::ERearwardDiagonalHatchBrush);
	aGc.SetBrushColor(KRgbWhite);
	/*aGc.DrawLine(destRect.iTl, destRect.iBr);
	aGc.DrawLine(TPoint(destRect.iTl.iX, destRect.iBr.iY), TPoint(destRect.iBr.iX, destRect.iTl.iY));*/
	aGc.DrawRect(destRect);
	
	// Draw error message
	destRect.Shrink(TSize(10, 10));
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbWhite);
	const CFont* font = iMapView->/*DefaultFont()*/SmallFont();
	aGc.UseFont(font);
	aGc.DrawText(aErrMsg, destRect, (KTileSize + font->AscentInPixels()) / 2, CGraphicsContext::ECenter, 0);
	aGc.DiscardFont();
	
	aGc.Reset();
	}

void CTiledMapLayer::OnTileLoaded(const TTile &/*aTile*/, const CFbsBitmap */*aBitmap*/)
	{
	//iMapView->DrawDeferred();
	iMapView->DrawNow();
	}

void CTiledMapLayer::OnTileLoadingFailed(const TTile &/*aTile*/, TInt /*aErrCode*/)
	{
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
	if (r == KErrNone && iMapView->CheckCoordVisibility(pos, 15))
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
	TUint x, y, maxXY;
	maxXY = MapMath::MaxTileXY(iMapView->GetZoom());
	for (y = topLeftTile.iY; y <= Min(bottomRightTile.iY, maxXY); y++)
		{
		for (x = topLeftTile.iX; x <= Min(bottomRightTile.iX, maxXY); x++)
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
	
//#if defined(__S60_30__)
#if !defined(SYMBIAN_FLEXIBLE_ALARM) // symbian 9.1
	TBool intersects = EFalse;
	for (TInt i = 0; i < iNameRegion.Count(); i++)
		{
		if (iNameRegion[i].Intersects(nameRect))
			{
			intersects = ETrue;
			break;
			}
		}
	if (iMapView->Rect().Intersects(nameRect) && !intersects)
#else // symbian >= 9.2
	if (iMapView->Rect().Intersects(nameRect) && !iNameRegion.Intersects(nameRect))
#endif
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
	const CSearchResultArray* searchResArr = search->Results();
	if (!searchResArr || !searchResArr->Count())
		return;
	

	
	TInt nearestItemIdx = -1;
	TReal32 distance, minDistance = 99999999;
	TCoordinate screenCenterCoord = iMapView->GetCenterCoordinate();
	TSearchResultItem item;
	for (TInt i = 0; i < searchResArr->Count(); i++)
		{
		item = (*searchResArr)[i];
		
		// Skip out of bounds items
		if (!iMapView->CheckCoordVisibility(item.iCoord, 30))
			{
			continue;
			}
		
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
		
		
		// Draw icon
		DrawIcon(aGc, item);
		}
	
	
	if (nearestItemIdx > -1)
		{
		item = (*searchResArr)[nearestItemIdx];
		TPoint resultPoint = iMapView->GeoCoordsToScreenCoords(item.iCoord);
		TPoint screenCenterPoint = iMapView->GeoCoordsToScreenCoords(iMapView->GetCenterCoordinate());		
		
		/*TRect popupArea = TRect(resultPoint, TSize(0, 0));
		const TInt areaSize = 30;
		popupArea.Grow(areaSize / 2, areaSize / 2);*/
		 
		TRect popupArea;
		IconRect(item, popupArea);
		const TInt areaIndent = 10;
		popupArea.Grow(areaIndent, areaIndent);
		
		////////////
		/*aGc.SetPenStyle(CGraphicsContext::EDashedPen);
		aGc.DrawRect(popupArea);*/
		/////////
		
		if (popupArea.Contains(screenCenterPoint)) // Check if showing popup needed
			{
			// Draw selected icon over the others
			DrawIcon(aGc, item, ETrue);
			
			// Draw text with box
			TRAP_IGNORE(DrawTextWithBackgroundL(aGc, item));
			}
		}
	}

void CSearchResultsLayer::DrawIcon(CWindowGc &aGc, const TSearchResultItem &aSearchResult,
		TBool aSelected)
	{
	const CAknIcon* icon = aSelected ? iIconSelected : iIcon;
	
	// Calculate icon position on the screen
	TRect dstRect;
	IconRect(aSearchResult, dstRect);
	
	TRect srcRect(TPoint(0, 0), dstRect.Size());
	
	// Draw icon
	aGc.DrawBitmapMasked(dstRect, icon->Bitmap(), srcRect, icon->Mask(), 0);
	}

void CSearchResultsLayer::DrawTextWithBackgroundL(CWindowGc &aGc,
		const TSearchResultItem &aSearchResult)
	{
	//const CFont* font = iMapView->SmallFont();
	const CFont* font = iMapView->DefaultFont();
	const TRgb KTextAndBoxBorderColor(0x4040cd);
	TRgb bgColor(KRgbWhite);
	bgColor.SetAlpha(195);
	
	// Skip leading TAB (used for propper display in list)
	TPtrC name(aSearchResult.iName.Right(aSearchResult.iName.Length() - 1));
	TPoint resultPoint = iMapView->GeoCoordsToScreenCoords(aSearchResult.iCoord);
	TRect firstLineRect(iMapView->Rect());
	firstLineRect.iTl.iY = resultPoint.iY + 20;
	firstLineRect.SetHeight(50);
	firstLineRect.Shrink(/*15*/25, 0);
		
	CArrayFix<TPtrC>* lines = new (ELeave) CArrayFixFlat<TPtrC>(10);
	CleanupStack::PushL(lines);
	AknTextUtils::WrapToArrayL(name, firstLineRect.Width(), *font, *lines);
	TInt maxLineWidth = StrUtils::MaxLineWidthInPixels(lines, font);
	const TInt lineHeight = font->HeightInPixels() + 3;
	TRect bgRect;
	bgRect.iTl.iX = iMapView->Rect().Center().iX - maxLineWidth / 2;
	bgRect.iTl.iY = firstLineRect.iTl.iY;
	bgRect.SetWidth(maxLineWidth);
	bgRect.SetHeight(lines->Count() * lineHeight);
	bgRect.Grow(/*3*/ 12, 3);

	aGc.SetPenColor(KTextAndBoxBorderColor);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(bgColor);
	
	DrawBackgroundBoxL(aGc, bgRect, resultPoint);
	DrawMultiLineText(aGc, lines, font, lineHeight, firstLineRect);
	
	CleanupStack::PopAndDestroy(lines);
	}

void CSearchResultsLayer::DrawBackgroundBoxL(CWindowGc &aGc, const TRect &aRect,
		const TPoint &aArrowTopPoint)
	{
	const TInt KPointCount = 7; 
	CArrayFix<TPoint>* points = new (ELeave) CArrayFixFlat<TPoint>(KPointCount);
	CleanupStack::PushL(points);
	const TInt KArrowHalfWidth = 7;
	points->AppendL(TPoint(aRect.iBr.iX, aRect.iTl.iY));
	points->AppendL(TPoint(aRect.iBr.iX, aRect.iBr.iY));
	points->AppendL(TPoint(aRect.iTl.iX, aRect.iBr.iY));
	points->AppendL(TPoint(aRect.iTl.iX, aRect.iTl.iY));
	points->AppendL(TPoint(aRect.Center().iX - KArrowHalfWidth, aRect.iTl.iY));
	points->AppendL(aArrowTopPoint);
	points->AppendL(TPoint(aRect.Center().iX + KArrowHalfWidth, aRect.iTl.iY));
	
	aGc.DrawPolygon(points);
	
	CleanupStack::PopAndDestroy(points);
	}

void CSearchResultsLayer::DrawMultiLineText(CWindowGc &aGc, CArrayFix<TPtrC>* aLines,
		const CFont* aFont, TInt aLineHeight, const TRect &aFirstLineRect)
	{
	TInt baselineOffset = /*aFont->BaselineOffsetInPixels()*/ /*aFont->AscentInPixels()*/ aFont->FontMaxAscent();
	TRect lineRect = aFirstLineRect;
	
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush); // Should be reset before text will be drawn
	aGc.UseFont(aFont);
	for (TInt i = 0; i < aLines->Count(); i++)
		{
		aGc.DrawText((*aLines)[i], lineRect, baselineOffset, CGraphicsContext::ECenter, 0);
		lineRect.Move(0, aLineHeight);
		}
	aGc.DiscardFont();
	}

void CSearchResultsLayer::IconRect(const TSearchResultItem &aSearchResult, TRect &aRect)
	{
	const CAknIcon* icon = /*aSelected ? iIconSelected :*/ iIcon; // Assume both icons have the same size in pixels!
	
	// Calculate icon position on the screen
	TPoint resultPoint = iMapView->GeoCoordsToScreenCoords(aSearchResult.iCoord);
	TSize iconSize = icon->Bitmap()->SizeInPixels();
	aRect = TRect(resultPoint, iconSize);
	aRect.Move(-iconSize.iWidth / 2, -iconSize.iHeight);
	}
