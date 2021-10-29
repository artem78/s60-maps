/*
 * Map.cpp
 *
 *  Created on: 14.08.2019
 *      Author: artem78
 */

#include "Map.h"
#include "S60MapsAppView.h"
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

CMapLayerBase::CMapLayerBase(/*const*/ CS60MapsAppView* aMapView) :
		iMapView(aMapView)
	{
	}


#ifdef DEBUG_SHOW_ADDITIONAL_INFO

// CMapLayerDebugInfo

CMapLayerDebugInfo::CMapLayerDebugInfo(/*const*/ CS60MapsAppView* aMapView) :
	CMapLayerBase(aMapView),
	iRedrawingsCount(0)
	{
	}

CMapLayerDebugInfo* CMapLayerDebugInfo::NewLC(CS60MapsAppView* aMapView)
	{
	CMapLayerDebugInfo* self = new (ELeave) CMapLayerDebugInfo(aMapView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMapLayerDebugInfo* CMapLayerDebugInfo::NewL(CS60MapsAppView* aMapView)
	{
	CMapLayerDebugInfo* self = CMapLayerDebugInfo::NewLC(aMapView);
	CleanupStack::Pop(); // self;
	return self;
	}

void CMapLayerDebugInfo::ConstructL()
	{
	// Set font
	_LIT(KFontName, "Series 60 Sans");
	const TInt KFontHeightInTwips = 11 * 12;
	TFontSpec fontSpec(KFontName, KFontHeightInTwips);
	fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	CGraphicsDevice* screenDevice = CCoeEnv::Static()->ScreenDevice();
	User::LeaveIfError(screenDevice->GetNearestFontInTwips(iFont, fontSpec));
	}

CMapLayerDebugInfo::~CMapLayerDebugInfo()
	{
	CCoeEnv::Static()->ScreenDevice()->ReleaseFont(iFont);
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
	
	iRedrawingsCount++;
	TCoordinate center = iMapView->GetCenterCoordinate();
	
	// Prepare strings
	/*TBuf<100> buff;
		_LIT(KFmt, "Redrawings: %d\rLat: %f\rLon: %f\rZoom: %d");
		buff.Format(KFmt, iRedrawingsCount, center.Latitude(), center.Longitude(), (TInt) iMapView->GetZoom());*/
	
	CDesCArrayFlat* strings = new (ELeave) CDesCArrayFlat(4);
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
	
	// Draw
	aGc.Reset();
	aGc.SetPenColor(KRgbDarkBlue);
	
	aGc.UseFont(iFont);
	TRect area = iMapView->Rect();
	area.Shrink(4, 4);
	TInt baselineOffset = 0;
	for (TInt i = 0; i < strings->Count(); i++)
		{
		baselineOffset += iFont->AscentInPixels() + 5;
		aGc.DrawText((*strings)[i], area, baselineOffset, CGraphicsContext::ERight);
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

CTiledMapLayer::CTiledMapLayer(CS60MapsAppView* aMapView) :
	CMapLayerBase(aMapView)
	{
	// No implementation required
	}

CTiledMapLayer::~CTiledMapLayer()
	{
	delete iTileProvider;
	delete iBitmapCache;
	}

CTiledMapLayer* CTiledMapLayer::NewL(CS60MapsAppView* aMapView, TWebTileProviderSettings* aTileProviderSettings)
	{
	CTiledMapLayer* self = CTiledMapLayer::NewLC(aMapView, aTileProviderSettings);
	CleanupStack::Pop(); // self;
	return self;
	}

CTiledMapLayer* CTiledMapLayer::NewLC(CS60MapsAppView* aMapView, TWebTileProviderSettings* aTileProviderSettings)
	{
	CTiledMapLayer* self = new (ELeave) CTiledMapLayer(aMapView);
	CleanupStack::PushL(self);
	self->ConstructL(aTileProviderSettings);
	return self;
	}

void CTiledMapLayer::ConstructL(TWebTileProviderSettings* aTileProviderSettings)
	{
	RFs fs = iMapView->ControlEnv()->FsSession();
	iBitmapCache = CTileBitmapMemCache::NewL();
	iTileProvider = CWebTileProvider::NewL(this, fs, aTileProviderSettings);
	}

void CTiledMapLayer::Draw(CWindowGc &aGc)
	{
	DEBUG(_L("Begin layer drawing"));
	
	RArray<TTile> tiles(10);
	VisibleTiles(tiles);
	for (TInt idx = 0; idx < tiles.Count(); idx++)
		{
		CFbsBitmap* bitmap;
		TInt err = iBitmapCache->GetTileBitmap(tiles[idx], bitmap);
		switch (err)
			{
			case KErrNone:
				{
				DrawTile(aGc, tiles[idx], bitmap);
				break;
				}
				
			case KErrNotFound:
				{
				iBitmapCache->ReserveItem(tiles[idx]);
				iTileProvider->RequestTileL(tiles[idx]);
				break;
				}
				
			/*default:
				break;*/
			}
		
		}
	
	tiles.Close();
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
	TCoordinate coord = MapMath::TileToGeoCoords(aTile, iMapView->GetZoom());
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

void CTiledMapLayer::OnTileLoaded(const TTile &aTile, const CFbsBitmap *aBitmap)
	{
	CTileBitmapMemCacheItem* item = iBitmapCache->Find(aTile);
	__ASSERT_DEBUG(item != NULL, Panic(ES60MapsTileBitmapManagerItemNotFoundPanic));
	__ASSERT_DEBUG(item->Bitmap() != NULL, Panic(ES60MapsTileBitmapIsNullPanic));
	item->Bitmap()->Duplicate(aBitmap->Handle());
	item->SetReady();
	
	//iMapView->DrawDeferred();
	iMapView->DrawNow();
	}

void CTiledMapLayer::SetTileProviderSettingsL(TWebTileProviderSettings* aTileProviderSettings)
	{
	//iMapView->SetZoomBounds(iTileProviderSettings->MinZoomLevel(), iTileProviderSettings->MaxZoomLevel());
	
	// Set new tile provider and cache dir for bitmap manager
	//iBitmapCache->ChangeTileProviderSettings(aTileProviderSettings);
	
	//iMapView->DrawNow();
	
	
	if (iTileProvider->iSettings->iId == aTileProviderSettings->iId)
		return; // Nothing changed
		
	iBitmapCache->Reset();
		
	iTileProvider->SetSettingsL(aTileProviderSettings);
	}


// CUserPositionLayer

CUserPositionLayer::CUserPositionLayer(/*const*/ CS60MapsAppView* aMapView) :
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
	
	// Drawing
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

CTileBorderAndXYZLayer::CTileBorderAndXYZLayer(CS60MapsAppView* aMapView) :
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
	TCoordinate coord = MapMath::TileToGeoCoords(aTile, iMapView->GetZoom());
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

CScaleBarLayer::CScaleBarLayer(CS60MapsAppView* aMapView):
		CMapLayerBase(aMapView)
	{
	}

CScaleBarLayer::~CScaleBarLayer()
	{
	CCoeEnv::Static()->ScreenDevice()->ReleaseFont(iFont);
	
	delete iKilometersUnit;
	delete iMetersUnit;
	}

CScaleBarLayer* CScaleBarLayer::NewLC(CS60MapsAppView* aMapView)
	{
	CScaleBarLayer* self = new (ELeave) CScaleBarLayer(aMapView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CScaleBarLayer* CScaleBarLayer::NewL(CS60MapsAppView* aMapView)
	{
	CScaleBarLayer* self = CScaleBarLayer::NewLC(aMapView);
	CleanupStack::Pop(); // self;
	return self;
	}

void CScaleBarLayer::ConstructL()
	{
	// Read strings from resources
	iMetersUnit = CCoeEnv::Static()->AllocReadResourceL(R_METERS_UNIT_SHORT);
	iKilometersUnit = CCoeEnv::Static()->AllocReadResourceL(R_KILOMETERS_UNIT_SHORT);
	
	// Load font for label text
	//iFont = const_cast<CFont*>(CEikonEnv::Static()->AnnotationFont());
	//iFont = CEikonEnv::Static()->AnnotationFont();
	_LIT(KFontName, /*"OpenSans"*/ "Series 60 Sans");
	const TInt KFontHeightInTwips = /*9*/ 10 * 12; // Twip = 1/12 point
	TFontSpec fontSpec(KFontName, KFontHeightInTwips);
	fontSpec.iTypeface.SetIsSerif(EFalse);
	fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	CGraphicsDevice* screenDevice = CCoeEnv::Static()->ScreenDevice();
	TInt r = screenDevice->/*GetNearestFontToMaxHeightInTwips*/ GetNearestFontInTwips(iFont, fontSpec);
	User::LeaveIfError(r);
	}

void CScaleBarLayer::Draw(CWindowGc &aGc)
	{
	const TInt KBarLeftMargin    = 14;
	const TInt KBarBottomMargin  = KBarLeftMargin;
	const TInt KBarWidth         = 100;
	const TInt KBarHeight        = 3;
	const TInt KTextBottomMargin = 6;
	
	// Draw scale line
	TPoint barStartPoint(KBarLeftMargin, iMapView->Rect().iBr.iY - KBarBottomMargin);
	TPoint barEndPoint(barStartPoint);
	barStartPoint.iY -= KBarHeight;
	barEndPoint.iX += KBarWidth;
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbBlack);
	aGc.DrawRect(TRect(barStartPoint, barEndPoint));
	aGc.Reset();
	
	// Getting distance value
	TReal32 horDist;
	{
		TReal32 vertDist; // unused
		MapMath::PixelsToMeters(iMapView->GetCenterCoordinate().Latitude(),
				iMapView->GetZoom(), KBarWidth, horDist, vertDist);
	}
	
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
	aGc.UseFont(iFont);
	//aGc.DrawText(text, startPoint);
	aGc.DrawText(text, textRect, baselineOffset, CGraphicsContext::ECenter);
	aGc.DiscardFont();
	}


// CLandmarksLayer

CLandmarksLayer::CLandmarksLayer(CS60MapsAppView* aMapView, CPosLandmarkDatabase* aLmDb):
		CMapLayerBase(aMapView),
		iLandmarksDb(aLmDb)
	{
	}

CLandmarksLayer::~CLandmarksLayer()
	{
	CCoeEnv::Static()->ScreenDevice()->ReleaseFont(iFont);
	ReleaseLandmarkResources();
	delete iIconMaskBitmap;
	delete iIconBitmap;
	}

CLandmarksLayer* CLandmarksLayer::NewLC(CS60MapsAppView* aMapView, CPosLandmarkDatabase* aLmDb)
	{
	CLandmarksLayer* self = new (ELeave) CLandmarksLayer(aMapView, aLmDb);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CLandmarksLayer* CLandmarksLayer::NewL(CS60MapsAppView* aMapView, CPosLandmarkDatabase* aLmDb)
	{
	CLandmarksLayer* self = CLandmarksLayer::NewLC(aMapView, aLmDb);
	CleanupStack::Pop(); // self;
	return self;
	}

void CLandmarksLayer::ConstructL()
	{
	_LIT(KMbmFilePathFmt, "%c:%Sicons.mbm");
	
	TFileName privateDir;
	User::LeaveIfError(CEikonEnv::Static()->FsSession().PrivatePath(privateDir));
	TFileName mbmFilePath;
	mbmFilePath.Format(KMbmFilePathFmt, FileUtils::InstallationDrive(), &privateDir);
	
	iIconBitmap = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iIconBitmap->Load(mbmFilePath, EMbmIconsStar));
	
	iIconMaskBitmap = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iIconMaskBitmap->Load(mbmFilePath, EMbmIconsStar_mask));
	
	// Load font
	_LIT(KFontName, /*"OpenSans"*/ "Series 60 Sans");
	const TInt KFontHeightInTwips = 10 /*12*/ * 12; // Twip = 1/12 point
	TFontSpec fontSpec(KFontName, KFontHeightInTwips);
	fontSpec.iTypeface.SetIsSerif(EFalse);
	fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	CGraphicsDevice* screenDevice = CCoeEnv::Static()->ScreenDevice();
	User::LeaveIfError(screenDevice->/*GetNearestFontToMaxHeightInTwips*/ GetNearestFontInTwips(iFont, fontSpec));
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

CArrayPtr<CPosLandmark>* CLandmarksLayer::GetVisibleLandmarksL()
	{
	const TInt KMaxVisibleLandmarksLimit = 100;
	
	CArrayPtr<CPosLandmark>* landmarks = NULL;
	
	DEBUG(_L("Start landmarks queing"));
	
	CPosLandmarkSearch* landmarkSearch = CPosLandmarkSearch::NewL(*iLandmarksDb);
	CleanupStack::PushL(landmarkSearch);
	landmarkSearch->SetMaxNumOfMatches(KMaxVisibleLandmarksLimit); // Set display amount limit
	TCoordinate topLeftCoord, bottomRightCoord;
	iMapView->Bounds(topLeftCoord, bottomRightCoord);
	CPosLmAreaCriteria* areaCriteria = CPosLmAreaCriteria::NewLC(
			bottomRightCoord.Latitude(),
			topLeftCoord.Latitude(),
			topLeftCoord.Longitude(),
			bottomRightCoord.Longitude());
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
		landmarks = iLandmarksDb->TakePreparedPartialLandmarksL(landmarkOp2);
	
		CleanupStack::PopAndDestroy(3, landmarkIter);
		}
		
	CleanupStack::PopAndDestroy(2, landmarkSearch);
	
	DEBUG(_L("End landmarks queing (found %d items)"), landmarksCount);
	
	return landmarks;
	}

void CLandmarksLayer::DrawL(CWindowGc &aGc)
	{
	CArrayPtr<CPosLandmark>* landmarks = GetVisibleLandmarksL();
	
	if (landmarks && landmarks->Count())
		{
		DrawLandmarks(aGc, landmarks);
		
		landmarks->ResetAndDestroy();
		}
	delete landmarks;
	}

void CLandmarksLayer::DrawLandmarks(CWindowGc &aGc,
		const CArrayPtr<CPosLandmark>* aLandmarks)
	{
	DEBUG(_L("Landmarks redrawing started"));
	
	//const TRgb KPenColor(59, 120, 162);
	const TRgb KPenColor(21, 63, 92);
	aGc.SetPenColor(KPenColor); // For drawing text
	aGc.UseFont(iFont);
	
	for (TInt i = 0; i < aLandmarks->Count(); i++)
		{	
		CPosLandmark* landmark = /*aLandmarks[i]*/ aLandmarks->At(i);
		
		if (!landmark) continue;
		
		DrawLandmark(aGc, landmark);
		}
	
	aGc.DiscardFont();
	
	DEBUG(_L("Landmarks redrawing ended"));	
	}

void CLandmarksLayer::DrawLandmark(CWindowGc &aGc,
		const CPosLandmark* aLandmark)
	{
	// Get landmark position and name
	TPtrC landmarkName;
	if (aLandmark->GetLandmarkName(landmarkName) != KErrNone)
		{
		landmarkName.Set(KNullDesC);
		}
	
	TLocality landmarkPos;
	if (aLandmark->GetPosition(landmarkPos) != KErrNone)
		{
		landmarkPos.SetCoordinate(KNaN, KNaN);
		}
	
	DEBUG(_L("Drawing landmark: lat=%f lon=%f name=%S"), landmarkPos.Latitude(),
			landmarkPos.Longitude(), &landmarkName);
	
	
	TPoint landmarkPoint = iMapView->GeoCoordsToScreenCoords(landmarkPos);
	
	// Draw landmark icon
	TSize iconSize = iIconBitmap->SizeInPixels();
	{
		TRect dstRect(landmarkPoint, TSize(0, 0));
		dstRect.Grow(iconSize.iWidth / 2, iconSize.iHeight / 2);
		TRect srcRect(TPoint(0, 0), iconSize);
		aGc.DrawBitmapMasked(dstRect, iIconBitmap, srcRect, iIconMaskBitmap, 0);
	}
	
	
	// Draw landmark name
	if (landmarkName.Length())
		{
		const TInt KLabelMargin = 5;
		TPoint labelPoint(landmarkPoint);
		labelPoint.iX += iconSize.iWidth / 2 + KLabelMargin;
		labelPoint.iY += /*iFont->HeightInPixels()*/ iFont->AscentInPixels() / 2;
		aGc.DrawText(landmarkName, labelPoint);
		}
	}

// CLandmarksLayer

CCrosshairLayer::CCrosshairLayer(CS60MapsAppView* aMapView) :
		CMapLayerBase(aMapView)
	  {	
	  }

void CCrosshairLayer::Draw(CWindowGc &aGc)
	{
	const TInt KLineHalfLength = 10; // in px
	TPoint center = iMapView->Rect().Center();
	
	aGc.SetPenColor(KRgbBlack);
	aGc.SetPenSize(TSize(1, 1));
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	
	TPoint start1 = center;
	start1.iX += KLineHalfLength;
	TPoint end1 = center;
	end1.iX -= (KLineHalfLength + 1);
	aGc.DrawLine(start1, end1);
	
	TPoint start2 = center;
	start2.iY += KLineHalfLength;
	TPoint end2 = center;
	end2.iY -= (KLineHalfLength + 1);
	aGc.DrawLine(start2, end2);
	}


// CSaverQueryItem

CSaverQueryItem* CSaverQueryItem::NewLC()
	{
	CSaverQueryItem* self = new (ELeave) CSaverQueryItem();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSaverQueryItem* CSaverQueryItem::NewL()
	{
	CSaverQueryItem* self = CSaverQueryItem::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

CSaverQueryItem::CSaverQueryItem()
	{
	// Empty
	}

void CSaverQueryItem::ConstructL()
	{
	iBitmap = new (ELeave) CFbsBitmap();
	}

CSaverQueryItem::~CSaverQueryItem()
	{
	delete iBitmap;
	}


// CTileBitmapSaver

_LIT(KSaverThreadName, "TileSaverThread");

CTileBitmapSaver::CTileBitmapSaver(CWebTileProvider* aTileProvider) :
		CActive(EPriorityLow),
		iTileProvider(aTileProvider),
		iThreadId(0),
		iItemsInQueue(0)
	{
	}

CTileBitmapSaver::~CTileBitmapSaver()
	{
	Cancel();
	
	// Stop and destroy running thread
	RThread thr;
	if (thr.Open(iThreadId) == KErrNone)
		{
		// Clear queue
		CSaverQueryItem* item = NULL;
		while (iQueue.Receive(item) != KErrUnderflow)
			{
			delete item;
			};
			
		// Add NULL-item to queue for stop the thread
		iQueue.Send(NULL);
		
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
	iRemovingQueue.Close();
	iQueue.Close();
	}

CTileBitmapSaver* CTileBitmapSaver::NewLC(CWebTileProvider* aTileProvider)
	{
	CTileBitmapSaver* self = new (ELeave) CTileBitmapSaver(aTileProvider);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTileBitmapSaver* CTileBitmapSaver::NewL(CWebTileProvider* aTileProvider)
	{
	CTileBitmapSaver* self = CTileBitmapSaver::NewLC(aTileProvider);
	CleanupStack::Pop(); // self;
	return self;
	}

void CTileBitmapSaver::ConstructL()
	{
	// Some initializations
	User::LeaveIfError(iQueue.CreateLocal(50)); // ToDo: Move number to constant
	User::LeaveIfError(iRemovingQueue.CreateLocal(10));
	
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
	
	CActiveScheduler::Add(this);
	iRemovingQueue.NotifyDataAvailable(iStatus);
	SetActive();
	}

void CTileBitmapSaver::AppendL(const TTile &aTile, CFbsBitmap *aBitmap)
	{
	CSaverQueryItem* item = CSaverQueryItem::NewL();
	item->iBitmap->Duplicate(aBitmap->Handle());
	item->iTile = aTile;
	
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
				CSaverQueryItem* item = NULL;
				saver->iQueue.ReceiveBlocking(item);
				//iItemsInQueue--;
				
				if (!item)
					{
					res = KErrNone;
					DEBUG(_L("Stop signal recieved. Going to exit."))
					break; // Going to exit
					}
				
				DEBUG(_L("Start saving %S"), &item->iTile.AsDes());
				saver->iItemsInQueue--;
				DEBUG(_L("Now %d items in saving queue"), saver->iItemsInQueue);
				TRAPD(r, saver->SaveL(*item, fs));
				if (r != KErrNone)
					{
					ERROR(_L("Saving of %S failed with code %d"),
							&item->iTile.AsDes(), r);
					}

				TRAP_IGNORE(saver->RemoveItemInThreadL(item));
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

void CTileBitmapSaver::SaveL(const CSaverQueryItem &anItem, RFs &aFs)
	{
	TFileName tileFileName;
	iTileProvider->TileFileName(anItem.iTile, tileFileName);
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

void CTileBitmapSaver::RemoveItemInThreadL(CSaverQueryItem *anItem)
	{
	TInt r = iRemovingQueue.Send(anItem);
	if (r != KErrNone)
		{
		ERROR(_L("Failed to add item in removing queue"));
		User::Leave(r);
		}
	}

void CTileBitmapSaver::RunL()
	{
	switch (iStatus.Int())
		{
		case KErrNone:
			{
			// Call destructor for all items in iRemovingQueue
			CSaverQueryItem* item = NULL;
			while (iRemovingQueue.Receive(item) != KErrUnderflow)
				{
				DEBUG(_L("Delete %S"), &item->iTile.AsDes());
				delete item;
				};
			
			// Start waiting for next item
			iRemovingQueue.NotifyDataAvailable(iStatus);
			SetActive();
			}
		break;
		
		default:
		break;
		}
	}

void CTileBitmapSaver::DoCancel()
	{
	iRemovingQueue.CancelDataAvailable();
	}


// CTileBitmapMemCache

CTileBitmapMemCache::CTileBitmapMemCache(TInt aLimit) :
		iLimit(aLimit)
	{
	// No implementation required
	}

CTileBitmapMemCache::~CTileBitmapMemCache()
	{
	iItems.ResetAndDestroy();
	iItems.Close();
	}

CTileBitmapMemCache* CTileBitmapMemCache::NewLC(TInt aLimit)
	{
	CTileBitmapMemCache* self = new (ELeave) CTileBitmapMemCache(aLimit);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTileBitmapMemCache* CTileBitmapMemCache::NewL(TInt aLimit)
	{
	CTileBitmapMemCache* self = CTileBitmapMemCache::NewLC(aLimit);
	CleanupStack::Pop(); // self;
	return self;
	}

void CTileBitmapMemCache::ConstructL()
	{
	iItems = RPointerArray<CTileBitmapMemCacheItem>(iLimit);
	}

TInt CTileBitmapMemCache::GetTileBitmap(const TTile &aTile, CFbsBitmap* &aBitmap)
	{
	CTileBitmapMemCacheItem* item = Find(aTile);
	DEBUG(_L("tile=%S, item=%x"), &aTile.AsDes(), item);
	
	if (item == NULL)
		return KErrNotFound;
	DEBUG(_L("tile=%S, isready=%d, bitmap=%x"), &aTile.AsDes(), item->IsReady(), item->Bitmap());
	
	if (!item->IsReady())
		return KErrNotReady;
	
	aBitmap = item->Bitmap();
	return KErrNone;
	}

void CTileBitmapMemCache::ReserveItem(const TTile &aTile)
	{
	CTileBitmapMemCacheItem* item = Find(aTile);
	
	if (item == NULL)
		Append(aTile);
	}

/*TInt*/ void CTileBitmapMemCache::Append/*L*/(const TTile &aTile)
	{
	if (iItems.Count() >= iLimit)
		{
		// Delete oldest item
		DEBUG(_L("Delete old bitmap of %S from cache"), &iItems[0]->Tile().AsDes());
		delete iItems[0];
		iItems.Remove(0);
		}
	
	// Add new one
	CTileBitmapMemCacheItem* item = CTileBitmapMemCacheItem::NewL(aTile/*, iObserver*/);
	iItems.Append(item);
	

	
	item->CreateBitmapIfNotExistL();
	
	//iWebTileProvider->RequestTileL(aTile);

	DEBUG(_L("Now %d items in bitmap cache"), iItems.Count());
	}

CTileBitmapMemCacheItem* CTileBitmapMemCache::Find(const TTile &aTile) const
	{
	if (!iItems.Count())
		return NULL;
	
	for (TInt idx = iItems.Count() - 1; idx >= 0; idx--) // Needed items more often
											// located at the end of array (newest)
		{
		if (iItems[idx]->Tile() == aTile)
			return iItems[idx];
		}
	
	return NULL;
	}

	

void CTileBitmapMemCache::Reset()
	{
	iItems.ResetAndDestroy();
	}

// CTileBitmapMemCacheItem

CTileBitmapMemCacheItem::~CTileBitmapMemCacheItem()
	{
	// FixMe: Bitmap pointer maybe still used outside when deleting
	delete iBitmap; // iBitmap already may be NULL
	
	if (iBitmap != NULL)
		DEBUG(_L("Bitmap of %S destroyed"), &iTile.AsDes());
	
	DEBUG(_L("Bitmap manager item of %S destroyed"), &iTile.AsDes());
	}

CTileBitmapMemCacheItem* CTileBitmapMemCacheItem::NewL(const TTile &aTile)
	{
	CTileBitmapMemCacheItem* self = CTileBitmapMemCacheItem::NewLC(aTile);
	CleanupStack::Pop(); // self;
	return self;
	}

CTileBitmapMemCacheItem* CTileBitmapMemCacheItem::NewLC(const TTile &aTile)
	{
	CTileBitmapMemCacheItem* self = new (ELeave) CTileBitmapMemCacheItem(aTile);
	CleanupStack::PushL(self);
	self->ConstructL();
	DEBUG(_L("Bitmap manager item of %S created"), &self->iTile.AsDes());
	return self;
	}

CTileBitmapMemCacheItem::CTileBitmapMemCacheItem(const TTile &aTile) :
		iTile(aTile)
	{
	// No implementation required
	}

void CTileBitmapMemCacheItem::ConstructL()
	{
	// Second phase construction is not used at the moment
	}

void CTileBitmapMemCacheItem::CreateBitmapIfNotExistL()
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

// TWebTileProviderSettings

TWebTileProviderSettings::TWebTileProviderSettings(const TDesC& anId, const TDesC& aTitle,
		const TDesC8& anUrlTemplate, TZoom aMinZoom, TZoom aMaxZoom)
	{
	iId.Copy(anId);
	iTitle.Copy(aTitle);
	iTileUrlTemplate.Copy(anUrlTemplate);
	iMinZoomLevel = aMinZoom;
	iMaxZoomLevel = aMaxZoom;
	}

void TWebTileProviderSettings::TileUrl(TDes8 &aUrl, const TTile &aTile)
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

TInt TWebTileProviderSettings::ParseRandCharRange(TLex8 &aLex, TChar &aReturnedChar)
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

TInt TWebTileProviderSettings::ParseVariable(TLex8 &aLex, const TTile aTile, /*TInt32*/ TUint32 &aReturnedVal)
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


// CWebTileProvider

CWebTileProvider::CWebTileProvider(MTileBitmapManagerObserver *aObserver,
		RFs &aFs) :
		CActive(EPriorityStandard),
		iObserver(aObserver),
		iState(/*TProcessingState::*/EIdle),
		iFs(aFs)
	{
	// No implementation required
	}

CWebTileProvider::~CWebTileProvider()
	{
	delete iSaver;
	delete iFileMapper;
	// cancel()
	delete iImgDecoder;
	iItemsLoadingQueue.Close();
	delete iHTTPClient;
	delete iBitmap;
	}

CWebTileProvider* CWebTileProvider::NewLC(MTileBitmapManagerObserver *aObserver,
		RFs &aFs, TWebTileProviderSettings* aSettings)
	{
	CWebTileProvider* self = new (ELeave) CWebTileProvider(aObserver, aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aSettings);
	return self;
	}

CWebTileProvider* CWebTileProvider::NewL(MTileBitmapManagerObserver *aObserver,
		RFs &aFs, TWebTileProviderSettings* aSettings)
	{
	CWebTileProvider* self = CWebTileProvider::NewLC(aObserver, aFs, aSettings/*, aCacheDir*/);
	CleanupStack::Pop(); // self;
	return self;
	}

void CWebTileProvider::ConstructL(TWebTileProviderSettings* aSettings)
	{
#ifdef __WINSCW__
	// Add some delay for network services have been started on the emulator,
	// otherwise CEcmtServer: 3 panic will be raised.
	User::After(10 * KSecond);
#endif
	
	// Create bitmap
	iBitmap = new (ELeave) CFbsBitmap();
	
	// Create and setup HTTP client
	iHTTPClient = CHTTPClient::NewL(this);
	
	TBuf8<32> userAgent;
	userAgent.Copy(_L8("S60Maps")); // ToDo: Move to constant
	userAgent.Append(' ');
	userAgent.Append('v');
	userAgent.Append(KProgramVersion.Name());
#ifdef _DEBUG
	_LIT8(KDebugStr, "DEV");
	userAgent.Append(' ');
	userAgent.Append(KDebugStr);
#endif
	iHTTPClient->SetUserAgentL(userAgent);
	_LIT8(KAllowedTypes, "image/png"); // At the moment only PNG supported
	iHTTPClient->SetHeaderL(HTTP::EAccept, KAllowedTypes);
	_LIT8(KKeepAlive, "Keep-Alive");
	iHTTPClient->SetHeaderL(HTTP::EConnection, KKeepAlive); // Not mandatory for HTTP 1.1
	
	iItemsLoadingQueue = RArray<TTile>(20); // ToDo: Move 20 to constant
	
	iImgDecoder = CBufferedImageDecoder::NewL(iFs);
	
	iFileMapper = CFileTreeMapper::NewL(/*cacheDir*/ KNullDesC, 2, 1, ETrue); // Cache dir will be set later
	
	SetSettingsL(aSettings);
	
	iSaver = CTileBitmapSaver::NewL(this);
	// ToDo: Start saver thread only when needed (at first downloaded tile)
	
	CActiveScheduler::Add(this);
	}

void CWebTileProvider::StartDownloadTileL(const TTile &aTile)
	{
	if (iIsOfflineMode)
		return;
	
	if (iState != /*TProcessingState::*/EIdle)
		return;
	
	iState = /*TProcessingState::*/EDownloading;
	iLoadingTile = aTile;
	
	RBuf8 tileUrl;
	const TInt KReserveLength = 30; // For variables substitution
	tileUrl.CreateL(iSettings->iTileUrlTemplate.Length() + KReserveLength);
	tileUrl.CleanupClosePushL();
	iSettings->TileUrl(tileUrl, aTile);
	iHTTPClient->GetL(tileUrl);
	// SetActive()
	DEBUG(_L8("Started download tile %S from url %S"), &aTile.AsDes8(), &tileUrl);
	CleanupStack::PopAndDestroy(&tileUrl);
	}

void CWebTileProvider::AddToDownloadQueue(const TTile &aTile)
	{
	// ToDo: Check array is not full
	iItemsLoadingQueue.Append(aTile);
	DEBUG(_L("Tile %S appended to download queue"), &aTile.AsDes());
	DEBUG(_L("Total %d tiles in download queue"), iItemsLoadingQueue.Count());
	}

void CWebTileProvider::RequestTileL(const TTile &aTile)
	{
	// Try to find on disk first
	if (IsTileFileExists(aTile))
		{
		
		//item->CreateBitmapIfNotExistL();
		TRAPD(r, LoadBitmapL(aTile, iBitmap));
		if (r == KErrNone)
			{
			//item->SetReady();
			INFO(_L("Tile %S readed from file"), &aTile.AsDes());
			iObserver->OnTileLoaded(aTile, iBitmap);
			return;
			}
		else // If read error, try to download
			{
			ERROR(_L("Error while reading %S from file (code: %d)"),
					&aTile.AsDes(), r);
			
			//iWebTileProvider->RequestTileL(aTile);
			}
		
		//LoadBitmapAsync(aTile);
		return;
		}
	else
		{
		DEBUG(_L("Tile %S not found in cache dir"), &aTile.AsDes());
		//// Start download now
		//iWebTileProvider->RequestTileL(aTile);
		}
	
	
	
	
	if (iState == EIdle)
		{ // Start download now
		StartDownloadTileL(aTile);
		}
	else
		{ // Add to loading queue
		AddToDownloadQueue(aTile);
		}
	}

void CWebTileProvider::DoCancel()
	{
	DEBUG(_L("Cancel"));
//	iHTTPClient->CancelRequest();
	iImgDecoder->Cancel();
	//ResetBitmapL();
	}

void CWebTileProvider::RunL()
	{
	DEBUG(_L("CTileBitmapManager::RunL"));
	if (iStatus.Int() == KErrNone)
		{
		/*CFbsBitmap* bitmap;
		TInt r = GetTileBitmap(iLoadingTile, bitmap);
		__ASSERT_DEBUG(r == KErrNone, User::Leave(KErrNotFound));
		CTileBitmapMemCacheItem* item = Find(iLoadingTile);
		__ASSERT_DEBUG(item != NULL, User::Leave(KErrNotFound));*/
		/*CTileBitmapMemCacheItem* item = iBmpMgr->Find(iLoadingTile);
		__ASSERT_DEBUG(item != NULL, Panic(ES60MapsTileBitmapManagerItemNotFoundPanic));
		__ASSERT_DEBUG(item->Bitmap() != NULL, Panic(ES60MapsTileBitmapIsNullPanic));
		
		item->SetReady();*/
		
		INFO(_L("Tile %S downloaded and decoded"), &iLoadingTile.AsDes());
		iObserver->OnTileLoaded(iLoadingTile, iBitmap);
		
		SaveBitmapInBackgroundL(iLoadingTile, /*item->Bitmap()*/ iBitmap);
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

/*TInt CWebTileProvider::RunError(TInt aError)
	{
	return aError;
	}*/

void CWebTileProvider::OnHTTPResponseDataChunkRecieved(
		const RHTTPTransaction aTransaction, const TDesC8 &aDataChunk,
		TInt anOverallDataSize, TBool anIsLastChunk)
	{
	DEBUG(_L("HTTP chunk recieved"));
	
	// Checking that mime-type is PNG
	// (If any error (for example: 404 Not Found) chunk may contains
	// HTML/text data instead correct PNG image. In this case, 
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
	if (fieldVal.StrF() != pngMimeType)
		{
		pngMimeType.Close();
		return; // Skip other types exept PNG
		}
	pngMimeType.Close();
	
	
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

void CWebTileProvider::OnHTTPResponse(const RHTTPTransaction aTransaction)
	{
	DEBUG(_L("HTTP response success"));
	
	iState = /*TProcessingState::*/EDecoding;
	
	// Start convert PNG to CFbsBitmap
	//CFbsBitmap* bitmap;
//	TInt r = GetTileBitmap(iLoadingTile, bitmap);
//	__ASSERT_DEBUG(r != KErrNotFound, User::Leave(KErrNotFound));
	
	ResetBitmapL();
	
	//iImgDecoder->ContinueOpenL();
	DEBUG(_L("Tile %S succesfully downloaded, starting decode"), &iLoadingTile.AsDes());
	iImgDecoder->Convert(&this->iStatus, /**bitmap*/ *iBitmap, 0);
	//iImgDecoder->ContinueConvert(&this->iStatus);
	SetActive();
	}

void CWebTileProvider::OnHTTPError(TInt aError,
		const RHTTPTransaction aTransaction)
	{
	//ERROR(_L("HTTP error: %d"), aError);
	ERROR(_L("Failed to download tile %S, error: %d"), &iLoadingTile.AsDes(), aError);
	iObserver->OnTileLoadingFailed(iLoadingTile, aError);
	
	iImgDecoder->Reset();
	iState = /*TProcessingState::*/EIdle;
	
	
	if (aError == KErrCancel)
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
		}
	else if (aError == KErrAbort) // Request aborted
		{
		INFO(_L("HTTP request cancelled"));
		// No any further action
		}
	else
		{	
		// Start download next tile in queue
		if (iItemsLoadingQueue.Count())
			{
			TTile tile = iItemsLoadingQueue[0]; 
			iItemsLoadingQueue.Remove(0);
			
			StartDownloadTileL(tile);
			}
		}
	}

void CWebTileProvider::OnHTTPHeadersRecieved(
		const RHTTPTransaction aTransaction)
	{
	DEBUG(_L("HTTP headers recieved"));
	
	iImgDecoder->Reset();
	_LIT8(KPNGMimeType, "image/png");
	iImgDecoder->OpenL(KNullDesC8, KPNGMimeType);
	}

void CWebTileProvider::SaveBitmapInBackgroundL(const TTile &aTile, /*const*/ CFbsBitmap *aBitmap)
	{
	iSaver->AppendL(aTile, aBitmap);
	}

TBool CWebTileProvider::IsTileFileExists(const TTile &aTile) /*const*/
	{
	TFileName tileFileName;
	TileFileName(aTile, tileFileName);
	return BaflUtils::FileExists(iFs, tileFileName);
	}

void CWebTileProvider::LoadBitmapL(const TTile &aTile, CFbsBitmap *aBitmap)
	{	
	TFileName tileFileName;
	TileFileName(aTile, tileFileName);
	
	RFile file;
	User::LeaveIfError(file.Open(iFs, tileFileName, EFileRead));
	CleanupClosePushL(file);
	User::LeaveIfError(aBitmap->Load(file));	
	CleanupStack::PopAndDestroy(&file);
	INFO(_L("Bitmap for %S sucessfully loaded from file \"%S\""), &aTile.AsDes(), &tileFileName);
	}

//void CWebTileProvider::LoadBitmapAsync(const TTile &aTile/*, CFbsBitmap *aBitmap*/) /*const*/
//	{
//	TFileName tileFileName;
//	TileFileName(aTile, tileFileName);
//		
//	RFile file;
//	User::LeaveIfError(file.Open(iFs, tileFileName, EFileRead));
//	CleanupClosePushL(file);
//	
//	iBitmap->
//	
//	
//	//User::LeaveIfError(aBitmap->Load(file));	
//	CleanupStack::PopAndDestroy(&file);
//	INFO(_L("Bitmap for %S sucessfully loaded from file \"%S\""), &aTile.AsDes(), &tileFileName);
//	}

void CWebTileProvider::TileFileName(const TTile &aTile, TFileName &aFileName) const
	{
	_LIT(KUnderline, "_");
	_LIT(KMBMExtension, ".mbm");
	
	/*TFileName*/ TBuf<32> originalFileName;
	originalFileName.AppendNum(aTile.iZ);
	originalFileName.Append(KUnderline);
	originalFileName.AppendNum(aTile.iX);
	originalFileName.Append(KUnderline);
	originalFileName.AppendNum(aTile.iY);
	originalFileName.Append(KMBMExtension);
	
	iFileMapper->GetFilePath(originalFileName, aFileName);
	}

void CWebTileProvider::StopAndReset()
	{
	Cancel();	
	iHTTPClient->CancelRequest();
	iItemsLoadingQueue.Reset(); // Should already be cleared by Cancel() call at previous line
	}

void CWebTileProvider::InitializeCacheDirL()
	{
	// Get path to cache directory
	TFileName cacheDir;
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	CS60MapsApplication* app = static_cast<CS60MapsApplication*>(appUi->Application());
	app->CacheDir(cacheDir);
	cacheDir.Append(iSettings->iId);
	cacheDir.Append(KPathDelimiter);
	
	// Create cache directory if not exists
	BaflUtils::EnsurePathExistsL(iFs, cacheDir);
	
	iFileMapper->SetBaseDir(cacheDir);
	}

void CWebTileProvider::SetSettingsL(TWebTileProviderSettings* aSettings)
	{
	// FixMe: On the program startup this method may be called twice with same tile provider 
	
	if (!iSettings)
		{ // First time call (from ConstructL)
		INFO(_L("Tile provider set to %S"), &aSettings->iTitle);
		}
	else if (iSettings->iId == aSettings->iId)
		{ // Exit if nothing changed
		return;
		}
	else
		{
		INFO(_L("Tile provider changed from %S to %S"),
				&iSettings->iTitle, &aSettings->iTitle);
		}
	
	iSettings = aSettings;
	
	StopAndReset();
	InitializeCacheDirL();
	}

void CWebTileProvider::ResetBitmapL()
	{
	// Clear exists bitmap
	if (iBitmap->Handle() != 0)
		{
		iBitmap->Reset();
		}
	
	// Create new bitmap
	TSize size(KTileSize, KTileSize);
	TDisplayMode mode = EColor16M;
	User::LeaveIfError(iBitmap->Create(size, mode));
	}
