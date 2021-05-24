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
#include <aknappui.h>
#include <f32file.h>
#include "Positioning.h"

// For media keys handling
#include <remconcoreapitargetobserver.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>

#include "Map.h" // For tile providers
#include "Settings.h"
//#include <aknprogressdialog.h> // For CAknProgressDialog
#include <aknwaitdialog.h> // For CAknWaitDialog 
#include <epos_cposlandmarkdatabase.h> // For CPosLandmarkDatabase

// FORWARD DECLARATIONS
class CS60MapsAppView;

// CLASS DECLARATION
/**
 * CS60MapsAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CS60MapsAppUi : public CAknAppUi, public MAsyncFileManObserver,
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
	
	void DynInitMenuPaneL(TInt aMenuID, CEikMenuPane* aMenuPane);

private:
	// Data

	/**
	 * The application view
	 * Owned by CS60MapsAppUi
	 */
	CS60MapsAppView* iAppView;

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
private:
	CSettings* iSettings;
	CAsyncFileMan* iFileMan;
	CPositionRequestor* iPosRequestor;
	
	CRemConInterfaceSelector* iInterfaceSelector;
	CRemConCoreApiTarget* iCoreTarget;
	
	TFixedArray<TTileProvider*, /*5*/ 4> iAvailableTileProviders;
	//TBuf<64> iTileProviderId
	TTileProvider* iActiveTileProvider;
	
	CAknWaitDialog* iCacheClearingWaitDialog;
	//CAknProgressDialog* iCacheResetProgressDialog;
	//CPeriodic* iCacheResetProgressChecker;
	CPosLandmarkDatabase* iLandmarksDb;
	CPosLmPartialReadParameters* iLandmarkPartialParameters;
	
	void ClearTilesCacheL();
	//static TInt UpdateTilesClearingProgress(TAny* aSelfPtr);
	void SendAppToBackground(); // Hide application window
	
	// Command handlers
	void HandleExitL();
	void HandleFindMeL();
	void HandleTileProviderChangeL(TInt aTileProviderIdx);
	void HandleTilesCacheStatsL();
	void HandleTilesCacheResetL();
#ifdef _HELP_AVAILABLE_
	void HandleHelpL();
#endif
	void HandleAboutL();
	void HandleToggleLandmarksVisibility();
	
public:
	inline const CSettings* Settings()
			{ return iSettings; }
	
	inline CPosLandmarkDatabase* LandmarkDb()
			{ return iLandmarksDb; }
	
	};

#endif // __S60MAPSAPPUI_h__
// End of File
