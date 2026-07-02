/*
 ============================================================================
 Name		: Routing.cpp
 Author	  : 
 Version	 : 
 Copyright   : 
 Description : CRouting implementation
 ============================================================================
 */

#include "Routing.h"
#include "S60MapsAppUi.h"
#include "ApiKeys.h"
#include <utf.h>
#include <escapeutils.h>

CRouting::CRouting(MRoutingObserver* aObserver) :
		iIsSrcSet(EFalse),
		iIsDstSet(EFalse),
		iObserver(aObserver)
	{
	}

CRouting::~CRouting()
	{
	delete iApi;
	delete iTrack;
	}

CRouting* CRouting::NewLC(MRoutingObserver* aObserver)
	{
	CRouting* self = new (ELeave) CRouting(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRouting* CRouting::NewL(MRoutingObserver* aObserver)
	{
	CRouting* self = CRouting::NewLC(aObserver);
	CleanupStack::Pop(); // self;
	return self;
	}

void CRouting::ConstructL()
	{
	iTrack = CTrack::NewL();
	iApi = COrsRoutingApi::NewL(this);
	}

void CRouting::FindRoute/*L*/()
	{
	if (not iIsSrcSet or not iIsDstSet)
		return;
	
	iTrack->Reset();
	iApi->SendRequestL(iSrcCoord, iDstCoord);
	}

void CRouting::Reset()
	{
	iIsSrcSet = EFalse;
	iIsDstSet = EFalse;
	iTrack->Reset();
	}

void CRouting::Source(TCoordinate& aSrc)
	{
	aSrc.SetCoordinate(iSrcCoord.Latitude(), iSrcCoord.Longitude());
	}

void CRouting::SetSource(const TCoordinate& aSrc)
	{
	iSrcCoord.SetCoordinate(aSrc.Latitude(), aSrc.Longitude());
	iIsSrcSet = ETrue;
	}

void CRouting::Destination(TCoordinate& aDst)
	{
	aDst.SetCoordinate(iDstCoord.Latitude(), iDstCoord.Longitude());
	}

void CRouting::SetDestination(const TCoordinate& aDst)
	{
	iDstCoord.SetCoordinate(aDst.Latitude(), aDst.Longitude());
	iIsDstSet = ETrue;
	}

void CRouting::OnRoutePointAddedL(const TCoordinate& aCoord)
	{
	iTrack->AddPointL(aCoord);
	}
void CRouting::OnFailedL()
	{
	iObserver->OnRouteFailedL();
	}


// CTrack

CTrack* CTrack::NewL()
	{
	CTrack* self = new (ELeave) CTrack();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
	}

CTrack::CTrack()
	{
	}

void CTrack::ConstructL()
	{
	const TInt KGranularity = 50;
	
	iPoints = RArray<TCoordinate>(KGranularity);
	}

CTrack::~CTrack()
	{
	iPoints.Close();
	}

void CTrack::AddPointL(const TCoordinate& aCoord)
	{
	iPoints.AppendL(aCoord);
	}

const TCoordinate& CTrack::operator[](TInt anIndex) const
	{
	return iPoints[anIndex];
	}


// COrsRoutingApi

COrsRoutingApi* COrsRoutingApi::NewL(MRoutingApiObserver* aObserver)
	{
	COrsRoutingApi* self = new (ELeave) COrsRoutingApi(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
	}

COrsRoutingApi::COrsRoutingApi(MRoutingApiObserver* aObserver)
		: iObserver(aObserver)
	{
	}

void COrsRoutingApi::ConstructL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	iHttpClient = CHTTPClient2::NewL(this, appUi->iSockServ, appUi->iConn);
	}

COrsRoutingApi::~COrsRoutingApi()
	{
	delete iHttpClient;
	}

void COrsRoutingApi::SendRequestL(const TCoordinate& aSrcCoord, const TCoordinate& aDstCoord)
	{
	TBuf8<32> srcDes, dstDes;
	
	CoordToDes8(aSrcCoord, srcDes);
	HBufC8* encodedSrc = EscapeUtils::EscapeEncodeL(srcDes, EscapeUtils::EEscapeUrlEncoded);
	CleanupStack::PushL(encodedSrc);
	
	CoordToDes8(aDstCoord, dstDes);
	HBufC8* encodedDst = EscapeUtils::EscapeEncodeL(dstDes, EscapeUtils::EEscapeUrlEncoded);
	CleanupStack::PushL(encodedDst);
	
	HBufC8* encodedApiKey = EscapeUtils::EscapeEncodeL(KORSApiKey, EscapeUtils::EEscapeUrlEncoded);
	CleanupStack::PushL(encodedApiKey);
	
	_LIT8(KApiUrlFmt, "https://api.openrouteservice.org/v2/directions/driving-car?api_key=%S&start=%S&end=%S");
	RBuf8 apiUrl;
	apiUrl.CreateL(KApiUrlFmt().Length() + encodedApiKey->Length() + encodedSrc->Length() + encodedDst->Length());
	CleanupClosePushL(apiUrl);
	
	apiUrl.Format(KApiUrlFmt, &*encodedApiKey, &*encodedSrc, &*encodedDst);
	///
	/*TInt lmax = apiUrl.MaxLength();
	TInt l = apiUrl.Length();*/
	///
	
	iHttpClient->GetL(apiUrl);
	
	CleanupStack::PopAndDestroy(4, encodedSrc);
	}

void COrsRoutingApi::CoordToDes8(const TCoordinate& aCoord, TDes8& aDes, TInt aPrecision)
	{
	TRealFormat realFmt;
	realFmt.iType = KRealFormatFixed;
	realFmt.iPlaces = /*6*/ aPrecision;
	realFmt.iPoint = '.';
	realFmt.iTriLen = 0;
	
	aDes.Zero();
	aDes.Num(aCoord.Longitude(), realFmt);
	aDes.Append(',');
	aDes.AppendNum(aCoord.Latitude(), realFmt);
	}

void COrsRoutingApi::OnHTTPResponseDataChunkRecieved(const RHTTPTransaction aTransaction,
		const TDesC8 &aDataChunk, TInt anOverallDataSize, TBool anIsLastChunk)
	{
	if (iResponseBuff == NULL)
		{
		iResponseBuff = HBufC8::NewL(anOverallDataSize);
		}
	
	iResponseBuff->Des().Append(aDataChunk);
	
//	if (anIsLastChunk)
//		{
//		/*// Close wait dialog
//		iWaitDialog->ProcessFinishedL();
//		iWaitDialog = NULL;*/
//		
//		//ProcessApiResponseAndShowResultDlgL();
//		
//		delete iResponseBuff;
//		iResponseBuff = NULL;
//		}
	}

void COrsRoutingApi::OnHTTPResponse(const RHTTPTransaction aTransaction)
	{
	TRAP_IGNORE(ProcessApiReponseL());
	
	delete iResponseBuff;
	iResponseBuff = NULL;
	}

void COrsRoutingApi::OnHTTPError(TInt aError, const RHTTPTransaction aTransaction)
	{
	delete iResponseBuff;
	iResponseBuff = NULL;
	
	/*// Close wait dialog
	iWaitDialog->ProcessFinishedL();
	iWaitDialog = NULL;*/
	
	TRAP_IGNORE(iObserver->OnFailedL());
	}

void COrsRoutingApi::OnHTTPHeadersRecieved(const RHTTPTransaction aTransaction)
	{
	
	}

void COrsRoutingApi::ProcessApiReponseL()
	{
	CJsonParser* parser = new (ELeave) CJsonParser();
	CleanupStack::PushL(parser);
	
	HBufC* jsonData = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*iResponseBuff);
	CleanupStack::PushL(jsonData);

	parser->StartDecodingL(*jsonData);
	
	_LIT(KCoordsPath, "[features][0][geometry][coordinates]");
	TInt itemsCount = parser->GetParameterCount(KCoordsPath);
	
	_LIT(KLonPathFmt, "[features][0][geometry][coordinates][%d][0]");
	_LIT(KLatPathFmt, "[features][0][geometry][coordinates][%d][1]");
	
	TReal64 lat, lon;
	TBuf<64> path;
	for (TInt i = 0; i < itemsCount; i++)
		{
		// read longitude
		path.Format(KLonPathFmt, i);
		ParseJsonValueL(parser, path, lon);
		
		// read latitude
		path.Format(KLatPathFmt, i);
		ParseJsonValueL(parser, path, lat);
		
		iObserver->OnRoutePointAddedL(TCoordinate(lat, lon));
		}
	
	// ...->Compress();
	
	CleanupStack::PopAndDestroy(2, parser);
	}

// todo: fix duplicates - https://github.com/search?q=repo%3Aartem78%2Fs60-maps%20ParseJsonValueL&type=code 
void COrsRoutingApi::ParseJsonValueL(CJsonParser* aParser, const TDesC &aParam, TReal64 &aVal)
	{
	TLex lex;
	TBuf<32> buff /*= KNullDesC*/;
	buff.Zero();
	aVal = KNaN;
	
	if (!aParser->GetParameterValue(aParam, &buff))
		User::Leave(KErrNotFound);
	lex.Assign(buff);
	User::LeaveIfError(lex.Val(aVal, '.'));
	}
