/*
 * MapView.cpp
 *
 *  Created on: 07.03.2021
 *      Author: artem78
 */

#include "MapView.h"
#include "S60Maps.hrh"
#include "S60Maps.pan"
#include <S60Maps_0xED689B88.rsg>
#include "S60MapsAppUi.h"
#include "S60MapsApplication.h"
#include <e32cmn.h>
#include <avkon.hrh>
#include "GitInfo.h"
#include <aknmessagequerydialog.h>
#include <epos_cposlandmarksearch.h>
#include <aknselectionlist.h>
#include <akntitle.h>

// CMapView

CMapView::CMapView()
	{
	}

CMapView::~CMapView()
	{
	delete iMapControl;
	}

CMapView* CMapView::NewLC()
	{
	CMapView* self = new (ELeave) CMapView();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMapView* CMapView::NewL()
	{
	CMapView* self = CMapView::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CMapView::ConstructL()
	{
	CAknView::BaseConstructL(R_MAP_VIEW);
	
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
	/*const*/ CSettings* settings = appUi->Settings();
	
	// Set initial map position
	TCoordinate position = TCoordinate(settings->GetLat(), settings->GetLon());
	TZoom zoom = settings->GetZoom();
	
	// Initialize control
	iMapControl = CMapControl::NewL(ClientRect(), position, zoom, appUi->TileProvider());
	iMapControl->MakeVisible(EFalse); // Will be shown later after settings will be loaded in CS60MapsAppUi::RestoreL
	
	// Make fullscreen
	//StatusPane()->MakeVisible(EFalse);
	//Cba()->MakeVisible(EFalse);
	iMapControl->SetRect(AppUi()/*iAvkonAppUi*/->ApplicationRect()); // Need to resize the view to fullscreen*/
	
	iMapControl->SetFollowUser(ETrue);
	}

TUid CMapView::Id() const
	{
	return TUid::Uid(EMapViewId);
	}

void CMapView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
			TUid /*aCustomMessageId*/,
			const TDesC8& /*aCustomMessage*/)
	{
	//if (iMapControl)
		AppUi()->AddToStackL(iMapControl);

	StatusPane()->MakeVisible(EFalse);
	//Cba()->MakeVisible(EFalse);
	iMapControl->MakeVisible(ETrue);
	}

void CMapView::DoDeactivate()
	{
	/*if (iMapControl)
		{
		//if (IsControlOnStack(iMapControl))*/
			AppUi()->RemoveFromStack(iMapControl);
		/*delete iMapControl;
		iMapControl = NULL;
		}*/

	const TUid KAknSgcClientStaticId = { 0x101f7674 }; // Defined in Symbian`s private sources
	TBool isGoingToExit = !CCoeEnv::Static(KAknSgcClientStaticId);
	DEBUG(_L("isGoingToExit=%d"), isGoingToExit);
	if (!isGoingToExit) // Otherwise KERN-EXEC 3 will be thrown on next line
		{
		StatusPane()->MakeVisible(ETrue); // Restore hidden status pane
		}
	//Cba()->MakeVisible(ETrue);
	iMapControl->MakeVisible(EFalse);
	}

void CMapView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EFindMe:
			HandleFindMeL();
			break;
			
		case ESetOsmStandardTileProvider:
		case ESetOsmCyclesTileProvider:
		case ESetOsmHumanitarianTileProvider:
		case ESetOsmTransportTileProvider:
		//case ESetOpenTopoMapTileProvider:
			HandleTileProviderChangeL(aCommand - ESetTileProviderBase);
			break;	
			
		case ETilesCacheStats:
			HandleTilesCacheStatsL();
			break;
			
		case EResetTilesCache:
			HandleTilesCacheResetL();
			break;
			
#ifdef _HELP_AVAILABLE_
		case EHelp:
			HandleHelpL();
			break;
#endif
			
		case EAbout:
			HandleAboutL();
			break;
			
		case EToggleLandmarksVisibility:
			HandleToggleLandmarksVisibility();
			break;

		case ECreateLandmark:
			HandleCreateLandmarkL();
			break;

		case ERenameLandmark:
			HandleRenameLandmarkL();
			break;

		case ECreateOrRenameLandmark:
			HandleCreateOrRenameLandmarkL();
			break;

		case EDeleteLandmark:
			HandleDeleteLandmarkL();
			break;

		case EGotoLandmark:
			HandleGotoLandmarkL();
			break;

		case EGotoCoordinate:
			HandleGotoCoordinateL();
			break;
			
		case ESettings:
			HandleSettingsL();
			break;
			
		case EReloadVisibleArea:
			HandleReloadVisibleAreaL();
			break;
			
		default:
			// Let the AppUi handle unknown for view commands
			AppUi()->HandleCommandL(aCommand);
			break;
		}
	}

void CMapView::DynInitMenuPaneL(TInt aMenuID, CEikMenuPane* aMenuPane)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
	
	if (aMenuID == R_MENU)
		{
		//aMenuPane->SetItemButtonState(EFindMe,
		//		iAppView->IsFollowingUser() ? EEikMenuItemSymbolOn : EEikMenuItemSymbolIndeterminate
		//);
		aMenuPane->SetItemDimmed(EFindMe, !appUi->IsPositioningAvailable() || MapControl()->IsFollowingUser());
		}
	else if (aMenuID == R_SUBMENU_GOTO)
		{
		CPosLmItemIterator* landmarkIterator = appUi->LandmarkDb()->LandmarkIteratorL();
		if (!(landmarkIterator && landmarkIterator->NumOfItemsL() > 0))
			aMenuPane->SetItemDimmed(EGotoLandmark, ETrue);
		delete landmarkIterator;
		}
	else if (aMenuID == R_SUBMENU_TILE_PROVIDERS)
		{
		// Fill list of available tiles services in menu
		
		for (TInt idx = 0; idx < appUi->AvailableTileProviders().Count(); idx++)
			{
			TInt commandId = ESetTileProviderBase + idx;
			
			CEikMenuPaneItem::SData menuItem;
			menuItem.iCommandId = commandId;
			menuItem.iCascadeId = 0;
			menuItem.iFlags = EEikMenuItemCheckBox;
			menuItem.iText.Copy(appUi->AvailableTileProviders()[idx]->iTitle);
			menuItem.iExtraText.Zero();
			aMenuPane->AddMenuItemL(menuItem);
			aMenuPane->SetItemButtonState(commandId,
					/*commandId == selectedTileProviderCommId*/
					appUi->AvailableTileProviders()[idx] == appUi->TileProvider() ?
							EEikMenuItemSymbolOn : EEikMenuItemSymbolIndeterminate);				
			}
		}
	else if (aMenuID == R_SUBMENU_LANDMARKS)
		{
		aMenuPane->SetItemButtonState(EToggleLandmarksVisibility,
				appUi->Settings()->GetLandmarksVisibility() ? EEikMenuItemSymbolOn : EEikMenuItemSymbolIndeterminate
		);
		CPosLandmark* nearestLandmark = GetNearestLandmarkAroundTheCenterL();
		TBool isDisplayEditOrDeleteLandmark = appUi->Settings()->GetLandmarksVisibility() && nearestLandmark;
		delete nearestLandmark;
		aMenuPane->SetItemDimmed(ERenameLandmark, !isDisplayEditOrDeleteLandmark);
		aMenuPane->SetItemDimmed(EDeleteLandmark, !isDisplayEditOrDeleteLandmark);
		}
	/*else
		{
		AppUi()->DynInitMenuPaneL(aMenuID, aMenuPane);
		}*/
	}

void CMapView::HandleFindMeL()
	{
	iMapControl->SetFollowUser(ETrue);
	}

void CMapView::HandleTileProviderChangeL(TInt aTileProviderIdx)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
	TTileProvider* tileProvider = appUi->AvailableTileProviders()[aTileProviderIdx];
	appUi->SetTileProvider(tileProvider);
	iMapControl->SetTileProviderL(tileProvider);	
	}

void CMapView::HandleTilesCacheStatsL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
	CS60MapsApplication* app = static_cast<CS60MapsApplication *>(appUi->Application());
	RFs fs = iEikonEnv->FsSession();
	
	// Prepare information text
	RBuf msg;
	msg.CreateL(2048);
	msg.CleanupClosePushL();
	
	TInt filesTotal = 0, bytesTotal = 0;
	
	TFileName baseCacheDir;
	app->CacheDir(baseCacheDir);
	
	CDir* cacheSubDirs = NULL;
	TInt r = fs.GetDir(baseCacheDir, KEntryAttDir, ESortByName, cacheSubDirs);
	if (r == KErrNone && cacheSubDirs != NULL)
		{
		for (TInt i = 0; i < cacheSubDirs->Count(); i++)
			{
			const TEntry &cacheSubDir = (*cacheSubDirs)[i];
			
			// Seems that KEntryAttDir doesn`t work
			if (!cacheSubDir.IsDir())
				continue;
			
			TDirStats dirStats;
			RBuf subDirFullPath;
			subDirFullPath.Create(KMaxFileName);
			subDirFullPath.Copy(baseCacheDir);
			TParsePtr parser(subDirFullPath);
			parser.AddDir(cacheSubDir.iName);
			r = FileUtils::DirectoryStats(fs, parser.FullName(), dirStats);
			subDirFullPath.Close();
			if (r != KErrNone)
				{ // Something went wrong
				dirStats.iFilesCount = 0;
				dirStats.iSize = 0;
				}
			
			filesTotal += dirStats.iFilesCount;
			bytesTotal += dirStats.iSize;
			
			TBuf<16> sizeBuff;
			FileUtils::FileSizeToReadableString(dirStats.iSize, sizeBuff);
			
			TPtrC itemName(cacheSubDir.iName);
			for (TInt providerIdx = 0; providerIdx < appUi->AvailableTileProviders().Count(); providerIdx++)
				{
				if (appUi->AvailableTileProviders()[providerIdx]->iId == cacheSubDir.iName)
					{
					itemName.Set(appUi->AvailableTileProviders()[providerIdx]->iTitle);
					break;
					}
				}
			
			TBuf<64> buf;
			iEikonEnv->Format128(buf, R_STATS_LINE, &itemName, dirStats.iFilesCount, &sizeBuff);
			msg.Append(buf);
			}
		
		delete cacheSubDirs;
		}
	
	msg.Append(_L("------------\n"));
	TBuf<16> totalSizeBuff;
	FileUtils::FileSizeToReadableString(bytesTotal, totalSizeBuff);
	TBuf<64> buf;
	iEikonEnv->Format128(buf, R_STATS_TOTAL, filesTotal, &totalSizeBuff);
	msg.Append(buf);
	
	
	
	// Show information
	CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog();
	dlg->PrepareLC(R_QUERY_DIALOG);
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_MAP_CACHE_STATS_DIALOG_TITLE);
	dlg->QueryHeading()->SetTextL(*title);
	CleanupStack::PopAndDestroy(title);
	dlg->SetMessageTextL(msg);
	//CleanupStack::PopAndDestroy(&msg);
	dlg->RunLD();
	
	CleanupStack::PopAndDestroy(&msg);
	//CleanupStack::PopAndDestroy(/*3*/2, &msg);
	}

void CMapView::HandleTilesCacheResetL()
	{
	CAknQueryDialog* dlg = CAknQueryDialog::NewL();
	dlg->PrepareLC(R_CONFIRM_DIALOG);
	/*HBufC* title = iEikonEnv->AllocReadResourceLC(R_CONFIRM_RESET_TILES_CACHE_DIALOG_TITLE);
	dlg->SetHeaderTextL(*title);
	CleanupStack::PopAndDestroy(); //title*/
	HBufC* msg = iEikonEnv->AllocReadResourceLC(R_CONFIRM_RESET_TILES_CACHE_DIALOG_TEXT);
	dlg->SetPromptL(*msg);
	CleanupStack::PopAndDestroy(); //msg
	TInt res = dlg->RunLD();
	if (res == EAknSoftkeyYes)
		{
		CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
		appUi->ClearTilesCacheL();
		}
	}

#ifdef _HELP_AVAILABLE_
void CMapView::HandleHelpL()
	{
	CArrayFix<TCoeHelpContext>* buf = CCoeAppUi::AppHelpContextL();
	HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
	}
#endif

void CMapView::HandleAboutL()
	{
	_LIT(KAuthor,	"artem78 (megabyte1024@ya.ru)");
	_LIT(KWebSite,	"https://github.com/artem78/s60-maps");
	_LIT(KThanksTo,	"baranovskiykonstantin, Symbian9, Men770, fizolas, bent");
	
	CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog();
	dlg->PrepareLC(R_QUERY_DIALOG);
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
	dlg->QueryHeading()->SetTextL(*title);
	CleanupStack::PopAndDestroy(); //title
	
	RBuf msg;
	msg.CreateL(512);
	msg.CleanupClosePushL();
	TBuf<32> version;
	version.Copy(KProgramVersion.Name());
#ifdef _DEBUG
	_LIT(KDebug, "DEBUG");
	version.Append(' ');
	version.Append(KDebug);
#endif
	TBuf<64> gitInfo;
	_LIT(KFmt,"%S (%S)");
	gitInfo.Format(KFmt, &KGITLongVersion, &KGITBranch);
	iEikonEnv->Format128/*256*/(msg, R_ABOUT_DIALOG_TEXT, &version,
			&gitInfo, &KAuthor, &KWebSite, &KThanksTo);
	dlg->SetMessageTextL(msg);
	CleanupStack::PopAndDestroy(&msg);
	dlg->RunLD();
	}
	
void CMapView::HandleToggleLandmarksVisibility()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
	appUi->Settings()->SetLandmarksVisibility(!appUi->Settings()->GetLandmarksVisibility());
	}

void CMapView::HandleCreateLandmarkL()
	{
	_LIT(KDefaultLandmarkName, "Landmark");
	_LIT(KLandmarkCategoryName, "S60Maps");
	
	RBuf landmarkName;
	landmarkName.CreateL(KPosLmMaxTextFieldLength);
	CleanupClosePushL(landmarkName);
	landmarkName.Copy(KDefaultLandmarkName);
	
	// Create landmark
	CPosLandmark* newLandmark = CPosLandmark::NewLC();
	TLocality pos = TLocality(MapControl()->GetCenterCoordinate(), KNaN, KNaN);
	newLandmark->SetPositionL(pos);
	// Ask landmark name
	HBufC* dlgTitle = iEikonEnv->AllocReadResourceLC(R_INPUT_NAME);
	CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog(landmarkName);
	if (dlg->ExecuteLD(R_LANDMARK_NAME_INPUT_QUERY, *dlgTitle) == EAknSoftkeyOk)
		{
		// Set landmark name
		newLandmark->SetLandmarkNameL(landmarkName);
		
		// Set landmark category
		CPosLandmarkDatabase* landmarkDb = static_cast<CS60MapsAppUi*>(AppUi())->LandmarkDb();
		CPosLmCategoryManager* catMgr = CPosLmCategoryManager::NewL(*landmarkDb);
		CleanupStack::PushL(catMgr);
		TPosLmItemId catId = catMgr->GetCategoryL(KLandmarkCategoryName);
		if (catId == KPosLmNullItemId)
			{ // If category didn`t found, create it
			CPosLandmarkCategory* category = CPosLandmarkCategory::NewLC();
			category->SetCategoryNameL(KLandmarkCategoryName);
			catId = catMgr->AddCategoryL(*category);	
			CleanupStack::PopAndDestroy(category);
			}
		newLandmark->AddCategoryL(catId);
		
		// Save landmark to DB
		landmarkDb->AddLandmarkL(*newLandmark);
		MapControl()->NotifyLandmarksUpdated();
		MapControl()->DrawDeferred();
		
		CleanupStack::PopAndDestroy(catMgr);
		}

	CleanupStack::PopAndDestroy(3, &landmarkName);
	}

void CMapView::HandleRenameLandmarkL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
	
	if (!appUi->Settings()->GetLandmarksVisibility())
		return;
	
	CPosLandmark* landmark = GetNearestLandmarkAroundTheCenterL(EFalse);
	if (!landmark)
		return; // Nothing to do
	CleanupStack::PushL(landmark);
	
	RBuf landmarkName;
	landmarkName.CreateL(KPosLmMaxTextFieldLength);
	CleanupClosePushL(landmarkName);
	
	TPtrC oldLandmarkName;
	landmark->GetLandmarkName(oldLandmarkName);
	landmarkName.Copy(oldLandmarkName);
	
	// Ask landmark new name
	HBufC* dlgTitle = iEikonEnv->AllocReadResourceLC(R_INPUT_NAME);
	CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog(landmarkName);
	if (dlg->ExecuteLD(R_LANDMARK_NAME_INPUT_QUERY, *dlgTitle) == EAknSoftkeyOk)
		{
		// Update landmark in DB	
		landmark->SetLandmarkNameL(landmarkName);
		appUi->LandmarkDb()->UpdateLandmarkL(*landmark);
		MapControl()->NotifyLandmarksUpdated();
		MapControl()->DrawDeferred();
		}
	
	CleanupStack::PopAndDestroy(3, landmark);
	}

void CMapView::HandleCreateOrRenameLandmarkL()
	{
	CPosLandmark* landmark = GetNearestLandmarkAroundTheCenterL(ETrue);
	if (landmark /*&& iSettings->GetLandmarksVisibility()*/)
		{
		delete landmark;
		HandleRenameLandmarkL();
		}
	else
		{
		HandleCreateLandmarkL();
		}
	}

void CMapView::HandleDeleteLandmarkL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
	
	if (!appUi->Settings()->GetLandmarksVisibility())
		return;
	
	CPosLandmark* landmark = GetNearestLandmarkAroundTheCenterL(ETrue);
	if (!landmark)
		return;
	CleanupStack::PushL(landmark);
	
	// Ask for confirmation
	TPtrC landmarkName;
	landmark->GetLandmarkName(landmarkName);
	RBuf msg;
	msg.CreateL(landmarkName.Length() + 100);
	CleanupClosePushL(msg);
	iEikonEnv->Format128(msg, R_CONFIRM_LANDMARK_DELETION, &landmarkName);
	CAknQueryDialog* dlg = CAknQueryDialog::NewL();
	dlg->PrepareLC(R_CONFIRM_DIALOG);
	dlg->SetPromptL(msg);
	if (dlg->RunLD() == EAknSoftkeyYes)
		{
		// Remove landmark from DB
		appUi->LandmarkDb()->RemoveLandmarkL(landmark->LandmarkId());
		MapControl()->NotifyLandmarksUpdated();
		MapControl()->DrawDeferred();
		}
	
	CleanupStack::PopAndDestroy(2, landmark);
	}

void CMapView::HandleGotoLandmarkL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
	const TInt KGranularity = 50;
	CDesCArraySeg* lmNameArray = new (ELeave) CDesCArraySeg(KGranularity);
	CleanupStack::PushL(lmNameArray);
	CArrayFixFlat<TPosLmItemId>* lmIdArray = new (ELeave) CArrayFixFlat<TPosLmItemId>(KGranularity);
	CleanupStack::PushL(lmIdArray);
	CPosLmItemIterator* lmIterator = appUi->LandmarkDb()->LandmarkIteratorL();
	CleanupStack::PushL(lmIterator);

	TPosLmItemId lmId;
	lmId = lmIterator->NextL();
	while (lmId != KPosLmNullItemId)
		{
		CPosLandmark* lm = appUi->LandmarkDb()->ReadPartialLandmarkLC(lmId);
		//if (lm->IsPositionFieldAvailable())
		TLocality pos;
		if (lm->GetPosition(pos) == KErrNone && !Math::IsNaN(pos.Latitude())
				&& !Math::IsNaN(pos.Longitude()))
			{ // Process landmarks only with position
			TPtrC lmName;
			lm->GetLandmarkName(lmName);
			TBuf<KPosLmMaxTextFieldLength + 1> buf;
			buf.Append('\t');
			buf.Append(lmName);
			lmNameArray->AppendL(buf);
			lmIdArray->AppendL(lmId);
			}
		CleanupStack::PopAndDestroy(lm);
		lmId = lmIterator->NextL();
		}

	TInt chosenItem;
	CAknSelectionListDialog* dlg = CAknSelectionListDialog::NewL(chosenItem, lmNameArray, R_LANDMARKS_QUERY_DIALOG_MENUBAR);
	iMapControl->MakeVisible(EFalse);
	AppUi()->StatusPane()->MakeVisible(ETrue);
	
	// Save original pane title
	CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
	CAknTitlePane* titlePane = (CAknTitlePane*) statusPane->ControlL(TUid::Uid(EEikStatusPaneUidTitle));
	HBufC* originalTitle = titlePane->Text()->AllocL();
	
	// Set new pane title
	HBufC* title = iEikonEnv->AllocReadResourceL(R_LANDMARKS);
	titlePane->SetText(title);
	
	TInt answer = dlg->ExecuteLD(R_LANDMARKS_QUERY_DIALOG);
	
	iMapControl->MakeVisible(ETrue);
	AppUi()->StatusPane()->MakeVisible(EFalse);
	
	// Restore original pane title
	titlePane->SetText(originalTitle);
	
	if (EAknSoftkeyOk == answer) 
		{
		CPosLandmark* lm = appUi->LandmarkDb()->ReadLandmarkLC(lmIdArray->At(chosenItem));
		TLocality pos;
		lm->GetPosition(pos);
		MapControl()->SetFollowUser(EFalse);
		MapControl()->MoveAndZoomIn(pos);
		CleanupStack::PopAndDestroy(lm);
		}

	CleanupStack::PopAndDestroy(3, lmNameArray);
	}

void CMapView::HandleGotoCoordinateL()
	{
	TCoordinate coord = MapControl()->GetCenterCoordinate();
	TPosition pos;
	pos.SetCoordinate(coord.Latitude(), coord.Longitude());
	CAknMultiLineDataQueryDialog* dlg=CAknMultiLineDataQueryDialog::NewL(pos);
	if(dlg->ExecuteLD(R_LOCATION_QUERY_DIALOG) == EAknSoftkeyOk)
		{
		coord.SetCoordinate(pos.Latitude(), pos.Longitude());
		MapControl()->SetFollowUser(EFalse);
		MapControl()->MoveAndZoomIn(coord);
		}
	}

CPosLandmark* CMapView::GetNearestLandmarkAroundTheCenterL(TBool aPartial)
	{
	const TInt KMaxDistanceInPixels = 25;
	
	TCoordinate center = MapControl()->GetCenterCoordinate();
	TReal32 maxDistance /* in meters */, unused;
	MapMath::PixelsToMeters(center.Latitude(), MapControl()->GetZoom(),
			KMaxDistanceInPixels, maxDistance, unused);
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
	return appUi->GetNearestLandmarkL(center, aPartial, maxDistance);
	}

void CMapView::HandleSettingsL()
	{
	//TVwsViewId viewId = TVwsViewId(KUidS60MapsApp, TUid::Uid(ESettingsViewId));
	//AppUi()->ActivateViewL(viewId);
	//AppUi()->ActivateViewL(iSettingsView->ViewId());
	AppUi()->ActivateLocalViewL(TUid::Uid(ESettingsViewId));
	}

void CMapView::HandleReloadVisibleAreaL()
	{
	iMapControl->ReloadVisibleAreaL();
	}
