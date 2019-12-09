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

CMapLayerBase::CMapLayerBase(/*const*/ CS60MapsAppView* aMapView) :
		iMapView(aMapView)
	{
	}

const TInt KStubImageSize = 256;

CMapLayerStub::CMapLayerStub(/*const*/ CS60MapsAppView* aMapView, RFs &aFs) :
		CMapLayerBase(aMapView),
		iFs(aFs)
	{
	iCoords[0] = TCoordinate(40.7283,-73.9942); // New York
	iCoords[1] = TCoordinate(51.5072, -0.1275); // London
	iCoords[2] = TCoordinate(-34.9333, 138.5833); // Adelaide 
	iCoords[3] = TCoordinate(48.8333, 2.3333); // Paris 
	iCoords[4] = TCoordinate(55.7558, 37.6178); // Moscow
	iCoords[5] = TCoordinate(0, 0); // 0, 0*/
	}

CMapLayerStub::~CMapLayerStub()
	{
	delete iBackgroundBitmap;
	delete iImgReader;
	}

CMapLayerStub* CMapLayerStub::NewLC(/*const*/ CS60MapsAppView* aMapView, RFs &aFs)
	{
	CMapLayerStub* self = new (ELeave) CMapLayerStub(aMapView, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMapLayerStub* CMapLayerStub::NewL(/*const*/ CS60MapsAppView* aMapView, RFs &aFs)
	{
	CMapLayerStub* self = CMapLayerStub::NewLC(aMapView, aFs);
	CleanupStack::Pop(); // self;
	return self;
	}

void CMapLayerStub::ConstructL()
	{
	iBackgroundBitmap = new (ELeave) CFbsBitmap();
	/*TSize size(343, 344);
	TDisplayMode mode = EColor16M;
	User::LeaveIfError(iBackgroundBitmap->Create(size, mode));*/
	LoadBackgroundImageL();
	}

void CMapLayerStub::LoadBackgroundImageL()
	{
	_LIT(KImagePath, "c:\\map_stub.png");
	//_LIT(KImageMime, "image/png");
	
	TSize size(KStubImageSize, KStubImageSize);
	TDisplayMode mode = EColor16M;
	User::LeaveIfError(iBackgroundBitmap->Create(size, mode));
	
	iImgReader = CImageReader::NewL(this, iBackgroundBitmap,
			KImagePath, iFs);
	iImgReader->StartRead();
	}

void CMapLayerStub::OnImageReaded()
	{
	iMapView->DrawNow();
	}

void CMapLayerStub::Draw(CWindowGc &aGc)
	{
	DrawMap(aGc);
	DrawMarks(aGc);
	}


void CMapLayerStub::DrawMap(CWindowGc &aGc)
	{
	TCoordinate imageTopLeftCoord(KMaxLatitudeMapBound, KMinLongitudeMapBound);
	TCoordinate imageBottomRightCoord(KMinLatitudeMapBound, KMaxLongitudeMapBound);
	TPoint topLeft;
	TPoint bottomRight;
	if (iMapView->CheckCoordVisibility(imageTopLeftCoord))
		topLeft = iMapView->GeoCoordsToScreenCoords(imageTopLeftCoord);
	else
		topLeft = iMapView->Rect().iTl;
	if (iMapView->CheckCoordVisibility(imageBottomRightCoord))
		bottomRight = iMapView->GeoCoordsToScreenCoords(imageBottomRightCoord);
	else
		bottomRight = iMapView->Rect().iBr;
	TRect destRect = TRect(topLeft, bottomRight);
	
	
	TCoordinate viewTopLeftCoord = iMapView->ScreenCoordsToGeoCoords(iMapView->Rect().iTl);
	TCoordinate viewBottomRightCoord = iMapView->ScreenCoordsToGeoCoords(iMapView->Rect().iBr);
	TTileReal topLeftTileReal = MapMath::GeoCoordsToTileReal(viewTopLeftCoord, 0);
	TTileReal bottomRightTileReal = MapMath::GeoCoordsToTileReal(viewBottomRightCoord, 0);
	TReal srcRectTlX = topLeftTileReal.iX * KStubImageSize;
	Math::Round(srcRectTlX, srcRectTlX, 0); // ToDo: Check possible errors
	TReal srcRectTlY = topLeftTileReal.iY * KStubImageSize;
	Math::Round(srcRectTlY, srcRectTlY, 0); // ToDo: Check possible errors
	TReal srcRectBrX = bottomRightTileReal.iX * KStubImageSize;
	Math::Round(srcRectBrX, srcRectBrX, 0); // ToDo: Check possible errors
	TReal srcRectBrY = bottomRightTileReal.iY * KStubImageSize;
	Math::Round(srcRectBrX, srcRectBrX, 0); // ToDo: Check possible errors
	TRect srcRect = TRect(
		srcRectTlX,
		srcRectTlY,
		srcRectBrX,
		srcRectBrY
	);
	
	
	aGc.DrawBitmap(destRect, iBackgroundBitmap, srcRect);
	}

void CMapLayerStub::DrawMarks(CWindowGc &aGc)
	{
	const TInt KMarkSize = 5;
	TSize penSize(KMarkSize, KMarkSize);
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc.SetPenColor(KRgbRed);
	aGc.SetPenSize(penSize);
	
	TInt i;
	for (i = 0; i < iCoords.Count(); i++)
		{
		TCoordinate coord = iCoords[i];
		if (iMapView->CheckCoordVisibility(coord)) // Check before drawing
			{
			TPoint point;
			point = iMapView->GeoCoordsToScreenCoords(coord);
			aGc.Plot(point);
			}
		}
	}


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

void MImageReaderObserver::OnImageReadingFailed(TInt aErr)
	{
	// No any action by default
	}
