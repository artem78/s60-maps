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


class TRectEx : public TRect
	{
public:
	TRectEx(TInt aAx, TInt aAy, TInt aBx, TInt aBy);
	
	inline TPoint TopRight()
		{
		return TPoint(iBr.iX, iTl.iY);
		};
	
	inline TPoint BottomLeft()
		{
		return TPoint(iTl.iX, iBr.iY);
		};
	};

class StrUtils
	{
public:
	static TBool ContainsL(const TDesC16& aStr, const TDesC16& aSubstr, TBool aIgnoreCase = EFalse);
	static TBool ContainsL(const TDesC8& aStr, const TDesC8& aSubstr, TBool aIgnoreCase = EFalse);
	static TBool StartsWithL(const TDesC16& aStr, const TDesC16& aSubstr, TBool aIgnoreCase = EFalse);
	static TBool StartsWithL(const TDesC8& aStr, const TDesC8& aSubstr, TBool aIgnoreCase = EFalse);
	//static TBool EndsWithL(const TDesC16& aStr, const TDesC16& aSubstr, TBool aIgnoreCase = EFalse);
	//static TBool EndsWithL(const TDesC8& aStr, const TDesC8& aSubstr, TBool aIgnoreCase = EFalse);
	};

#endif // UTILS_H
