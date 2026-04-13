/*
 ============================================================================
 Name		: UpdateChecker.cpp
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : CUpdateChecker implementation
 ============================================================================
 */

// API docs: https://docs.github.com/en/rest/releases/releases?apiVersion=2022-11-28#get-the-latest-release

#include "UpdateChecker.h"
#include "S60MapsAppUi.h"
#include "S60Maps.pan"
#include <utf.h>


CUpdateChecker::CUpdateChecker(MUpdateCheckerObserver* aObserver)
		: iObserver(aObserver)
	{
	// No implementation required
	}

CUpdateChecker::~CUpdateChecker()
	{
	delete iHttpClient;
	}

CUpdateChecker* CUpdateChecker::NewLC(MUpdateCheckerObserver* aObserver)
	{
	CUpdateChecker* self = new (ELeave) CUpdateChecker(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CUpdateChecker* CUpdateChecker::NewL(MUpdateCheckerObserver* aObserver)
	{
	CUpdateChecker* self = CUpdateChecker::NewLC(aObserver);
	CleanupStack::Pop(); // self;
	return self;
	}

void CUpdateChecker::ConstructL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	iHttpClient = CHTTPClient2::NewL(this, appUi->iSockServ, appUi->iConn);
	}

void CUpdateChecker::LoadDataL()
	{
	_LIT8(KApiEndpointUrl, "https://api.github.com/repos/artem78/s60-maps/releases/latest");
	iHttpClient->GetL(KApiEndpointUrl);
	}

void CUpdateChecker::OnHTTPResponseDataChunkRecieved(const RHTTPTransaction /*aTransaction*/,
		const TDesC8 &aDataChunk, TInt anOverallDataSize, TBool anIsLastChunk)
	{
	if (iResponseBuff == NULL)
		{
		iResponseBuff = HBufC8::NewL(anOverallDataSize);
		}
	
	iResponseBuff->Des().Append(aDataChunk);
	
	if (anIsLastChunk)
		{
		ProcessResponseL();
		
		delete iResponseBuff;
		iResponseBuff = NULL;
		}
	}

void CUpdateChecker::OnHTTPResponse(const RHTTPTransaction /*aTransaction*/)
	{
	// empty
	}

void CUpdateChecker::OnHTTPError(TInt aError, const RHTTPTransaction /*aTransaction*/)
	{
	delete iResponseBuff;
	iResponseBuff = NULL;
	
	iObserver->OnUpdateCheckFailedL();
	}

void CUpdateChecker::OnHTTPHeadersRecieved(const RHTTPTransaction /*aTransaction*/)
	{
	// empty
	}
	
void CUpdateChecker::ProcessResponseL()
	{
	__ASSERT_DEBUG(iResponseBuff->Length(), Panic());
	
	CJsonParser* parser = new (ELeave) CJsonParser();
	CleanupStack::PushL(parser);
	
	HBufC* jsonData = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*iResponseBuff);
	CleanupStack::PushL(jsonData);

	parser->StartDecodingL(*jsonData);
	
	_LIT(KTagNamePath, "[tag_name]");
	_LIT(KDatePath, /*"[created_at]"*/ "[published_at]");
	_LIT(KSisContType, "application/vnd.symbian.install");
	_LIT(KContTypePathFmt, "[assets][%d][content_type]");
	_LIT(KSisUrlFmt, "[assets][%d][browser_download_url]");
	_LIT(KDescrPath, "[body]");
	_LIT(KAssetsPath, "[assets]");
	
	TBuf<16> tagName;
	ParseJsonValueL(parser, KTagNamePath, tagName);
	
	TVersionEx ver;
	ver.ParseL(tagName);
	
	TBuf<32> dt;
	ParseJsonValueL(parser, KDatePath, dt);
	
	RBuf descr;
	descr.CreateL(2048);
	CleanupClosePushL(descr);
	ParseJsonValueL(parser, KDescrPath, descr);
	
	RBuf url;
	url.CreateL(256);
	CleanupClosePushL(url);
	TInt assetsCount = parser->GetParameterCount(KAssetsPath);
	for (TInt i = 0; i < assetsCount; i++)
		{
		TBuf<48> path;
		path.Format(KContTypePathFmt, i);
		TBuf<64> contType;
		ParseJsonValueL(parser, path, contType);
		if (contType == KSisContType) // skip possible other files except SIS
			{
			path.Format(KSisUrlFmt, i);
			ParseJsonValueL(parser, path, url);
			break;
			}
		}
	
	iObserver->OnUpdateCheckSuccessL(ver, dt, descr, url);
	
	CleanupStack::PopAndDestroy(4, parser);
	}

void CUpdateChecker::ParseJsonValueL(CJsonParser* aParser, const TDesC &aParam, TDes &aVal)
	{
	aVal = KNullDesC;
	
	if (!aParser->GetParameterValue(aParam, &aVal))
		User::Leave(KErrNotFound);
	}
