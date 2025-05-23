/*
 ============================================================================
 Name		: S60MapsAppUi.h
 Author	  : artem78
 Copyright   : 
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef __S60MAPSAPPUI_h__
#define __S60MAPSAPPUI_h__

// INCLUDES
#include <aknviewappui.h>
#include <f32file.h>
#include "Positioning.h"

// For media keys handling
#include <remconcoreapitargetobserver.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>

#include "Map.h" // For tile providers
#include "Settings.h"
#include "SettingsView.h"
//#include <aknprogressdialog.h> // For CAknProgressDialog
#include <aknwaitdialog.h> // For CAknWaitDialog 
#include <epos_cposlandmarkdatabase.h> // For CPosLandmarkDatabase
#include <lbssatellite.h>
#include <hwrmlight.h> // For CHWRMLight

// FORWARD DECLARATIONS
class CMapView;

// CLASS DECLARATION
/**
 * CS60MapsAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CS60MapsAppUi : public CAknViewAppUi, public MAsyncFileManObserver,
		public MPositionListener, public MRemConCoreApiTargetObserver,
		public MProgressDialogCallback
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CS60MapsAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CS60MapsAppUi();

	/**
	 * ~CS60MapsAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CS60MapsAppUi();

private:
	// Functions from base classes
	
	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL(TInt aCommand);

//	/**
//	 * From CEikAppUi, HandleCommandL.
//	 * Takes care of command handling.
//	 * @param aCommand Command to be handled.
//	 */
//	void HandleCommandL(TInt aCommand);

	/**
	 *  HandleStatusPaneSizeChange.
	 *  Called by the framework when the application status pane
	 *  size is changed.
	 */
	void HandleStatusPaneSizeChange();

	/**
	 *  From CCoeAppUi, HelpContextL.
	 *  Provides help context for the application.
	 *  size is changed.
	 */
	CArrayFix<TCoeHelpContext>* HelpContextL() const;
	
	//void PrepareToExit();
	
	// From CAknViewAppUi
	void HandleForegroundEventL(TBool aForeground);

private:
	// Views
	CMapView* iMapView;
	CSettingsView* iSettingsView;

public:
	TStreamId StoreL(CStreamStore& aStore) const;
	void RestoreL(const CStreamStore& aStore, TStreamId aStreamId);
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);

	// MAsyncFileManObserver
public:
	MFileManObserver::TControl NotifyFileManStarted();
	MFileManObserver::TControl NotifyFileManOperation();
	MFileManObserver::TControl NotifyFileManEnded();
	void OnFileManFinished(TInt aStatus);
	
	// MPositionListener
public:
	void OnPositionUpdated();
	void OnPositionPartialUpdated();
	void OnPositionRestored();
	void OnPositionLost();
	void OnPositionError(TInt aErrCode);
	
	// MRemConCoreApiTargetObserver
public:
	void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
			TRemConCoreApiButtonAction aButtonAct);
	
	// MProgressDialogCallback
public:
	 void DialogDismissedL(TInt aButtonId);
	
	// Custom properties and methods
public:
	inline const TFixedArray<TTileProvider*, 6>& AvailableTileProviders()
			{ return iAvailableTileProviders;  };
	inline TTileProvider* /*Active*/TileProvider()
			{ return iActiveTileProvider; };
	void /*SetActiveTileProvider*/ SetTileProvider(TTileProvider* aTileProvider)
			{ iActiveTileProvider = aTileProvider; };
	
	void ClearTilesCacheL();
	void SaveAndExitL();
	void SendAppToBackground(); // Hide application window
	CPosLandmark* GetNearestLandmarkL(const TCoordinate &aCoord, TBool aPartial = ETrue,
			TReal32 aMaxDistance = KNaN); // The client takes ownership of the returned landmark object. Returns NULL if nothing found.
	void ChangeLanguageL(TLanguage aLang);
	TBool IsLanguageExists(TLanguage aLang);
	void AvailableLanguagesL(RArray<TLanguage> &aLangArr);
	
	void ShowStatusPaneAndHideMapControlL(TInt aPaneTitleResourceId);
	void HideStatusPaneAndShowMapControlL();

private:
	CSettings* iSettings;
	CAsyncFileMan* iFileMan;
	CPositionRequestor* iPosRequestor;
	
	CRemConInterfaceSelector* iInterfaceSelector;
	CRemConCoreApiTarget* iCoreTarget;
	
	TFixedArray<TTileProvider*, 6> iAvailableTileProviders;
	//TBuf<64> iTileProviderId
	TTileProvider* iActiveTileProvider;
	
	CAknWaitDialog* iCacheClearingWaitDialog;
	//CAknProgressDialog* iCacheResetProgressDialog;
	//CPeriodic* iCacheResetProgressChecker;
	CPosLandmarkDatabase* iLandmarksDb;
	CPosLmPartialReadParameters* iLandmarkPartialParameters;
	TInt iResourceOffset;
	CHWRMLight* iLight;
	CPeriodic* iResetInactivityTimer;
	HBufC* iOriginalPaneTitle;
	
	//static TInt UpdateTilesClearingProgress(TAny* aSelfPtr);
	
	TLanguage PreferredLanguage();
	static TInt ResetInactivityTimer(TAny* aPtr);
	void EnableInfiniteBacklight();
	void DisableInfiniteBacklight();
	void EnableScreenSaver();
	void DisableScreenSaver();

	
	// Command handlers
private:
	void HandleExitL();

	
public:
	inline /*const*/ CSettings* Settings()
			{ return iSettings; }
	
	inline CPosLandmarkDatabase* LandmarkDb()
			{ return iLandmarksDb; }
	
//	inline CPositionRequestor Positionrequestor()
//			{ return iPosRequestor; }
	
	inline TBool IsPositioningAvailable()
			{ return (TBool) iPosRequestor; }
	
	inline TBool IsPositionRecieved()
			{ return IsPositioningAvailable() && iPosRequestor->IsPositionRecieved(); }
	
	inline CMapView* MapView()
			{ return iMapView; }
	
	inline CSettingsView* SettingsView()
			{ return iSettingsView; }
	
	inline const TPositionSatelliteInfo* SatelliteInfo()
			{ return IsPositioningAvailable() && iPosRequestor->LastKnownPositionInfo()->PositionClassType() & EPositionSatelliteInfoClass ?
					static_cast<const TPositionSatelliteInfo*>(iPosRequestor->LastKnownPositionInfo()) :
					NULL;
			}
	
	};

#endif // __S60MAPSAPPUI_h__
// End of File
