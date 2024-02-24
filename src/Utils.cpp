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

// TCoordRect

void TCoordRect::SetCoords(const TCoordinate &aTlCoord, const TCoordinate &aBrCoord)
	{
	iTlCoord = aTlCoord;
	iBrCoord = aBrCoord;
	}

TBool TCoordRect::Contains(const TCoordRect &aCoordRect) const
	{
	return (aCoordRect.iTlCoord.Latitude() <= iTlCoord.Latitude())
			&& (aCoordRect.iTlCoord.Longitude() >= iTlCoord.Longitude())
			&& (aCoordRect.iBrCoord.Latitude() >= iBrCoord.Latitude())
			&& (aCoordRect.iBrCoord.Longitude() <= iBrCoord.Longitude());
	}

bool operator == (const TCoordRect &aCoordRect1, const TCoordRect &aCoordRect2)
	{
	return (aCoordRect1.iTlCoord.Longitude() == aCoordRect2.iTlCoord.Longitude())
			&& (aCoordRect1.iTlCoord.Latitude() == aCoordRect2.iTlCoord.Latitude())
			&& (aCoordRect1.iBrCoord.Longitude() == aCoordRect2.iBrCoord.Longitude())
			&& (aCoordRect1.iBrCoord.Latitude() == aCoordRect2.iBrCoord.Latitude());
	}

bool operator != (const TCoordRect &aCoordRect1, const TCoordRect &aCoordRect2)
	{
	return !(aCoordRect1 == aCoordRect2);
	}

