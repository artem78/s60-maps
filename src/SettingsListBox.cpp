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
#include <akninputlanguageinfo.h>

// CSettingsListBox

CAknSettingItem* CSettingsListBox::CreateSettingItemL(TInt aSettingId)
	{
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(iCoeEnv->Static()->AppUi());
	CAknSettingItem* settingItem = NULL;
	
	switch (aSettingId)
		{
		case ESettingLanguage:
			{
			TInt* ptr = (TInt*)(&appUi->Settings()->iLanguage); // TLanguage& --> TInt&
			settingItem = new (ELeave) CLanguageListSettingItem(aSettingId,
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
	
	//(*SettingItemArray())[aIndex]->UpdateListBoxTextL();
	(*SettingItemArray())[aIndex]->StoreL();
	
	switch ((*SettingItemArray())[aIndex]->Identifier())
		{
		case ESettingLanguage:
			{
			appUi->ChangeLanguageL(appUi->Settings()->iLanguage);
			}
			break;
		}
	}


// CLanguageListSettingItem

CLanguageListSettingItem::CLanguageListSettingItem(TInt aResourceId, TInt& aValue) :
		CAknEnumeratedTextPopupSettingItem(aResourceId, aValue),
		iValue(aValue)
	{
	
	}

void CLanguageListSettingItem::CompleteConstructionL()
	{
	CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();
	
	LoadLanguageListL();
	}

void CLanguageListSettingItem::LoadLanguageListL()
	{
	RArray<TLanguage> langArr = RArray<TLanguage>(10);
	CleanupClosePushL(langArr);
	
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	appUi->AvailableLanguagesL(langArr);
	
	CAknInputLanguageInfo* langInfo = AknInputLanguageInfoFactory::CreateInputLanguageInfoL();
	CleanupStack::PushL(langInfo);
	
	EnumeratedTextArray()->ResetAndDestroy();
	for (TInt i = 0; i < langArr.Count(); i++)
		{
		TLanguage langCode = langArr[i];
		TAknLanguageName langName;
		
		// Fix for Kern-Exec 3 panic when call langInfo->LanguageName() for some languages
		_LIT(KLangGalicianName, /*"Galician"*/ "Galego");
		_LIT(KLangLatAmSpanishName, /*"Latin American Spanish"*/ "American Spanish");
		const TInt ELangGalician = 103; // Not defined in s60v3 SDK
		switch (langCode)
			{
			case ELangGalician:
				{
				langName = KLangGalicianName;
				break;
				}
				
			case ELangLatinAmericanSpanish:
				{
				langName = KLangLatAmSpanishName;
				break;
				}
			
			default:
				{ // Get names for other languages
				langName = langInfo->LanguageName(langCode);
				break;
				}
			}
		
		HBufC* text = langName.AllocLC();
		CAknEnumeratedText* enumText = new (ELeave) CAknEnumeratedText(langCode, text);
		CleanupStack::Pop(text);
		CleanupStack::PushL(enumText);
		EnumeratedTextArray()->AppendL(enumText);
		CleanupStack::Pop(enumText);
		}
	
	
	CleanupStack::PopAndDestroy(2, &langArr);
	}
