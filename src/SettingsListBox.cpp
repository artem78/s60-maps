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
#include <S60Maps_0xED689B88.rsg>

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
			
			// Reload settings view for translate strings to the new language
			appUi->SettingsView()->Reload();
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
	const TInt ELangGalician = 103; // Not defined in s60v3 SDK
	
	RArray<TLanguage> langArr = RArray<TLanguage>(10);
	CleanupClosePushL(langArr);
	
	CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	appUi->AvailableLanguagesL(langArr);
	
	EnumeratedTextArray()->ResetAndDestroy();
	for (TInt i = 0; i < langArr.Count(); i++)
		{
		TLanguage langCode = langArr[i];
		TInt resourceId = 0;
		
		switch (langCode)
			{
			case ELangEnglish:
				{
				resourceId = R_LANG_ENGLISH_NAME;		
				break;
				}
				
			case ELangSpanish:
				{
				resourceId = R_LANG_SPANISH_NAME;
				break;
				}
				
			case ELangGalician:
				{
				resourceId = R_LANG_GALICIAN_NAME;
				break;
				}
				
			case ELangPortuguese:
				{
				resourceId = R_LANG_PORTUGUESE_NAME;
				break;
				}
				
			case ELangRussian:
				{
				resourceId = R_LANG_RUSSIAN_NAME;
				break;
				}
				
			case ELangPolish:
				{
				resourceId = R_LANG_POLISH_NAME;
				break;
				}
				
			case ELangHebrew:
				{
				resourceId = R_LANG_HEBREW_NAME;
				break;
				}
				
			case ELangLatinAmericanSpanish:
				{
				resourceId = R_LANG_LAT_AM_SPANISH_NAME;
				break;
				}
				
			case ELangUkrainian:
				{
				resourceId = R_LANG_UKRAINISN_NAME;
				break;
				}
			}
		
		HBufC* langName = NULL;
		if (resourceId > 0)
			{
			langName = CCoeEnv::Static()->AllocReadResourceLC(resourceId);
			}
		else
			{
			_LIT(KUnknownFmt, "<unknown %d>");
			TBuf<16> tmp;
			tmp.Format(KUnknownFmt, (TInt) langCode);
			langName = tmp.AllocLC();
			}

		DEBUG(_L("Language %d => %S"), langCode, &*langName);

		CAknEnumeratedText* enumText = new (ELeave) CAknEnumeratedText(langCode, langName);
		CleanupStack::Pop(langName);
		CleanupStack::PushL(enumText);
		EnumeratedTextArray()->AppendL(enumText);
		CleanupStack::Pop(enumText);
		}
	
	
	CleanupStack::PopAndDestroy(&langArr);
	}
