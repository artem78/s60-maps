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
	 * @param	TInt - 0 <= aPower <= 20 (other values not yet implemented)
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
	static void DbgMsgL(const TDesC &aMsg);
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
	
    friend bool operator == (const TBounds &aCoordRect1, const TBounds &aCoordRect2);
    friend bool operator != (const TBounds &aCoordRect1, const TBounds &aCoordRect2);
	};

#endif // UTILS_H
