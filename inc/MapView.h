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
	
	
	// Custom properties and methods
public:
	// Getters/setters
	inline CMapControl* MapControl()
				{ return iMapControl; };
	
private:
	// Controls
	CMapControl* iMapControl;
	
	CPosLandmark* GetNearestLandmarkAroundTheCenterL(TBool aPartial = ETrue); // The client takes ownership of the returned landmark object. Returns NULL if nothing found.
	
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
	
	// Others
	CSearch* iSearch;
	void OnSearchFinished(TBool aSuccess, const TCoordinate &aCoord, const TBounds &aBounds);
	};

#endif /* __MAPVIEW_H_ */
