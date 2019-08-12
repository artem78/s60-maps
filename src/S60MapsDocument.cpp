/*
 ============================================================================
 Name		: S60MapsDocument.cpp
 Author	  : artem78
 Copyright   : 
 Description : CS60MapsDocument implementation
 ============================================================================
 */

// INCLUDE FILES
#include "S60MapsAppUi.h"
#include "S60MapsDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CS60MapsDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CS60MapsDocument* CS60MapsDocument::NewL(CEikApplication& aApp)
	{
	CS60MapsDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CS60MapsDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CS60MapsDocument* CS60MapsDocument::NewLC(CEikApplication& aApp)
	{
	CS60MapsDocument* self = new (ELeave) CS60MapsDocument(aApp);

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CS60MapsDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CS60MapsDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CS60MapsDocument::CS60MapsDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CS60MapsDocument::CS60MapsDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CS60MapsDocument::~CS60MapsDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CS60MapsDocument::~CS60MapsDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CS60MapsDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CS60MapsDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new (ELeave) CS60MapsAppUi;
	}

// End of File
