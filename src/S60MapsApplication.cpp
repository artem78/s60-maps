/*
 ============================================================================
 Name		: S60MapsApplication.cpp
 Author	  : artem78
 Copyright   : 
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "S60Maps.hrh"
#include "S60MapsDocument.h"
#include "S60MapsApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CS60MapsApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CS60MapsApplication::CreateDocumentL()
	{
	// Create an S60Maps document, and return a pointer to it
	return CS60MapsDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CS60MapsApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CS60MapsApplication::AppDllUid() const
	{
	// Return the UID for the S60Maps application
	return KUidS60MapsApp;
	}

// End of File
