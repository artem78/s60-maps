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
#include <apgwgnam.h>

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
	CSettings* settings = appUi->Settings();
	
	// Set initial map position
	TCoordinate position = TCoordinate(settings->GetLat(), settings->GetLon());
	TZoom zoom = settings->GetZoom();
	
	// Initialize control
	iMapControl = CMapControl::NewL(ClientRect(), position, zoom, appUi->TileProvider());
	
	// Make fullscreen
	//StatusPane()->MakeVisible(EFalse);
	//Cba()->MakeVisible(EFalse);
	iMapControl->SetRect(AppUi()/*iAvkonAppUi*/->ApplicationRect()); // Need to resize the view to fullscreen*/
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
	//	{
		AppUi()->AddToStackL(iMapControl);
		iMapControl->MakeVisible(ETrue);
	//	}
	}

void CMapView::DoDeactivate()
	{
	/*if (iMapControl)
		{
		//if (IsControlOnStack(iMapControl))*/
		//	{
			AppUi()->RemoveFromStack(iMapControl);
			iMapControl->MakeVisible(EFalse);
		//	}
		/*delete iMapControl;
		iMapControl = NULL;
		}*/
	}

void CMapView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			HandleExitL();
			break;
			
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
			
		case ESettings:
			HandleSettingsL();
			break;
			
		default:
			Panic(ES60MapsUi);
			break;
		}
	}

void CMapView::DynInitMenuPaneL(TInt aMenuID, CEikMenuPane* aMenuPane)
	{	
	if (aMenuID == R_SUBMENU_TILE_PROVIDERS)
		{
		CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
		
		// Fill list of available tiles services in menu
		
		for (TInt idx = 0; idx < appUi->AvailableTileProviders().Count(); idx++)
			{
			TInt commandId = ESetTileProviderBase + idx;
			
			CEikMenuPaneItem::SData menuItem;
			menuItem.iCommandId = commandId;
			menuItem.iCascadeId = 0;
			//menuItem.iFlags = ???
			menuItem.iText.Copy(appUi->AvailableTileProviders()[idx]->iTitle);
			//menuItem.iExtraText = ???
			aMenuPane->AddMenuItemL(menuItem);
			aMenuPane->SetItemButtonState(commandId,
					/*commandId == selectedTileProviderCommId*/
					appUi->AvailableTileProviders()[idx] == appUi->TileProvider() ?
							EEikMenuItemSymbolOn : EEikMenuItemSymbolIndeterminate);				
			}
		}
	/*else
		{
		AppUi()->DynInitMenuPaneL(aMenuID, aMenuPane);
		}*/
	}

void CMapView::HandleExitL()
	{
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

	//AppUi()->Exit();
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(AppUi());
	appUi->SaveAndExitL();
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
	_LIT(KThanksTo,	"baranovskiykonstantin, Symbian9, Men770, fizolas");
	
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
	iEikonEnv->Format128/*256*/(msg, R_ABOUT_DIALOG_TEXT, &version,
			&KGITBranch, &KGITCommit, &KAuthor, &KWebSite, &KThanksTo);
	dlg->SetMessageTextL(msg);
	CleanupStack::PopAndDestroy(&msg);
	dlg->RunLD();
	}

void CMapView::HandleSettingsL()
	{
	//TVwsViewId viewId = TVwsViewId(KUidS60MapsApp, TUid::Uid(ESettingsViewId));
	//AppUi()->ActivateViewL(viewId);
	//AppUi()->ActivateViewL(iSettingsView->ViewId());
	AppUi()->ActivateLocalViewL(TUid::Uid(ESettingsViewId));
	}
