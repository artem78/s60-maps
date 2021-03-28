/*
 * IapUtils.cpp
 *
 *  Created on: 27.03.2021
 *      Author: artem78
 */

#include "IapUtils.h"
#include <metadatabase.h> // CMDBSession, CMDBRecordSet
#include <commsdattypesv1_1.h> // CCDIAPRecord, KCDTIdIAPRecord

// Constants
const TInt KDefaultIapArrayGranularity = 5 /*10*/;


void IapUtils::GetAllIapsL(CIapArray* anArray)
	{
	using namespace CommsDat;
	
	anArray->Reset();
	
	// Open the comms repository
	CMDBSession* db = CMDBSession::NewLC(KCDVersion1_1 /*KCDCurrentVersion*/);
	// Create a recordset (table) of IAPs
	CMDBRecordSet<CCDIAPRecord>* iapRecordSet =
			new (ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecordSet);
	// Load the IAP recordset from the comms repository
	iapRecordSet->LoadL(*db);
	// Iterate through the recordset
	CCDIAPRecord* iap = NULL;
	for (TInt i = 0; i < iapRecordSet->iRecords.Count(); ++i)
		{
		// Get the IAP record (CCDIAPRecord)
		iap = static_cast<CCDIAPRecord*>(iapRecordSet->iRecords[i]);
		// We can now get information about the IAP.
		
		TIapItem item;
		item.iId = iap->RecordId();
		
		/////////////
		_LIT(KFmt, "%S (id %d)");
		TPtrC ptr(static_cast<const TDesC&>(iap->iRecordName));
		item.iName.Format(KFmt, /*&iap->iRecordName*/ &ptr, iap->RecordId());
		/////////////
		
		//item.iName.Copy(iap->iRecordName);
		
		anArray->AppendL(item);
		}
	// Close the IAP recordset and the database session
	CleanupStack::PopAndDestroy(2, db);
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

TUint32 IapUtils::GetPreferredIapL()
	{
	CIapArray* iaps = new (ELeave) CIapArray(KDefaultIapArrayGranularity);
	CleanupStack::PushL(iaps);
	
	GetAllIapsL(iaps);
	
	if (iaps->Count() == 0)
		User::Leave(KErrNotFound);
	
	// ToDo: Use connection preference
	
	// Return first IAP in array
	TUint32 iap = (*iaps)[0].iId;
	
	CleanupStack::PopAndDestroy(iaps);
	
	return iap;
	}
