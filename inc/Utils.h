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
#include <lbsposition.h>
#include <w32std.h>
#include <eikenv.h>
#include <eikinfo.h>

// Constants

const TInt KMaxErrorNameLength = 32;

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
	
	/*
	 * Returns power of two (2^aPower)
	 *
	 * @param	TInt - 0 <= aPower <= 30 (greater values will raise panic due to TInt result overflow)
	 */
	static TInt Pow2(TInt aPower);
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
	static TBool Contains(const TDesC16& aStr, const TDesC16& aSubstr, TBool aIgnoreCase = EFalse);
	static TBool Contains(const TDesC8& aStr, const TDesC8& aSubstr, TBool aIgnoreCase = EFalse);
	static TBool StartsWith(const TDesC16& aStr, const TDesC16& aSubstr, TBool aIgnoreCase = EFalse);
	static TBool StartsWith(const TDesC8& aStr, const TDesC8& aSubstr, TBool aIgnoreCase = EFalse);
	//static TBool EndsWith(const TDesC16& aStr, const TDesC16& aSubstr, TBool aIgnoreCase = EFalse);
	//static TBool EndsWith(const TDesC8& aStr, const TDesC8& aSubstr, TBool aIgnoreCase = EFalse);
	};


// Similar as TRect but for couple of TCoordinate objects
class TCoordRect
	{
public:
	TCoordinate iTlCoord, iBrCoord;
	
	void SetCoords(const TCoordinate &aTlCoord, const TCoordinate &aBrCoord);
	
	// Returns ETrue if given rect completely located inside this rect
	TBool Contains(const TCoordRect &aCoordRect) const;
	
    friend bool operator == (const TCoordRect &aCoordRect1, const TCoordRect &aCoordRect2);
    friend bool operator != (const TCoordRect &aCoordRect1, const TCoordRect &aCoordRect2);
	};


// Other utils
class MiscUtils
	{
public:
	inline static void DbgMsg(const TDesC &aMsg)
		{
			CEikonEnv::Static()->InfoMsg(aMsg);
		};
	
	/* 
	 * Converts given error code to string representation (KErrNotFound => "Not found",
	 * KErrNoError => "No error" and etc.). For unknown codes returns "Error number -XXX".
	 * Defined error names up to KErrNoSecureTime (-49).
	 * 
	 * @param	aErrCode - error code
	 * @param	aText - output descriptor (KMaxErrorNameLength chars should be enough)
	 *  
	 */
	static void ErrorToDes(TInt aErrCode, TDes &aText);
	
	/*
	 * Writes NULL bytes to given stream
	 * 
	 * @param	aStream - Stream for write
	 * @param	aLength - Length (in bytes)
	 */
	static void WriteZeroesToStreamL(RWriteStream &aStream, TInt aLength=1);
	static void WriteTUint64ToStreamL(RWriteStream &aStream, const TUint64 &aNum);
	static void ReadTUint64FromStreamL(RReadStream &aStream, TUint64 &aNum);
	
	static void LanguageToIso639Code(TLanguage aLang, /*TDes*/ TBuf</*3*/2> &aCode);

	};


// Extended CWindowGc class
class CWindowGcEx : public CWindowGc
	{
public:
	// Constructor
	CWindowGcEx(CWsScreenDevice *aDevice);
	
	/*
	 * Similar as DrawText(), but adds 1px outline around the text.
	 * Note: this method doesn't use pen color from context,
	 * set aTextColor instead (default is black).
	 * 
	 * @param	aHighQuality -
	 * 			ETrue - better quality for outline, but more draw operations needed (1+8 ops.)
	 * 			EFalse - faster, but may be less accurate (especially with thin font) (1+4 ops.)
	 * 			Preview: https://ibb.co/C5mdkLLJ
	 */
	void DrawOutlinedText(const TDesC &aBuf,const TPoint &aPos,
			const TRgb &aTextColor = KRgbBlack,
			const TRgb &aOutlineColor = KRgbWhite,
			TBool aHighQuality = EFalse);
	
	/*
	 * Similar as DrawText(), but adds 1px outline around the text.
	 * Note: this method doesn't use pen color from context,
	 * set aTextColor instead (default is black).
	 * 
	 * @param	aHighQuality -
	 * 			ETrue - better quality for outline, but more draw operations needed (1+8 ops.)
	 * 			EFalse - faster, but may be less accurate (especially with thin font) (1+4 ops.)
	 * 			Preview: https://ibb.co/C5mdkLLJ
	 */
	void DrawOutlinedText(const TDesC &aBuf,const TRect &aBox,TInt aBaselineOffset,
			TTextAlign aHoriz=ELeft,TInt aLeftMrg=0, const TRgb &aTextColor = KRgbBlack,
			const TRgb &aOutlineColor = KRgbWhite,
			TBool aHighQuality = EFalse);
	};


// Holds bounding box defined with top-left and bottom-right TCoordinate
class /*TCoordRect*/ /*TBoundingRect*/ TBounds
	{
public:
	TCoordinate iTlCoord, iBrCoord;
	
	void SetCoords(const TCoordinate &aTlCoord, const TCoordinate &aBrCoord);
	void SetCoords(TReal64 &aLat1, TReal64 &aLon1, TReal64 &aLat2, TReal64 &aLon2);
	
	// Returns ETrue if given rect completely located inside this rect
	TBool Contains(const TBounds &aCoordRect) const;
	
	// Sets this bound to minimum bound which contains both this bound and specified bound.
	// This is similar to TRect::BoundingRect().
	void Join(const TBounds &aCoordRect);
	
	void Center(TCoordinate &aCoord) const;
	
    friend bool operator == (const TBounds &aCoordRect1, const TBounds &aCoordRect2);
    friend bool operator != (const TBounds &aCoordRect1, const TBounds &aCoordRect2);
	};

#endif // UTILS_H
