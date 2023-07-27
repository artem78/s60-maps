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
		iIsSignalIndicatorVisible(ETrue),
		iIsScaleBarVisible(ETrue),
		iSignalIndicatorType(ESignalIndicatorGeneralType)
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
	
	// Added in version 1.8
	aStream << (TInt8) iIsSignalIndicatorVisible; // No built-in bool to stream conversion
	aStream << (TInt8) iIsScaleBarVisible;
	
	// Added in version X.X
	aStream << static_cast<TInt8>(iSignalIndicatorType);
	}

void CSettings::DoInternalizeL(RReadStream& aStream)
	{
	// FixMe: Reads mess in new settings for old config file (make sure to validate them)
	
	TCardinality zoom, language;
	TInt8 isLandmarksVisible, isSignalIndicatorVisible, isScaleBarVisible,
		signalIndicatorType;
	
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
	
	// Added in version 1.8
	aStream >> isSignalIndicatorVisible;
	iIsSignalIndicatorVisible = (TBool) isSignalIndicatorVisible;
	aStream >> isScaleBarVisible;
	iIsScaleBarVisible = (TBool) isScaleBarVisible;
	
	// Added in version X.X
	aStream >> signalIndicatorType;
	iSignalIndicatorType = static_cast<TSignalIndicatorType>(signalIndicatorType);
	}

void CSettings::InternalizeL(RReadStream& aStream)
	{
	TRAPD(r, DoInternalizeL(aStream)); // Ignore any errors
	if (r != KErrNone)
		{
		WARNING(_L("Settings file broken or incomplete (code: %d)"), r);
		}
	}
