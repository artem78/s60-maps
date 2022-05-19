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
#include "f32file.h"
//#include "Logger.h"

#include <coemain.h>
#include "FileUtils.h"

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

void CS60MapsApplication::GetDefaultDocumentFileName(TFileName& aDocumentName) const
	{
	aDocumentName.Zero();
	DataDir(aDocumentName);
	_LIT(KDocFileName, "store.dat");
	RelPathToAbsFromDataDir(KDocFileName, aDocumentName);	
	}

void CS60MapsApplication::DataDir(TFileName &aDataDir) const
	{
#ifdef __WINSCW__
	// Emulator do not have E drive, use C instead
	_LIT(KProgramDataDir, "c:\\data\\S60Maps\\");
#else
	// Change data directory to e:\data\S60Maps
	// Note: program data need to be stored on E drive
	// regardless of on which drive program installed
	// because tiles cache will be use much space
	_LIT(KProgramDataDir, "e:\\data\\S60Maps\\");
#endif
	aDataDir.Copy(KProgramDataDir);
	}

void CS60MapsApplication::RelPathToAbsFromDataDir(const TDesC &aRelPath, TFileName &anAbsPath) const
	{
	DataDir(anAbsPath);
	anAbsPath.Append(aRelPath);
	
	//DEBUG(_L("%S --> %S"), &aRelPath, &anAbsPath);
	}

void CS60MapsApplication::CacheDir(TFileName &aCacheDir) const
	{
	_LIT(KCacheDirRel, "cache\\_PAlbTN\\");
	RelPathToAbsFromDataDir(KCacheDirRel, aCacheDir);
	}

void CS60MapsApplication::IconFileL(TFileName &aFileName) const
	{
	_LIT(KMbmFilePathFmt, "%c:%Sicons.mbm");
	
	TFileName privateDir;
	User::LeaveIfError(CCoeEnv::Static()->FsSession().PrivatePath(privateDir));
	aFileName.Format(KMbmFilePathFmt, FileUtils::InstallationDrive(), &privateDir);
	}

CAknIcon* CS60MapsApplication::LoadIconL(TInt aBitmapId, TInt aMaskId)
	{
	CAknIcon* icon = CAknIcon::NewL();
	CleanupStack::PushL(icon);
	TFileName mbmFilePath;
	IconFileL(mbmFilePath);
	CFbsBitmap* bitmap = NULL;
	CFbsBitmap* mask = NULL;
	AknIconUtils::CreateIconL(bitmap, mask, mbmFilePath, aBitmapId, aMaskId);
	icon->SetBitmap(bitmap);
	icon->SetMask(mask);
	CleanupStack::Pop(icon); // not ...AndDestroy()
	return icon;
	}

// End of File
