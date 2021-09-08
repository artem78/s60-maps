/*
 * Map.h
 *
 *  Created on: 14.08.2019
 *      Author: artem78
 */

#ifndef MAP_H_
#define MAP_H_

//#include "MapControl.h"
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
class CMapControl;


// Classes

class CMapLayerBase : public CBase
	{
protected:
	CMapControl* iMapView;
public:
	CMapLayerBase(/*const*/ CMapControl* aMapView);
	virtual void Draw(CWindowGc &aGc) = 0;
	};


#ifdef DEBUG_SHOW_ADDITIONAL_INFO
// Debug layer with additional info
class CMapLayerDebugInfo : public CMapLayerBase
	{
	// Constructor/destructor
private:
	CMapLayerDebugInfo(/*const*/ CMapControl* aMapView);
	void ConstructL();
	
public:
	static CMapLayerDebugInfo* NewL(CMapControl* aMapView);
	static CMapLayerDebugInfo* NewLC(CMapControl* aMapView);
	~CMapLayerDebugInfo();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// Own
private:
	TInt iRedrawingsCount;

	void DrawInfoL(CWindowGc &aGc);
	};
#endif

class CTileBitmapManager;
//class MTileBitmapManagerObserver;
class TWebTileProviderSettings;

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
	static CTiledMapLayer* NewL(CMapControl* aMapView, TWebTileProviderSettings* aTileProviderSettings);
	static CTiledMapLayer* NewLC(CMapControl* aMapView, TWebTileProviderSettings* aTileProviderSettings);

private:
	CTiledMapLayer(CMapControl* aMapView);
	void ConstructL(TWebTileProviderSettings* aTileProviderSettings);
	
// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
// From MTileBitmapManagerObserver
public:
	void OnTileLoaded(const TTile &aTile, const CFbsBitmap *aBitmap);
	
// Custom properties and methods
private:
	CTileBitmapManager *iBitmapMgr;
	TWebTileProviderSettings *iTileProviderSettings;
	void VisibleTiles(RArray<TTile> &aTiles); // Return list of visible tiles
	void DrawTile(CWindowGc &aGc, const TTile &aTile, const CFbsBitmap *aBitmap);
	
public:
	void SetTileProviderSettingsL(TWebTileProviderSettings* aTileProviderSettings);
	void ReloadVisibleAreaL();
	};


// Displays user location
class CUserPositionLayer : public CMapLayerBase
	{
// From CMapLayerBase
public:
	CUserPositionLayer(/*const*/ CMapControl* aMapView);
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
	CTileBorderAndXYZLayer(CMapControl* aMapView);
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
	static CScaleBarLayer* NewL(CMapControl* aMapView);
	static CScaleBarLayer* NewLC(CMapControl* aMapView);

private:
	CScaleBarLayer(CMapControl* aMapView);
	void ConstructL();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// Own
private:
	HBufC* iMetersUnit;
	HBufC* iKilometersUnit;
	
	void GetOptimalLength(TInt &optimalLength, TReal32 &optimalDistance);
	
public:
	void ReloadStringsFromResourceL();
	
	};

class CLandmarksLayer : public CMapLayerBase
	{
	// Constructor / destructor
public:
	~CLandmarksLayer();
	static CLandmarksLayer* NewL(CMapControl* aMapView, CPosLandmarkDatabase* aLmDb);
	static CLandmarksLayer* NewLC(CMapControl* aMapView, CPosLandmarkDatabase* aLmDb);

private:
	CLandmarksLayer(CMapControl* aMapView, CPosLandmarkDatabase* aLmDb);
	void ConstructL();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// Own
private:
	CPosLandmarkDatabase* iLandmarksDb; // Not owned
	CFbsBitmap* iIconBitmap;
	CFbsBitmap* iIconMaskBitmap;
	
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
	CCrosshairLayer(CMapControl* aMapView);
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	};


class CSignalIndicatorLayer : public CMapLayerBase
	{	
	// Constructor / Destructor
public:
	static CSignalIndicatorLayer* NewL(CMapControl* aMapView);
	static CSignalIndicatorLayer* NewLC(CMapControl* aMapView);
	~CSignalIndicatorLayer();

private:
	CSignalIndicatorLayer(CMapControl* aMapView);
	void ConstructL();
	
	// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
	// New
private:
	enum TSignalStrength
		{
		ESignalNone,
		ESignalVeryLow,
		ESignalLow,
		ESignalMedium,
		ESignalGood,
		ESignalVeryGood,
		ESignalHigh
		};
	
	// Constants for bars settings ("K" prefix used instead of "E" for enums)
	enum {
		KBarWidth		= 4,
		KStartBarHeight	= 5,
		KBarBorderWidth	= 1,
		KBarsSpacing	= 2,
		KBarHeightIncremement	= 3,
		KBarsCount		= ESignalHigh - ESignalVeryLow + 1,
		KBarsTotalWidth		= KBarsCount * KBarWidth + (KBarsCount - 1) * KBarsSpacing,
		KBarsTotalHeight	= KStartBarHeight + (KBarsCount - 1) * KBarHeightIncremement
	};
	
	CFbsBitmap* iSatelliteIconBitmap;
	CFbsBitmap* iSatelliteIconMaskBitmap;
	
	void DrawBars(CWindowGc &aGc, TSignalStrength aBarsCount);
	void DrawSatelliteIcon(CWindowGc &aGc, const TPoint &aPos);
	
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



class TWebTileProviderSettings;

class CTileBitmapManagerItem;

class CWebTileProvider;

// Stores bitmaps for tiles. When count of stored bitmaps
// reach maximum limit, oldest one will be deleted before insert new.
class CTileBitmapManager : public CBase
	{
// Constructors / destructors
public:
	~CTileBitmapManager();
	static CTileBitmapManager* NewL(MTileBitmapManagerObserver *aObserver,
			RFs aFs, TWebTileProviderSettings* aTileProviderSettings, const TDesC &aCacheDir, TInt aLimit = 50);
	static CTileBitmapManager* NewLC(MTileBitmapManagerObserver *aObserver,
			RFs aFs, TWebTileProviderSettings* aTileProviderSettings, const TDesC &aCacheDir, TInt aLimit = 50);

private:
	CTileBitmapManager(RFs aFs, TWebTileProviderSettings* aTileProviderSettings, TInt aLimit);
	void ConstructL(MTileBitmapManagerObserver *aObserver, const TDesC &aCacheDir);
	
// Custom properties and methods
private:
	TInt iLimit; // Max amount of stored in memory bitmaps
	RPointerArray<CTileBitmapManagerItem> iItems;
	/*TInt*/ void Append/*L*/(const TTile &aTile); 
	
	TWebTileProviderSettings* iTileProviderSettings;
	RFs iFs;
	CFileTreeMapper* iFileMapper;
	CWebTileProvider* iWebTileProvider;
	
	// @return Pointer to CTileBitmapManagerItem object or NULL if not found
	CTileBitmapManagerItem* Find(const TTile &aTile) const;
	
	// Restore tile bitmap from file
	void LoadBitmapL(const TTile &aTile, CFbsBitmap *aBitmap) /*const*/;
	
	void TileFileName(const TTile &aTile, TFileName &aFileName) const;
	TBool IsTileFileExists(const TTile &aTile) /*const*/;
	void DeleteTileFile(const TTile &aTile);
	void Delete(const TTile &aTile);
	
public:
	// @return Error codes: KErrNotFound, KErrNotReady or KErrNone
	TInt GetTileBitmap(const TTile &aTile, CFbsBitmap* &aBitmap);
	void AddToLoading(const TTile &aTile, TBool aForce = EFalse);
	void ChangeTileProviderSettings(TWebTileProviderSettings* aTileProvider, const TDesC &aCacheDir);
	
// Friends
	friend class CTileBitmapSaver;
//////////
	friend class CWebTileProvider;
//////////
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
 * Loads tile from specified web-service (like openstreetmap, google maps, etc...).
 */
class CWebTileProvider : public CActive, public MHTTPClientObserver
	{
// Constructors / destructors
public:
	~CWebTileProvider();
	static CWebTileProvider* NewL(MTileBitmapManagerObserver *aObserver,
			RFs &aFs, TWebTileProviderSettings* aSettings,
			CTileBitmapManager* aBmpMgr);
	static CWebTileProvider* NewLC(MTileBitmapManagerObserver *aObserver,
			RFs &aFs, TWebTileProviderSettings* aSettings,
			CTileBitmapManager* aBmpMgr);

private:
	CWebTileProvider(MTileBitmapManagerObserver *aObserver, RFs &aFs,
			TWebTileProviderSettings* aSettings, CTileBitmapManager* aBmpMgr);
	void ConstructL();
	
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
	MTileBitmapManagerObserver *iObserver;
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
	CTileBitmapManager* iBmpMgr;
	
	void StartDownloadTileL(const TTile &aTile);
	void AddToDownloadQueue(const TTile &aTile);

public:
	void /*StartLoadTileL*/ RequestTileL(const TTile &aTile);
	
private:
	// Save tile bitmap to file
	void SaveBitmapInBackgroundL(const TTile &aTile, /*const*/ CFbsBitmap *aBitmap);
	
public:
	void ChangeSettings(TWebTileProviderSettings* aSettings, const TDesC &aCacheDir);
	inline TProcessingState State()
		{ return iState; };
	
// Friends
	friend class CTileBitmapSaver;
	};


#endif /* MAP_H_ */
