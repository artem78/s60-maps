/*
 * SettingsListBox.cpp
 *
 *  Created on: 09.03.2021
 *      Author: artem78
 */

#include "SettingsListBox.h"
#include "S60Maps.hrh"
#include "S60MapsAppUi.h"
#include "MapView.h"

// CSettingsListBox

CAknSettingItem* CSettingsListBox::CreateSettingItemL(TInt aSettingId)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(iCoeEnv->Static()->AppUi());
	CAknSettingItem* settingItem = NULL;
	
	switch (aSettingId)
		{
		case ESettingFullScreen:
			{
			settingItem = new (ELeave) CAknBinaryPopupSettingItem(aSettingId,
					appUi->Settings()->iFullScreen);
			}
			break;
			
		case ESettingIapConnectionMode:
			{
			TInt* ptr = (TInt*)(&appUi->Settings()->iIapConnMode); // TIapConnectionMode& --> TInt&
			settingItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(aSettingId,
					*ptr);
			}
			break;
		}
	
	return settingItem;
	}

void CSettingsListBox::EditItemL(TInt aIndex, TBool aCalledFromMenu)
	{
	CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);
	
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(iCoeEnv->Static()->AppUi());
	
	// ToDo: Add validation
	
	(*SettingItemArray())[aIndex]->UpdateListBoxTextL();
	(*SettingItemArray())[aIndex]->StoreL();
	
	switch ((*SettingItemArray())[aIndex]->Identifier())
		{
		case ESettingFullScreen:
			{
			CMapView* mapView = static_cast<CMapView*>(appUi->View(TUid::Uid(EMapViewId)));  
			mapView->MakeFullScreen(appUi->Settings()->iFullScreen);
			}
			break;
			
		case ESettingIapConnectionMode:
			{
			// ...
			}
			break;
		}
	}
