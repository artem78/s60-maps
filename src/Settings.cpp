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
#include "Utils.h"


// Constants

// Center of Europe
const TReal64 KDefaultLat = 50.0;
const TReal64 KDefaultLon = 9.0;
const TZoom KDefaultZoom = 3;
_LIT(KDefaultTileProviderId, "osm");
_LIT/*8*/(KDefaultHttpsProxyUrl, "http://s60maps.publicvm.com:8088/proxy?url=");


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
		iSignalIndicatorType(ESignalIndicatorGeneralType),
		iUseHttpsProxy(ETrue),
		iHttpsProxyUrl(KDefaultHttpsProxyUrl),
		iUseDiskCache(ETrue),
		iPositioningEnabled(ETrue)
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

// Save setting to file
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
	
	// Added in version 1.10
	aStream << static_cast<TInt8>(iSignalIndicatorType);
	
	// Added in version 1.12
	aStream << static_cast<TInt8>(iUseHttpsProxy);
	aStream << iHttpsProxyUrl;
	aStream << static_cast<TInt8>(iUseDiskCache);
	
	// Added in version 1.14
	aStream << static_cast<TInt8>(iPositioningEnabled);
	}

// Load settings from file
void CSettings::DoInternalizeL(RReadStream& aStream)
	{
	// FixMe: Reads mess in new settings for old config file (make sure to validate them)
	
	TCardinality zoom, language;
	TInt8 int8Val;
	
	aStream >> iLat;
	aStream >> iLon;
	aStream >> zoom;
	iZoom = (TInt) zoom;
	aStream >> iTileProviderId;
	aStream >> int8Val;
	iIsLandmarksVisible = (TBool) int8Val;

	// Added in version 1.7
	aStream >> language;
	iLanguage = static_cast<TLanguage>((TInt) language);
	
	// Added in version 1.8
	aStream >> int8Val;
	iIsSignalIndicatorVisible = (TBool) int8Val;
	aStream >> int8Val;
	iIsScaleBarVisible = (TBool) int8Val;
	
	// Added in version 1.10
	aStream >> int8Val;
	iSignalIndicatorType = static_cast<TSignalIndicatorType>(int8Val);
	
	// Added in version 1.12
	aStream >> int8Val;
	iUseHttpsProxy = static_cast<TBool>(int8Val);
	aStream >> iHttpsProxyUrl;
	ValidateHttpsProxyUrl();
	aStream >> int8Val;
	iUseDiskCache = static_cast<TBool>(int8Val);
	
	// Added in version 1.14
	aStream >> int8Val;
	iPositioningEnabled = static_cast<TBool>(int8Val);
	}

void CSettings::InternalizeL(RReadStream& aStream)
	{
	TRAPD(r, DoInternalizeL(aStream)); // Ignore any errors
	if (r != KErrNone)
		{
		WARNING(_L("Settings file broken or incomplete (code: %d)"), r);
		}
	}

void CSettings::ValidateHttpsProxyUrl()
	{
	// just check that string starts with "http"
	// (for better check TUriC16::Validate() may be used instead)
	// and restore to default if old domain used
	
	_LIT(KUrlStart, "http");
	_LIT(KOldProxyDomain, "s60maps.work.gd");
	if (!StrUtils::StartsWith(iHttpsProxyUrl, KUrlStart, ETrue)
			|| StrUtils::Contains(iHttpsProxyUrl, KOldProxyDomain, ETrue))
		{
		iHttpsProxyUrl = KDefaultHttpsProxyUrl;
		}
	}
