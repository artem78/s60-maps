/*
 * Utils.h
 *
 *  Created on: 24.11.2021
 *      Author: artem78
 */

#ifndef UTILS_H
#define UTILS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

class MathUtils
	{
public:
	static TInt Digits(TInt aNum);
	//static TInt IntRound(TInt aNum, TInt aDigits);
	static TInt IntFloor(TInt aNum, TInt aDigits);
	
	/*
	 * Searchs for integer number in given descriptor. For example: alpha1234beta --> 1234.
	 * Note that only first number wil be returned: aaa456bbb789 --> 456.
	 * 
	 * @param	aDes - input string
	 * @param	aResNum - integer number if found
	 * @return	 KErrNone or KErrNotFound when error
	 */
	static TInt ParseInt(const TDesC& aDes, TInt& aResNum);
	};

#endif // UTILS_H
