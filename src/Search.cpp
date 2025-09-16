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
#include "JsonParser.h"
#include "S60Maps.pan"
#include "EscapeUtils.h"
#include <utf.h>
#include <aknnotewrappers.h> 


CSearch::CSearch(MSearchObserver* aObserver)
		: iObserver(aObserver)
	{
	// No implementation required
	}

CSearch::~CSearch()
	{
	delete iHttpClient;
	
	DEBUG(_L("Destructor"));
	}

CSearch* CSearch::NewLC(MSearchObserver* aObserver)
	{
	CSearch* self = new (ELeave) CSearch(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSearch* CSearch::NewL(MSearchObserver* aObserver)
	{
	CSearch* self = CSearch::NewLC(aObserver);
	CleanupStack::Pop(); // self;
	return self;
	}

void CSearch::ConstructL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	iHttpClient = CHTTPClient2::NewL(this,appUi->iSockServ, appUi->iConn);
	
	DEBUG(_L("Constructor"));
	}

TBool CSearch::RunL()
	{
	if (!RunQueryDialogL())
		return EFalse;
	
	RunApiReqestL();
	
	return ETrue;
	}

TBool CSearch::RunQueryDialogL()
	{
	DEBUG(_L("begin"));
	
	CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog(iQuery);
	dlg->SetMaxLength(iQuery.MaxLength());
	TBool res = dlg->ExecuteLD(R_SEARCH_INPUT_QUERY_DLG) == EAknSoftkeySearch;
	if (res)
		{
		DEBUG(_L("Search query: %S"), &iQuery);
		}
	
	DEBUG(_L("end with result=%d"), res);
	return res;	
	}

void CSearch::RunResultsDialogL()
	{
	DEBUG(_L("begin"));
	
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(iAvkonAppUi);
	
	const TInt KGranularity = 10;
	
	CArrayFixSeg<TSearchResultItem>* items = new (ELeave) CArrayFixSeg<TSearchResultItem>(KGranularity);
	CleanupStack::PushL(items);
	
	ParseApiResponseL(items);
	
	switch (items->Count())
		{
		case 0:
			{ // nothing found
			CAknErrorNote* dlg = new (ELeave) CAknErrorNote();
			HBufC* msg = CCoeEnv::Static()->AllocReadResourceLC(R_NO_RESULTS);
			dlg->ExecuteLD(*msg);
			CleanupStack::PopAndDestroy(msg);
			
			//iObserver->OnSearchFailed();
			
			break;
			}
		
		case 1:
			{ // go directly to single result
			iObserver->OnSearchFinished(items->At(0));
			
			break;
			}
		
		default:
			{ // show choosing list if more than one result
			CPtrCArray* namesArray = new (ELeave) CPtrCArray(KGranularity);
			CleanupStack::PushL(namesArray);
			
			for (TInt i = 0; i < items->Count(); i++)
				{
				namesArray->AppendL(TPtrC(items->At(i).iName));
				}
			
			TInt chosenItem = -1;
			CAknSelectionListDialog* dlg = CAknSelectionListDialog::NewL(chosenItem, namesArray,
					R_SEARCH_RESULTS_QUERY_DIALOG_MENUBAR);
			appUi->ShowStatusPaneAndHideMapControlL(R_SEARCH_RESULTS);
			TBool res = dlg->ExecuteLD(R_SEARCH_RESULTS_QUERY_DIALOG) != 0;
			appUi->HideStatusPaneAndShowMapControlL();
			
			CleanupStack::PopAndDestroy(namesArray);
			
			if (res)
				{
				DEBUG(_L("Selected name=%S idx=%d lat=%f lon=%f"), &items->At(chosenItem).iName,
						chosenItem,
						items->At(chosenItem).iCoord.Latitude(),
						items->At(chosenItem).iCoord.Longitude());
				
				iObserver->OnSearchFinished(items->At(chosenItem));
				}
			else
				{
				//iObserver->OnSearchFailed();
				}
			
			break;
			}
		}
	
	CleanupStack::PopAndDestroy(items);
	
	
	DEBUG(_L("end"));
	}

void CSearch::ParseApiResponseL(CArrayFix<TSearchResultItem>* aResultsArr)
	{
	DEBUG(_L("begin"));
	
	enum TBoundsArrIdx {ELat1, ELat2, ELon1, ELon2};
	
	CJsonParser* parser = new (ELeave) CJsonParser();
	CleanupStack::PushL(parser);
	
	HBufC* jsonData = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*iResponseBuff);
	CleanupStack::PushL(jsonData);

	parser->StartDecodingL(*jsonData);
	
	TInt itemsCount = parser->GetParameterCount(KNullDesC);
	TReal64 lat, lon, bLat1, bLat2, bLon1, bLon2;
	TBuf<32> path;
	_LIT(KNamePathFmt, "[%d][display_name]");
	_LIT(KLatPathFmt, "[%d][lat]");
	_LIT(KLonPathFmt, "[%d][lon]");
	_LIT(KBBoxPathFmt, "[%d][boundingbox][%d]");
	_LIT(KTab, "\t");
	TSearchResultItem result;
	for (TInt i = 0; i < itemsCount; i++)
		{
		// Parse name
		path.Format(KNamePathFmt, i);
		ParseJsonValueL(parser, path, result.iName);
		result.iName.Insert(0, KTab);
		
		// Parse coordinates
		path.Format(KLatPathFmt, i);
		ParseJsonValueL(parser, path, lat);

		path.Format(KLonPathFmt, i);
		ParseJsonValueL(parser, path, lon);

		result.iCoord.SetCoordinate(lat, lon);
		
		// Parse bounds
		path.Format(KBBoxPathFmt, i, ELat1);
		ParseJsonValueL(parser, path, bLat1);
		
		path.Format(KBBoxPathFmt, i, ELat2);
		ParseJsonValueL(parser, path, bLat2);
		
		path.Format(KBBoxPathFmt, i, ELon1);
		ParseJsonValueL(parser, path, bLon1);
		
		path.Format(KBBoxPathFmt, i, ELon2);
		ParseJsonValueL(parser, path, bLon2);
		
		result.iBounds.SetCoords(bLat1, bLon1, bLat2, bLon2);
		
		
		aResultsArr->AppendL(result);
		}
	
	CleanupStack::PopAndDestroy(2, parser);
	
	DEBUG(_L("end"));
	}

void CSearch::ParseJsonValueL(CJsonParser* aParser, const TDesC &aParam, TDes &aVal)
	{
	aVal = KNullDesC;
	
	if (!aParser->GetParameterValue(aParam, &aVal))
		User::Leave(KErrNotFound);
	}

void CSearch::ParseJsonValueL(CJsonParser* aParser, const TDesC &aParam, TReal64 &aVal)
	{
	TLex lex;
	TBuf<32> buff /*= KNullDesC*/;
	buff.Zero();
	aVal = KNaN;
	
	if (!aParser->GetParameterValue(aParam, &buff))
		User::Leave(KErrNotFound);
	lex.Assign(buff);
	User::LeaveIfError(lex.Val(aVal, '.'));
	}

void CSearch::RunApiReqestL()
	{
	DEBUG(_L("begin"));
	__ASSERT_DEBUG(iQuery != KNullDesC, Panic());
	
	_LIT8(KApiBaseUrl, "https://nominatim.openstreetmap.org/search?format=json&q=");
	
	HBufC8* utf8Query = CnvUtfConverter::ConvertFromUnicodeToUtf8L(iQuery);
	CleanupStack::PushL(utf8Query);
	
	HBufC8* encodedQuery = EscapeUtils::EscapeEncodeL(*utf8Query, EscapeUtils::EEscapeUrlEncoded);
	CleanupStack::PushL(encodedQuery);
	
	RBuf8 url;
	url.CreateL(KApiBaseUrl().Length() + encodedQuery->Length());
	CleanupClosePushL(url);
	url = KApiBaseUrl;
	url.Append(*encodedQuery);
	
	iHttpClient->GetL(url);
		
	CleanupStack::PopAndDestroy(3, utf8Query);
	
	DEBUG(_L("end"));
	}

void CSearch::OnHTTPResponseDataChunkRecieved(const RHTTPTransaction /*aTransaction*/,
		const TDesC8 &aDataChunk, TInt anOverallDataSize, TBool anIsLastChunk)
	{
	if (iResponseBuff == NULL)
		{
		iResponseBuff = HBufC8::NewL(anOverallDataSize);
		}
	
	iResponseBuff->Des().Append(aDataChunk);
	
	if (anIsLastChunk)
		{
		RunResultsDialogL();
		
		delete iResponseBuff;
		iResponseBuff = NULL;
		}
	}

void CSearch::OnHTTPResponse(const RHTTPTransaction /*aTransaction*/)
	{
	
	}

void CSearch::OnHTTPError(TInt aError, const RHTTPTransaction /*aTransaction*/)
	{
	delete iResponseBuff;
	iResponseBuff = NULL;
	
	iObserver->OnSearchFailedL(aError);
	}

void CSearch::OnHTTPHeadersRecieved(const RHTTPTransaction /*aTransaction*/)
	{

	}

void MSearchObserver::OnSearchFailedL(TInt aErrCode)
	{
	TBuf<32> errStr;
	MiscUtils::ErrorToDes(aErrCode, errStr);
	HBufC* msgFmt = /*iEikonEnv*/CEikonEnv::Static()->AllocReadResourceLC(R_SEARCH_FAILED_FMT);
	TBuf</*32*/64> msg;
	msg.Format(*msgFmt, &errStr);
	CEikonEnv::Static()->AlertWin(msg);
	CleanupStack::PopAndDestroy(msgFmt);
	}
