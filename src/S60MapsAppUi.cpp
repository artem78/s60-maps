/*
 ============================================================================
 Name		: S60MapsAppUi.cpp
 Author	  : artem78
 Copyright   : 
 Description : CS60MapsAppUi implementation
 ============================================================================
 */

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <s32file.h>
#include <hlplch.h>

#include <S60Maps_0xED689B88.rsg>

#ifdef _HELP_AVAILABLE_
#include "S60Maps_0xED689B88.hlp.hrh"
#endif
#include "S60Maps.hrh"
#include "S60Maps.pan"
#include "S60MapsApplication.h"
#include "S60MapsAppUi.h"
#include "MapView.h"
#include "Defs.h"
#include "FileUtils.h"
//#include <eikprogi.h>
#include <epos_cposlmnearestcriteria.h>
#include <epos_cposlandmarksearch.h>
#include <apgwgnam.h>
#include <bautils.h>
#include "Utils.h"
#include "ApiKeys.h"


// Constants

const TInt KLightTarget = CHWRMLight::EPrimaryDisplay /*CHWRMLight::EPrimaryDisplayAndKeyboard*/;


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CS60MapsAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CS60MapsAppUi::ConstructL()
	{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);
	
	iSettings = new (ELeave) CSettings();
	
	_LIT8(KApiKeyArgFmt, "?apikey=%S");
	
	// Set several predefined available tiles providers

	// OpenStreetMap standard tile layer
	// https://www.openstreetmap.org/
	iAvailableTileProviders[0] = new (ELeave) TTileProvider(
			_L("osm"), _L("OpenStreetMap"),
			_L8("http://tile.openstreetmap.org/{$z}/{$x}/{$y}.png"),
			0, 19);
	
	// OpenCycleMap
	// https://wiki.openstreetmap.org/wiki/OpenCycleMap
	// https://www.thunderforest.com/maps/opencyclemap/
	_LIT8(KOpenCycleMapUrl, "http://tile.thunderforest.com/cycle/{$z}/{$x}/{$y}.png");
	RBuf8 openCycleMapUrl;
	openCycleMapUrl.CreateMaxL(KOpenCycleMapUrl().Length() + KApiKeyArgFmt().Length() + KThunderForestApiKey().Length());
	openCycleMapUrl.CleanupClosePushL();
	openCycleMapUrl.Copy(KOpenCycleMapUrl);
	if (KThunderForestApiKey().Length())
		{
		openCycleMapUrl.AppendFormat(KApiKeyArgFmt, &KThunderForestApiKey);
		}
	iAvailableTileProviders[1] = new (ELeave) TTileProvider(
			_L("opencycle"), _L("OpenCycleMap"),
			openCycleMapUrl,
			0, 22);
	CleanupStack::PopAndDestroy(&openCycleMapUrl);
	
	// Transport Map
	// https://wiki.openstreetmap.org/wiki/Transport_Map
	// https://www.thunderforest.com/maps/transport/
	_LIT8(KTransportMapUrl, "http://tile.thunderforest.com/transport/{$z}/{$x}/{$y}.png");
	RBuf8 transportMapUrl;
	transportMapUrl.CreateMaxL(KTransportMapUrl().Length() + KApiKeyArgFmt().Length() + KThunderForestApiKey().Length());
	transportMapUrl.CleanupClosePushL();
	transportMapUrl.Copy(KTransportMapUrl);
	if (KThunderForestApiKey().Length())
		{
		transportMapUrl.AppendFormat(KApiKeyArgFmt, &KThunderForestApiKey);
		}
	iAvailableTileProviders[2] = new (ELeave) TTileProvider(
			_L("transport"), _L("Transport Map"),
			transportMapUrl,
			0, 22);
	CleanupStack::PopAndDestroy(&transportMapUrl);
	
	// Humanitarian Map
	// https://wiki.openstreetmap.org/wiki/Humanitarian_map_style
	// https://www.openstreetmap.org/?layers=H
	iAvailableTileProviders[3] = new (ELeave) TTileProvider(
			_L("humanitarian"), _L("Humanitarian"),
			_L8("https://a.tile.openstreetmap.fr/hot/{$z}/{$x}/{$y}.png"),
			0, 20);
	
	// OpenTopoMap
	// https://wiki.openstreetmap.org/wiki/OpenTopoMap
	// https://opentopomap.org/
	iAvailableTileProviders[4] = new (ELeave) TTileProvider(
			_L("opentopomap"), _L("OpenTopoMap"),
			_L8("https://tile.opentopomap.org/{$z}/{$x}/{$y}.png"),
			0, /*17*/ 15);
	
	// Esri World Imagery (Clarity) Beta
	// https://wiki.openstreetmap.org/wiki/Esri
	iAvailableTileProviders[5] = new (ELeave) TTileProvider(
			_L("esri"), _L(/*"Esri World Imagery (Clarity) Beta"*/ "Esri (Clarity) Beta"),
			_L8("http://clarity.maptiles.arcgis.com/arcgis/rest/services/World_Imagery/MapServer/tile/{$z}/{$y}/{$x}"),
			0, 22);
	
	iActiveTileProvider = iAvailableTileProviders[0]; // Use first
	
	
	iFileMan = CAsyncFileMan::NewL(CCoeEnv::Static()->FsSession(), this);
	
	// Connect to landmark database
	iLandmarkPartialParameters = CPosLmPartialReadParameters::NewLC();
	CleanupStack::Pop();
	iLandmarkPartialParameters->SetRequestedAttributes(
			CPosLandmark::ELandmarkName | CPosLandmark::EPosition
			/*| CPosLandmark::EIcon*/);
	//User::LeaveIfError(iLandmarkPartialParameters->SetRequestedPositionFields(...));	
	
	iLandmarksDb = CPosLandmarkDatabase::OpenL();
	if (iLandmarksDb->IsInitializingNeeded())
		{
		DEBUG(_L("Landmarks DB initialization needed"));
		
		CPosLmOperation* op = iLandmarksDb->InitializeL();
		ExecuteAndDeleteLD(op);
		
		INFO(_L("Landmarks DB initialized"));
		}
	iLandmarksDb->SetPartialReadParametersL(*iLandmarkPartialParameters);

	
	// Create view objects
	iMapView = CMapView::NewL();
	//iMapView->MapControl()->MakeVisible(EFalse); // Will be shown later after settings will be loaded in CS60MapsAppUi::RestoreL
	AddViewL(iMapView);
	SetDefaultViewL(*iMapView);

	iSettingsView = CSettingsView::NewL();
	AddViewL(iSettingsView);
	
	// Position requestor
	_LIT(KPosRequestorName, "S60 Maps"); // ToDo: Move to global const
	TRAPD(err, iPosRequestor = CPositionRequestor::NewL(this, KPosRequestorName));
	if (err == KErrNone)
		{
		iPosRequestor->Start(); // Must be started after view created
		}
	else
		{
		// Message to user will be shown later after language will be readed from settings
		WARNING(_L("Failed to create position requestor (error: %d), continue without GPS"), err);
		}
	
	// Media keys catching
	iInterfaceSelector = CRemConInterfaceSelector::NewL();
	iCoreTarget = CRemConCoreApiTarget::NewL(*iInterfaceSelector, *this);
	iInterfaceSelector->OpenTargetL();
	
	/*// Make fullscreen
	//SetFullScreenApp(ETrue);*/
	
	// Make infinite backlight
	iLight = CHWRMLight::NewL();
	EnableInfiniteBacklight();
	//DEBUG(_L("light status=%d"), iLight->LightStatus(KLightTarget));
	
	// Prevent screensaver to be visible
	iResetInactivityTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	DisableScreenSaver();
	}
// -----------------------------------------------------------------------------
// CS60MapsAppUi::CS60MapsAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CS60MapsAppUi::CS60MapsAppUi() :
		iResourceOffset(1531863040) // ToDo: "magic number" - where is it from?
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CS60MapsAppUi::~CS60MapsAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CS60MapsAppUi::~CS60MapsAppUi()
	{
	EnableScreenSaver();
	delete iResetInactivityTimer;
	DisableInfiniteBacklight();
	delete iLight;
	
	//delete iCoreTarget;
	/* Panic KERN-EXEC 3 - Seems that there are no need to manually destroy core target,
	 because interface selector brings ownership and will delete target by itself. */
	
	/*iCacheResetProgressChecker->Cancel();
	delete iCacheResetProgressChecker;*/
	
	//delete iCacheClearingWaitDialog;
	
	delete iInterfaceSelector;
	
	delete iPosRequestor;
	
	delete iLandmarksDb;
	delete iLandmarkPartialParameters;
	
	delete iFileMan;
	
	//delete iAvailableTileProviders;
	iAvailableTileProviders.DeleteAll();
	
	delete iSettings;
	
	//if (iResourceOffset != -1)
	//	{
	//	CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
	//	}
	}

//// -----------------------------------------------------------------------------
//// CS60MapsAppUi::HandleCommandL()
//// Takes care of command handling.
//// -----------------------------------------------------------------------------
////
//void CS60MapsAppUi::HandleCommandL(TInt aCommand)
//	{
//	switch (aCommand)
//		{
//		case EEikCmdExit:
//		case EAknSoftkeyExit:
//			HandleExitL();
//			break;
//			
//		default:
//			Panic(ES60MapsUi);
//			break;
//		}
//	}

// -----------------------------------------------------------------------------
// CS60MapsAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CS60MapsAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			HandleExitL();
			break;
			
		default:
			Panic(ES60MapsUi);
			break;
		}
	}

// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CS60MapsAppUi::HandleStatusPaneSizeChange()
	{
	if (iMapView && iMapView->MapControl())
		{
		TCoordinate coord = iMapView->MapControl()->GetCenterCoordinate();
		if (iMapView->MapControl()->IsSoftkeysShown())
			{
			iMapView->MapControl()->SetRect(/*iAvkonAppUi->*/ClientRect());
			}
		else
			{
			iMapView->MapControl()->SetRect(/*iAvkonAppUi->*/ApplicationRect());
			}
		iMapView->MapControl()->Move(coord);
		}
	}

CArrayFix<TCoeHelpContext>* CS60MapsAppUi::HelpContextL() const
	{
#warning "Please see comment about help and UID3..."
	// Note: Help will not work if the application uid3 is not in the
	// protected range.  The default uid3 range for projects created
	// from this template (0xE0000000 - 0xEFFFFFFF) are not in the protected range so that they
	// can be self signed and installed on the device during testing.
	// Once you get your official uid3 from Symbian Ltd. and find/replace
	// all occurrences of uid3 in your project, the context help will
	// work. Alternatively, a patch now exists for the versions of 
	// HTML help compiler in SDKs and can be found here along with an FAQ:
	// http://www3.symbian.com/faq.nsf/AllByDate/E9DF3257FD565A658025733900805EA2?OpenDocument
#ifdef _HELP_AVAILABLE_
	CArrayFixFlat<TCoeHelpContext>* array = new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
	CleanupStack::PushL(array);
	array->AppendL(TCoeHelpContext(KUidS60MapsApp, KGeneral_Information));
	CleanupStack::Pop(array);
	return array;
#else
	return NULL;
#endif
	}

//void CS60MapsAppUi::DynInitMenuPaneL(TInt aMenuID, CEikMenuPane* aMenuPane)
//	{
//	
//	}

TStreamId CS60MapsAppUi::StoreL(CStreamStore& aStore) const
	{
	RStoreWriteStream stream;
	TStreamId id = stream.CreateLC(aStore);
	stream << *this;
	stream.CommitL() ;
	CleanupStack::PopAndDestroy(&stream);
	return id;
	}

void CS60MapsAppUi::RestoreL(const CStreamStore& aStore,
		TStreamId aStreamId)
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore, aStreamId);
	stream >> *this;
	CleanupStack::PopAndDestroy(&stream);
	
	DEBUG(_L("Settings restored"));
	
	// Show fully constructed view
	iMapView->MapControl()->MakeVisible(ETrue);      //   ???????
	}

void CS60MapsAppUi::ExternalizeL(RWriteStream& aStream) const
	{
	DEBUG(_L("Settings saving started"));
	
	// Update settings
	TCoordinate coord = iMapView->MapControl()->GetCenterCoordinate();
	iSettings->SetLat(coord.Latitude());
	iSettings->SetLon(coord.Longitude());
	iSettings->SetZoom(iMapView->MapControl()->GetZoom());
	iSettings->SetTileProviderId(iActiveTileProvider->iId);	
	
	// And save
	aStream << *iSettings;
	
	DEBUG(_L("Settings saving ended"));
	}

void CS60MapsAppUi::InternalizeL(RReadStream& aStream)
	{
	DEBUG(_L("Settings reading started"));
	
	TRAP_IGNORE(aStream >> *iSettings);
	iMapView->MapControl()->Move(iSettings->GetLat(), iSettings->GetLon(), iSettings->GetZoom());
	
	// Tile provider
	TTileProviderId tileProviderId(iSettings->GetTileProviderId());
	TBool isFound = EFalse;
	for (TInt idx = 0; idx < iAvailableTileProviders.Count(); idx++)
		{
		if (tileProviderId == iAvailableTileProviders[idx]->iId)
			{
			iActiveTileProvider = iAvailableTileProviders[idx];
			iMapView->MapControl()->SetTileProviderL(iAvailableTileProviders[idx]);
			isFound = ETrue;
			break;
			}
		}
	
	if (!isFound)
		{ // Set default
		iActiveTileProvider = iAvailableTileProviders[0];
		iMapView->MapControl()->SetTileProviderL(iAvailableTileProviders[0]);
		}
	
	// Language
	if (!IsLanguageExists(iSettings->iLanguage))
		{ // Wrong language fix
		iSettings->iLanguage = PreferredLanguage();
		}
	ChangeLanguageL(iSettings->iLanguage);
	
	// After localization loaded show translated message if positioning unavailable
	if (!iPosRequestor)
		{
		HBufC* msg = iEikonEnv->AllocReadResourceLC(R_POSITIONING_DISABLED);
		//CAknWarningNote* note = new (ELeave) CAknWarningNote;
		CAknErrorNote* note = new (ELeave) CAknErrorNote;
		note->ExecuteLD(*msg);
		CleanupStack::PopAndDestroy(msg);
		}
	
	// Signal indicator type
	if (iSettings->iSignalIndicatorType != CSettings::ESignalIndicatorGeneralType
			and iSettings->iSignalIndicatorType != CSettings::ESignalIndicatorPerSatelliteType)
		{
		iSettings->iSignalIndicatorType = CSettings::ESignalIndicatorGeneralType;
		}
	
	DEBUG(_L("Settings reading ended"));
	}

MFileManObserver::TControl CS60MapsAppUi::NotifyFileManStarted()
	{
	return MFileManObserver::EContinue;
	}

MFileManObserver::TControl CS60MapsAppUi::NotifyFileManOperation()
	{
	return MFileManObserver::EContinue;
	}

MFileManObserver::TControl CS60MapsAppUi::NotifyFileManEnded()
	{
	return MFileManObserver::EContinue;
	}

void CS60MapsAppUi::ClearTilesCacheL()
	{
	TFileName cacheDir;
	static_cast<CS60MapsApplication *>(Application())->CacheDir(cacheDir);
	
	INFO(_L("Start cleaning of cache directory"));
	iFileMan->Delete(cacheDir, CFileMan::ERecurse);
	
	/*// Prepare and show progress dialog
	iCacheResetProgressDialog = new (ELeave) CAknProgressDialog(
			REINTERPRET_CAST(CEikDialog**, &iCacheResetProgressDialog)
	);
	iCacheResetProgressDialog->ExecuteLD(R_PROGRESS_DIALOG);
	const KInterval = KSecond / 5;
	TCallBack callback(UpdateTilesClearingProgress, this);*/
	//iCacheResetProgressChecker->Start(0/*KInterval*/, KInterval, callback);
	
	// Prepare and show wait dialog
	iCacheClearingWaitDialog = new (ELeave) CAknWaitDialog(
			REINTERPRET_CAST(CEikDialog**, &iCacheClearingWaitDialog)
	);
	iCacheClearingWaitDialog->SetCallback(this);
	iCacheClearingWaitDialog->ExecuteLD(R_WAIT_DIALOG);
	}

void CS60MapsAppUi::OnFileManFinished(TInt aStatus)
	{
	INFO(_L("FileMan operation ended with code=%d"), aStatus);
	
	//iCacheResetProgressDialog->ProcessFinishedL();
	if (iCacheClearingWaitDialog)
		iCacheClearingWaitDialog->ProcessFinishedL();
	
	if (aStatus == KErrNone)
		{
		// Show "Done" message
		HBufC* msg = iEikonEnv->AllocReadResourceLC(R_DONE);
		CAknInformationNote* note = new (ELeave) CAknInformationNote;
		note->ExecuteLD(*msg);
		CleanupStack::PopAndDestroy(msg);
		}
	}

/*TInt CS60MapsAppUi::UpdateTilesClearingProgress(TAny* aSelfPtr)
	{
	CS60MapsAppUi* self = static_cast<CS60MapsAppUi*>(aSelfPtr);
	CEikProgressInfo* progressInfo = self->iCacheResetProgressDialog->GetProgressInfoL();
	progressInfo->SetFinalValue(self->iFileMan->TotalFiles());
	progressInfo->SetAndDraw(self->iFileMan->ProcessedFiles());
	DEBUG(_L("Progress: %d/%d"), self->iFileMan->ProcessedFiles(), self->iFileMan->TotalFiles());
	
	return ETrue;
	}*/

void CS60MapsAppUi::OnPositionUpdated()
	{
	__ASSERT_DEBUG(iPosRequestor != NULL, Panic(ES60MapsPosRequestorIsNull));
	
	const TPositionInfo* posInfo = iPosRequestor->LastKnownPositionInfo();
	TPosition pos;
	posInfo->GetPosition(pos);
	TCoordinateEx coord = pos;
	coord.SetCourse(KNaN);
	if (posInfo->PositionClassType() & EPositionCourseInfoClass)
		{
		const TPositionCourseInfo* courseInfo =
				static_cast<const TPositionCourseInfo*>(posInfo);
		TCourse course;
		courseInfo->GetCourse(course);
		
		coord.SetCourse(course.Heading());
		}
	coord.SetHorAccuracy(pos.HorizontalAccuracy());
	iMapView->MapControl()->SetUserPosition(coord);
	}

void CS60MapsAppUi::OnPositionPartialUpdated()
	{
	iMapView->MapControl()->DrawNow();
	}

void CS60MapsAppUi::OnPositionRestored()
	{
	}

void CS60MapsAppUi::OnPositionLost()
	{
	iMapView->MapControl()->HideUserPosition();
	}

void CS60MapsAppUi::OnPositionError(TInt /*aErrCode*/)
	{
	
	}

void CS60MapsAppUi::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
		TRemConCoreApiButtonAction aButtonAct)
	{
	//TRequestStatus status;
	
	if (aButtonAct == ERemConCoreApiButtonClick)
		{
		switch (aOperationId)
			{
			case ERemConCoreApiVolumeUp:
				{
				//DEBUG(_L("VolumeUp pressed\n"));
				iMapView->MapControl()->ZoomIn();
				
				/*iCoreTarget->VolumeUpResponse(status, KErrNone);
				User::WaitForRequest(status);*/
				}
			break;
			
			case ERemConCoreApiVolumeDown:
				{
				//DEBUG(_L("VolumeDown pressed\n"));
				iMapView->MapControl()->ZoomOut();
				
				/*iCoreTarget->VolumeDownResponse(status, KErrNone);
				User::WaitForRequest(status);*/
				}
			break;
			
			default:
			break;
			}
		}
	}

void CS60MapsAppUi::DialogDismissedL(TInt aButtonId)
	{
	if (aButtonId != EAknSoftkeyCancel)
		return;
	
	iFileMan->Cancel();
	
	INFO(_L("Clearing cache operation cancelled"));
	}

/*void CS60MapsAppUi::PrepareToExit()
	{
	// Save all changes before exit
	TRAP_IGNORE(SaveL());
	
	CAknAppUi::PrepareToExit();
	}*/

void CS60MapsAppUi::SaveAndExitL()
	{
	SaveL();
	Exit();
	}

void CS60MapsAppUi::SendAppToBackground()
	{
	TApaTask task(iEikonEnv->WsSession());
	task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
	task.SendToBackground();
	}

CPosLandmark* CS60MapsAppUi::GetNearestLandmarkL(const TCoordinate &aCoord,
		TBool aPartial, TReal32 aMaxDistance)
	{
	CPosLandmark* landmark = NULL; // Returned value
	
	DEBUG(_L("Start nearest landmark queing (max distance = %f m.)"), aMaxDistance);
	
	CPosLandmarkSearch* landmarkSearch = CPosLandmarkSearch::NewL(*iLandmarksDb);
	CleanupStack::PushL(landmarkSearch);
	landmarkSearch->SetMaxNumOfMatches(1);
	CPosLmNearestCriteria* nearestCriteria = CPosLmNearestCriteria::NewLC(aCoord);
	nearestCriteria->SetMaxDistance(aMaxDistance);
	CPosLmOperation* landmarkOp = landmarkSearch->StartLandmarkSearchL(*nearestCriteria, EFalse);
	ExecuteAndDeleteLD(landmarkOp);
	if (landmarkSearch->NumOfMatches())
		{		
		CPosLmItemIterator* landmarkIter = landmarkSearch->MatchIteratorL();
		CleanupStack::PushL(landmarkIter);
		
		landmarkIter->Reset();
		TPosLmItemId landmarkId = landmarkIter->NextL();
		if (landmarkId == KPosLmNullItemId)
			User::Leave(KErrNotFound);
		
		if (aPartial)
			landmark = iLandmarksDb->ReadPartialLandmarkLC(landmarkId);
		else
			landmark = iLandmarksDb->ReadLandmarkLC(landmarkId);
		
		CleanupStack::Pop(); // landmark
		
		CleanupStack::PopAndDestroy(landmarkIter);		
		}
		
	CleanupStack::PopAndDestroy(2, landmarkSearch);
	
	DEBUG(_L("End nearest landmark queing"));
	
	if (!landmark)
		{
		DEBUG(_L("Nothing found"));
		}
	
	return landmark;
	}

void CS60MapsAppUi::HandleExitL()
	{
	// FixMe: Called twice if "No" key pressed in confirm dialog
	
	RWsSession& session = CEikonEnv::Static()->WsSession();
	TInt WgId = session.GetFocusWindowGroup();
	CApaWindowGroupName* Wgn = CApaWindowGroupName::NewL(session, WgId);
	TUid forgroundApp = Wgn->AppUid();
	delete Wgn;
	const TUid KAppUid = {_UID3};
	//If application is in background Symbian OS will show its own quit confirmation.
	if(forgroundApp == KAppUid)
		{
		CAknQueryDialog* dlg = CAknQueryDialog::NewL();
		dlg->PrepareLC(R_CONFIRM_DIALOG);
		/*HBufC* title = iEikonEnv->AllocReadResourceLC(R_CONFIRM_EXIT_DIALOG_TITLE);
		dlg->SetHeaderTextL(*title);
		CleanupStack::PopAndDestroy(); //title*/
		HBufC* msg = iEikonEnv->AllocReadResourceLC(R_CONFIRM_EXIT_DIALOG_TEXT);
		dlg->SetPromptL(*msg);
		CleanupStack::PopAndDestroy(); //msg
		TInt res = dlg->RunLD();
		if (res != EAknSoftkeyYes)
			{
			return;
			}
		}
	
	// Send window to background to increase visible speed of shutdown
	SendAppToBackground();

	SaveAndExitL();
	}

void CS60MapsAppUi::ChangeLanguageL(TLanguage aLang)
	{
	__ASSERT_DEBUG(iResourceOffset == 1531863040, Panic(ES60MapsInvalidResourceOffset));
	
	INFO(_L("Set language to %d"), (TInt) aLang);
	
	// Close previous opened resource file first
	CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
	
	TParse parser;
	TFileName appFullName(RProcess().FileName());
	parser.Set(appFullName, NULL, NULL);
	
	//_LIT(KResFilePathFmt, "%S\\resource\\apps\\%S.r%02d");
	//TFileName resFileFullName;
	//resFileFullName.Format(KResFilePathFmt, &parser.Drive(), &parser.Name(), (TInt) aLang); // FixMe: truncates 3-digits codes
	
	_LIT(KResFilePathFmt, "%S\\resource\\apps\\%S.rsc");
	TFileName resFileFullName;
	resFileFullName.Format(KResFilePathFmt, &parser.Drive(), &parser.Name());
	BaflUtils::SetIdealLanguage(aLang);
	TLanguage lang;
	BaflUtils::NearestLanguageFile(CCoeEnv::Static()->FsSession(), resFileFullName, lang);
	DEBUG(_L("Found language %d"), lang);
	
	DEBUG(_L("Trying to load file \"%S\""), &resFileFullName);
	// ToDo: Check file exists
	iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resFileFullName);
	
	// Update strings for scale bar layer
	iMapView->MapControl()->HandleLanguageChangedL();
	}

TBool CS60MapsAppUi::IsLanguageExists(TLanguage aLang)
	{
	TLanguage oldIdealLang = BaflUtils::IdealLanguage();
	
	TParse parser;
	TFileName appFullName(RProcess().FileName());
	parser.Set(appFullName, NULL, NULL);
	
	_LIT(KResFilePathFmt, "%S\\resource\\apps\\%S.rsc");
	TFileName resFileFullName;
	resFileFullName.Format(KResFilePathFmt, &parser.Drive(), &parser.Name());
	BaflUtils::SetIdealLanguage(aLang);
	TLanguage foundLang;
	BaflUtils::NearestLanguageFile(CCoeEnv::Static()->FsSession(), resFileFullName, foundLang);
	TBool res = foundLang == aLang;
	DEBUG(_L("Language %d found? - %d"), (TInt) aLang, res);
	
	// Restore previous ideal language value
	BaflUtils::SetIdealLanguage(oldIdealLang);
	
	return res;
	}

void CS60MapsAppUi::AvailableLanguagesL(RArray<TLanguage> &aLangArr)
	{
	aLangArr.Reset();
	
	TParse parser;
	TFileName appFullName(RProcess().FileName());
	parser.Set(appFullName, NULL, NULL);
	
	_LIT(KResFilePathFmt, "%S\\resource\\apps\\%S.r*");
	TFileName resFilePath;
	resFilePath.Format(KResFilePathFmt, &parser.Drive(), &parser.Name());
	
	CDir* files = NULL;
	User::LeaveIfError(CCoeEnv::Static()->FsSession().GetDir(resFilePath, KEntryAttNormal, ESortNone, files));
	if (files)
		{
		CleanupStack::PushL(files);
		for (TInt i = 0; i < files->Count(); i++)
			{
			parser.Set((*files)[i].iName, NULL, NULL);
			TInt langCode;
			if (MathUtils::ParseInt(parser.Ext(), langCode) == KErrNone)
				{
				aLangArr.Append(static_cast<TLanguage>(langCode));
				DEBUG(_L("lang=%d"), (TInt) langCode);
				}
			}
		CleanupStack::PopAndDestroy(files);
		}
	}

TLanguage CS60MapsAppUi::PreferredLanguage()
	{
	// ToDo: Try to get preffered languange depends on system language
	return ELangEnglish;
	}

TInt CS60MapsAppUi::ResetInactivityTimer(TAny* /*aPtr*/)
	{
	//DEBUG(_L("reset inactivity timer"));
	
	User::ResetInactivityTime();
	return ETrue;
	}

void CS60MapsAppUi::HandleForegroundEventL(TBool aForeground)
	{
	DEBUG(_L("HandleForegroundEventL aForeground=%d"), aForeground);
	if (aForeground)
		{
		DisableScreenSaver();
		}
	else
		{
		EnableScreenSaver();
		}
	}

void CS60MapsAppUi::EnableInfiniteBacklight()
	{
	iLight->ReserveLightL(KLightTarget);
	iLight->LightOnL(KLightTarget);
	
	DEBUG(_L("Infinite backlight enabled"));
	}

void CS60MapsAppUi::DisableInfiniteBacklight()
	{
	iLight->ReleaseLight(KLightTarget);
	
	DEBUG(_L("Infinite backlight disabled"));
	}

void CS60MapsAppUi::EnableScreenSaver()
	{
	if (iResetInactivityTimer->IsActive())
		{
		iResetInactivityTimer->Cancel();
		DEBUG(_L("Screensaver enabled"));
		}
	}

void CS60MapsAppUi::DisableScreenSaver()
	{
	const TInt KMinScreenSaverTimeout = 5000000; // 5 seconds is minimal value on my phone
												// ToDo: Is there system constant for this value?
	TCallBack callback(ResetInactivityTimer, NULL);
	if (iResetInactivityTimer->IsActive())
		{
		iResetInactivityTimer->Cancel();
		}
	iResetInactivityTimer->Start(0, KMinScreenSaverTimeout - KSecond, callback);
	DEBUG(_L("Screensaver disabled"));
	}

// End of File
