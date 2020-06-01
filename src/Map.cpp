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
#include "S60Maps.pan"

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
	delete iTileProvider;
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
	iTileProvider = new (ELeave) TOsmStandardTileProvider;
	TBuf<32> tileProviderID;
	iTileProvider->ID(tileProviderID);
	
	TFileName cacheDir;
	//////////
	// ToDo: Get base cache dir from CS60MapsApplication::CacheDir()
#ifdef __WINSCW__
	cacheDir.Append(_L("c:\\data\\s60maps\\cache\\"));
#else
	cacheDir.Append(_L("e:\\data\\s60maps\\cache\\"));
#endif
	/////////
	cacheDir.Append(tileProviderID);
	cacheDir.Append(KPathDelimiter);
	
	RFs fs = iMapView->ControlEnv()->FsSession();
	
	// Create cache dir (if not exists)
	TInt r = fs.MkDirAll(cacheDir);
	if (r != KErrAlreadyExists)
		User::LeaveIfError(r);
	
	iBitmapMgr = CTileBitmapManager::NewL(this, fs, iTileProvider, cacheDir);
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

CTileBitmapManager::CTileBitmapManager(MTileBitmapManagerObserver *aObserver,
		RFs aFs, TTileProviderBase* aTileProvider, TInt aLimit) :
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
	delete iTileProvider;
	iItemsLoadingQueue.Close();
	iItems.ResetAndDestroy();
	iItems.Close();
	delete iHTTPClient;
	}

CTileBitmapManager* CTileBitmapManager::NewLC(MTileBitmapManagerObserver *aObserver,
		RFs aFs, TTileProviderBase* aTileProvider, const TDesC &aCacheDir, TInt aLimit)
	{
	CTileBitmapManager* self = new (ELeave) CTileBitmapManager(aObserver, aFs, aTileProvider, aLimit);
	CleanupStack::PushL(self);
	self->ConstructL(aCacheDir);
	return self;
	}

CTileBitmapManager* CTileBitmapManager::NewL(MTileBitmapManagerObserver *aObserver,
		RFs aFs, TTileProviderBase* aTileProvider, const TDesC &aCacheDir, TInt aLimit)
	{
	CTileBitmapManager* self = CTileBitmapManager::NewLC(aObserver, aFs, aTileProvider, aCacheDir, aLimit);
	CleanupStack::Pop(); // self;
	return self;
	}

void CTileBitmapManager::ConstructL(const TDesC &aCacheDir)
	{
#ifdef __WINSCW__
	// Add some delay for network services have been started on the emulator,
	// otherwise CEcmtServer: 3 panic will be raised.
	User::After(10 * 1000 * 1000);
#endif
	iHTTPClient = CHTTPClient::NewL(this);
	iHTTPClient->SetUserAgentL(_L8("S60Maps")); // ToDo: Move to constant
	
	iItems = RPointerArray<CTileBitmapManagerItem>(iLimit);
	iItemsLoadingQueue = RArray<TTile>(20); // ToDo: Move 20 to constant

	iCacheDir.Copy(aCacheDir);
	
	iImgDecoder = CBufferedImageDecoder::NewL(iFs);
	
	CActiveScheduler::Add(this);
	}

TInt CTileBitmapManager::GetTileBitmap(const TTile &aTile, CFbsBitmap* &aBitmap)
	{
	CTileBitmapManagerItem* item = Find(aTile);
	LOG(_L8("tile=%S, item=%x"), &aTile.AsDes8(), item);
	
	if (item == NULL)
		return KErrNotFound;
	LOG(_L8("tile=%S, isready=%d, bitmap=%x"), &aTile.AsDes8(), item->IsReady(), item->Bitmap());
	
	if (!item->IsReady())
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
		LOG(_L8("Delete old bitmap of %S from cache"), &iItems[0]->Tile().AsDes8());
		delete iItems[0];
		iItems.Remove(0);
		}
	
	// Add new one
	CTileBitmapManagerItem* item = CTileBitmapManagerItem::NewL(aTile/*, iObserver*/);
	iItems.Append(item);
	
	if (iState == EIdle)
		{
		// Try to find on disk first
		if (IsTileFileExist(aTile))
			{
			item->CreateBitmapIfNotExistL();
			LoadBitmapL(aTile, item->Bitmap());
			item->SetReady();
			}
		else
			// Start download now
			StartDownloadTileL(aTile);
		}
	else
		{
		// Add to loading queue
		// ToDo: Check array is not full
		iItemsLoadingQueue.Append(aTile);
		LOG(_L8("Tile %S appended to download queue"), &aTile.AsDes8());
		LOG(_L8("Total %d tiles in download queue"), iItemsLoadingQueue.Count());
		}
	LOG(_L8("Now %d items in bitmap cache"), iItems.Count());
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

void CTileBitmapManager::StartDownloadTileL(const TTile &aTile)
	{
	if (iState != /*TProcessingState::*/EIdle)
		return;
	
	iState = /*TProcessingState::*/EDownloading;
	iLoadingTile = aTile;
	
	TBuf8<100> tileUrl;
	iTileProvider->TileUrl(tileUrl, aTile);
	iHTTPClient->GetL(tileUrl);
	LOG(_L8("Started download tile %S from url %S"), &aTile.AsDes8(), &tileUrl);
	}

void CTileBitmapManager::DoCancel()
	{
	iImgDecoder->Cancel();
	}

void CTileBitmapManager::RunL()
	{
	LOG(_L8("CTileBitmapManager::RunL"));
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
		
#ifdef _DEBUG
		item->DrawTileBorderAndNumbersL();
#endif
		
		item->SetReady();
		
		LOG(_L8("Tile %S downloaded and decoded"), &iLoadingTile.AsDes8());
		iObserver->OnTileLoaded(iLoadingTile, item->Bitmap());
		
		SaveBitmapL(iLoadingTile, item->Bitmap());
		}
	else
		{
		LOG(_L8("Image decoding error: %d"), iStatus.Int());
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
		TInt anOverallDataSize, TBool anIsLastChunk)
	{
	LOG(_L8("HTTP chunk recieved"));
	
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
		return; // Skip other types exept PNG
	
	
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

void CTileBitmapManager::OnHTTPResponse(const RHTTPTransaction aTransaction)
	{
	LOG(_L8("HTTP response success"));
	
	iState = /*TProcessingState::*/EDecoding;
	
	// Start convert PNG to CFbsBitmap
	//CFbsBitmap* bitmap;
//	TInt r = GetTileBitmap(iLoadingTile, bitmap);
//	__ASSERT_DEBUG(r != KErrNotFound, User::Leave(KErrNotFound));
	CTileBitmapManagerItem* item = Find(iLoadingTile);
	__ASSERT_DEBUG(item != NULL, Panic(ES60MapsTileBitmapManagerItemNotFoundPanic));
	item->CreateBitmapIfNotExistL();
	__ASSERT_DEBUG(item->Bitmap() != NULL, Panic(ES60MapsTileBitmapIsNullPanic));
	
	//iImgDecoder->ContinueOpenL();
	LOG(_L8("Tile %S succesfully downloaded, starting decode"), &iLoadingTile.AsDes8());
	iImgDecoder->Convert(&this->iStatus, /**bitmap*/ *item->Bitmap(), 0);
	//iImgDecoder->ContinueConvert(&this->iStatus);
	SetActive();
	}

void CTileBitmapManager::OnHTTPError(TInt aError,
		const RHTTPTransaction aTransaction)
	{
	//LOG(_L8("HTTP error: %d"), aError);
	LOG(_L8("Failed to download tile %S, error: %d"), &iLoadingTile.AsDes8(), aError);
	iObserver->OnTileLoadingFailed(iLoadingTile, aError);
	
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
void CTileBitmapManager::OnHTTPHeadersRecieved(
		const RHTTPTransaction aTransaction)
	{
	LOG(_L8("HTTP headers recieved"));
	
	iImgDecoder->Reset();
	_LIT8(KPNGMimeType, "image/png");
	iImgDecoder->OpenL(KNullDesC8, KPNGMimeType);
	}

void CTileBitmapManager::SaveBitmapL(const TTile &aTile, /*const*/ CFbsBitmap *aBitmap/*, TBool aRewrite*/)
	{
	TFileName tileFileName;
	TileFileName(aTile, tileFileName);
	
	RFile file;
	/*if (aRewrite)
		{*/
		User::LeaveIfError(file.Replace(iFs, tileFileName, EFileWrite));
		CleanupClosePushL(file);
	/*	}
	else
		{
		TInt r = file.Create(iFs, tileFileName, EFileWrite);
		CleanupClosePushL(file);
		if (r != KErrAlreadyExists)
			User::LeaveIfError(r);
		}*/
	User::LeaveIfError(aBitmap->Save(file));	
	CleanupStack::PopAndDestroy(&file);
	LOG(_L8("Bitmap for %S sucessfully saved to file \"%S\""), &aTile.AsDes8(), &tileFileName);
	}

void CTileBitmapManager::LoadBitmapL(const TTile &aTile, CFbsBitmap *aBitmap)
	{	
	TFileName tileFileName;
	TileFileName(aTile, tileFileName);
	
	RFile file;
	User::LeaveIfError(file.Open(iFs, tileFileName, EFileRead));
	CleanupClosePushL(file);
	User::LeaveIfError(aBitmap->Load(file));	
	CleanupStack::PopAndDestroy(&file);
	LOG(_L8("Bitmap for %S sucessfully loaded from file \"%S\""), &aTile.AsDes8(), &tileFileName);
	}

TBool CTileBitmapManager::IsTileFileExist(const TTile &aTile) /*const*/
	{
	TFileName tileFileName;
	TileFileName(aTile, tileFileName);
	
	// ToDo: Can we make this check without opening file?
	
	RFile file;
	TInt r = file.Open(iFs, tileFileName, EFileRead);
	if (r == KErrNone)
		file.Close();
	
	return r == KErrNone;
	}

void CTileBitmapManager::TileFileName(const TTile &aTile, TFileName &aFileName) const
	{
	_LIT(KUnderline, "_");
	_LIT(KMBMExtension, ".mbm");
	
	aFileName.Copy(iCacheDir);
	aFileName.AppendNum(aTile.iZ);
	aFileName.Append(KUnderline);
	aFileName.AppendNum(aTile.iX);
	aFileName.Append(KUnderline);
	aFileName.AppendNum(aTile.iY);
	aFileName.Append(KMBMExtension);
	}


// CTileBitmapManagerItem

CTileBitmapManagerItem::~CTileBitmapManagerItem()
	{
	// FixMe: Bitmap pointer maybe still used outside when deleting
	delete iBitmap; // iBitmap already may be NULL
	
	if (iBitmap != NULL)
		LOG(_L8("Bitmap of %S destroyed"), &iTile.AsDes8());
	
	LOG(_L8("Bitmap manager item of %S destroyed"), &iTile.AsDes8());
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
	LOG(_L8("Bitmap manager item of %S created"), &self->iTile.AsDes8());
	return self;
	}

CTileBitmapManagerItem::CTileBitmapManagerItem(const TTile &aTile) :
		iTile(aTile)
	{
	// No implementation required
	}

void CTileBitmapManagerItem::ConstructL()
	{
	// Second phase construction is not used at the moment
	}

#ifdef _DEBUG
void CTileBitmapManagerItem::DrawTileBorderAndNumbersL()
	{
	//LOG(_L8("Start drawing bitmap of %S"), &iTile.AsDes8());
	
	CreateBitmapIfNotExistL();
	
	CFbsBitmapDevice* bdev = CFbsBitmapDevice::NewL(iBitmap);
	CleanupStack::PushL(bdev);
	
	CFbsBitGc* bgc = CFbsBitGc::NewL();
	CleanupStack::PushL(bgc);	
	bgc->Activate(bdev);
	
	TRect rect = TRect(iBitmap->SizeInPixels());
	
	// Draw tiles`s boundary
	bgc->SetPenColor(KRgbDarkBlue);
	bgc->SetPenStyle(CGraphicsContext::EDottedPen);
	bgc->SetPenSize(TSize(1, 1));
	bgc->SetBrushStyle(CGraphicsContext::ENullBrush);
	bgc->DrawRect(rect);
	
	// Draw tile`s numbers
	_LIT(KTextFormat, "x=%d y=%d z=%d");
	TBuf<30> buff;
	buff.Format(KTextFormat, iTile.iX, iTile.iY, iTile.iZ);
	const CFont* font = CEikonEnv::Static()->SymbolFont();
	bgc->UseFont(font);
	TInt baseline = rect.Height() / 2 + font->AscentInPixels() / 2;
	bgc->DrawText(buff, rect, baseline, CGraphicsContext::ECenter);
	bgc->DiscardFont();
	
	CleanupStack::PopAndDestroy(2, bdev);
	
	//LOG(_L8("End drawing bitmap of %S"), &iTile.AsDes8());
	}
#endif

void CTileBitmapManagerItem::CreateBitmapIfNotExistL()
	{
	if (iBitmap != NULL)
		return;
	
	iBitmap = new (ELeave) CFbsBitmap();
	TSize size(256, 256);
	TDisplayMode mode = EColor16M;
	User::LeaveIfError(iBitmap->Create(size, mode));
	}


// OsmStandardTileProvider

void TOsmStandardTileProvider::ID(TDes &aDes)
	{
	_LIT(KProviderID, "osm");
	aDes.Copy(KProviderID);
	}

void TOsmStandardTileProvider::Title(TDes &aDes)
	{
	_LIT(KProviderTitle, "OpenStreetMap");
	aDes.Copy(KProviderTitle);
	}

void TOsmStandardTileProvider::TileUrl(TDes8 &aUrl, const TTile &aTile)
	{
	_LIT8(KUrlFmt, "http://%c.tile.openstreetmap.org/%u/%u/%u.png");
	TChar chr('a');
	chr += Math::Random() % 3; // a-c
	aUrl.Format(KUrlFmt, (TUint) chr, (TUint) aTile.iZ, aTile.iX, aTile.iY);
	}
