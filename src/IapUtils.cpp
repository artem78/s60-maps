/*
 * IapUtils.cpp
 *
 *  Created on: 27.03.2021
 *      Author: artem78
 */

#include "IapUtils.h"

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
