/*
 * MapView.h
 *
 *  Created on: 07.03.2021
 *      Author: artem78
 */

#ifndef __MAPVIEW_H_
#define __MAPVIEW_H_

// Includes

#include <aknview.h>
#include "MapControl.h"
#include "Search.h"
#include "S60Maps.hrh"


// Classes

class CMapView : public CAknView, public MSearchObserver
	{
	// Constructors / destructors
public:
	~CMapView();
	static CMapView* NewL();
	static CMapView* NewLC();

private:
	CMapView();
	void ConstructL();
	
	
	// From CAknView
public:
	TUid Id() const;
	void HandleCommandL(TInt aCommand);
	
protected:
	void DoActivateL(const TVwsViewId& aPrevViewId,
			TUid aCustomMessageId,
			const TDesC8& aCustomMessage);
	void DoDeactivate();
	
	
	// From MEikMenuObserver
//public:
private:
	void DynInitMenuPaneL(TInt aMenuID, CEikMenuPane* aMenuPane);
	
	
	// From MSearchObserver
private:
	void OnSearchFinished(const TSearchResultItem &aResultData);
	/*void OnSearchFailed();*/
	void OnSearchClosed/*L*/();
	
	
	// Custom properties and methods
public:
	// Getters/setters
	inline CMapControl* MapControl()
				{ return iMapControl; };
	inline CSearch* Search()
				{ return iSearch; };
	
private:
	// Constants
	static const TInt KSetTileProviderCommandBaseId; // Start id for tile provider change commands
	
	// Controls
	CMapControl* iMapControl;
	
	CPosLandmark* GetNearestLandmarkAroundTheCenterL(TBool aPartial = ETrue); // The client takes ownership of the returned landmark object. Returns NULL if nothing found.
	inline static TInt TileProviderIdxToCommandId(/*TTileProviderIdx*/ TInt aIdx)
			{ return KSetTileProviderCommandBaseId + aIdx; };
	inline static TTileProviderIdx CommandIdToTileProviderIdx(TInt aCommandId)
			{ return TTileProviderIdx(aCommandId - KSetTileProviderCommandBaseId); };
	
	// Command handlers
	void HandleFindMeL();
	void HandleTileProviderChangeL(TInt aTileProviderIdx);
	void HandleTilesCacheStatsL();
	void HandleTilesCacheResetL();
#ifdef _HELP_AVAILABLE_
	void HandleHelpL();
#endif
	void HandleAboutL();
	void HandleToggleLandmarksVisibility();
	void HandleCreateLandmarkL();
	void HandleRenameLandmarkL();
	void HandleCreateOrRenameLandmarkL();
	void HandleDeleteLandmarkL();
	void HandleGotoLandmarkL();
	void HandleGotoCoordinateL();
	void HandleSettingsL();
	void HandleReloadVisibleAreaL();
	void HandleSearchL();
	void HandleTrafficCounterL();
	void HandleClearSearchResultsL();
	
	// Others
	CSearch* iSearch;

	};

#endif /* __MAPVIEW_H_ */
