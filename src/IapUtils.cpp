/*
 * IapUtils.cpp
 *
 *  Created on: 27.03.2021
 *      Author: artem78
 */

#include "IapUtils.h"

// Constants
const TInt KDefaultIapArrayGranularity = 5 /*10*/;


void IapUtils::GetAllIapsL(CIapArray* anArray)
	{
	anArray->Reset();
	
	////// STUB //////
	// ToDo: Replace by real code
	for (TInt i = 1; i <= 5; i++)
		{
		TIapItem item;
		item.iId = i - 1;
		item.iName.Copy(_L("IAP "));
		item.iName.AppendNum(i);
		anArray->AppendL(item);
		}
	//////////////////
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
