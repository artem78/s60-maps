/*
 ============================================================================
 Name		: Settings.cpp
 Author	  : artem78
 Version	 :
 Copyright   : 
 Description : CSettings implementation
 ============================================================================
 */

#include "Settings.h"
#include "Logger.h"


// Constants

// Center of Europe
const TReal64 KDefaultLat = 50.0;
const TReal64 KDefaultLon = 9.0;
const TZoom KDefaultZoom = 3;
_LIT(KDefaultTileProviderId, "osm");


CSettings::CSettings() :
		// Set default values
		iLat(KDefaultLat),
		iLon(KDefaultLon),
		iZoom(KDefaultZoom),
		iTileProviderId(KDefaultTileProviderId),
		iIsLandmarksVisible(EFalse),
		iLanguage(ELangEnglish),
		iIsSignalIndicatorVisible(ETrue)
	{
	}

//CSettings::~CSettings()
//	{
//	}
//
//CSettings* CSettings::NewLC()
//	{
//	CSettings* self = new (ELeave) CSettings();
//	CleanupStack::PushL(self);
//	self->ConstructL();
//	return self;
//	}
//
//CSettings* CSettings::NewL()
//	{
//	CSettings* self = CSettings::NewLC();
//	CleanupStack::Pop(); // self;
//	return self;
//	}
//
//void CSettings::ConstructL()
//	{
//
//	}

void CSettings::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iLat;
	aStream << iLon;
	aStream << TCardinality(iZoom);
	aStream << iTileProviderId;
	aStream << (TInt8) iIsLandmarksVisible;

	// Added in version 1.7
	aStream << TCardinality(iLanguage);
	
	// Added in develop
	aStream << (TInt8) iIsSignalIndicatorVisible; // No built-in bool to stream conversion
	}

void CSettings::DoInternalizeL(RReadStream& aStream)
	{
	// FixMe: Reads mess in new settings for old config file (make sure to validate them)
	
	TCardinality zoom, language;
	TInt8 isLandmarksVisible, isSignalIndicatorVisible;
	
	aStream >> iLat;
	aStream >> iLon;
	aStream >> zoom;
	iZoom = (TInt) zoom;
	aStream >> iTileProviderId;
	aStream >> isLandmarksVisible;
	iIsLandmarksVisible = (TBool) isLandmarksVisible;

	// Added in version 1.7
	aStream >> language;
	iLanguage = static_cast<TLanguage>((TInt) language);
	
	// Added in develop
	aStream >> isSignalIndicatorVisible;
	iIsSignalIndicatorVisible = (TBool) isSignalIndicatorVisible;
	}

void CSettings::InternalizeL(RReadStream& aStream)
	{
	TRAPD(r, DoInternalizeL(aStream)); // Ignore any errors
	if (r != KErrNone)
		{
		WARNING(_L("Settings file broken or incomplete (code: %d)"), r);
		}
	}
