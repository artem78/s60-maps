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
			
		case ESettingIapId:
			{
			TInt* ptr = (TInt*)(&appUi->Settings()->iIapId); // TUint& --> TInt&
			settingItem = new (ELeave) CIapSettingItem(aSettingId,
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
			UpdateIapSettingVisibilityL();
			}
			break;
			
		case ESettingIapId:
			{
			// ...
			}
			break;
		}
	}

void CSettingsListBox::CompleteConstructionL()
	{
	UpdateIapSettingVisibilityL();
	}

void CSettingsListBox::UpdateIapSettingVisibilityL()
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(iCoeEnv->Static()->AppUi());
	TBool isHidden = appUi->Settings()->iIapConnMode != CSettings::ESpecified;
	SettingItemArray()->At(ESettingIapId)->SetHidden(isHidden);
	HandleChangeInItemArrayOrVisibilityL();
	}
	


// CIapSettingItem

CIapSettingItem::CIapSettingItem(TInt aResourceId, TInt& aValue) :
		CAknEnumeratedTextPopupSettingItem(aResourceId, aValue)
		{}

void CIapSettingItem::CompleteConstructionL()
	{
	CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();
	
	FillIapsListL();
	}

void CIapSettingItem::FillIapsListL()
	{
	CArrayPtr<CAknEnumeratedText>* textArray = EnumeratedTextArray();
	textArray->ResetAndDestroy();
	
	////// STUB //////
	// ToDo: Replace by real code
	for (TInt i = 1; i <= 5; i++)
		{
		TBuf<16> buff;
		buff.Append(_L("IAP "));
		buff.AppendNum(i);
		
		HBufC* hbuff = buff.AllocLC();
		CAknEnumeratedText* enumText = new (ELeave) CAknEnumeratedText(i - 1, hbuff);
		CleanupStack::Pop(hbuff);
		CleanupStack::PushL(enumText);
		textArray->AppendL(enumText);
		CleanupStack::Pop(enumText);
		}
	//////////////////
	}
