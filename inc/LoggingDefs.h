/*
 * LoggingDefs.h
 *
 *  Created on: 28.02.2020
 *      Author: user
 */

#ifndef LOGGINGDEFS_H_
#define LOGGINGDEFS_H_

// ToDo: Is there any way to global defining
// of LOGGING_ENABLED constant with different values
// for debug and release builds without including this
// header file in other cpp files where this constant need to be checked?

// Define constant for enable logging only in Debug builds
#ifdef _DEBUG
#define LOGGING_ENABLED 1
#else
#define LOGGING_ENABLED 0
#endif

#endif /* LOGGINGDEFS_H_ */
