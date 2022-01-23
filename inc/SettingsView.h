/*
 * SettingsView.h
 *
 *  Created on: 09.03.2021
 *      Author: artem78
 */

#ifndef __SETTINGSVIEW_H_
#define __SETTINGSVIEW_H_

#include <aknview.h>
#include "S60Maps.hrh"
#include "SettingsListBox.h"

class CSettingsView : public CAknView
	{
	// Constructors / destructors
public:
	~CSettingsView();
	static CSettingsView* NewL();
	static CSettingsView* NewLC();

private:
	CSettingsView();
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
	
	
	// Custom properties and methods

private:
	// Controls
	CSettingsListBox* iListBox;
	
	void ConstructContainerL();
	
	};

#endif /* __SETTINGSVIEW_H_ */
