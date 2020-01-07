/*
 * Map.h
 *
 *  Created on: 14.08.2019
 *      Author: user
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


// Constants
const TReal64 KMinLatitudeMapBound = -85.051129;
const TReal64 KMaxLatitudeMapBound = 85.051129;
const TReal64 KMinLongitudeMapBound = -180;
const TReal64 KMaxLongitudeMapBound = 180;


// Forward declaration
class CS60MapsAppView;
//class MImageReaderObserver;
class CImageReader;


// Classes

class CMapLayerBase : public CBase
	{
protected:
	CS60MapsAppView* iMapView;
public:
	CMapLayerBase(/*const*/ CS60MapsAppView* aMapView);
	virtual void Draw(CWindowGc &aGc) = 0;
	};

// Observer class for image reader
class MImageReaderObserver
	{
public:
	virtual void OnImageReaded() = 0;
	virtual void OnImageReadingFailed(TInt aErr);
	};

// Stub layer (just for testing, currently not nedeed more)
//class CMapLayerStub : public CMapLayerBase, public MImageReaderObserver
//	{
//private:
//	TFixedArray<TCoordinate, 6> iCoords;
//	RFs iFs;
//	CFbsBitmap* iBackgroundBitmap;
//	CImageReader* iImgReader;
//	
//	CMapLayerStub(/*const*/ CS60MapsAppView* aMapView, RFs &aFs);
//	void ConstructL();
//	
//	void DrawMap(CWindowGc &aGc);
//	void DrawMarks(CWindowGc &aGc);
//	void LoadBackgroundImageL();
//public:
//	~CMapLayerStub();
//	static CMapLayerStub* NewL(/*const*/ CS60MapsAppView* aMapView, RFs &aFs);
//	static CMapLayerStub* NewLC(/*const*/ CS60MapsAppView* aMapView, RFs &aFs);
//	
//	void Draw(CWindowGc &aGc);
//	void OnImageReaded();
//	};

// Debug layer with zoom, lat and lon info
class CMapLayerDebugInfo : public CMapLayerBase
	{
public:
	CMapLayerDebugInfo(/*const*/ CS60MapsAppView* aMapView);
	//~CMapLayerDebugInfo();
	void Draw(CWindowGc &aGc);
	};

class CTileBitmapManager;
//class MTileBitmapManagerObserver;

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
	static CTiledMapLayer* NewL(CS60MapsAppView* aMapView);
	static CTiledMapLayer* NewLC(CS60MapsAppView* aMapView);

private:
	CTiledMapLayer(CS60MapsAppView* aMapView);
	void ConstructL();
	
// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
// From MTileBitmapManagerObserver
public:
	void OnTileLoaded(const TTile &aTile, const CFbsBitmap *aBitmap);
	
// Custom properties and methods
private:
	CTileBitmapManager *iBitmapMgr;
	void VisibleTiles(RArray<TTile> &aTiles); // Return list of visible tiles
	void DrawTile(CWindowGc &aGc, const TTile &aTile, const CFbsBitmap *aBitmap);
	
	};


// Class for asynchronous image reading from file
class CImageReader : public CActive
	{
public:
	~CImageReader();
	static CImageReader* NewL(MImageReaderObserver* aObserver, CFbsBitmap* aBitmap,
			const TDesC &aFilePath, RFs &aFs);
	static CImageReader* NewLC(MImageReaderObserver* aObserver, CFbsBitmap* aBitmap,
			const TDesC &aFilePath, RFs &aFs);

	void StartRead();

private:
	CImageReader(MImageReaderObserver* aObserver, CFbsBitmap* aBitmap);
	void ConstructL(const TDesC &aFilePath, RFs &aFs);

	void RunL();
	void DoCancel();
	//TInt RunError(TInt aError);

	MImageReaderObserver* iObserver;
	CFbsBitmap* iBitmap;
	//HBufC* iFilePath;
	CImageDecoder* iDecoder;
	};

//// Observer class for image reader
//class MImageReaderObserver
//	{
//public:
//	virtual void OnImageReaded() = 0;
//	virtual void OnImageReadingFailed(TInt aErr);
//	};


//class MTileBitmapManagerObserver
//	{
//public:
//	void OnTileLoaded(const TTile &aTile, const CFbsBitmap *aBitmap);
//	};

class CTileBitmapManagerItem;

// Stores and loads bitmaps for tiles. When count of stored bitmaps
// reach maximum limit, oldest one will be deleted before insert new.
class CTileBitmapManager : public CBase
	{
// Base methods
public:
	~CTileBitmapManager();
	static CTileBitmapManager* NewL(MTileBitmapManagerObserver *aObserver, TInt aLimit = 50);
	static CTileBitmapManager* NewLC(MTileBitmapManagerObserver *aObserver, TInt aLimit = 50);

private:
	CTileBitmapManager(MTileBitmapManagerObserver *aObserver, TInt aLimit);
	void ConstructL();

// Custom properties and methods
private:
	MTileBitmapManagerObserver *iObserver;
	TInt iLimit;
	RPointerArray<CTileBitmapManagerItem> iItems;
	/*TInt*/ void Append/*L*/(const TTile &aTile); 
	
	// @return Pointer to CTileBitmapManagerItem object or NULL if not found
	CTileBitmapManagerItem* Find(const TTile &aTile) const;
	
public:
	// @return Error codes: KErrNotFound, KErrNotReady or KErrNone
	TInt GetTileBitmap(const TTile &aTile, CFbsBitmap* &aBitmap);
	void AddToLoading(const TTile &aTile);
	};


class CTileBitmapManagerItem : public CActive
	{
// Base methods
public:
	~CTileBitmapManagerItem();
	static CTileBitmapManagerItem* NewL(const TTile &aTile, MTileBitmapManagerObserver *aObserver);
	static CTileBitmapManagerItem* NewLC(const TTile &aTile, MTileBitmapManagerObserver *aObserver);

private:
	CTileBitmapManagerItem(const TTile &aTile, MTileBitmapManagerObserver *aObserver);
	void ConstructL();

// From CActive
private:
	void RunL();
	void DoCancel();
	//TInt RunError(TInt aError);
	
// Custom properties and methods
private:
	MTileBitmapManagerObserver* iObserver;
	TTile iTile;
	CFbsBitmap* iBitmap;
	void StartLoadL();
	void DrawStubBitmapL();
	void CreateBitmapIfNotExistL();
	
	RTimer iTimer;

public:
	TBool IsBitmapLoaded() const;
	
	// Getters
	inline TTile Tile() const;
	
	// @return Pointer to bitmap or NULL if it`s not loaded yet.
	inline CFbsBitmap* Bitmap() const;
	};


#endif /* MAP_H_ */
