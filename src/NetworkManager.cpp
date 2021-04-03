/*
 ============================================================================
 Name		: NetworkManager.cpp
 Author	  : artem78
 Version	 : 
 Copyright   : 
 Description : CNetworkManager implementation
 ============================================================================
 */

#include "NetworkManager.h"
#include "Defs.h"

CNetworkManager::CNetworkManager(CSettings* aSettings,
			MHTTPClientObserver* aHttpClientObserver) :
		iIsOfflineMode(EFalse),
		iHttpClientObserver(aHttpClientObserver),
		iHttpClient(NULL),
		iSettings(aSettings)
	{
	// No implementation required
	}

CNetworkManager::~CNetworkManager()
	{
	delete iHttpClient;
	}

CNetworkManager* CNetworkManager::NewLC(CSettings* aSettings,
		MHTTPClientObserver* aHttpClientObserver)
	{
	CNetworkManager* self = new (ELeave) CNetworkManager(aSettings, aHttpClientObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetworkManager* CNetworkManager::NewL(CSettings* aSettings,
		MHTTPClientObserver* aHttpClientObserver)
	{
	CNetworkManager* self = CNetworkManager::NewLC(aSettings, aHttpClientObserver);
	CleanupStack::Pop(); // self;
	return self;
	}

void CNetworkManager::ConstructL()
	{
	}

void CNetworkManager::CreateHttpClientL()
	{
	if (iHttpClient) return; // Already created
	
	iHttpClient = CHTTPClient::NewL(iHttpClientObserver);
	
	// Set user agent
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
	iHttpClient->SetUserAgentL(userAgent);
	}

CHTTPClient* CNetworkManager::HttpClient()
	{
	// Defered construction
	if (!iHttpClient)
		TRAP_IGNORE(CreateHttpClientL());
	
	return iHttpClient;
	}

