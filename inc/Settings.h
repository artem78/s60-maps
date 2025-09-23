/*
 ============================================================================
 Name		: Settings.h
 Author	  : artem78
 Version	 :
 Copyright   : 
 Description : Settings store class
 ============================================================================
 */

#ifndef SETTINGS_H
#define SETTINGS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "Defs.h"
#include <s32strm.h>
#include "Map.h"

// Types

typedef TUint64 TBytesCount;

// CLASS DECLARATION

/**
 *  CSettings
 * 
 */
class CSettings : public CBase
	{
public:
	// Constructors and destructor

//	/**
//	 * Destructor.
//	 */
//	~CSettings();
//
//	/**
//	 * Two-phased constructor.
//	 */
//	static CSettings* NewL();
//
//	/**
//	 * Two-phased constructor.
//	 */
//	static CSettings* NewLC();

private:

//	/**
//	 * Constructor for performing 1st stage construction
//	 */
//	CSettings();
//
//	/**
//	 * EPOC default constructor for performing 2nd stage construction
//	 */
//	void ConstructL();

public:
	// Types
	enum TSignalIndicatorType {
		ESignalIndicatorGeneralType,
		ESignalIndicatorPerSatelliteType
	};
	
	
private:
	void DoExternalizeL(RWriteStream& aStream) const;
	void DoInternalizeL(RReadStream& aStream);
	void DoInternalizeL(RReadStream& aStream, TBool aLegacy, TUint16 aDataLength,
			TVersion aConfigVersion);
	
	// Parameters
	
	TReal64 iLat;
	TReal64 iLon;
	TZoom iZoom;
	TTileProviderId iTileProviderId;
	TBool iIsLandmarksVisible;
public:
	TLanguage iLanguage;
	TBool iIsSignalIndicatorVisible;
	TBool iIsScaleBarVisible;
	TSignalIndicatorType iSignalIndicatorType;
	TBool iUseHttpsProxy;
	TBuf/*8*/<0x100> iHttpsProxyUrl;
	TBool iUseDiskCache;
	TBool iPositioningEnabled;
	TBytesCount iTotalBytesRecieved;
	TBytesCount iTotalBytesSent;
	
public:
	CSettings();
	
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
	
	
	// Getters and setters for access to stored parameters
	
	inline TReal64 GetLat() const
		{ return iLat; }
	inline void SetLat(const TReal64& aLat)
		{ iLat = aLat; }
	
	inline TReal64 GetLon() const
		{ return iLon; }
	inline void SetLon(const TReal64& aLon)
		{ iLon = aLon; }
	
	inline TZoom GetZoom() const
		{ return iZoom; }
	inline void SetZoom(TZoom aZoom)
		{ iZoom = aZoom; }
	
	inline TDes& GetTileProviderId() //const
		{ return iTileProviderId; }
	inline void SetTileProviderId(const TDesC& aTileProviderId)
		{ iTileProviderId.Copy(aTileProviderId); }
	
	inline TBool GetLandmarksVisibility() const
		{ return iIsLandmarksVisible; }
	inline void SetLandmarksVisibility(TBool aVisible = ETrue)
		{ iIsLandmarksVisible = aVisible; }
	
	/*inline TBool GetSignalIndicatorVisibility() const
		{ return iIsSignalIndicatorVisible; }
	inline void SetSignalIndicatorVisibility(TBool aVisible)
		{ iIsSignalIndicatorVisible = aVisible; }*/
	
	// Check if https proxy url is valid string otherwise reset to default value
	void ValidateHttpsProxyUrl();
	
	};

#endif // SETTINGS_H
