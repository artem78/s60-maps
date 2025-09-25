/*
 ============================================================================
 Name		: Settings.cpp
 Author	  : artem78
 Version	 :
 Copyright   : 
 Description : CSettings implementation
 
 
  Data structure in stream:
 
  |---------- [40 bytes] reserved for headers -------|------------ Settings data ---------|
  |------|--|----|--|--------------------------------|
     ^    ^   ^   ^     ^--- [24 bytes] Zeroes (unused)
     |    |   |   +----- [2 bytes]  Config file version (TUint16)
     |    |   +--------- [4 bytes]  Program version (TVersion)
     |    +------------- [2 bytes]  Settings data length (TUint16, max 64KB)
     +------------------ [8 bytes]  "Magic number" to identify new config structure
 
 
 ============================================================================
 */

#include "Settings.h"
#include "Logger.h"
#include "Utils.h"
#include <s32mem.h>


// Constants

// Center of Europe
const TReal64 KDefaultLat = 50.0;
const TReal64 KDefaultLon = 9.0;
const TZoom KDefaultZoom = 3;
_LIT(KDefaultTileProviderId, "osm");
_LIT/*8*/(KDefaultHttpsProxyUrl, "http://s60maps.publicvm.com:8088/proxy?url=");
const TInt KReservedBlockLength = 40; // in bytes
const TUint64 KMagicNumber = 0x1991071619452025ULL; // 8 bytes to identify NEW config file format (after v1.15)


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
	// Create temporary memory buffer
	const TInt KMemBlockSize = /*0xFFFF*/ 1024; // 1KB should be enough at this time
	TAny* tmpPtr = User::AllocLC(KMemBlockSize);

	RMemWriteStream tmpStream(tmpPtr, KMemBlockSize);
	CleanupClosePushL(tmpStream);
	
	// Call main method
	DoExternalizeL(tmpStream);
	
	// Write headers (magic number, data length, program version, config version)
	MiscUtils::WriteTUint64ToStreamL(aStream, KMagicNumber);
	TUint16 dataLength = tmpStream.Sink()->TellL(MStreamBuf::EWrite).Offset();
//	DEBUG(_L("data length=%d"), (TInt)dataLength);
	aStream << dataLength;
	//aStream << KProgramVersion;
	aStream << KProgramVersion.iMajor;
	aStream << KProgramVersion.iMinor;
	aStream << KProgramVersion.iBuild;
	aStream << KConfigFileVersion;
	TInt nullBytesCount = KReservedBlockLength - aStream.Sink()->TellL(MStreamBuf::EWrite).Offset();
	MiscUtils::WriteZeroesToStreamL(aStream, nullBytesCount);
//	DEBUG(_L("begin data offset=%d"), (TInt)(aStream.Sink()->TellL(MStreamBuf::EWrite).Offset()));
	
	aStream.WriteL((TUint8*)tmpPtr, dataLength);
	
//	DEBUG(_L("end data offset=%d"), (TInt)(aStream.Sink()->TellL(MStreamBuf::EWrite).Offset()));
	
	CleanupStack::PopAndDestroy(2, tmpPtr);
	}

// Save setting to file
void CSettings::DoExternalizeL(RWriteStream& aStream) const
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
	
	// Added in version 1.16
	MiscUtils::WriteTUint64ToStreamL(aStream, iTotalBytesRecieved);
	MiscUtils::WriteTUint64ToStreamL(aStream, iTotalBytesSent);
	
	/* Do not forget to increment KConfigFileVersion value
	   in inc/Defs.h after new setting was added !!! */
	}

// Load settings from file
void CSettings::DoInternalizeL(RReadStream& aStream)
	{	
	const TStreamPos beginPos = aStream.Source()->TellL(MStreamBuf::ERead);
	
	// Try to test first 8 bytes as magic number
	TUint64 magicNumber(0);
	MiscUtils::ReadTUint64FromStreamL(aStream, magicNumber);
	TBool legacy = magicNumber != KMagicNumber;
	
	TUint16 dataLength(0);
	TVersion programVersion(0, 0, 0);
	TUint16 configFileVersion(0);
	
	if (legacy)
		{ // Return to 0 offset if config file in old format
		aStream.Source()->SeekL(MStreamBuf::ERead, beginPos);
		}
	else
		{ // Read headers and go to KReservedBlockLength offset
		aStream >> dataLength;
		
		//aStream >> configVersion;
		aStream >> programVersion.iMajor;
		aStream >> programVersion.iMinor;
		aStream >> programVersion.iBuild;
		
		aStream >> configFileVersion;
		
		TInt nullBytesCount = KReservedBlockLength - aStream.Source()->TellL(MStreamBuf::ERead).Offset() - beginPos.Offset();
		aStream.ReadL(nullBytesCount);
		}
	
	// Start reading settings data itself
	DoInternalizeL(aStream, legacy, dataLength, programVersion, configFileVersion);
	}

// Load settings from file
void CSettings::DoInternalizeL(RReadStream& aStream, TBool aLegacy, TUint16 aDataLength,
		TVersion /*aConfigVersion*/, TUint16 aConfigFileVersion)
	{
	const TStreamPos dataBeginPos = aStream.Source()->TellL(MStreamBuf::ERead);
	
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
	
	if (aLegacy) return;
	
	const TStreamPos dataEndPos = dataBeginPos + aDataLength;
	
	// Added in version 1.16
	MiscUtils::ReadTUint64FromStreamL(aStream, iTotalBytesRecieved);
	MiscUtils::ReadTUint64FromStreamL(aStream, iTotalBytesSent);
	
	//if (aStream.Source()->TellL(MStreamBuf::ERead) >= dataEndPos) return;
	if (aConfigFileVersion <= 15) return;
	
	// ...
	
	}

// Load settings from file
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
