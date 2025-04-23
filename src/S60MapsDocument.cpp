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
#include "GitInfo.h"

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
	_LIT(KLogFileName, "s60maps_log.txt");
	TFileName logFilePath;
	static_cast<CS60MapsApplication *>(Application())->RelPathToAbsFromDataDir(KLogFileName, logFilePath);
	TParse pathParser;
	pathParser.Set(logFilePath, NULL, NULL);
	BaflUtils::EnsurePathExistsL(CEikonEnv::Static()->FsSession(), pathParser.DriveAndPath());
	iLogFile.Replace(CEikonEnv::Static()->FsSession(), logFilePath, EFileWrite);
#if __WINS__
	TBool autoFlush(ETrue);
#else
	TBool autoFlush(EFalse);
#endif
	iLogger = CLogger::NewL(iLogFile, CLogger::ELevelAll, CLogger::EUtf8, autoFlush);
	LoggerStatic::SetLogger(iLogger);
	
	// Write some info to log
	TBuf8<32> dateTimeBuff8;
	dateTimeBuff8.Append(_L8(__DATE__));
	dateTimeBuff8.Append(' ');
	dateTimeBuff8.Append(_L8(__TIME__));
	TBuf<32> dateTimeBuff;
	dateTimeBuff.Copy(dateTimeBuff8);
	INFO(_L("Program info: S60Maps v%S, git: %S (branch %S), build: %S"), &KProgramVersion.Name(),
			&KGITLongVersion, &KGITBranch, &dateTimeBuff);
	
	_LIT(KDateFmt, "%F%Y-%M-%D %H:%T:%S");
	TTime now;
	now.HomeTime();
	TBuf<32> nowBuf;
	now.FormatL(nowBuf, KDateFmt);
	INFO(_L("Current date and time: %S"), &nowBuf);
	
	INFO(_L("Log started"));
#endif
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
	INFO(_L("Log ended"));
	delete iLogger;
	iLogFile.Close();
#endif
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
