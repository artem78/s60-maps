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

// CLASS DECLARATION

/*
 * Adds https url proxyfication ability for CHTTPClient
 */
class CHTTPClient2 : public CHTTPClient
	{
	// Constructors and destructor
public:
	static CHTTPClient2* NewL(MHTTPClientObserver* aObserver);
	static CHTTPClient2* NewLC(MHTTPClientObserver* aObserver);

private:
	CHTTPClient2(MHTTPClientObserver* aObserver);
	
	// From CHTTPClient
private:
	void SendRequestL(THTTPMethod aMethod, const TDesC8 &aUrl);
	
	// New
private:
	static HBufC8* ProxyfiHttpsUrlL(const TDesC8 &aUrl);
	};

#endif // HTTPCLIENT2_H
