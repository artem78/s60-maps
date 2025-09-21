/*
 ============================================================================
 Name		: HTTPClient2.h
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : CHTTPClient2 declaration
 ============================================================================
 */

#ifndef HTTPCLIENT2_H
#define HTTPCLIENT2_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "HTTPClient.h"
#include <es_sock.h>

// CLASS DECLARATION

/*
 * Adds https url proxyfication ability for CHTTPClient
 */
class CHTTPClient2 : public CHTTPClient
	{
	// Constructors and destructor
public:
	static CHTTPClient2* NewL(MHTTPClientObserver* aObserver, RSocketServ &aSockServ, RConnection &aConn);
	static CHTTPClient2* NewLC(MHTTPClientObserver* aObserver, RSocketServ &aSockServ, RConnection &aConn);

private:
	CHTTPClient2(MHTTPClientObserver* aObserver);
	void ConstructL(RSocketServ &aSockServ, RConnection &aConn);
	
	// From CHTTPClient
private:
	void SendRequestL(THTTPMethod aMethod, const TDesC8 &aUrl);
	void SetUserAgentL(const TDesC8 &aDes); /* make private, not needed to be visible
											outside because was set in ContructL() */
	
	// New
private:
	static HBufC8* ProxyfiHttpsUrlL(const TDesC8 &aUrl);
	};

#endif // HTTPCLIENT2_H
