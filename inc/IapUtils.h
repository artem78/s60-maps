/*
 * IapUtils.h
 *
 *  Created on: 27.03.2021
 *      Author: artem78
 */

#ifndef IAPUTILS_H_
#define IAPUTILS_H_

#include <e32base.h>

class TIapItem
	{
public:
	TUint32 iId;
	/*HBufC**/ TBuf<32> iName;
	};

typedef CArrayFixFlat<TIapItem> CIapArray;

class IapUtils
	{
public:
	static void GetAllIapsL(CIapArray* anArray); // Get array of all available IAPs
	static TBool IsIapAvailableL(TUint32 anIapId);
	static TUint32 GetPreferredIapL();
	};

#endif /* IAPUTILS_H_ */
