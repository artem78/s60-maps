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

// Stub layer (for testing)
class CMapLayerStub : public CMapLayerBase, public MImageReaderObserver
	{
private:
	TFixedArray<TCoordinate, 6> iCoords;
	RFs iFs;
	CFbsBitmap* iBackgroundBitmap;
	CImageReader* iImgReader;
	
	CMapLayerStub(/*const*/ CS60MapsAppView* aMapView, RFs &aFs);
	void ConstructL();
	
	void DrawMap(CWindowGc &aGc);
	void DrawMarks(CWindowGc &aGc);
	void LoadBackgroundImageL();
public:
	~CMapLayerStub();
	static CMapLayerStub* NewL(/*const*/ CS60MapsAppView* aMapView, RFs &aFs);
	static CMapLayerStub* NewLC(/*const*/ CS60MapsAppView* aMapView, RFs &aFs);
	
	void Draw(CWindowGc &aGc);
	void OnImageReaded();
	};

// Debug layer with zoom, lat and lon info
class CMapLayerDebugInfo : public CMapLayerBase
	{
public:
	CMapLayerDebugInfo(/*const*/ CS60MapsAppView* aMapView);
	//~CMapLayerDebugInfo();
	void Draw(CWindowGc &aGc);
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

#endif /* MAP_H_ */
