/*
 * Utils.cpp
 *
 *  Created on: 24.11.2021
 *      Author: artem78
 */

#include "Utils.h"
#include <e32math.h>
#include <aknglobalnote.h>

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

TInt MathUtils::Pow2(TInt aPower)
	{
	switch (aPower)
		{
		case 0: return 1;
		case 1: return 2;
		case 2: return 4;
		case 3: return 8;
		case 4: return 16;
		case 5: return 32;
		case 6: return 64;
		case 7: return 128;
		case 8: return 256;
		case 9: return 512;
		case 10: return 1024;
		case 11: return 2048;
		case 12: return 4096;
		case 13: return 8192;
		case 14: return 16384;
		case 15: return 32768;
		case 16: return 65536;
		case 17: return 131072;
		case 18: return 262144;
		case 19: return 524288;
		case 20: return 1048576;
		// should be enought
		
		default:
			{
			_LIT(KPanicCat,"MathUtils::Pow2");
			User::Panic(KPanicCat, KErrNotSupported);
			}
		}
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


// MiscUtils

void MiscUtils::DbgMsgL(const TDesC &aMsg)
	{
	TPtrC ptr(aMsg);
	CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
	globalNote->ShowNoteL(EAknGlobalInformationNote, ptr);
	CleanupStack::PopAndDestroy(globalNote);
	}


// CWindowGcEx

CWindowGcEx::CWindowGcEx(CWsScreenDevice *aDevice)
		: CWindowGc(aDevice)
	{};

void CWindowGcEx::DrawOutlinedText(const TDesC &aBuf,const TPoint &aPos,
		const TRgb &aTextColor, const TRgb &aOutlineColor, TBool aHighQuality)
	{
	TInt increment = aHighQuality ? 1 : 2;
	
	// Draw outline
	SetPenColor(aOutlineColor);
	for (TInt dx = -1; dx <= 1; dx += increment)
		{
		for (TInt dy = -1; dy <= 1; dy += increment)
			{
			if (dx == 0 && dy == 0)
				{
				continue;
				}
			
			TPoint outlinePoint(aPos);
			outlinePoint += TPoint(dx, dy);
			DrawText(aBuf, outlinePoint);
			}
		}
	
	// Draw text
	SetPenColor(aTextColor);
	DrawText(aBuf, aPos);
	}

void CWindowGcEx::DrawOutlinedText(const TDesC &aBuf,const TRect &aBox,TInt aBaselineOffset,
			TTextAlign aHoriz,TInt aLeftMrg, const TRgb &aTextColor,
			const TRgb &aOutlineColor, TBool aHighQuality)
	{
	TInt increment = aHighQuality ? 1 : 2;
	
	// Draw outline
	SetPenColor(aOutlineColor);
	for (TInt dx = -1; dx <= 1; dx += increment)
		{
		for (TInt dy = -1; dy <= 1; dy += increment)
			{
			if (dx == 0 && dy == 0)
				{
				continue;
				}
			
			TRect outlineBox(aBox);
			outlineBox.Move(dx, dy);
			DrawText(aBuf, outlineBox, aBaselineOffset, aHoriz, aLeftMrg);
			}
		}
	
	// Draw text
	SetPenColor(aTextColor);
	DrawText(aBuf, aBox, aBaselineOffset, aHoriz, aLeftMrg);
	}

