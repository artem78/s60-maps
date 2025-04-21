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

TBool CSearch::RunL(TCoordinate &aCoord)
	{
	if (!RunQueryDialogL())
		return EFalse;
				
	if (!RunResultsDialogL())
		return EFalse;
	
	aCoord = iCoord;
	return ETrue;
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
	
	return res;
	}

void CSearch::ParseApiResponseL(CDesCArray* aNamesArr, CArrayFix<TCoordinate>* aCoordsArr)
	{
	////////////
	_LIT(KJson, "[{\"place_id\":182003351,\"licence\":\"Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright\",\"osm_type\":\"relation\",\"osm_id\":102269,\"lat\":\"55.6255780\",\"lon\":\"37.6063916\",\"class\":\"boundary\",\"type\":\"administrative\",\"place_rank\":8,\"importance\":0.8585195484457739,\"addresstype\":\"state\",\"name\":\"Москва\",\"display_name\":\"Москва, Центральный федеральный округ, Россия\",\"boundingbox\":[\"55.1421745\",\"56.0212238\",\"36.8031012\",\"37.9674277\"]},{\"place_id\":181667215,\"licence\":\"Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright\",\"osm_type\":\"relation\",\"osm_id\":2555133,\"lat\":\"55.7505412\",\"lon\":\"37.6174782\",\"class\":\"place\",\"type\":\"city\",\"place_rank\":16,\"importance\":0.8585195484457739,\"addresstype\":\"city\",\"name\":\"Москва\",\"display_name\":\"Москва, Центральный федеральный округ, Россия\",\"boundingbox\":[\"55.5166840\",\"55.9577717\",\"37.2905020\",\"37.9674277\"]},{\"place_id\":305728094,\"licence\":\"Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright\",\"osm_type\":\"relation\",\"osm_id\":129775,\"lat\":\"37.3240640\",\"lon\":\"-101.2054630\",\"class\":\"boundary\",\"type\":\"administrative\",\"place_rank\":16,\"importance\":0.42676956835189356,\"addresstype\":\"village\",\"name\":\"Moscow\",\"display_name\":\"Moscow, Stevens County, Канзас, 67952, Соединённые Штаты Америки\",\"boundingbox\":[\"37.3221469\",\"37.3296130\",\"-101.2126096\",\"-101.2030970\"]},{\"place_id\":336246695,\"licence\":\"Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright\",\"osm_type\":\"relation\",\"osm_id\":12196304,\"lat\":\"45.0710960\",\"lon\":\"-69.8915860\",\"class\":\"boundary\",\"type\":\"administrative\",\"place_rank\":16,\"importance\":0.4164100037473589,\"addresstype\":\"town\",\"name\":\"Moscow\",\"display_name\":\"Moscow, Somerset County, Мэн, 04920, Соединённые Штаты Америки\",\"boundingbox\":[\"45.0647356\",\"45.1773584\",\"-69.9665917\",\"-69.7874730\"]},{\"place_id\":310517220,\"licence\":\"Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright\",\"osm_type\":\"relation\",\"osm_id\":197198,\"lat\":\"35.0619984\",\"lon\":\"-89.4039612\",\"class\":\"boundary\",\"type\":\"administrative\",\"place_rank\":16,\"importance\":0.38360068645110323,\"addresstype\":\"village\",\"name\":\"Moscow\",\"display_name\":\"Moscow, Fayette County, West Tennessee, Теннесси, 38057, Соединённые Штаты Америки\",\"boundingbox\":[\"35.0507500\",\"35.0661730\",\"-89.4110410\",\"-89.3718350\"]},{\"place_id\":322916586,\"licence\":\"Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright\",\"osm_type\":\"node\",\"osm_id\":157606151,\"lat\":\"39.5437014\",\"lon\":\"-79.0050273\",\"class\":\"place\",\"type\":\"hamlet\",\"place_rank\":20,\"importance\":0.37360304719775245,\"addresstype\":\"hamlet\",\"name\":\"Moscow\",\"display_name\":\"Moscow, Allegany County, Мэриленд, 21521, Соединённые Штаты Америки\",\"boundingbox\":[\"39.5237014\",\"39.5637014\",\"-79.0250273\",\"-78.9850273\"]},{\"place_id\":313782700,\"licence\":\"Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright\",\"osm_type\":\"relation\",\"osm_id\":182742,\"lat\":\"38.8570117\",\"lon\":\"-84.2291017\",\"class\":\"boundary\",\"type\":\"administrative\",\"place_rank\":16,\"importance\":0.36105344208987866,\"addresstype\":\"village\",\"name\":\"Moscow\",\"display_name\":\"Moscow, Washington Township, Clermont County, Огайо, Соединённые Штаты Америки\",\"boundingbox\":[\"38.8543290\",\"38.8666503\",\"-84.2336270\",\"-84.2195162\"]}]");
	///////////////
	

	CJsonParser* parser = new (ELeave) CJsonParser();
	CleanupStack::PushL(parser);
	
	parser->StartDecodingL(KJson);
	
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
		User::LeaveIfError(lex.Val(lat));

		path.Format(KLonPathFmt, i);
		parser->GetParameterValue(path, &lonDes);
		lex.Assign(lonDes);
		lon = KNaN;
		User::LeaveIfError(lex.Val(lon));

		coord.SetCoordinate(lat, lon);
		aCoordsArr->AppendL(coord);
		}
	
	CleanupStack::PopAndDestroy(parser);
	}
