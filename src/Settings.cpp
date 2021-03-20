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
		iFullScreen(ETrue)
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
	aStream << (TInt8) iFullScreen; // No buil in bool to stream conversion
	}

void CSettings::DoInternalizeL(RReadStream& aStream)
	{
	TCardinality zoom;
	TInt8 fullScreen;
	
	aStream >> iLat;
	aStream >> iLon;
	aStream >> zoom;
	iZoom = (TInt) zoom;
	aStream >> iTileProviderId;
	aStream >> fullScreen;
	iFullScreen = (TBool) fullScreen;
	}

void CSettings::InternalizeL(RReadStream& aStream)
	{
	TRAPD(r, DoInternalizeL(aStream)); // Ignore any errors
	if (r != KErrNone)
		{
		WARNING(_L("Settings file broken or incomplete (code: %d)"), r);
		}
	}
