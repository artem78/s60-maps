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

void CSearch::RunDialogL()
	{
	const TInt KSearchInputFieldMaxLength = 128;
	
	TBuf<KSearchInputFieldMaxLength> queryText;	
	CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog(queryText);
	dlg->SetMaxLength(KSearchInputFieldMaxLength);
	if (dlg->ExecuteLD(R_SEARCH_INPUT_QUERY_DLG) == EAknSoftkeySearch)
		{
		DEBUG(_L("Search query: %S"), &queryText);
		}
	}
