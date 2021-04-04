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
#include <CommDbConnPref.h>
#include "Logger.h"

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
	iConnection.Close();
	iSocketServ.Close();
	
	DEBUG(_L("Network Manager destroyed"));
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
	User::LeaveIfError(iSocketServ.Connect());
	User::LeaveIfError(iConnection.Open(iSocketServ));
	
	DEBUG(_L("Network Manager constructed"));
	}

void CNetworkManager::CreateHttpClientL()
	{
	if (iHttpClient) return; // Already created
	
	iHttpClient = CHTTPClient::NewL(iHttpClientObserver, iSocketServ, iConnection);
	
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
	
	DEBUG(_L("Http Client created"));
	}

CHTTPClient* CNetworkManager::HttpClient()
	{
	// Defered construction
	if (!iHttpClient)
		TRAP_IGNORE(CreateHttpClientL());
	
	return iHttpClient;
	}

void CNetworkManager::UpdateConnectionSettings()
	{
	DEBUG(_L("Start update connection settings"));
	
	iConnection.Stop();
	DEBUG(_L("Network connection stopped"));
	
	switch (iSettings->iIapConnMode)
		{
		case CSettings::ESpecified:
			{
			TCommDbConnPref connPref;
			
			connPref.SetIapId(iSettings->iIapId);
			connPref.SetDialogPreference(/*ECommDbDialogPrefPromptIfWrongMode*/ ECommDbDialogPrefDoNotPrompt);
			//connPref.SetDirection(); // ???
			//connPref.SetBearerSet(ECommDbBearerGPRS | ECommDbBearerWLAN);
			
			// Restart connection with updated preferences
			iConnection.Start(connPref); // ToDo: Check error code
			DEBUG(_L("Network connection started with user specified IAP"));
			}
			break;
			
		case CSettings::EAlwaysAsk:
			{
			// Restart connection with default CommDb preferences
			iConnection.Start(); // ToDo: Check error code
			DEBUG(_L("Network connection started with default preferences"));
			}
			break;
			
		case CSettings::ENotUse:
			{
			// No need to restart connection (do nothing)
			}
			break;
		}
	}


