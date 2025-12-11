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
		case 21: return 2097152;
		case 22: return 4194304;
		case 23: return 8388608;
		case 24: return 16777216;
		case 25: return 33554432;
		case 26: return 67108864;
		case 27: return 134217728;
		case 28: return 268435456;
		case 29: return 536870912;
		case 30: return 1073741824;
		// should be enought and greater power will raise TInt overflow
		
		default:
			{
			_LIT(KPanicCat, "MathUtils::Pow2");
			User::Panic(KPanicCat, /*KErrNotSupported*/ KErrOverflow);
			}
		}
	
	return 0; // suppress "return value expected" warning
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


// TBounds

void TBounds::SetCoords(const TCoordinate &aTlCoord, const TCoordinate &aBrCoord)
	{
	iTlCoord = aTlCoord;
	iBrCoord = aBrCoord;
	}

void TBounds::SetCoords(TReal64 &aLat1, TReal64 &aLon1, TReal64 &aLat2, TReal64 &aLon2)
	{
	iTlCoord.SetCoordinate(Max(aLat1, aLat2), Min(aLon1, aLon2));
	iBrCoord.SetCoordinate(Min(aLat1, aLat2), Max(aLon1, aLon2));
	}

TBool TBounds::Contains(const TBounds &aCoordRect) const
	{
	return (aCoordRect.iTlCoord.Latitude() <= iTlCoord.Latitude())
			&& (aCoordRect.iTlCoord.Longitude() >= iTlCoord.Longitude())
			&& (aCoordRect.iBrCoord.Latitude() >= iBrCoord.Latitude())
			&& (aCoordRect.iBrCoord.Longitude() <= iBrCoord.Longitude());
	}

bool operator == (const TBounds &aCoordRect1, const TBounds &aCoordRect2)
	{
	return (aCoordRect1.iTlCoord.Longitude() == aCoordRect2.iTlCoord.Longitude())
			&& (aCoordRect1.iTlCoord.Latitude() == aCoordRect2.iTlCoord.Latitude())
			&& (aCoordRect1.iBrCoord.Longitude() == aCoordRect2.iBrCoord.Longitude())
			&& (aCoordRect1.iBrCoord.Latitude() == aCoordRect2.iBrCoord.Latitude());
	}


bool operator != (const TBounds &aCoordRect1, const TBounds &aCoordRect2)
	{
	return !(aCoordRect1 == aCoordRect2);
	}

void TBounds::Join(const TBounds &aCoordRect)
	{
	iTlCoord.SetCoordinate(
			Max(iTlCoord.Latitude(), aCoordRect.iTlCoord.Latitude()),
			Min(iTlCoord.Longitude(), aCoordRect.iTlCoord.Longitude())
		);
	iBrCoord.SetCoordinate(
			Min(iBrCoord.Latitude(), aCoordRect.iBrCoord.Latitude()),
			Max(iBrCoord.Longitude(), aCoordRect.iBrCoord.Longitude())
		);
	}

void TBounds::Center(TCoordinate &aCoord) const
	{
	aCoord.SetCoordinate(
			(iTlCoord.Latitude() + iBrCoord.Latitude()) / 2,
			(iTlCoord.Longitude() + iBrCoord.Longitude()) / 2
		);
	}


// MiscUtils

void MiscUtils::ErrorToDes(TInt aErrCode, TDes &aText)
	{
	/* Just parsed from e32err.r with https://regex101.com/r/XMYthY/1
	   and https://onecompiler.com/python/43wvdc6jf
	   
	   todo: replace switch/case to:
			1) strings in resource file
			2) or constant array of strings and get string by index from it */
	
	switch (aErrCode)
		{
		case KErrNone: // 0
			{
			_LIT(KText, "No error");
			aText.Copy(KText);
			break;
			}

		case KErrNotFound: // -1
			{
			_LIT(KText, "Not found");
			aText.Copy(KText);
			break;
			}

		case KErrGeneral: // -2
			{
			_LIT(KText, "General error");
			aText.Copy(KText);
			break;
			}

		case KErrCancel: // -3
			{
			_LIT(KText, "Cancelled");
			aText.Copy(KText);
			break;
			}

		case KErrNoMemory: // -4
			{
			_LIT(KText, "Not enough memory");
			aText.Copy(KText);
			break;
			}

		case KErrNotSupported: // -5
			{
			_LIT(KText, "Not supported");
			aText.Copy(KText);
			break;
			}

		case KErrArgument: // -6
			{
			_LIT(KText, "Argument out of range");
			aText.Copy(KText);
			break;
			}

		case KErrTotalLossOfPrecision: // -7
			{
			_LIT(KText, "Loss of precision");
			aText.Copy(KText);
			break;
			}

		case KErrBadHandle: // -8
			{
			_LIT(KText, "Invalid handle");
			aText.Copy(KText);
			break;
			}

		case KErrOverflow: // -9
			{
			_LIT(KText, "Overflow");
			aText.Copy(KText);
			break;
			}

		case KErrUnderflow: // -10
			{
			_LIT(KText, "Underflow");
			aText.Copy(KText);
			break;
			}

		case KErrAlreadyExists: // -11
			{
			_LIT(KText, "Already exists");
			aText.Copy(KText);
			break;
			}

		case KErrPathNotFound: // -12
			{
			_LIT(KText, "Path not found");
			aText.Copy(KText);
			break;
			}

		case KErrDied: // -13
			{
			_LIT(KText, "Handle refers to died thread");
			aText.Copy(KText);
			break;
			}

		case KErrInUse: // -14
			{
			_LIT(KText, "Already in use");
			aText.Copy(KText);
			break;
			}

		case KErrServerTerminated: // -15
			{
			_LIT(KText, "Server terminated");
			aText.Copy(KText);
			break;
			}

		case KErrServerBusy: // -16
			{
			_LIT(KText, "Server busy");
			aText.Copy(KText);
			break;
			}

		case KErrCompletion: // -17
			{
			_LIT(KText, "Completion");
			aText.Copy(KText);
			break;
			}

		case KErrNotReady: // -18
			{
			_LIT(KText, "Not ready");
			aText.Copy(KText);
			break;
			}

		case KErrUnknown: // -19
			{
			_LIT(KText, "Unknown device type");
			aText.Copy(KText);
			break;
			}

		case KErrCorrupt: // -20
			{
			_LIT(KText, "Corrupted");
			aText.Copy(KText);
			break;
			}

		case KErrAccessDenied: // -21
			{
			_LIT(KText, "Access denied");
			aText.Copy(KText);
			break;
			}

		case KErrLocked: // -22
			{
			_LIT(KText, "Locked");
			aText.Copy(KText);
			break;
			}

		case KErrWrite: // -23
			{
			_LIT(KText, "Write error");
			aText.Copy(KText);
			break;
			}

		case KErrDisMounted: // -24
			{
			_LIT(KText, "Volume dismounted");
			aText.Copy(KText);
			break;
			}

		case KErrEof: // -25
			{
			_LIT(KText, "End of file");
			aText.Copy(KText);
			break;
			}

		case KErrDiskFull: // -26
			{
			_LIT(KText, "Disk full");
			aText.Copy(KText);
			break;
			}

		case KErrBadDriver: // -27
			{
			_LIT(KText, "Bad driver");
			aText.Copy(KText);
			break;
			}

		case KErrBadName: // -28
			{
			_LIT(KText, "Bad name");
			aText.Copy(KText);
			break;
			}

		case KErrCommsLineFail: // -29
			{
			_LIT(KText, "Communication line failed");
			aText.Copy(KText);
			break;
			}

		case KErrCommsFrame: // -30
			{
			_LIT(KText, "Communication frame error");
			aText.Copy(KText);
			break;
			}

		case KErrCommsOverrun: // -31
			{
			_LIT(KText, "Communication overrun");
			aText.Copy(KText);
			break;
			}

		case KErrCommsParity: // -32
			{
			_LIT(KText, "Communication parity error");
			aText.Copy(KText);
			break;
			}

		case KErrTimedOut: // -33
			{
			_LIT(KText, "Timed out");
			aText.Copy(KText);
			break;
			}

		case KErrCouldNotConnect: // -34
			{
			_LIT(KText, "Could not connect");
			aText.Copy(KText);
			break;
			}

		case KErrCouldNotDisconnect: // -35
			{
			_LIT(KText, "Could not disconnect");
			aText.Copy(KText);
			break;
			}

		case KErrDisconnected: // -36
			{
			_LIT(KText, "Disconnected");
			aText.Copy(KText);
			break;
			}

		case KErrBadLibraryEntryPoint: // -37
			{
			_LIT(KText, "Bad library entry point");
			aText.Copy(KText);
			break;
			}

		case KErrBadDescriptor: // -38
			{
			_LIT(KText, "Bad descriptor");
			aText.Copy(KText);
			break;
			}

		case KErrAbort: // -39
			{
			_LIT(KText, "Aborted");
			aText.Copy(KText);
			break;
			}

		case KErrTooBig: // -40
			{
			_LIT(KText, "Too big number");
			aText.Copy(KText);
			break;
			}

		case KErrDivideByZero: // -41
			{
			_LIT(KText, "Divide by zero");
			aText.Copy(KText);
			break;
			}

		case KErrBadPower: // -42
			{
			_LIT(KText, "Bad power");
			aText.Copy(KText);
			break;
			}

		case KErrDirFull: // -43
			{
			_LIT(KText, "Dir full");
			aText.Copy(KText);
			break;
			}

		case KErrHardwareNotAvailable: // -44
			{
			_LIT(KText, "Hardware not available");
			aText.Copy(KText);
			break;
			}

		case KErrSessionClosed: // -45
			{
			_LIT(KText, "Session closed");
			aText.Copy(KText);
			break;
			}

		case KErrPermissionDenied: // -46
			{
			_LIT(KText, "Permission denied");
			aText.Copy(KText);
			break;
			}

		case KErrExtensionNotSupported: // -47
			{
			_LIT(KText, "Extension not supported");
			aText.Copy(KText);
			break;
			}

		case KErrCommsBreak: // -48
			{
			_LIT(KText, "Break in communication operation");
			aText.Copy(KText);
			break;
			}

		case /*KErrNoSecureTime*/ -49: // Added in s60v5 SDK
			{
			_LIT(KText, "No secure time");
			aText.Copy(KText);
			break;
			}

		default:
			{
			_LIT(KFmt, "Error number %d");
			aText.Format(KFmt, aErrCode);
			break;
			}
		}
	}

void MiscUtils::WriteZeroesToStreamL(RWriteStream &aStream, TInt aLength)
	{
	RBuf8 tmp;
	tmp.CreateMaxL(aLength);
	tmp.CleanupClosePushL();
	tmp.FillZ();
	//aStream.WriteL(tmp); // ! wrong !
	aStream.WriteL(tmp.Ptr(), tmp.Length());
	CleanupStack::PopAndDestroy(&tmp);
	}

void MiscUtils::WriteTUint64ToStreamL(RWriteStream &aStream, const TUint64 &aNum)
	{
	aStream.WriteL((TUint8*)&aNum, sizeof(aNum));
	}

void MiscUtils::ReadTUint64FromStreamL(RReadStream &aStream, TUint64 &aNum)
	{
	aStream.ReadL((TUint8*)&aNum, sizeof(aNum));
	}

void MiscUtils::LanguageToIso639Code(TLanguage aLang, /*TDes*/ TBuf</*3*/2> &aCode)
	{
	const TInt ELangGalician = 103; // Not defined in s60v3 SDK
	
	aCode.Zero();
	
	switch (aLang)
		{
		case ELangEnglish:
			{
			aCode.Append('e');
			aCode.Append('n');
			//aCode.Append('g');
			break;
			}
			
		case ELangSpanish:
			{
			/*aCode.Append('s');
			aCode.Append('p');
			aCode.Append('a');*/
			aCode.Append('e');
			aCode.Append('s');
			break;
			}
			
		case ELangGalician:
			{
			aCode.Append('g');
			aCode.Append('l');
			//aCode.Append('g');
			break;
			}
			
		case ELangPortuguese:
			{
			/*aCode.Append('p');
			aCode.Append('o');
			aCode.Append('r');*/
			aCode.Append('p');
			aCode.Append('t');
			break;
			}
			
		case ELangRussian:
			{
			aCode.Append('r');
			aCode.Append('u');
			//aCode.Append('s');
			break;
			}
			
		case ELangPolish:
			{
			aCode.Append('p');
			//aCode.Append('o');
			aCode.Append('l');
			break;
			}
			
		case ELangHebrew:
			{
			aCode.Append('h');
			aCode.Append('e');
			//aCode.Append('b');
			break;
			}
			
		case ELangLatinAmericanSpanish: // todo: check code
			{
			/*aCode.Append('s'); // ?
			aCode.Append('p'); // ?
			aCode.Append('a'); // ?*/
			aCode.Append('e'); // ?
			aCode.Append('s'); // ?
			break;
			}
			
		case ELangUkrainian:
			{
			aCode.Append('u');
			aCode.Append('k');
			//aCode.Append('r');
			break;
			}
			
		default:
			{} // supress compiller warnings
		}
	}


// CWindowGcEx

CWindowGcEx::CWindowGcEx(CWsScreenDevice *aDevice)
		: CWindowGc(aDevice)
	{};

void CWindowGcEx::DrawOutlinedText(const TDesC &aBuf,const TPoint &aPos,
		const TRgb &aTextColor, const TRgb &aOutlineColor, TBool aHighQuality)
	{
	if (!aBuf.Length())
		{
		return; // nothing to draw
		}
	
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
	if (!aBuf.Length())
		{
		return; // nothing to draw
		}
	
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
