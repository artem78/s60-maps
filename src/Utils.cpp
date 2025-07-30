/*
 * Utils.cpp
 *
 *  Created on: 24.11.2021
 *      Author: artem78
 */

#include "Utils.h"
#include <e32math.h>

TInt MathUtils::Digits(TInt aNum)
	{
	//if (aNum == 0)
	//	return 0;
	
	TBuf<16> tmp; // Enough for int32 text representation
	tmp.Num(aNum);
	return tmp.Length();
	}

/*TInt MathUtils::IntRound(TInt aNum, TInt aDigits)
	{
	
	}*/

TInt MathUtils::IntFloor(TInt aNum, TInt aDigits)
	{
	TReal p;
	Math::Pow10(p, aDigits);
	return aNum - aNum % (TInt) p;
	}

TInt MathUtils::ParseInt(const TDesC& aDes, TInt& aResNum)
	{
	TInt start = -1;
	TInt end = -1;
	
	for (TInt i = 0; i < aDes.Length(); i++)
		{
		TChar ch = aDes[i];
		if (start == -1 && ch.IsDigit())
			{
			start = i;
			}
		else if (start != -1 && !ch.IsDigit())
			{
			end = i;
			break;
			}
		}
		
	if (start != -1 && end == -1)
		{
		end = aDes.Length() - 1;
		}
	
	if (start == -1 || end == -1)
		{
		return KErrNotFound;
		}
	
	TPtrC numPtr = aDes.Mid(start, end - start + 1);
	
	TLex lex(numPtr);
	TInt number = 0;
	if (lex.Val(number) == KErrNone)
		{
		aResNum = number;
		return KErrNone;
		}
	
	return KErrNotFound;	
	}


// TRectEx

TRectEx::TRectEx(TInt aAx, TInt aAy, TInt aBx, TInt aBy)
		: TRect(aAx, aAy, aBx, aBy)
	{

	}


// StrUtils
// todo: reduce code duplications

TBool StrUtils::Contains(const TDesC& aStr, const TDesC& aSubstr, TBool aIgnoreCase)
	{
	if (aStr.Length() < aSubstr.Length())
		{
		return EFalse;
		}
	
	if (!aIgnoreCase)
		{
		return aStr.Find(aSubstr) != KErrNotFound;
		}
	else
		{		
		for (TInt pos = 0; pos <= aStr.Length() - aSubstr.Length(); pos++)
			{
			TPtrC s = aStr.Mid(pos, aSubstr.Length());
			if (StartsWith(s, aSubstr, aIgnoreCase))
				{
				return ETrue;
				}
			}
		}
		return EFalse;
	}

TBool StrUtils::Contains(const TDesC8& aStr, const TDesC8& aSubstr, TBool aIgnoreCase)
	{
	if (aStr.Length() < aSubstr.Length())
		{
		return EFalse;
		}
	
	if (!aIgnoreCase)
		{
		return aStr.Find(aSubstr) != KErrNotFound;
		}
	else
		{		
		for (TInt pos = 0; pos <= aStr.Length() - aSubstr.Length(); pos++)
			{
			TPtrC8 s = aStr.Mid(pos, aSubstr.Length());
			if (StartsWith(s, aSubstr, aIgnoreCase))
				{
				return ETrue;
				}
			}
		}
		return EFalse;
	}

TBool StrUtils::StartsWith(const TDesC& aStr, const TDesC& aSubstr, TBool aIgnoreCase)
	{
	if (aStr.Length() < aSubstr.Length())
		{
		return EFalse;
		}
	
	for (TInt i = 0; i < aSubstr.Length(); i++)
		{
		TChar ch1, ch2;
		ch1 = aStr[i];
		ch2 = aSubstr[i];
		if (aIgnoreCase)
			{
			ch1.LowerCase();
			ch2.LowerCase();
			}
		
		if (ch1 != ch2)
			{
			return EFalse;
			}
		}
	return ETrue;
	}

TBool StrUtils::StartsWith(const TDesC8& aStr, const TDesC8& aSubstr, TBool aIgnoreCase)
	{
	if (aStr.Length() < aSubstr.Length())
		{
		return EFalse;
		}
	
	for (TInt i = 0; i < aSubstr.Length(); i++)
		{
		TChar ch1, ch2;
		ch1 = aStr[i];
		ch2 = aSubstr[i];
		if (aIgnoreCase)
			{
			ch1.LowerCase();
			ch2.LowerCase();
			}
		
		if (ch1 != ch2)
			{
			return EFalse;
			}
		}
	return ETrue;
	}

/*TBool StrUtils::EndsWith(const TDesC16& aStr, const TDesC16& aSubstr, TBool aIgnoreCase)
	{
	//...
	}
	
TBool StrUtils::EndsWith(const TDesC8& aStr, const TDesC8& aSubstr, TBool aIgnoreCase)
	{
	//...
	}
*/
