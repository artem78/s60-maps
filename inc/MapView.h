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


// Classes

class CMapView : public CAknView
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
	
	// Command handlers
	void HandleExitL();
	void HandleFindMeL();
	void HandleTileProviderChangeL(TInt aTileProviderIdx);
	void HandleTilesCacheStatsL();
	void HandleTilesCacheResetL();
#ifdef _HELP_AVAILABLE_
	void HandleHelpL();
#endif
	void HandleAboutL();
	void HandleSettingsL();
	
	// New
public:
	void MakeFullScreen(TBool aEnable=ETrue);
	
	};

#endif /* __MAPVIEW_H_ */
