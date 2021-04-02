/*
 * IapUtils.cpp
 *
 *  Created on: 27.03.2021
 *      Author: artem78
 */

#include "IapUtils.h"
#include <commdb.h>

// Constants
const TInt KDefaultIapArrayGranularity = 5 /*10*/;


void IapUtils::GetAllIapsL(CIapArray* anArray)
	{
	anArray->Reset();
	
	CCommsDatabase* commsDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(commsDb);
	CCommsDbTableView* commsView = commsDb->OpenIAPTableViewMatchingBearerSetLC(
			ECommDbBearerGPRS | ECommDbBearerWLAN, 
			ECommDbConnectionDirectionOutgoing);
	
	if (commsView->GotoFirstRecord() == KErrNone)
		{
		do
			{
			TBuf<KCommsDbSvrMaxColumnNameLength> iapName;
			TUint32 iapId;

			commsView->ReadTextL(TPtrC(COMMDB_NAME), iapName);
			commsView->ReadUintL(TPtrC(COMMDB_ID), iapId);
			
			TIapItem item;
			item.iId = iapId;
			
			/////////////
			_LIT(KFmt, "%S (id %d)");
			item.iName.Format(KFmt, &iapName, iapId);
			/////////////
			
			//item.iName.Copy(iapName); // ToDo: Uncomment this line
			
			anArray->AppendL(item);
			}
		while (commsView->GotoNextRecord() == KErrNone);
		}
			
	CleanupStack::PopAndDestroy(2, commsDb);
	}

TBool IapUtils::IsIapAvailableL(TUint32 anIapId)
	{
	CIapArray* iaps = new (ELeave) CIapArray(KDefaultIapArrayGranularity);
	CleanupStack::PushL(iaps);
	
	GetAllIapsL(iaps);
	
	TBool isFound = EFalse;
	for (TInt idx = 0; idx < iaps->Count(); idx++)
		{
		if ((*iaps)[idx].iId == anIapId)
			{
			isFound = ETrue;
			break;
			}
		}
	
	CleanupStack::PopAndDestroy(iaps);
	
	return isFound;
	}

TUint32 IapUtils::GetFirstIapL()
	{
	CIapArray* iaps = new (ELeave) CIapArray(KDefaultIapArrayGranularity);
	CleanupStack::PushL(iaps);
	
	GetAllIapsL(iaps);
	
	if (iaps->Count() == 0)
		User::Leave(KErrNotFound);
	
	// Return first IAP in array
	TUint32 iap = (*iaps)[0].iId;
	
	CleanupStack::PopAndDestroy(iaps);
	
	return iap;
	}

/*TUint32 IapUtils::GetPreferredIapL()
	{
	// ToDo: Make this...
	}*/
