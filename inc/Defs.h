// Main definitions

#ifndef DEFS_H_
#define DEFS_H_

// Main definitions
const TVersion KProgramVersion(1, 7, 1);

// Debugging
/*#ifdef _DEBUG
#define DEBUG_SHOW_ADDITIONAL_INFO
#endif*/

// Types
typedef /*TUInt8*/ TInt TZoom;
const TReal KNaN = 0.0 / 0.0;

// API Keys
#ifdef _DEBUG
_LIT(KThunderForestApiKey, "c27959d0b4784cd58b671dc611660c5e"); // Development key
#else
_LIT(KThunderForestApiKey, "96a5d596e0cd4bdda652031db7b46049"); // Production key
#endif

#endif /* DEFS_H_ */
