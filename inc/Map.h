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

class CTileImageManager;
//class MTileImageManagerObserver;

class MTileImageManagerObserver
	{
public:
	virtual void OnTileLoaded(const TTile &aTile, const CFbsBitmap *aBitmap) = 0;
	};

// OpenStreetMap layer
class CMapLayerOSM : public CMapLayerBase, public MTileImageManagerObserver
	{
// Base methods
public:
	~CMapLayerOSM();
	static CMapLayerOSM* NewL(CS60MapsAppView* aMapView);
	static CMapLayerOSM* NewLC(CS60MapsAppView* aMapView);

private:
	CMapLayerOSM(CS60MapsAppView* aMapView);
	void ConstructL();
	
// From CMapLayerBase
public:
	void Draw(CWindowGc &aGc);
	
// From MTileImageManagerObserver
public:
	void OnTileLoaded(const TTile &aTile, const CFbsBitmap *aBitmap);
	
// Custom properties and methods
private:
	CTileImageManager *iImgMgr;
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


//class MTileImageManagerObserver
//	{
//public:
//	void OnTileLoaded(const TTile &aTile, const CFbsBitmap *aBitmap);
//	};

class CTileImagesCache;

class CTileImageManager : public CBase
	{
// Base methods
public:
	~CTileImageManager();
	static CTileImageManager* NewL(MTileImageManagerObserver *aObserver);
	static CTileImageManager* NewLC(MTileImageManagerObserver *aObserver);

private:
	CTileImageManager(MTileImageManagerObserver *aObserver);
	void ConstructL();

// Custom properties and methods
private:
	MTileImageManagerObserver *iObserver;
	CTileImagesCache *iCache;
	void DrawStubTileL(const TTile &aTile, CFbsBitmap* aBitmap);
	
public:
	void GetTileImage/*L*/(const TTile &aTile);
	void GetTileImages/*L*/(const RArray<TTile> &aTilesArray);
	};


template <class T1, class T2>
class TPair
	{
public:
	TPair();
	TPair(const T1 &aItem1, const T2 &aItem2);
	T1 iA;
	T2 iB;
	};

typedef TPair<TTile, CFbsBitmap*> TTileBitmapPair;

class CTileImagesCache : public CBase
	{
// Base methods
public:
	~CTileImagesCache();
	static CTileImagesCache* NewL();
	static CTileImagesCache* NewLC();
private:
	CTileImagesCache();
	void ConstructL();
	
// Custom properties and methods
private:
	RArray<TTileBitmapPair> iItems;
public:
	TInt Append(const TTile &aTile, /*const*/ CFbsBitmap *Bitmap); 
	CFbsBitmap* Find(const TTile &aTile) const;
	};


//// Array of key-value pairs
//template <class K, class V>
//class CDictionary : public CBase
//	{
//// Base methods
//public:
//	~CDictionary();
//	static CDictionary* NewL();
//	static CDictionary* NewLC();
//
//private:
//	CDictionary();
//	void ConstructL();
//	
//// Custom properties and methods
//private:
//	RArray<K> iKeysArray;
//	RArray<V> iValuesArray;
//public:
//	TInt Append(const K &aKey, const &V &aValue);
//	V Find(const K &aKey): const;
//	void Remove(const K &aKey);
//	void Clear();
//	TInt Count(): const;
//	};

#endif /* MAP_H_ */
