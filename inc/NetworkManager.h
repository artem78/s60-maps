/*
 ============================================================================
 Name		: NetworkManager.h
 Author	  : artem78
 Version	 : 
 Copyright   : 
 Description : CNetworkManager declaration
 ============================================================================
 */

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "HTTPClient.h"
#include "Settings.h"

// CLASS DECLARATION

/**
 *  CNetworkManager
 * 
 */
class CNetworkManager : public CBase
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CNetworkManager();

	/**
	 * Two-phased constructor.
	 */
	static CNetworkManager* NewL(CSettings* aSettings, MHTTPClientObserver* aHttpClientObserver);

	/**
	 * Two-phased constructor.
	 */
	static CNetworkManager* NewLC(CSettings* aSettings, MHTTPClientObserver* aHttpClientObserver);

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CNetworkManager(CSettings* aSettings, MHTTPClientObserver* aHttpClientObserver);

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();
	
	
	// New properties and methods
public:
	CHTTPClient* HttpClient();
	inline TBool IsOfflineMode()
		{ return iSettings->iIapConnMode == CSettings::ENotUse || iIsOfflineMode; }
	inline void SetOfflineMode(TBool anEnabled = ETrue)
		{ iIsOfflineMode = anEnabled; }
	inline void SetHttpClientObserver(MHTTPClientObserver* aHttpClientObserver)
		{ iHttpClientObserver = aHttpClientObserver; };
	
private:
	MHTTPClientObserver* iHttpClientObserver;
	CHTTPClient* iHttpClient;
	TBool iIsOfflineMode;
	CSettings* iSettings;
	
	void CreateHttpClientL(); // Deferred construction for iHttpClient

	};

#endif // NETWORKMANAGER_H
