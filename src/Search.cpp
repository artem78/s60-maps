/*
 ============================================================================
 Name		: Search.cpp
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : CSearch implementation
 ============================================================================
 */

#include "Search.h"
#include <S60Maps_0xED689B88.rsg>
#include "Logger.h"
#include <aknquerydialog.h>
#include <aknselectionlist.h>
#include "S60MapsAppUi.h"


CSearch::CSearch()
	{
	// No implementation required
	}

CSearch::~CSearch()
	{
	}

CSearch* CSearch::NewLC()
	{
	CSearch* self = new (ELeave) CSearch();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSearch* CSearch::NewL()
	{
	CSearch* self = CSearch::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CSearch::ConstructL()
	{

	}

void CSearch::RunL()
	{
	if (!RunQueryDialogL())
		return;
				
	if (!RunResultsDialogL())
		return;
	}

TBool CSearch::RunQueryDialogL()
	{
	//TBuf<KSearchInputFieldMaxLength> queryText;	
	CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog(iQuery);
	dlg->SetMaxLength(/*KSearchInputFieldMaxLength*/iQuery.MaxLength());
	TBool res = dlg->ExecuteLD(R_SEARCH_INPUT_QUERY_DLG) == EAknSoftkeySearch;
	if (res)
		{
		DEBUG(_L("Search query: %S"), &iQuery);
		}
	
	return res;	
	}

TBool CSearch::RunResultsDialogL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(iAvkonAppUi);
	
	const TInt KGranularity = 10;
	CDesCArraySeg* resultsArray = new (ELeave) CDesCArraySeg(KGranularity);
	CleanupStack::PushL(resultsArray);
	
	///
	resultsArray->AppendL(_L("\tOne"));
	resultsArray->AppendL(_L("\tTwo"));
	resultsArray->AppendL(_L("\tThree"));
	resultsArray->AppendL(_L("\tFour"));
	///
	
	TInt chosenItem = -1;
	CAknSelectionListDialog* dlg = CAknSelectionListDialog::NewL(chosenItem, resultsArray,
			R_SEARCH_RESULTS_QUERY_DIALOG_MENUBAR);
	appUi->ShowStatusPaneAndHideMapControlL(R_SEARCH_RESULTS);
	TBool res = dlg->ExecuteLD(R_SEARCH_RESULTS_QUERY_DIALOG) != 0;
	appUi->HideStatusPaneAndShowMapControlL();
	
	if (res)
		{
		//DEBUG(_L("Selected result: %S"), &resultsArray[chosenItem]);
		DEBUG(_L("Selected result: %d"), chosenItem);
		}
	
	CleanupStack::PopAndDestroy(resultsArray);
	
	return res;
	}
