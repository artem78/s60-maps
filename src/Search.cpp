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
	iHttpClient = CHTTPClient2::NewL(this);
	
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
	
	//TBuf<KSearchInputFieldMaxLength> queryText;	
	CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog(iQuery);
	dlg->SetMaxLength(/*KSearchInputFieldMaxLength*/iQuery.MaxLength());
	TBool res = dlg->ExecuteLD(R_SEARCH_INPUT_QUERY_DLG) == EAknSoftkeySearch;
	if (res)
		{
		DEBUG(_L("Search query: %S"), &iQuery);
		}
	
	DEBUG(_L("end with result=%d"), res);
	return res;	
	}

TBool CSearch::RunResultsDialogL()
	{
	DEBUG(_L("begin"));
	
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(iAvkonAppUi);
	
	const TInt KGranularity = 10;
	CDesCArraySeg* namesArray = new (ELeave) CDesCArraySeg(KGranularity);
	CleanupStack::PushL(namesArray);
	CArrayFixSeg<TCoordinate>* coordsArray = new (ELeave) CArrayFixSeg<TCoordinate>(KGranularity);
	CleanupStack::PushL(coordsArray);
	
	ParseApiResponseL(namesArray, coordsArray);
	
	TInt chosenItem = -1;
	CAknSelectionListDialog* dlg = CAknSelectionListDialog::NewL(chosenItem, namesArray,
			R_SEARCH_RESULTS_QUERY_DIALOG_MENUBAR);
	appUi->ShowStatusPaneAndHideMapControlL(R_SEARCH_RESULTS);
	TBool res = dlg->ExecuteLD(R_SEARCH_RESULTS_QUERY_DIALOG) != 0;
	appUi->HideStatusPaneAndShowMapControlL();
	
	if (res)
		{
		DEBUG(_L("Selected name=%S idx=%d lat=%f lon=%f"), &(*namesArray)[chosenItem],
				chosenItem,
				coordsArray->At(chosenItem).Latitude(),
				coordsArray->At(chosenItem).Longitude());
		
		iCoord = coordsArray->At(chosenItem);
		}
	
	CleanupStack::PopAndDestroy(2, namesArray);
	
	iObserver->OnSearchFinished(res, iCoord);
	
	DEBUG(_L("end"));
	}

void CSearch::ParseApiResponseL(CDesCArray* aNamesArr, CArrayFix<TCoordinate>* aCoordsArr)
	{
	DEBUG(_L("begin"));
	
	CJsonParser* parser = new (ELeave) CJsonParser();
	CleanupStack::PushL(parser);
	
	HBufC* jsonData = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*iResponseBuff);
	CleanupStack::PushL(jsonData);

	parser->StartDecodingL(*jsonData);
	
	TInt itemsCount = parser->GetParameterCount(KNullDesC);
	TBuf<256> name;
	TBuf<16> latDes, lonDes;
	TReal64 lat, lon;
	TBuf<32> path;
	_LIT(KNamePathFmt, "[%d][display_name]");
	_LIT(KLatPathFmt, "[%d][lat]");
	_LIT(KLonPathFmt, "[%d][lon]");
	_LIT(KTab, "\t");
	TLex lex;
	TCoordinate coord;
	for (TInt i = 0; i < itemsCount; i++)
		{
		// Parse name
		path.Format(KNamePathFmt, i);
		parser->GetParameterValue(path, &name);
		name.Insert(0, KTab);
		aNamesArr->AppendL(name);
		
		// Parse coordinates
		path.Format(KLatPathFmt, i);
		parser->GetParameterValue(path, &latDes);
		lex.Assign(latDes);
		lat = KNaN;
		User::LeaveIfError(lex.Val(lat, '.'));

		path.Format(KLonPathFmt, i);
		parser->GetParameterValue(path, &lonDes);
		lex.Assign(lonDes);
		lon = KNaN;
		User::LeaveIfError(lex.Val(lon, '.'));

		coord.SetCoordinate(lat, lon);
		aCoordsArr->AppendL(coord);
		}
	
	CleanupStack::PopAndDestroy(2, parser);
	
	DEBUG(_L("end"));
	}

void CSearch::RunApiReqestL()
	{
	DEBUG(_L("begin"));
	__ASSERT_DEBUG(iQuery != KNullDesC, Panic());
	
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(iAvkonAppUi);
	
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

void CSearch::OnHTTPError(TInt /*aError*/, const RHTTPTransaction /*aTransaction*/)
	{
	delete iResponseBuff;
	iResponseBuff = NULL;
	}

void CSearch::OnHTTPHeadersRecieved(const RHTTPTransaction /*aTransaction*/)
	{

	}
