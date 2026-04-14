/*
 ============================================================================
 Name		: UpdateChecker.h
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : CUpdateChecker declaration
 ============================================================================
 */

#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "HTTPClient2.h"
#include "JsonParser.h"
#include "Utils.h"


// Forward declarations
class MUpdateCheckerObserver;

// CLASS DECLARATION

/**
 *  CUpdateChecker
 * 
 */
class CUpdateChecker : public CBase, public MHTTPClientObserver
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CUpdateChecker();

	/**
	 * Two-phased constructor.
	 */
	static CUpdateChecker* NewL(MUpdateCheckerObserver* aObserver);

	/**
	 * Two-phased constructor.
	 */
	static CUpdateChecker* NewLC(MUpdateCheckerObserver* aObserver);

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CUpdateChecker(MUpdateCheckerObserver* aObserver);

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();
	
	
	// From MHTTPClientObserver
private:
	virtual void OnHTTPResponseDataChunkRecieved(const RHTTPTransaction aTransaction,
			const TDesC8 &aDataChunk, TInt anOverallDataSize, TBool anIsLastChunk);
	virtual void OnHTTPResponse(const RHTTPTransaction aTransaction);
	virtual void OnHTTPError(TInt aError, const RHTTPTransaction aTransaction);
	virtual void OnHTTPHeadersRecieved(const RHTTPTransaction aTransaction);
	
	
	// New members
private:
	CHTTPClient2* iHttpClient;
	HBufC8* iResponseBuff;
	MUpdateCheckerObserver* iObserver; // NOT owned
	
	void ProcessResponseL();
	void ParseJsonValueL(CJsonParser* aParser, const TDesC &aParam, TDes &aVal);
	
public:
	void LoadDataL();

	};

class MUpdateCheckerObserver
	{
protected:
	virtual void OnUpdateCheckSuccessL(const TVersionEx& aVersion, const /*TTime&*/ TDesC& aDateTime, 
			const TDesC& aDescription, const TDesC& aDownloadUrl) = 0;
	virtual void OnUpdateCheckFailedL() = 0;
	
	friend class CUpdateChecker;
	};

#endif // UPDATECHECKER_H
