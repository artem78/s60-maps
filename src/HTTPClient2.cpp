/*
 ============================================================================
 Name		: HTTPClient2.cpp
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : CHTTPClient2 implementation
 ============================================================================
 */

#include "HTTPClient2.h"
#include "EscapeUtils.h"
#include "S60MapsAppUi.h"
#include "Utils.h"

CHTTPClient2::CHTTPClient2(MHTTPClientObserver* aObserver)
		: CHTTPClient(aObserver)
	{}

void CHTTPClient2::ConstructL(RSocketServ &aSockServ, RConnection &aConn)
	{
	// Run parent method
	CHTTPClient::ConstructL();
	
	// Set default user-agent
	TBuf8<32> userAgent;
	userAgent.Copy(_L8("S60Maps")); // ToDo: Move to constant
	userAgent.Append(' ');
	userAgent.Append('v');
	userAgent.Append(KProgramVersion.Name());
#ifdef _DEBUG
	_LIT8(KDebugStr, "DEV");
	userAgent.Append(' ');
	userAgent.Append(KDebugStr);
#endif
	SetUserAgentL(userAgent);
	
	//Connection
    RStringPool strPool = iSession.StringPool();
    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();
    connInfo.SetPropertyL(strPool.StringF(HTTP::EHttpSocketServ,
        RHTTPSession::GetTable()), THTTPHdrVal(aSockServ.Handle()));
    TInt connPtr = reinterpret_cast<TInt>(&aConn);
    connInfo.SetPropertyL(strPool.StringF(HTTP::EHttpSocketConnection,
        RHTTPSession::GetTable()), THTTPHdrVal(connPtr));
	}

CHTTPClient2* CHTTPClient2::NewLC(MHTTPClientObserver* aObserver,
		RSocketServ &aSockServ, RConnection &aConn)
	{
	CHTTPClient2* self = new (ELeave) CHTTPClient2(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSockServ,aConn);
	return self;
	}

CHTTPClient2* CHTTPClient2::NewL(MHTTPClientObserver* aObserver,
		RSocketServ &aSockServ, RConnection &aConn)
	{
	CHTTPClient2* self = CHTTPClient2::NewLC(aObserver, aSockServ, aConn);
	CleanupStack::Pop(); // self;
	return self;
	}

void CHTTPClient2::SendRequestL(THTTPMethod aMethod, const TDesC8 &aUrl)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	if (!appUi->IsNetworkConnected())
		{	
		appUi->StartNetworkConnection();
		}
	
		
	_LIT8(KHttpsUrlStart, "https://");
	if (appUi->Settings()->iUseHttpsProxy && StrUtils::StartsWith(aUrl, KHttpsUrlStart, ETrue))
		{
		DEBUG(_L("HTTPS-proxy used for request"));
		
		HBufC8* url = ProxyfiHttpsUrlL(aUrl);
		CleanupStack::PushL(url);
		CHTTPClient::SendRequestL(aMethod, *url);
		CleanupStack::PopAndDestroy(url);
		}
	else
		{
		CHTTPClient::SendRequestL(aMethod, aUrl);
		}
	}

HBufC8* CHTTPClient2::ProxyfiHttpsUrlL(const TDesC8 &aUrl)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	
	HBufC8* encodedUrl = EscapeUtils::EscapeEncodeL(aUrl, EscapeUtils::EEscapeUrlEncoded);
	CleanupStack::PushL(encodedUrl);

	HBufC8* proxifiedUrl = HBufC8::NewL(appUi->Settings()->iHttpsProxyUrl.Length() + encodedUrl->Length());
	proxifiedUrl->Des().Copy(appUi->Settings()->iHttpsProxyUrl);
	proxifiedUrl->Des().Append(*encodedUrl);
	//DEBUG(_L("%s => %s"), &aUrl, &proxifiedUrl);
	
	CleanupStack::PopAndDestroy(encodedUrl);
	
	return proxifiedUrl;
	}

void CHTTPClient2::SetUserAgentL(const TDesC8 &aDes)
	{
	CHTTPClient::SetUserAgentL(aDes);
	}
