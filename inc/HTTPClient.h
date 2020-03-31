/*
 ============================================================================
 Name		: HTTPClient.h
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : CHTTPClient declaration
 ============================================================================
 */

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <http.h>

// FORWARD DECLARATION
class MHTTPClientObserver; 

// CLASS DECLARATION

/**
 *  CHTTPClient
 * 
 */
// ToDo: Allow to cancel ongoing request
class CHTTPClient : public CBase
	{
	// Constructors and destructor
public:
	~CHTTPClient();
	static CHTTPClient* NewL(MHTTPClientObserver* aObserver);
	static CHTTPClient* NewLC(MHTTPClientObserver* aObserver);

private:
	CHTTPClient(MHTTPClientObserver* aObserver);
	void ConstructL();
	
	// Custom properties and methods
public:
	// ToDo: Add other methods (POST, HEAD, etc...)
	void GetL(const TDesC8 &aUrl);
	void SetUserAgentL(const TDesC8 &aDes);
	
private:
	// Enum
	enum THTTPMethod
		{
		// ToDo: Add other methods (POST, HEAD, etc...)
		EGet
		};
	
	RHTTPSession iSession;
	MHTTPClientObserver* iObserver;
	
	void SendRequestL(THTTPMethod aMethod, const TDesC8 &aUrl);
	void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8 &aHdrValue);
	};


class MHTTPClientObserver : public MHTTPTransactionCallback
	{
	// Inherited from MHTTPTransactionCallback
private:
	virtual void MHFRunL(RHTTPTransaction aTransaction,
			const THTTPEvent& aEvent);
	virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction,
			const THTTPEvent& aEvent);
	
	// Custom properties and methods
public:
	virtual void OnHTTPResponseDataChunkRecieved(const RHTTPTransaction aTransaction,
			const TDesC8 &aDataChunk, TInt anOverallDataSize, TBool anIsLastChunk) = 0;
	virtual void OnHTTPResponse(const RHTTPTransaction aTransaction) = 0;
	virtual void OnHTTPError(TInt aError, const RHTTPTransaction aTransaction) /*= 0*/;
	virtual void OnHTTPHeadersRecieved(const RHTTPTransaction aTransaction) = 0;
	};



#endif // HTTPCLIENT_H
