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
public:
	CMapLayerDebugInfo(/*const*/ CS60MapsAppView* aMapView);
	void Draw(CWindowGc &aGc);
	
private:
	TInt iRedrawingsCount;
	
	void DrawInfoL(CWindowGc &aGc);
	};
#endif

class CTileBitmapManager;
//class MTileBitmapManagerObserver;
class TTileProvider;

class MTileBitmapManagerObserver
	{
public:
	virtual void OnTileLoaded(const TTile &aTile, const CFbsBitmap *aBitmap) = 0;
	virtual void OnTileLoadingFailed(const TTile &aTile, TInt aErrCode);
	};

// Class for drawing map tiles
class CTiledMapLayer : public CMapLayerBase, public MTileBitmapManagerObserver
	{
// Base methods
public:
	~CTiledMapLayer();
	static CTiledMapLayer* NewL(CS60MapsAppView* aMapView, TTileProvider* aTileProvider);
	static CTiledMapLayer* NewLC(CS60MapsAppView* aMapView, TTileProvider* aTileProvider);

private:
	CTiledMapLayer(CS60MapsAppView* aMapView);
	void ConstructL(TTileProvider* aTileProvider);
	
// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
// From MTileBitmapManagerObserver
public:
	void OnTileLoaded(const TTile &aTile, const CFbsBitmap *aBitmap);
	
// Custom properties and methods
private:
	CTileBitmapManager *iBitmapMgr;
	TTileProvider *iTileProvider;
	void VisibleTiles(RArray<TTile> &aTiles); // Return list of visible tiles
	void DrawTile(CWindowGc &aGc, const TTile &aTile, const CFbsBitmap *aBitmap);
	
public:
	void SetTileProviderL(TTileProvider* aTileProvider);
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
	static CLandmarksLayer* NewL(CS60MapsAppView* aMapView);
	static CLandmarksLayer* NewLC(CS60MapsAppView* aMapView);

private:
	CLandmarksLayer(CS60MapsAppView* aMapView);
	void ConstructL();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// Own
private:
	CPosLandmarkDatabase* iLandmarksDb;
	CPosLmPartialReadParameters* iLandmarkPartialParameters;
	
	void DrawL(CWindowGc &aGc);
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

class CTileBitmapSaver: public CBase
	{
	// Constructors/destructors
public:
	~CTileBitmapSaver();
	static CTileBitmapSaver* NewL(CTileBitmapManager* aMgr);
	static CTileBitmapSaver* NewLC(CTileBitmapManager* aMgr);

private:
	CTileBitmapSaver(CTileBitmapManager* aMgr);
	void ConstructL();
	
	// Custom
public:
	void AppendL(const TTile &aTile, CFbsBitmap *aBitmap);
	
private:
	CTileBitmapManager* iMgr;
	RMsgQueue<TSaverQueryItem> iQueue;
	TInt iItemsInQueue;
	TThreadId iThreadId;
	
	static TInt ThreadFunction(TAny* anArg);
	void SaveL(const TSaverQueryItem &anItem, RFs &aFs);
	};



class TTileProvider;

class CTileBitmapManagerItem;

// Stores and loads bitmaps for tiles. When count of stored bitmaps
// reach maximum limit, oldest one will be deleted before insert new.
class CTileBitmapManager : public CActive, public MHTTPClientObserver
	{
// Base methods
public:
	~CTileBitmapManager();
	static CTileBitmapManager* NewL(MTileBitmapManagerObserver *aObserver,
			RFs aFs, TTileProvider* aTileProvider, const TDesC &aCacheDir, TInt aLimit = 50);
	static CTileBitmapManager* NewLC(MTileBitmapManagerObserver *aObserver,
			RFs aFs, TTileProvider* aTileProvider, const TDesC &aCacheDir, TInt aLimit = 50);

private:
	CTileBitmapManager(MTileBitmapManagerObserver *aObserver, RFs aFs,
			TTileProvider* aTileProvider, TInt aLimit);
	void ConstructL(const TDesC &aCacheDir);
	
// From CActive
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
	MTileBitmapManagerObserver *iObserver;
	TInt iLimit;
	RPointerArray<CTileBitmapManagerItem> iItems;
	/*TInt*/ void Append/*L*/(const TTile &aTile); 
	
	RArray<TTile> /*iItemsForLoading*/ iItemsLoadingQueue;
	CHTTPClient* iHTTPClient;
	TTileProvider* iTileProvider;
	//TFileName iCacheDir;
	//TBool iIsLoading;
	enum TProcessingState
		{
		EIdle,
		EDownloading,
		EDecoding
		};
	TProcessingState iState;
	CBufferedImageDecoder* iImgDecoder;
	RFs iFs;
	TTile iLoadingTile;
	TBool iIsOfflineMode;
	CFileTreeMapper* iFileMapper;
	CTileBitmapSaver* iSaver;
	
	// @return Pointer to CTileBitmapManagerItem object or NULL if not found
	CTileBitmapManagerItem* Find(const TTile &aTile) const;
	void StartDownloadTileL(const TTile &aTile);
	
	// Save tile bitmap to file
	void SaveBitmapInBackgroundL(const TTile &aTile, /*const*/ CFbsBitmap *aBitmap);
	
	// Restore tile bitmap from file
	void LoadBitmapL(const TTile &aTile, CFbsBitmap *aBitmap) /*const*/;
	
	void TileFileName(const TTile &aTile, TFileName &aFileName) const;
	TBool IsTileFileExists(const TTile &aTile) /*const*/;
	
public:
	// @return Error codes: KErrNotFound, KErrNotReady or KErrNone
	TInt GetTileBitmap(const TTile &aTile, CFbsBitmap* &aBitmap);
	void AddToLoading(const TTile &aTile);
	void ChangeTileProvider(TTileProvider* aTileProvider, const TDesC &aCacheDir);
	
// Friends
	friend class CTileBitmapSaver;
	};


/* Links Tile`s x,y,z with CFbsBitmap loaded to image server.
 * Used in CTileBitmapManager class.
 * 
 * Initially bitmap pointer is NULL. You need to call CreateBitmapIfNotExistL()
 * before start drawing bitmap. After drawing complete, you need to call SetReady().
 */
class CTileBitmapManagerItem : public CBase
	{
// Base methods
public:
	~CTileBitmapManagerItem();
	static CTileBitmapManagerItem* NewL(const TTile &aTile);
	static CTileBitmapManagerItem* NewLC(const TTile &aTile);

private:
	CTileBitmapManagerItem(const TTile &aTile);
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


typedef TBuf<32> TTileProviderId;
typedef TBuf<32> TTileProviderTitle;
typedef TBuf8<512> TTileProviderUrl;

class TTileProvider
	{
public:
	TTileProvider(const TDesC& anId, const TDesC& aTitle, const TDesC8& anUrlTemplate,
			TZoom aMinZoom, TZoom aMaxZoom);

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


#endif /* MAP_H_ */
