/*
 * Map.h
 *
 *  Created on: 14.08.2019
 *      Author: artem78
 */

#ifndef MAP_H_
#define MAP_H_

//#include "S60MapsAppView.h"
#include <w32std.h>
#include <lbsposition.h>
#include <f32file.h>
#include <fbs.h>
#include <imageconversion.h>
#include "MapMath.h"
#include <e32base.h>
#include <e32std.h>		// For RTimer
#include "HttpClient.h"
#include "FileUtils.h"
#include <e32msgqueue.h>
#include <epos_cposlandmarkdatabase.h>


// Constants
const TReal64 KMinLatitudeMapBound = -85.051129;
const TReal64 KMaxLatitudeMapBound = 85.051129;
const TReal64 KMinLongitudeMapBound = -180;
const TReal64 KMaxLongitudeMapBound = 180;


// Forward declaration
class CS60MapsAppView;


// Classes

class CMapLayerBase : public CBase
	{
protected:
	CS60MapsAppView* iMapView;
public:
	CMapLayerBase(/*const*/ CS60MapsAppView* aMapView);
	virtual void Draw(CWindowGc &aGc) = 0;
	};


#ifdef DEBUG_SHOW_ADDITIONAL_INFO
// Debug layer with additional info
class CMapLayerDebugInfo : public CMapLayerBase
	{
	// Constructor/destructor
private:
	CMapLayerDebugInfo(/*const*/ CS60MapsAppView* aMapView);
	void ConstructL();
	
public:
	static CMapLayerDebugInfo* NewL(CS60MapsAppView* aMapView);
	static CMapLayerDebugInfo* NewLC(CS60MapsAppView* aMapView);
	~CMapLayerDebugInfo();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// Own
private:
	TInt iRedrawingsCount;
	CFont* iFont;

	void DrawInfoL(CWindowGc &aGc);
	};
#endif

class CTileBitmapMemCache;
//class MTileBitmapManagerObserver;
class TWebTileProviderSettings;

class MTileBitmapManagerObserver
	{
public:
	virtual void OnTileLoaded(const TTile &aTile, const CFbsBitmap *aBitmap) = 0;
	virtual void OnTileLoadingFailed(const TTile &aTile, TInt aErrCode);
	};

class CTileProviderBase;

// Class for drawing map tiles
class CTiledMapLayer : public CMapLayerBase, public MTileBitmapManagerObserver
	{
// Base methods
public:
	~CTiledMapLayer();
	static CTiledMapLayer* NewL(CS60MapsAppView* aMapView, TWebTileProviderSettings* aTileProviderSettings);
	static CTiledMapLayer* NewLC(CS60MapsAppView* aMapView, TWebTileProviderSettings* aTileProviderSettings);

private:
	CTiledMapLayer(CS60MapsAppView* aMapView);
	void ConstructL(TWebTileProviderSettings* aTileProviderSettings);
	
// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
// From MTileBitmapManagerObserver
public:
	void OnTileLoaded(const TTile &aTile, const CFbsBitmap *aBitmap);
	
// Custom properties and methods
private:
	CTileBitmapMemCache* iBitmapCache;
	CTileProviderBase* iTileProvider;
	void VisibleTiles(RArray<TTile> &aTiles); // Return list of visible tiles
	void DrawTile(CWindowGc &aGc, const TTile &aTile, const CFbsBitmap *aBitmap);
	
public:
	void SetTileProviderSettingsL(TWebTileProviderSettings* aTileProviderSettings);
	};


// Displays user location
class CUserPositionLayer : public CMapLayerBase
	{
// From CMapLayerBase
public:
	CUserPositionLayer(/*const*/ CS60MapsAppView* aMapView);
	void Draw(CWindowGc &aGc);
	
// Own methods
private:
	void DrawAccuracyCircle(CWindowGc &aGc, const TPoint &aScreenPos, TSize aSize);
	void DrawDirectionMarkL(CWindowGc &aGc, const TPoint &aScreenPos, TReal aRotation);
	void DrawRoundMark(CWindowGc &aGc, const TPoint &aScreenPos);
	};


#ifdef DEBUG_SHOW_TILE_BORDER_AND_XYZ
// Debug layer for drawing tile`s border and x/y/z values.
// May be used as stub.
class CTileBorderAndXYZLayer : public CMapLayerBase
	{
// From CMapLayerBase
public:
	CTileBorderAndXYZLayer(CS60MapsAppView* aMapView);
	void Draw(CWindowGc &aGc);

// Custom properties and methods
private:
	void VisibleTiles(RArray<TTile> &aTiles); // Return list of visible tiles
	void DrawTile(CWindowGc &aGc, const TTile &aTile);
	
	};
#endif


class CScaleBarLayer : public CMapLayerBase
	{
	// Constructor / destructor
public:
	~CScaleBarLayer();
	static CScaleBarLayer* NewL(CS60MapsAppView* aMapView);
	static CScaleBarLayer* NewLC(CS60MapsAppView* aMapView);

private:
	CScaleBarLayer(CS60MapsAppView* aMapView);
	void ConstructL();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// Own
private:
	HBufC* iMetersUnit;
	HBufC* iKilometersUnit;
	/*const*/ CFont* iFont;
	};

class CLandmarksLayer : public CMapLayerBase
	{
	// Constructor / destructor
public:
	~CLandmarksLayer();
	static CLandmarksLayer* NewL(CS60MapsAppView* aMapView, CPosLandmarkDatabase* aLmDb);
	static CLandmarksLayer* NewLC(CS60MapsAppView* aMapView, CPosLandmarkDatabase* aLmDb);

private:
	CLandmarksLayer(CS60MapsAppView* aMapView, CPosLandmarkDatabase* aLmDb);
	void ConstructL();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// Own
private:
	CPosLandmarkDatabase* iLandmarksDb; // Not owned
	CFbsBitmap* iIconBitmap;
	CFbsBitmap* iIconMaskBitmap;
	CFont* iFont; // For drawing labels
	
	// Result may be NULL if nothing found
	CArrayPtr<CPosLandmark>* GetVisibleLandmarksL(); // ToDo: Is moving to another class needed?
	void DrawL(CWindowGc &aGc);
	void DrawLandmarks(CWindowGc &aGc, const CArrayPtr<CPosLandmark>* aLandmarks);
	void DrawLandmark(CWindowGc &aGc, const CPosLandmark* aLandmark);
	};


class CCrosshairLayer : public CMapLayerBase
	{
	// Constructor / Destructor
public:
	CCrosshairLayer(CS60MapsAppView* aMapView);
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	};


class CSaverQueryItem : public CBase
	{
	// Constructor / Destructor
public:
	static CSaverQueryItem* NewL();
	static CSaverQueryItem* NewLC();
	~CSaverQueryItem();

private:
	CSaverQueryItem();
	void ConstructL();
	
	// New
public:
	TTile iTile;
	CFbsBitmap *iBitmap; // Owned
	};


class CWebTileProvider;

// Class for save tile bitmaps in separate thread
// for improve bad performance on some phones

class CTileBitmapSaver: public CActive
	{
	// Constructors/destructors
public:
	~CTileBitmapSaver();
	static CTileBitmapSaver* NewL(CWebTileProvider* aTileProvider);
	static CTileBitmapSaver* NewLC(CWebTileProvider* aTileProvider);

private:
	CTileBitmapSaver(CWebTileProvider* aTileProvider);
	void ConstructL();
	
	// From CActive
private:
	void RunL();
	void DoCancel();
	
	// Custom
public:
	void AppendL(const TTile &aTile, CFbsBitmap *aBitmap);
	
private:
	CWebTileProvider* iTileProvider;
	RMsgQueue<CSaverQueryItem*> iQueue;
	RMsgQueue<CSaverQueryItem*> iRemovingQueue;
	TInt iItemsInQueue;
	TThreadId iThreadId;
	
	static TInt ThreadFunction(TAny* anArg);
	void SaveL(const CSaverQueryItem &anItem, RFs &aFs);
	
	/* Used from saver thread to delete processed items */
	void RemoveItemInThreadL(CSaverQueryItem* anItem);
	};



class TWebTileProviderSettings;

class CTileBitmapMemCacheItem;

/* 
 * In-memory cache for storing tile bitmaps. Used to prevent read data from file
 * system or network twice. It has limited capacity - when amount of stored
 * bitmaps reachs maximum, oldest items will be deleted before adding new ones.
 */
class CTileBitmapMemCache : public CBase
	{
// Constructors / destructors
public:
	~CTileBitmapMemCache();
	static CTileBitmapMemCache* NewL(TInt aLimit = 50);
	static CTileBitmapMemCache* NewLC(TInt aLimit = 50);

private:
	CTileBitmapMemCache(TInt aLimit);
	void ConstructL();
	
// Custom properties and methods
private:
	TInt iLimit; // Max amount of stored in memory bitmaps
	RPointerArray<CTileBitmapMemCacheItem> iItems;
	/*TInt*/ void Append/*L*/(const TTile &aTile);
	
	// @return Pointer to CTileBitmapMemCacheItem object or NULL if not found
	CTileBitmapMemCacheItem* Find(const TTile &aTile) const;

public:
	// @return Error codes: KErrNotFound, KErrNotReady or KErrNone
	TInt GetTileBitmap(const TTile &aTile, CFbsBitmap* &aBitmap);
	void ReserveItem(const TTile &aTile);
	void Reset();
	
// Friends
	friend class CTiledMapLayer; // ToDo: delete
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
	TBool iIsReady; // ETrue when image completely drawn and ready to use
public:
	void CreateBitmapIfNotExistL();
	inline TBool IsReady() { return iIsReady && iBitmap != NULL; };
	inline void SetReady() { iIsReady = ETrue; };
	
// Getters
public:
	inline TTile Tile() const { return iTile; };
	
	// @return Pointer to bitmap or NULL if it`s not loaded yet.
	inline CFbsBitmap* Bitmap() /*const*/ { return iBitmap; };
	};


typedef TBuf<32> TWebTileProviderId;
typedef TBuf<32> TWebTileProviderTitle;
typedef TBuf8<512> TWebTileProviderUrl;

class TWebTileProviderSettings
	{
public:
	TWebTileProviderSettings(const TDesC& anId, const TDesC& aTitle, const TDesC8& anUrlTemplate,
			TZoom aMinZoom, TZoom aMaxZoom);

	// Short string identifier of tile provider. Used in cache subdir name.
	// Must be unique and do not contains any special symbols (allowed: a-Z, 0-9, - and _). 
	TWebTileProviderId iId;
	
	// Readable name of tile provider. Will be display in settings.
	TWebTileProviderTitle iTitle;
	
	// Tile URL template with placeholders
	// Note: prefer not to use HTTPS protocol because unfortunately 
	// at the present time SSL works not on all Symbian based phones
	TWebTileProviderUrl iTileUrlTemplate;
	
	// Minimum and maximum zoom level
	TZoom iMinZoomLevel; // /*Default is 0*/
	TZoom iMaxZoomLevel; // /*Default is 18*/
	
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

/*
 * Base class for any tiles source
 */
class CTileProviderBase : public CActive
	{
// Constructors / destructors
protected:
	CTileProviderBase(MTileBitmapManagerObserver *aObserver);
	
// Custom properties and methods
protected:
	MTileBitmapManagerObserver *iObserver;
	
public:
	// Asynchronous method to request tile image
	virtual void RequestTileL(const TTile &aTile) = 0;
	};


/*
 * Loads tile from specified web-service (like openstreetmap, google maps, etc...).
 */
class CWebTileProvider : public CTileProviderBase, /*public CActive,*/ public MHTTPClientObserver
	{
// Constructors / destructors
public:
	~CWebTileProvider();
	static CWebTileProvider* NewL(MTileBitmapManagerObserver *aObserver,
			RFs &aFs, TWebTileProviderSettings* aSettings);
	static CWebTileProvider* NewLC(MTileBitmapManagerObserver *aObserver,
			RFs &aFs, TWebTileProviderSettings* aSettings);

private:
	CWebTileProvider(MTileBitmapManagerObserver *aObserver, RFs &aFs);
	void ConstructL(TWebTileProviderSettings* aSettings);
	
// From CActive
private:
	void RunL();
	void DoCancel();
	//TInt RunError(TInt aError);

// From MHTTPClientObserver
public:
	virtual void OnHTTPResponseDataChunkRecieved(const RHTTPTransaction aTransaction,
			const TDesC8 &aDataChunk, TInt anOverallDataSize, TBool anIsLastChunk);
	virtual void OnHTTPResponse(const RHTTPTransaction aTransaction);
	virtual void OnHTTPError(TInt aError, const RHTTPTransaction aTransaction);
	virtual void OnHTTPHeadersRecieved(const RHTTPTransaction aTransaction);
	
// Custom properties and methods
private:
	RArray<TTile> iItemsLoadingQueue;
	CHTTPClient* iHTTPClient;
	TWebTileProviderSettings* iSettings;
	
public:
	enum TProcessingState
		{
		EIdle,
		EDownloading,
		EDecoding
		};
	
private:
	TProcessingState iState;
	CBufferedImageDecoder* iImgDecoder;
	RFs iFs;
	TTile iLoadingTile;
	TBool iIsOfflineMode;
	CTileBitmapSaver* iSaver;
	CFileTreeMapper* iFileMapper;
	CFbsBitmap* iBitmap;
	
	void StartDownloadTileL(const TTile &aTile);
	void AddToDownloadQueue(const TTile &aTile);

public:
	void /*StartLoadTileL*/ RequestTileL(const TTile &aTile);
	
private:
	// Save tile bitmap to file
	void SaveBitmapInBackgroundL(const TTile &aTile, /*const*/ CFbsBitmap *aBitmap);
	void /*Reset*/ StopAndReset();
	void InitializeCacheDirL();

	TBool IsTileFileExists(const TTile &aTile) /*const*/;
	
	// Restore tile bitmap from file
	void LoadBitmapL(const TTile &aTile, CFbsBitmap *aBitmap) /*const*/;
	//void LoadBitmapAsync(const TTile &aTile/*, CFbsBitmap *aBitmap*/) /*const*/;
	void TileFileName(const TTile &aTile, TFileName &aFileName) const;
	void ResetBitmapL();
	
public:
	void SetSettingsL(TWebTileProviderSettings* aSettings);
	inline TProcessingState State()
		{ return iState; };
	
// Friends
	friend class CTileBitmapSaver;
	friend class CTiledMapLayer;
	};


#endif /* MAP_H_ */
