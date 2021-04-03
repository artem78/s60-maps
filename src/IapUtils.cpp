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
	TBool isFound = EFalse;
	
	CCommsDatabase* commsDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CCommsDbTableView* commsView = NULL;
	TRAPD(r, commsView = commsDb->OpenIAPTableViewMatchingNetworkLC(anIapId);
				CleanupStack::Pop(commsView));
	if (r == KErrNone)
		isFound = commsView->GotoFirstRecord() == KErrNone;
	else
		isFound = EFalse;
	
	delete commsView;
	delete commsDb;
	
	return isFound;
	}

TUint32 IapUtils::GetFirstIapL()
	{	
	TUint32 iapId = 0;
	
	CCommsDatabase* commsDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(commsDb);
	CCommsDbTableView* commsView = commsDb->OpenIAPTableViewMatchingBearerSetLC(
			ECommDbBearerGPRS | ECommDbBearerWLAN, 
			ECommDbConnectionDirectionOutgoing);
	
	if (commsView->GotoFirstRecord() == KErrNone)
		commsView->ReadUintL(TPtrC(COMMDB_ID), iapId);
	else
		User::Leave(KErrNotFound);
	
	CleanupStack::PopAndDestroy(2, commsDb);
	
	return iapId;
	}

/*TUint32 IapUtils::GetPreferredIapL()
	{
	// ToDo: Make this...
	}*/
