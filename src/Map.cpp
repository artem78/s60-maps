/*
 * Map.cpp
 *
 *  Created on: 14.08.2019
 *      Author: artem78
 */

#include "Map.h"
#include <eikenv.h>
#include <imageconversion.h>
#include <e32math.h>
#include "Logger.h"
#include "S60Maps.pan"
#include "S60MapsAppUi.h"
#include <bautils.h>
#include "Defs.h"


// MTileBitmapManagerObserver
void MTileBitmapManagerObserver::OnTileLoadingFailed(const TTile &/*aTile*/, TInt /*aErrCode*/)
	{
	// No any action by default
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
	
	TBuf<64> errMsg;
	MiscUtils::ErrorToDes(aError, errMsg);
	errMsg.Append(' ');
	errMsg.Append('(');
	errMsg.AppendNum(aError);
	errMsg.Append(')');
	Find(iLoadingTile)->SetErrorMsg/*L*/(errMsg);
	
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
	
	TInt statusCode = aTransaction.Response().StatusCode();
	if (statusCode < 400)
		{} // Ok
	else
		{ // Error
		RStringF statusStr = aTransaction.Response().StatusText();
		/*TBuf<256> buf16;
		buf16.Copy(statusStr.DesC());
		DEBUG(_L("Status code: %d"), statusCode);
		DEBUG(_L("Status string: %S"), &buf16);
		buf16.Copy(aTransaction.Request().URI().UriDes());
		DEBUG(_L("URL: %S"), &buf16);*/
		TBuf<64> errMsg;
		errMsg.Copy(statusStr.DesC());
		errMsg.Append(' ');
		errMsg.Append('(');
		errMsg.AppendNum(statusCode);
		errMsg.Append(')');
		Find(iLoadingTile)->SetErrorMsg/*L*/(errMsg);
		
		iObserver->OnTileLoadingFailed(iLoadingTile, statusCode);
		}

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

TBool CTileBitmapManager::HasError(const TTile &aTile)
	{
	CTileBitmapManagerItem* item = Find(aTile);
	if (!item)
		return EFalse;
	else
		return item->HasError();
	};


const HBufC* CTileBitmapManager::ErrMsg(const TTile &aTile)
	{
	CTileBitmapManagerItem* item = Find(aTile);
	if (!item)
		return NULL;
	else
		return item->ErrorMsg();
	}

// CTileBitmapManagerItem

CTileBitmapManagerItem::~CTileBitmapManagerItem()
	{
	delete iErrorMsg;
	
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
		iTile(aTile),
		iState(ENotReady)
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

void CTileBitmapManagerItem::SetErrorMsg/*L*/(const TDesC& aErrMsg)
	{
	if (iErrorMsg) delete iErrorMsg;
	
	iState = EError;
	iErrorMsg = aErrMsg.Alloc/*L*/();
	}

// TTileProvider

TTileProvider::TTileProvider(const TDesC& anId, const TDesC& aTitle,
		const TDesC8& anUrlTemplate, TZoom aMinZoom, TZoom aMaxZoom,
		const TDesC& aCopyrightTextShort, /*const TDesC& aCopyrightText,*/
		const TDesC& aCopyrightUrl)
	{
	iId.Copy(anId);
	iTitle.Copy(aTitle);
	iTileUrlTemplate.Copy(anUrlTemplate);
	iMinZoomLevel = aMinZoom;
	iMaxZoomLevel = aMaxZoom;
	iCopyrightTextShort = aCopyrightTextShort;
	/*if (aCopyrightText.Length() == 0 && aCopyrightTextShort.Length() > 0)
		{*/
		iCopyrightText = aCopyrightTextShort;
		/*}
	else
		{
		iCopyrightText = aCopyrightText;
		}*/
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
