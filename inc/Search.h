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
#include "badesca.h"

// CLASS DECLARATION

/**
 *  CSearch
 * 
 */
class CSearch : public CBase
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
	static CSearch* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CSearch* NewLC();

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CSearch();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();
	
// New members
	
private:
	TBuf<128> iQuery;
	TCoordinate iCoord;
	
	TBool RunQueryDialogL();
	TBool RunResultsDialogL();
	void ParseApiResponseL(CDesCArray* aNamesArr, CArrayFix<TCoordinate>* aCoordsArr);
	
public:
	TBool RunL(TCoordinate &aCoord);

	};

#endif // SEARCH_H
