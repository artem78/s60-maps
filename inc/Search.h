/*
 ============================================================================
 Name		: Search.h
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : CSearch declaration
 ============================================================================
 */

#ifndef SEARCH_H
#define SEARCH_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <lbsposition.h>
#include "HTTPClient2.h"
#include <badesca.h>
#include "Utils.h"



class MSearchObserver;
class TSearchResultItem;
class CJsonParser;


// CLASS DECLARATION

/**
 *  CSearch
 * 
 */
class CSearch : public CBase, MHTTPClientObserver
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CSearch();

	/**
	 * Two-phased constructor.
	 */
	static CSearch* NewL(MSearchObserver* aObserver);

	/**
	 * Two-phased constructor.
	 */
	static CSearch* NewLC(MSearchObserver* aObserver);

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CSearch(MSearchObserver* aObserver);

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();
	
	
// From MHTTPClientObserver
	
	virtual void OnHTTPResponseDataChunkRecieved(const RHTTPTransaction aTransaction,
			const TDesC8 &aDataChunk, TInt anOverallDataSize, TBool anIsLastChunk);
	virtual void OnHTTPResponse(const RHTTPTransaction aTransaction);
	virtual void OnHTTPError(TInt aError, const RHTTPTransaction aTransaction);
	virtual void OnHTTPHeadersRecieved(const RHTTPTransaction aTransaction);
	
	
// New members
	
private:
	TBuf<128> iQuery;
	CHTTPClient2* iHttpClient;
	HBufC8* iResponseBuff;
	MSearchObserver* iObserver;
	
	TBool RunQueryDialogL();
	/*TBool*/ void RunResultsDialogL();
	void ParseApiResponseL(CArrayFix<TSearchResultItem>* aResultsArr);
	void RunApiReqestL();
	static void ParseJsonValueL(CJsonParser* aParser, const TDesC &aParam, TDes &aVal);
	static void ParseJsonValueL(CJsonParser* aParser, const TDesC &aParam, TReal64 &aVal);
	
public:
	TBool RunL();

	};


class MSearchObserver
	{
protected:
	virtual void OnSearchFinished(const TSearchResultItem &aResultData) = 0;
	virtual void OnSearchFailed(/*TInt aError*/);
	
	friend class CSearch;
	};


class TSearchResultItem
	{
public:
	TBuf<256> iName;
	TCoordinate iCoord;
	TBounds iBounds;
	};

#endif // SEARCH_H
