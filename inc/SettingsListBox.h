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
	
	// New properties and methods
public:
	void CompleteConstructionL(); // Additional initializations
	
private:
	void UpdateIapSettingVisibilityL(); // Update visibility of IAP selection field
	
	};

class CIapSettingItem : public CAknEnumeratedTextPopupSettingItem
	{
	// From CAknEnumeratedTextPopupSettingItem
public:
	CIapSettingItem(TInt aResourceId, TInt& aValue);
	
protected:
	void CompleteConstructionL();

	// New properties and methods
private:
	void FillIapsListL(); // Load list of available IAPs
	
	};

#endif /* __SETTINGSLISTBOX_H_ */
