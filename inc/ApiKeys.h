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
_LIT8(KThunderForestApiKey, "PLACE_YOUR_KEY_HERE"); // Development key
#else
_LIT8(KThunderForestApiKey, "PLACE_YOUR_KEY_HERE"); // Production key
#endif

// OpenRouteService.org (routing API)
// NOTE:  Register on https://account.heigit.org/signup to get you own API-key (absolutely free)
_LIT8(KORSApiKey, "PLACE_YOUR_KEY_HERE");


#endif /* APIKEYS_H_ */
