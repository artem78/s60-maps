// Main definitions

#ifndef DEFS_H_
#define DEFS_H_

// Main definitions
const TVersion KProgramVersion(1, 7, 2);

// Debugging
/*#ifdef _DEBUG
#define DEBUG_SHOW_ADDITIONAL_INFO
#endif*/

// Types
typedef /*TUInt8*/ TInt TZoom;
const TReal KNaN = 0.0 / 0.0;

// API Keys
// You can get your own here: https://manage.thunderforest.com/
#ifdef _DEBUG
_LIT8(KThunderForestApiKey, ""); // Development key
#else
_LIT8(KThunderForestApiKey, ""); // Production key
#endif

#endif /* DEFS_H_ */
