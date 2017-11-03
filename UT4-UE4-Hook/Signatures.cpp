#include "Signatures.h"
#include "Logging.h"

DWORD64 dwSignatures[NumSigs] = { 0 };

bool bDataCompare(const BYTE *pData, const BYTE *bMask, const char *szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask) if (*szMask == 'x' && *pData != *bMask) return false;
	return (*szMask) == NULL;
}

DWORD64 dwFindPattern(DWORD64 dwAddress, DWORD dwLen, BYTE *bMask, char *szMask)
{
	for (DWORD i = 0; i < dwLen; i++) if (bDataCompare((BYTE*)(dwAddress + i), bMask, szMask)) return (DWORD64)(dwAddress + i);
	return 0;
}

void LoadSignatures()
{

	DWORD64 UCoreAddress = (DWORD64)GetModuleHandle("UE4-Core-Win64-Shipping.dll") + 0x1000;
	DWORD64 UObjectCoreAddress = (DWORD64)GetModuleHandle("UE4-CoreUObject-Win64-Shipping.dll") + 0x1000;
	DWORD64 UTAddress = (DWORD64)GetModuleHandle("UE4-UnrealTournament-Win64-Shipping.dll") + 0x1000;
	DWORD64 UEngineAddress = (DWORD64)GetModuleHandle("UE4-Engine-Win64-Shipping.dll") + 0x1000;
	 
	// GObjects pointer used to dump UE4-SDK related objects
	dwSignatures[0] = dwFindPattern(UObjectCoreAddress, 0x160000, (BYTE*)"\x48\x8D\x0D\x00\x00\x00\x00\xC6\x05", "xxx????xx");

	// GNames pointer used to dump UE4-SDK related objects
	dwSignatures[1] = dwFindPattern(UCoreAddress, 0x1e0000, (BYTE*)"\x48\x89\x1D\x00\x00\x00\x00\x48\x8B\x5C\x24\x20", "xxx????xxxxx");
	
	// ProcessEvent (x64) address (detour sizes 8, 10, 12, 19, 24)
	dwSignatures[2] = dwFindPattern(UObjectCoreAddress, 0x160000, (BYTE*)"\x40\x55\x56\x57\x41\x54\x41\x55", "xxxxxxxx");

	// GameViewClient.PostRender (x64) address (detour sizes 19, 23)
	dwSignatures[3] = dwFindPattern(UTAddress, 0x1000000, (BYTE*)"\x48\x8B\xC4\x48\x89\x50\x10\x55\x48\x8D\x68\xA1", "xxxxxxxxxxxx");

	// GWorld pointer
	dwSignatures[4] = dwFindPattern(UEngineAddress, 0x1000000, (BYTE*)"\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x5C\x24\x00\x48\x0F\x45\xC1", "xxx????xxxx?xxxx");

	// Automatically exit if a signature fails 
	for (int i = 0; i < NumSigs; i++) if (dwSignatures[i] == 0)
	{
		#ifdef Logging
		LogSignatures();
		#endif
		exit(0); 
	}

}

#ifdef Logging
void LogSignatures()
{
	for (int i = 0; i < NumSigs; i++) add_log("%i: 0x%x", i, dwSignatures[i]);
}
#endif


