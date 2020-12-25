/*
 ============================================================================
 Name		: Settings.cpp
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : CSettings implementation
 ============================================================================
 */

#include "Settings.h"


// Constants

// Center of Europe
const TReal64 KDefaultLat = 47.100;
const TReal64 KDefaultLon = 5.361;
const TZoom KDefaultZoom = 2;
_LIT(KDefaultTileProviderId, "osm");


CSettings::CSettings() :
		// Set default values
		iLat(KDefaultLat),
		iLon(KDefaultLon),
		iZoom(KDefaultZoom),
		iTileProviderId(KDefaultTileProviderId)
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
	}

void CSettings::InternalizeL(RReadStream& aStream)
	{
	TRAP_IGNORE(aStream >> iLat);
	TRAP_IGNORE(aStream >> iLon);
	TCardinality zoom;
	TRAP_IGNORE(aStream >> zoom);
	iZoom = (TInt) zoom;
	TRAP_IGNORE(aStream >> iTileProviderId);
	}
