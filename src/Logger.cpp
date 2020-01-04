/*
 * Logger.cpp
 *
 *  Created on: 01.09.2019
 *      Author: user
 */

#include "Logger.h"

#if LOGGING_ENABLED

_LIT8(KLineBreak, "\r\n");
_LIT8(KTab, "\t");


// Set initial values for static properties
TBool Logger::iIsConfigured = EFalse;
RFile Logger::iFile = RFile();


Logger::Logger()
	{
	// No implemenation needed
	}

void Logger::Configure(const RFile &aFile)
	{
	iFile = aFile;
	iIsConfigured = ETrue;
	}

void Logger::Write(const TDesC8 &aModule, const TDesC8 &aDes)
	{
	// Deny write to not configured logger
	if (!iIsConfigured)
		return;
	
	// Strings
	_LIT(KTimeFormat, "%H:%T:%S.%*C3");
	_LIT8(KOpeningSquareBracket, "[");
	_LIT8(KClosingSquareBracket, "]");
	_LIT8(KThreeSpaces, "   ");	
	
	// Print current time
	TBuf<20> timeBuff;
	TTime time;
	time.HomeTime();
	time.FormatL(timeBuff, KTimeFormat);
	
	TBuf8<20> timeBuff8;
	timeBuff8.Copy(timeBuff);
	iFile.Write(timeBuff8);

	// Print module/class/function name
	iFile.Write(KTab);
	iFile.Write(KOpeningSquareBracket);
	iFile.Write(aModule);
	iFile.Write(KClosingSquareBracket);
	iFile.Write(KThreeSpaces);
	
	// Print message
	iFile.Write(aDes);
	iFile.Write(KLineBreak);
	}

void Logger::WriteFormat(const TDesC8 &aModule, TRefByValue<const TDesC8> aFmt, ...)
	{
	// ToDo: Add format for date and time
	
	// Deny write to not configured logger
	if (!iIsConfigured)
		return;

	VA_LIST list;
	VA_START(list, aFmt);
	
	TBuf8<256> buff;
	buff.Zero();
	buff.FormatList(aFmt, list);
	VA_END(list);
	
	Write(aModule, buff);
	}

//void Logger::WriteEmptyLine()
//	{
//	iFile.Write(KLineBreak);
//	}

#endif
