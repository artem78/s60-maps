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
	
// New methods
public:
	void RunDialogL();

	};

#endif // SEARCH_H
