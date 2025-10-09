/*
 * SettingsListBox.h
 *
 *  Created on: 09.03.2021
 *      Author: artem78
 */

#ifndef __SETTINGSLISTBOX_H_
#define __SETTINGSLISTBOX_H_

#include <aknsettingitemlist.h>

class CSettingsListBox : public CAknSettingItemList
	{
	// From CAknSettingItemList
protected:
	CAknSettingItem* CreateSettingItemL(TInt aSettingId);
	
public:
	void EditItemL(TInt aIndex, TBool aCalledFromMenu);

	};


class CLanguageListSettingItem : public CAknEnumeratedTextPopupSettingItem
	{
	// From CAknEnumeratedTextPopupSettingItem
public:
	CLanguageListSettingItem(TInt aResourceId, TInt& aValue);
	void CompleteConstructionL();
	
	// New properties and methods
private:
	TInt& iValue;
	
	void LoadLanguageListL();
	};
	
	
class CDriveListSettingItem : public CAknEnumeratedTextPopupSettingItem
	{
	// From CAknEnumeratedTextPopupSettingItem
public:
	CDriveListSettingItem(TInt aResourceId, TInt& aValue);
	void CompleteConstructionL();
	
	// New properties and methods
private:
	TInt& iValue;
	
	void FillDriveListL(); 
	
	};

#endif /* __SETTINGSLISTBOX_H_ */
