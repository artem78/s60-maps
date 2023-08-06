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
#ifndef _DEBUG
#error Todo: Do not forget to set API keys for release build!
#endif
#ifdef _DEBUG
_LIT8(KThunderForestApiKey, ""); // Development key
#else
_LIT8(KThunderForestApiKey, ""); // Production key
#endif

#endif /* APIKEYS_H_ */
