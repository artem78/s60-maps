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
#include <e32base.h>
#include "S60MapsApplication.h"
#include <eikenv.h>
#include <bautils.h>

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
#if LOGGING_ENABLED
	_LIT(KLogFileName, "log.txt");
	TFileName logFilePath;
	static_cast<CS60MapsApplication *>(Application())->RelPathToAbsFromDataDir(KLogFileName, logFilePath);
	TParse pathParser;
	pathParser.Set(logFilePath, NULL, NULL);
	BaflUtils::EnsurePathExistsL(CEikonEnv::Static()->FsSession(), pathParser.DriveAndPath());
	iLogFile.Replace(CEikonEnv::Static()->FsSession(), logFilePath, EFileWrite);
	/////////
	TBool autoFlush(ETrue);
	/////////
	iLogger = CLogger::NewL(iLogFile, CLogger::ELevelAll, CLogger::EUtf8, autoFlush);
	LoggerStatic::SetLogger(iLogger);
	LOG(_L8("Log started"));
#endif
	
	DEBUG(_L("Document created"));
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
#if LOGGING_ENABLED
	LOG(_L8("Log ended"));
	delete iLogger;
	iLogFile.Close();
#endif
	
	DEBUG(_L("Document destroyed"));
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

CFileStore* CS60MapsDocument::OpenFileL(TBool aDoOpen,
		const TDesC& aFilename, RFs& aFs)
	{
	return CEikDocument::OpenFileL(aDoOpen, aFilename, aFs);
	}

void CS60MapsDocument::StoreL(CStreamStore& aStore,
		CStreamDictionary& aStreamDic) const
	{
	TStreamId id = ((CS60MapsAppUi*)iAppUi)->StoreL(aStore);
	aStreamDic.AssignL(KUidS60MapsApp, id);
	}

void CS60MapsDocument::RestoreL(const CStreamStore& aStore,
		const CStreamDictionary& aStreamDic)
	{
	TStreamId id = aStreamDic.At(KUidS60MapsApp);
	((CS60MapsAppUi*)iAppUi)->RestoreL(aStore, id);
	}

// End of File
