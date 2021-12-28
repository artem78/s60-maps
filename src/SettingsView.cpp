/*
 * SettingsView.cpp
 *
 *  Created on: 09.03.2021
 *      Author: artem78
 */

#include "SettingsView.h"
#include "S60Maps.hrh"
#include <S60Maps_0xED689B88.rsg>
//#include "S60MapsAppUi.h"
#include "aknviewappui.h"

// CSettingsView

CSettingsView::CSettingsView()
	{
	}

CSettingsView::~CSettingsView()
	{
	delete iListBox;
	}

CSettingsView* CSettingsView::NewLC()
	{
	CSettingsView* self = new (ELeave) CSettingsView();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSettingsView* CSettingsView::NewL()
	{
	CSettingsView* self = CSettingsView::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CSettingsView::ConstructL()
	{
	CAknView::BaseConstructL(R_SETTINGS_VIEW);
	
	// Listbox will be initialized later on view activation
	}

TUid CSettingsView::Id() const
	{
	return TUid::Uid(ESettingsViewId);
	}

void CSettingsView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EAknSoftkeyBack:
			{
			AppUi()->ActivateLocalViewL(TUid::Uid(EMapViewId));
			}
			break;
			
		default:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		}
	}
	
void CSettingsView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
			TUid /*aCustomMessageId*/,
			const TDesC8& /*aCustomMessage*/)
	{
	StatusPane()->MakeVisible(ETrue); // If hidden by previous view
	if (!iListBox)
		{
		ConstructContainerL();
		
		AppUi()->AddToStackL(*this, iListBox);
		iListBox->SetRect(ClientRect());
		iListBox->ActivateL();
		iListBox->MakeVisible(ETrue);
		iListBox->DrawNow();
		}
	}

void CSettingsView::DoDeactivate()
	{
	if (iListBox)
		{
		AppUi()->RemoveFromStack(iListBox);
		
		delete iListBox;
		iListBox = NULL;
		}
	}

void CSettingsView::ConstructContainerL()
	{
	if (!iListBox)
		{
		iListBox = new (ELeave) CSettingsListBox;
		iListBox->SetMopParent(this);
		iListBox->ConstructFromResourceL(R_SETTING_ITEM_LIST);
		}
	}
