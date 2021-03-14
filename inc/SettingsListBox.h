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

#endif /* __SETTINGSLISTBOX_H_ */
