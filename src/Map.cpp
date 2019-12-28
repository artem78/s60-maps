/*
 * Map.cpp
 *
 *  Created on: 14.08.2019
 *      Author: user
 */

#include "Map.h"
#include "S60MapsAppView.h"
#include <eikenv.h>
#include <gdi.h>
#include <imageconversion.h>
#include <e32math.h>
#include <bitstd.h>
#include "Logger.h"

CMapLayerBase::CMapLayerBase(/*const*/ CS60MapsAppView* aMapView) :
		iMapView(aMapView)
	{
	}

const TInt KStubImageSize = 256;

//CMapLayerStub::CMapLayerStub(/*const*/ CS60MapsAppView* aMapView, RFs &aFs) :
//		CMapLayerBase(aMapView),
//		iFs(aFs)
//	{
//	iCoords[0] = TCoordinate(40.7283,-73.9942); // New York
//	iCoords[1] = TCoordinate(51.5072, -0.1275); // London
//	iCoords[2] = TCoordinate(-34.9333, 138.5833); // Adelaide 
//	iCoords[3] = TCoordinate(48.8333, 2.3333); // Paris 
//	iCoords[4] = TCoordinate(55.7558, 37.6178); // Moscow
//	iCoords[5] = TCoordinate(0, 0); // 0, 0*/
//	}
//
//CMapLayerStub::~CMapLayerStub()
//	{
//	delete iBackgroundBitmap;
//	delete iImgReader;
//	}
//
//CMapLayerStub* CMapLayerStub::NewLC(/*const*/ CS60MapsAppView* aMapView, RFs &aFs)
//	{
//	CMapLayerStub* self = new (ELeave) CMapLayerStub(aMapView, aFs);
//	CleanupStack::PushL(self);
//	self->ConstructL();
//	return self;
//	}
//
//CMapLayerStub* CMapLayerStub::NewL(/*const*/ CS60MapsAppView* aMapView, RFs &aFs)
//	{
//	CMapLayerStub* self = CMapLayerStub::NewLC(aMapView, aFs);
//	CleanupStack::Pop(); // self;
//	return self;
//	}
//
//void CMapLayerStub::ConstructL()
//	{
//	iBackgroundBitmap = new (ELeave) CFbsBitmap();
//	/*TSize size(343, 344);
//	TDisplayMode mode = EColor16M;
//	User::LeaveIfError(iBackgroundBitmap->Create(size, mode));*/
//	LoadBackgroundImageL();
//	}
//
//void CMapLayerStub::LoadBackgroundImageL()
//	{
//	_LIT(KImagePath, "c:\\map_stub.png");
//	//_LIT(KImageMime, "image/png");
//	
//	TSize size(KStubImageSize, KStubImageSize);
//	TDisplayMode mode = EColor16M;
//	User::LeaveIfError(iBackgroundBitmap->Create(size, mode));
//	
//	iImgReader = CImageReader::NewL(this, iBackgroundBitmap,
//			KImagePath, iFs);
//	iImgReader->StartRead();
//	}
//
//void CMapLayerStub::OnImageReaded()
//	{
//	iMapView->DrawNow();
//	}
//
//void CMapLayerStub::Draw(CWindowGc &aGc)
//	{
//	DrawMap(aGc);
//	DrawMarks(aGc);
//	}
//
//
//void CMapLayerStub::DrawMap(CWindowGc &aGc)
//	{
//	TCoordinate imageTopLeftCoord(KMaxLatitudeMapBound, KMinLongitudeMapBound);
//	TCoordinate imageBottomRightCoord(KMinLatitudeMapBound, KMaxLongitudeMapBound);
//	TPoint topLeft;
//	TPoint bottomRight;
//	if (iMapView->CheckCoordVisibility(imageTopLeftCoord))
//		topLeft = iMapView->GeoCoordsToScreenCoords(imageTopLeftCoord);
//	else
//		topLeft = iMapView->Rect().iTl;
//	if (iMapView->CheckCoordVisibility(imageBottomRightCoord))
//		bottomRight = iMapView->GeoCoordsToScreenCoords(imageBottomRightCoord);
//	else
//		bottomRight = iMapView->Rect().iBr;
//	TRect destRect = TRect(topLeft, bottomRight);
//	
//	
//	TCoordinate viewTopLeftCoord = iMapView->ScreenCoordsToGeoCoords(iMapView->Rect().iTl);
//	TCoordinate viewBottomRightCoord = iMapView->ScreenCoordsToGeoCoords(iMapView->Rect().iBr);
//	TTileReal topLeftTileReal = MapMath::GeoCoordsToTileReal(viewTopLeftCoord, 0);
//	TTileReal bottomRightTileReal = MapMath::GeoCoordsToTileReal(viewBottomRightCoord, 0);
//	TReal srcRectTlX = topLeftTileReal.iX * KStubImageSize;
//	Math::Round(srcRectTlX, srcRectTlX, 0); // ToDo: Check possible errors
//	TReal srcRectTlY = topLeftTileReal.iY * KStubImageSize;
//	Math::Round(srcRectTlY, srcRectTlY, 0); // ToDo: Check possible errors
//	TReal srcRectBrX = bottomRightTileReal.iX * KStubImageSize;
//	Math::Round(srcRectBrX, srcRectBrX, 0); // ToDo: Check possible errors
//	TReal srcRectBrY = bottomRightTileReal.iY * KStubImageSize;
//	Math::Round(srcRectBrY, srcRectBrY, 0); // ToDo: Check possible errors
//	TRect srcRect = TRect(
//		srcRectTlX,
//		srcRectTlY,
//		srcRectBrX,
//		srcRectBrY
//	);
//	
//	
//	aGc.DrawBitmap(destRect, iBackgroundBitmap, srcRect);
//	}
//
//void CMapLayerStub::DrawMarks(CWindowGc &aGc)
//	{
//	const TInt KMarkSize = 5;
//	TSize penSize(KMarkSize, KMarkSize);
//	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
//	aGc.SetPenColor(KRgbRed);
//	aGc.SetPenSize(penSize);
//	
//	TInt i;
//	for (i = 0; i < iCoords.Count(); i++)
//		{
//		TCoordinate coord = iCoords[i];
//		if (iMapView->CheckCoordVisibility(coord)) // Check before drawing
//			{
//			TPoint point;
//			point = iMapView->GeoCoordsToScreenCoords(coord);
//			aGc.Plot(point);
//			}
//		}
//	}


// CMapLayerDebugInfo

CMapLayerDebugInfo::CMapLayerDebugInfo(/*const*/ CS60MapsAppView* aMapView) :
	CMapLayerBase(aMapView)
	{
	/*CWindowGc& gc = iMapView->SystemGc();
	gc.UseFont(CEikonEnv::Static()->AnnotationFont());*/
	}

/*CMapLayerDebugInfo::~CMapLayerDebugInfo()
	{
	CWindowGc& gc = iMapView->SystemGc();
	gc.DiscardFont();
	}*/

void CMapLayerDebugInfo::Draw(CWindowGc &aGc)
	{
	TBuf<100> buff;
	TCoordinate center = iMapView->GetCenterCoordinate();
	_LIT(KInfoText, "pos: %f %f   zoom: %d");
	buff.Format(KInfoText, center.Latitude(), center.Longitude(), (TInt) iMapView->GetZoom());
	
	_LIT(KFontName, "Series 60 Sans");
	TFontSpec fontSpec(KFontName, 100);
	CFont* font;
	TInt res = CEikonEnv::Static()->ScreenDevice()->GetNearestFontInTwips(font, fontSpec);
	if (res)
		return;
	aGc.UseFont(font);
	TRect area = iMapView->Rect();
	area.Shrink(4, 4);
	TInt baselineOffset = area.Height() - font->AscentInPixels();
	aGc.DrawText(buff, area, baselineOffset);
	aGc.DiscardFont();
	CEikonEnv::Static()->ScreenDevice()->ReleaseFont(font);
	};


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
	delete iBitmapMgr;
	}

CTiledMapLayer* CTiledMapLayer::NewL(CS60MapsAppView* aMapView)
	{
	CTiledMapLayer* self = CTiledMapLayer::NewLC(aMapView);
	CleanupStack::Pop(); // self;
	return self;
	}

CTiledMapLayer* CTiledMapLayer::NewLC(CS60MapsAppView* aMapView)
	{
	CTiledMapLayer* self = new (ELeave) CTiledMapLayer(aMapView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CTiledMapLayer::ConstructL()
	{
	iBitmapMgr = CTileBitmapManager::NewL(this);
	}

void CTiledMapLayer::Draw(CWindowGc &aGc)
	{
	LOG(_L8("Begin layer drawing"));
	
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
				break;
				}
				
			/*default:
				break;*/
			}
		
		}
	
	tiles.Close();
	LOG(_L8("End layer drawing"));
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
	destRect.SetSize(TSize(256, 256));
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


// CImageReader

CImageReader::CImageReader(MImageReaderObserver* aObserver, CFbsBitmap* aBitmap) :
	CActive(EPriorityStandard),
	iObserver(aObserver),
	iBitmap(aBitmap)
	{
	}

CImageReader* CImageReader::NewLC(MImageReaderObserver* aObserver,
		CFbsBitmap* aBitmap, const TDesC &aFilePath, RFs &aFs)
	{
	CImageReader* self = new (ELeave) CImageReader(aObserver, aBitmap);
	CleanupStack::PushL(self);
	self->ConstructL(aFilePath, aFs);
	return self;
	}

CImageReader* CImageReader::NewL(MImageReaderObserver* aObserver,
		CFbsBitmap* aBitmap, const TDesC &aFilePath, RFs &aFs)
	{
	CImageReader* self = CImageReader::NewLC(aObserver, aBitmap, aFilePath, aFs);
	CleanupStack::Pop();
	return self;
	}

void CImageReader::ConstructL(const TDesC &aFilePath, RFs &aFs)
	{
	//iFilePath = aFilePath.AllocL();
	iDecoder = CImageDecoder::FileNewL(aFs, aFilePath);	
	CActiveScheduler::Add(this);
	}

CImageReader::~CImageReader()
	{
	Cancel();
	delete iDecoder;
	}

void CImageReader::DoCancel()
	{
	iDecoder->Cancel();
	}

void CImageReader::StartRead()
	{
	Cancel();
	iDecoder->Convert(&iStatus, *iBitmap, 0);
	SetActive();
	}

void CImageReader::RunL()
	{
	switch (iStatus.Int())
		{
		case KErrNone:
			iObserver->OnImageReaded();
			break;
			
		default:
			iObserver->OnImageReadingFailed(iStatus.Int());
			break;
		}
	}

//TInt CImageReader::RunError(TInt aError)
//	{
//	return aError;
//	}


// MImageReaderObserver

void MImageReaderObserver::OnImageReadingFailed(TInt /*aErr*/)
	{
	// No any action by default
	}


// CTileBitmapManager

CTileBitmapManager::CTileBitmapManager(MTileBitmapManagerObserver *aObserver, TInt aLimit) :
		iObserver(aObserver),
		iLimit(aLimit)
	{
	// No implementation required
	}

CTileBitmapManager::~CTileBitmapManager()
	{
	iItems.ResetAndDestroy();
	iItems.Close();
	}

CTileBitmapManager* CTileBitmapManager::NewLC(MTileBitmapManagerObserver *aObserver, TInt aLimit)
	{
	CTileBitmapManager* self = new (ELeave) CTileBitmapManager(aObserver, aLimit);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTileBitmapManager* CTileBitmapManager::NewL(MTileBitmapManagerObserver *aObserver, TInt aLimit)
	{
	CTileBitmapManager* self = CTileBitmapManager::NewLC(aObserver, aLimit);
	CleanupStack::Pop(); // self;
	return self;
	}

void CTileBitmapManager::ConstructL()
	{
	iItems = RPointerArray<CTileBitmapManagerItem>(iLimit);
	}

TInt CTileBitmapManager::GetTileBitmap(const TTile &aTile, CFbsBitmap* &aBitmap)
	{
	CTileBitmapManagerItem* item = Find(aTile);
	
	if (item == NULL)
		return KErrNotFound;
	
	if (item->Bitmap() == NULL)
		return KErrNotReady;
	
	aBitmap = item->Bitmap();
	return KErrNone;
	}

void CTileBitmapManager::AddToLoading(const TTile &aTile)
	{
	CTileBitmapManagerItem* item = Find(aTile);
	
	if (item == NULL)
		Append(aTile);
	}

/*TInt*/ void CTileBitmapManager::Append/*L*/(const TTile &aTile)
	{
	if (iItems.Count() >= iLimit)
		{
		// Delete oldest item
		delete iItems[0];
		iItems.Remove(0);
		}
	
	// Add new one
	CTileBitmapManagerItem* item = CTileBitmapManagerItem::NewL(aTile, iObserver);
	iItems.Append(item);
	}

CTileBitmapManagerItem* CTileBitmapManager::Find(const TTile &aTile) const
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


// CTileBitmapManagerItem

CTileBitmapManagerItem::~CTileBitmapManagerItem()
	{
	Cancel();
	iTimer.Close();

	// FixMe: Bitmap pointer maybe still used outside when deleting
	delete iBitmap; // iBitmap may be NULL
	
	LOG(_L8("Item with %S destroyed"), &iTile.AsDes8());
	}

CTileBitmapManagerItem* CTileBitmapManagerItem::NewL(const TTile &aTile, MTileBitmapManagerObserver *aObserver)
	{
	CTileBitmapManagerItem* self = CTileBitmapManagerItem::NewLC(aTile, aObserver);
	CleanupStack::Pop(); // self;
	return self;
	}

CTileBitmapManagerItem* CTileBitmapManagerItem::NewLC(const TTile &aTile, MTileBitmapManagerObserver *aObserver)
	{
	CTileBitmapManagerItem* self = new (ELeave) CTileBitmapManagerItem(aTile, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	LOG(_L8("Item with %S created"), &self->iTile.AsDes8());
	return self;
	}

CTileBitmapManagerItem::CTileBitmapManagerItem(const TTile &aTile, MTileBitmapManagerObserver *aObserver) :
		CActive(EPriorityStandard),
		iTile(aTile),
		iObserver(aObserver)
	{
	// No implementation required
	}

void CTileBitmapManagerItem::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
	
	StartLoadL(); // Start load bitmap just after object has been created
	}

void CTileBitmapManagerItem::RunL()
	{
	switch (iStatus.Int())
		{
		case KErrNone:
			{
			TRAPD(r, DrawStubBitmapL());
			
			if (r == KErrNone)
				{
				LOG(_L8("%S bitmap successfuly loaded"), &iTile.AsDes8());
				iObserver->OnTileLoaded(iTile, iBitmap);
				}
			else
				{
				LOG(_L8("%S bitmap loading failed (error=%d)"), &iTile.AsDes8(), r);
				iObserver->OnTileLoadingFailed(iTile, r);
				}
			break;
			}
	
		case KErrCancel:
			{
			LOG(_L8("%S bitmap loading cancelled"), &iTile.AsDes8());
			break;
			}
			
		default:
			{
			LOG(_L8("%S bitmap loading failed (active object, error=%d)"), &iTile.AsDes8(), iStatus.Int());
			break;
			}
		}
	}

void CTileBitmapManagerItem::DoCancel()
	{
	iTimer.Cancel();
	}

//TInt CTileBitmapManagerItem::RunError(TInt aError)
//	{
//	return aError;
//	}

void CTileBitmapManagerItem::StartLoadL()
	{
	LOG(_L8("Start loading item with %S"), &iTile.AsDes8());
	
	Cancel();
	
	// Imitate long task
	TInt delay = Math::Random() % 3000000 + 80000; // 0,8 - 3.8 sec.
	iTimer.After(iStatus, delay);
	
	SetActive();
	}

void CTileBitmapManagerItem::DrawStubBitmapL()
	{
	LOG(_L8("Start drawing bitmap of %S"), &iTile.AsDes8());
	
	CreateBitmapIfNotExistL();
	
	CFbsBitmapDevice* bdev = CFbsBitmapDevice::NewL(iBitmap);
	CleanupStack::PushL(bdev);
	
	CFbsBitGc* bgc = CFbsBitGc::NewL();
	CleanupStack::PushL(bgc);	
	bgc->Activate(bdev);
	
	TRect rect = TRect(iBitmap->SizeInPixels());
	
	// Background and border
	bgc->SetPenColor(KRgbDarkBlue);
	bgc->SetPenStyle(CGraphicsContext::ENullPen);
	bgc->SetBrushColor(KRgbDarkBlue);
	bgc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bgc->DrawRect(rect);
	rect.Shrink(1, 1);
	bgc->SetBrushColor(TRgb(226, 238, 253));
	bgc->DrawRect(rect);
	
	// Text
	_LIT(KStubTileTextFormat, "x=%d y=%d z=%d");
	TBuf<30> buff;
	buff.Format(KStubTileTextFormat, iTile.iX, iTile.iY, iTile.iZ);
	const CFont* font = CEikonEnv::Static()->SymbolFont();
	bgc->UseFont(font);
	TInt baseline = rect.Height() / 2 + font->AscentInPixels() / 2;
	bgc->DrawText(buff, rect, baseline, CGraphicsContext::ECenter);
	bgc->DiscardFont();
	
	CleanupStack::PopAndDestroy(2, bdev);
	
	LOG(_L8("End drawing bitmap of %S"), &iTile.AsDes8());
	}

void CTileBitmapManagerItem::CreateBitmapIfNotExistL()
	{
	if (iBitmap != NULL)
		return;
	
	iBitmap = new (ELeave) CFbsBitmap();
	TSize size(256, 256);
	TDisplayMode mode = EColor16M;
	User::LeaveIfError(iBitmap->Create(size, mode));
	}

inline TTile CTileBitmapManagerItem::Tile() const
	{
	return iTile;
	}

inline CFbsBitmap* CTileBitmapManagerItem::Bitmap() const
	{
	return iBitmap;
	}
