#include <Windows.h>
#ifndef _SignaturesHeader_
#define _SignaturesHeader_
#define NumSigs 5

extern DWORD64 dwSignatures[NumSigs];
DWORD64 dwFindPattern(DWORD64 dwAddress, DWORD dwLen, BYTE *bMask, char *szMask);
bool bDataCompare(const BYTE *pData, const BYTE *bMask, const char *szMask);
void LoadSignatures();


#ifdef Logging
extern void LogSignatures();
#endif

#endif
