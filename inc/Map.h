/*
 * Map.h
 *
 *  Created on: 14.08.2019
 *      Author: artem78
 */

#ifndef MAP_H_
#define MAP_H_

#include <w32std.h>
#include <f32file.h>
#include <fbs.h>
#include <imageconversion.h>
#include "MapMath.h"		// For TTile
#include <e32base.h>
#include "HttpClient2.h"
#include "FileUtils.h"
#include <e32msgqueue.h>


// Forward declaration
class CTileBitmapManager;


class MTileBitmapManagerObserver
	{
public:
	virtual void OnTileLoaded() = 0;
	virtual void OnTileLoadingFailed();
	};


class TSaverQueryItem
	{
public:
	TTile iTile;
	CFbsBitmap *iBitmap;
	
	TBool iShouldStop; // If true, thread processing will be stopped on this item
	};


// Class for save tile bitmaps in separate thread
// for improve bad performance on some phones

class CTileDiskCache;
class CTileBitmapSaver: public CBase
	{
	// Constructors/destructors
public:
	~CTileBitmapSaver();
	static CTileBitmapSaver* NewL(CTileDiskCache* aDiskCache);
	static CTileBitmapSaver* NewLC(CTileDiskCache* aDiskCache);

private:
	CTileBitmapSaver(CTileDiskCache* aDiskCache);
	void ConstructL();
	
	// Custom
public:
	void AppendL(const TTile &aTile, CFbsBitmap *aBitmap);
	
private:
	CTileDiskCache* iDiskCache;
	RMsgQueue<TSaverQueryItem> iQueue;
	TInt iItemsInQueue;
	TThreadId iThreadId;
	
	static TInt ThreadFunction(TAny* anArg);
	void SaveL(const TSaverQueryItem &anItem, RFs &aFs);
	};


class CTileBitmapMemCacheItem;

/*	Stores in memory bitmaps for tiles. When count of stored bitmaps
	reach maximum limit, oldest one will be deleted before insert new. */
class CTileBitmapMemCache: public CBase
	{
	// Constructor & Destructor
public:
	CTileBitmapMemCache(TInt aLimit);
	~CTileBitmapMemCache();
	
	// New
private:
	TInt iLimit;
	RPointerArray<CTileBitmapMemCacheItem> iItems;
	void /*DeleteItem()*/ DeleteLeastUsedItem(); // Deletes one most rarely used item
	
public:
	CTileBitmapMemCacheItem* Find(const TTile &aTile) const;
	///*TInt*/ void Append/*L*/(const TTile &aTile);
	CTileBitmapMemCacheItem* Append/*L*/(const TTile &aTile);
	// @return Error codes: KErrNotFound, KErrNotReady or KErrNone
	TInt GetTileBitmap(const TTile &aTile, CFbsBitmap* &aBitmap);
	void Delete(const TTile &aTile);
	TBool HasError(const TTile &aTile);
	const HBufC* ErrMsg(const TTile &aTile);
	inline void /*Reset*/Clear()
		{ iItems.ResetAndDestroy(); };
	};


class TTileProvider;
class CTileDiskCache;

class CTileBitmapManager : public CActive, public MHTTPClientObserver
// todo: rename to CTileDownloader in the future
	{
// Base methods
public:
	~CTileBitmapManager();
	static CTileBitmapManager* NewL(MTileBitmapManagerObserver *aObserver,
			RFs aFs, TTileProvider* aTileProvider, const TDesC &aCacheDir);
	static CTileBitmapManager* NewLC(MTileBitmapManagerObserver *aObserver,
			RFs aFs, TTileProvider* aTileProvider, const TDesC &aCacheDir);

private:
	CTileBitmapManager(MTileBitmapManagerObserver *aObserver,
			TTileProvider* aTileProvider);
	void ConstructL(const TDesC &aCacheDir, RFs aFs);
	
// From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

// From MHTTPClientObserver
public:
	virtual void OnHTTPResponseDataChunkRecieved(const RHTTPTransaction aTransaction,
			const TDesC8 &aDataChunk, TInt anOverallDataSize, TBool anIsLastChunk);
	virtual void OnHTTPResponse(const RHTTPTransaction aTransaction);
	virtual void OnHTTPError(TInt aError, const RHTTPTransaction aTransaction);
	virtual void OnHTTPHeadersRecieved(const RHTTPTransaction aTransaction);
	
// Custom properties and methods
private:
	MTileBitmapManagerObserver* iObserver;
	CTileBitmapMemCache* iBmpMemCache;
	/*TInt*/ void Append/*L*/(const TTile &aTile); 
	
	RArray<TTile> /*iItemsForLoading*/ iItemsLoadingQueue;
	CHTTPClient2* iHTTPClient;
	TTileProvider* iTileProvider;
	//TBool iIsLoading;
	enum TProcessingState
		{
		EIdle,
		EDownloading,
		EDecoding
		};
	TProcessingState iState;
	CBufferedImageDecoder* iImgDecoder;
	TTile iLoadingTile;
	TBool iIsOfflineMode;
	CTileDiskCache* iDiskCache;
	
	void StartDownloadTileL(const TTile &aTile);
	void GoToNextTileInQueueL();
	void SetErrorForProcessingTile/*L*/(const TDesC &aErrMsg);
	void SetErrorForProcessingTile/*L*/(const TDesC &aErrMsg, TInt aErrCode);
	
public:
	/////////////
	// @return Error codes: KErrNotFound, KErrNotReady or KErrNone
	inline TInt GetTileBitmap(const TTile &aTile, CFbsBitmap* &aBitmap){return iBmpMemCache->GetTileBitmap(aTile, aBitmap);};
	////////////////
	void AddToLoading(const TTile &aTile, TBool aForce = EFalse);
	void ChangeTileProvider(TTileProvider* aTileProvider, const TDesC &aCacheDir);
	
	///////////////
	inline TBool HasError(const TTile &aTile){return iBmpMemCache->HasError(aTile);};
	inline const HBufC* ErrMsg(const TTile &aTile) {return iBmpMemCache->ErrMsg(aTile);};
	/////////////////
	};


/* Links Tile`s x,y,z with CFbsBitmap loaded to image server.
 * Used in CTileBitmapMemCache class.
 * 
 * Initially bitmap pointer is NULL. You need to call CreateBitmapIfNotExistL()
 * before start drawing bitmap. After drawing complete, you need to call SetReady().
 */
class CTileBitmapMemCacheItem : public CBase
	{
// Base methods
public:
	~CTileBitmapMemCacheItem();
	static CTileBitmapMemCacheItem* NewL(const TTile &aTile);
	static CTileBitmapMemCacheItem* NewLC(const TTile &aTile);

private:
	CTileBitmapMemCacheItem(const TTile &aTile);
	void ConstructL();

// Custom properties and methods
private:
	TTile iTile;
	CFbsBitmap* iBitmap;
	
	enum TState
		{
		ENotReady,	// image is not loading/processing yet
		EReady,		// when image completely drawn and ready to use
		EError		// error when download or process image
		};
	
	TState iState;
	HBufC* iErrorMsg; // owned
	
public:
	TTime iLastAccessTime;

	void CreateBitmapIfNotExistL();
	inline TBool IsReady() { return iState == EReady && iBitmap != NULL; };
	inline void SetReady() { iState = EReady; };
	inline TBool HasError()
		{ return iState == EError; };
	
	void SetErrorMsg/*L*/(const TDesC& aErrMsg);
	inline const HBufC* ErrorMsg()
			{ return iErrorMsg; };
	
// Getters
public:
	inline TTile Tile() const { return iTile; };
	
	// @return Pointer to bitmap or NULL if it`s not loaded yet.
	inline CFbsBitmap* Bitmap() /*const*/ { return iBitmap; };
	};


typedef TBuf<32> TTileProviderId;
typedef TBuf<32> TTileProviderTitle;
typedef TBuf8<512> TTileProviderUrl;

class TTileProvider
	{
public:
	TTileProvider(const TDesC& anId, const TDesC& aTitle, const TDesC8& anUrlTemplate,
			TZoom aMinZoom, TZoom aMaxZoom, const TDesC& aCopyrightTextShort = KNullDesC,
			/*const TDesC& aCopyrightText = KNullDesC,*/ const TDesC& aCopyrightUrl = KNullDesC);

	// Short string identifier of tile provider. Used in cache subdir name.
	// Must be unique and do not contains any special symbols (allowed: a-Z, 0-9, - and _). 
	TTileProviderId iId;
	
	// Readable name of tile provider. Will be display in settings.
	TTileProviderTitle iTitle;
	
	// Tile URL template with placeholders
	// Note: prefer not to use HTTPS protocol because unfortunately 
	// at the present time SSL works not on all Symbian based phones
	TTileProviderUrl iTileUrlTemplate;
	
	// Minimum and maximum zoom level
	TZoom iMinZoomLevel; // /*Default is 0*/
	TZoom iMaxZoomLevel; // /*Default is 18*/
	
	/*HBufC**/ TBuf<64> iCopyrightTextShort; // Short text, visible on the slippy map
	/*HBufC**/ TBuf<128> iCopyrightText; // Full text, visible in About dialog
	/*HBufC8**/ TBuf/*8*/<128> iCopyrightUrl;
	
	// Return url of given tile
	// ToDo: It is a good idea to make tests for this method
	void TileUrl(TDes8 &aUrl, const TTile &aTile);
	
private:
	TInt ParseRandCharRange(TLex8 &aLex, TChar &aReturnedChar);
	TInt ParseVariable(TLex8 &aLex, const TTile aTile, /*TInt32*/ TUint32 &aReturnedVal);
	};
	

class TCoordinateEx : public TCoordinate
	{
protected:
	TReal32 iCourse;
	TReal32 iHorAccuracy;
	
public:
	TCoordinateEx();
	TCoordinateEx(const TCoordinateEx &aCoordEx);
	TCoordinateEx(const TCoordinate &aCoord);
	
	inline TReal32 Course() const
		{ return iCourse; };
	inline void SetCourse(TReal32 aCourse)
		{ iCourse = aCourse; };

	inline TReal32 HorAccuracy() const
		{ return iHorAccuracy; }
	inline void SetHorAccuracy(TReal32 aHorAccuracy)
		{ iHorAccuracy = aHorAccuracy; }
	
	//operator TCoordinate() const;
	};


// Reads tiles from the file system (cache on disk).
class CTileDiskCache /*CTileFSCache*/ : public CBase
	{
	// Constructor / Destructor
public:
	static CTileDiskCache* NewL(RFs &aFs, const TDesC &aCacheDir);
	~CTileDiskCache();

private:
	CTileDiskCache(RFs &aFs);
	void ConstructL(const TDesC &aCacheDir);
	
	// New members
private:
	RFs iFs;
	CTileBitmapSaver* iSaver;
	CFileTreeMapper* iFileMapper;
	
public:
	void LoadBitmapL /*LoadBitmapFromFileL*/ /*ReadBitmapFromDiskL*/ (const TTile &aTile, CFbsBitmap *aBitmap) /*const*/;
	inline void SaveBitmapInBackgroundL(const TTile &aTile, /*const*/ CFbsBitmap *aBitmap)
		{ iSaver->AppendL(aTile, aBitmap); };
	void TileFileName(const TTile &aTile, TFileName &aFileName) const;
	TBool IsTileFileExists(const TTile &aTile) const;
	void DeleteTileFile(const TTile &aTile);
	inline void SetCacheDir(const TDesC &aCacheDir)
		{ iFileMapper->SetBaseDir(aCacheDir); };

	};


#endif /* MAP_H_ */
