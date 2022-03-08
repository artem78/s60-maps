/*
 * ApiKeys.h
 *
 *  Created on: 08.03.2022
 *      Author: artem78
 */

#ifndef APIKEYS_H_
#define APIKEYS_H_

// API Keys
// You can get your own here: https://manage.thunderforest.com/
#ifdef _DEBUG
_LIT8(KThunderForestApiKey, ""); // Development key
#else
_LIT8(KThunderForestApiKey, ""); // Production key
#endif

#endif /* APIKEYS_H_ */
