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
	
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	
	CJsonParser* parser = new (ELeave) CJsonParser();
	CleanupStack::PushL(parser);
	
	HBufC* jsonData = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*iResponseBuff);
	CleanupStack::PushL(jsonData);

	parser->StartDecodingL(*jsonData);
	
	_LIT(KTagNamePath, "[tag_name]");
	_LIT(KDatePath, /*"[created_at]"*/ "[published_at]");
	_LIT(KSisContType, "application/vnd.symbian.install");
	_LIT(KContTypePathFmt, "[assets][%d][content_type]");
	_LIT(KAssetUrlFmt, "[assets][%d][browser_download_url]");
	_LIT(KDescrPath, "[body]");
	_LIT(KAssetsPath, "[assets]");
//#if defined(__S60_30__)
#if !defined(SYMBIAN_FLEXIBLE_ALARM) // symbian 9.1
	_LIT(KSearchStr, "symbian9.1");
#else // symbian >= 9.2
	_LIT(KSearchStr, "symbian9.2");
#endif
	_LIT(KFallbackUrl, "https://github.com/artem78/s60-maps/releases/latest");
	
	TBuf<16> tagName;
	ParseJsonValueL(parser, KTagNamePath, tagName);
	
	TVersionEx ver;
	ver.ParseL(tagName);
#ifdef __WINSCW__
	//ver.ParseL(_L("9.99")); // just for testing
#endif
	
	TBuf<32> dt;
	ParseJsonValueL(parser, KDatePath, dt);
	
	RBuf descr;
	descr.CreateL(2048);
	CleanupClosePushL(descr);
	ParseJsonValueL(parser, KDescrPath, descr);
	
	TPtrC downloadUrl(KNullDesC);
	RBuf assetUrl;
	assetUrl.CreateL(256);
	CleanupClosePushL(assetUrl);
	assetUrl.Zero();
	TInt assetsCount = parser->GetParameterCount(KAssetsPath);
	for (TInt i = 0; i < assetsCount; i++)
		{
		TBuf<48> path;
		path.Format(KContTypePathFmt, i);
		TBuf<64> contType;
		ParseJsonValueL(parser, path, contType);
		if (contType == KSisContType) // skip possible other files except SIS
			{
			path.Format(KAssetUrlFmt, i);
			ParseJsonValueL(parser, path, assetUrl);
			if (StrUtils::Contains(assetUrl, KSearchStr, ETrue)) // filter by symbian version
				{
				downloadUrl.Set(assetUrl);
				break;
				}
			}
		}
	
	__ASSERT_DEBUG(downloadUrl.Length(), Panic(ES60MapsSisDownloadUrlNotFoundPanic));
	if (!downloadUrl.Length())
		{
		downloadUrl.Set(KFallbackUrl);
		}
	
	if (appUi->Settings()->iUseHttpsProxy)
		{ // proxify url
		RBuf8 url8;
		url8.CreateL(downloadUrl.Length());
		CleanupClosePushL(url8);
		url8.Copy(downloadUrl);
		
		HBufC8* urlProxified8 = CHTTPClient2::ProxyfiHttpsUrlL(url8);
		CleanupStack::PushL(urlProxified8);
		
		RBuf urlProxified;
		urlProxified.CreateL(urlProxified8->Length());
		CleanupClosePushL(urlProxified);
		urlProxified.Copy(*urlProxified8);
		
		iObserver->OnUpdateCheckSuccessL(ver, dt, descr, urlProxified);
		
		CleanupStack::PopAndDestroy(3, &url8);
		}
	else
		{
		iObserver->OnUpdateCheckSuccessL(ver, dt, descr, downloadUrl);
		}
	
	CleanupStack::PopAndDestroy(4, parser);
	}

void CUpdateChecker::ParseJsonValueL(CJsonParser* aParser, const TDesC &aParam, TDes &aVal)
	{
	aVal = KNullDesC;
	
	if (!aParser->GetParameterValue(aParam, &aVal))
		User::Leave(KErrNotFound);
	}
