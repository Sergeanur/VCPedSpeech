// Ped Speech Patch for Vice City
// Made by Sergeanur in 2015

#include <windows.h>
#include <iostream>

#define nop(a, s) _nop((void*)(a), (s))

static void _nop(void* pAddress, DWORD size)
{
	register DWORD dwValue;
	if (!VirtualProtect((void*)pAddress, size, PAGE_EXECUTE_READWRITE, &dwValue))
		return;

	DWORD dwAddress = (DWORD)pAddress;
	if ( size % 2 )
	{
		*(BYTE*)pAddress = 0x90;
		dwAddress++;
	}
	if ( size - ( size % 2 ) )
	{
		DWORD sizeCopy = size - ( size % 2 );
		do
		{
			*(WORD*)dwAddress = 0xFF8B;
			dwAddress += 2;
			sizeCopy -= 2;
		}
		while ( sizeCopy );	
	}

	VirtualProtect((void*)pAddress, size, dwValue, NULL);
}

enum
{
	VER_RETAIL_10 = 0,
	VER_RETAIL_11,
	VER_STEAM
};
int EXEver = -1;

int WINAPI GetEXEVersion()
{
	if (EXEver == -1)
	{
		if (*(DWORD*)0x667BF4 == 0x5548EC83) EXEver = VER_RETAIL_10;	// 1.0
		else if (*(DWORD*)0x667C44 == 0x5548EC83) EXEver = VER_RETAIL_11;	// 1.1
		else if (*(DWORD*)0x666BA4 == 0x5548EC83) EXEver = VER_STEAM;	// Steam
	}
	return EXEver;
};

DWORD VersionAddress(DWORD adr1_0, DWORD adr1_1, DWORD adr_steam)
{
	switch(GetEXEVersion())
	{
	case VER_RETAIL_10: return adr1_0;
	case VER_RETAIL_11: return adr1_1;
	case VER_STEAM: return adr_steam;
	default: return 0;
	}
}

template <class T>
void memset_v(T* address, T value)
{
	register DWORD dwValue;
	if (!VirtualProtect((void*)address, sizeof(T), PAGE_EXECUTE_READWRITE, &dwValue))
		return;

	*address = value;
	VirtualProtect((void*)address, sizeof(T), dwValue, NULL);
}

void PatchTommy()
{
	memset_v((BYTE*)VersionAddress(0x5E9AD0, 0x5E9AF0, 0x5E9730), (BYTE)0xEB);
	memset_v((BYTE*)VersionAddress(0x5E9B70, 0x5E9B90, 0x5E97D0), (BYTE)0xEB);
	memset_v((BYTE*)VersionAddress(0x5E9C13, 0x5E9C33, 0x5E9873), (BYTE)0xEB);
	memset_v((BYTE*)VersionAddress(0x5E9CB6, 0x5E9CD6, 0x5E9916), (BYTE)0xEB);
	memset_v((BYTE*)VersionAddress(0x5E9D60, 0x5E9D80, 0x5E99C0), (BYTE)0xEB);
	memset_v((BYTE*)VersionAddress(0x5E9E03, 0x5E9E23, 0x5E9A63), (BYTE)0xEB);
	memset_v((BYTE*)VersionAddress(0x5E9EA6, 0x5E9EC6, 0x5E9B06), (BYTE)0xEB);
	memset_v((BYTE*)VersionAddress(0x5E9F50, 0x5E9F70, 0x5E9BB0), (BYTE)0xEB);
	memset_v((BYTE*)VersionAddress(0x5EA043, 0x5EA063, 0x5E9CA3), (BYTE)0xEB);
}

void Patch(HMODULE& dllModule)
{
	nop(VersionAddress(0x5DDBD4, 0x5DDBF4, 0x5DD834), 6);

	// CPed::Say
	memset_v((BYTE*)VersionAddress(0x5227A9, 0x5227C9, 0x522699), (BYTE)0xC3);
	memset_v((BYTE*)VersionAddress(0x5227F2, 0x522812, 0x5226E2), (BYTE)0xD3);
	nop(VersionAddress(0x5227F3, 0x522813, 0x5226E3), 4);
	
	char modulePath[MAX_PATH], value[32];

	GetModuleFileName(dllModule, modulePath, MAX_PATH);
	*strrchr(modulePath, '.') = 0;
	strcat(modulePath, ".ini");

	int DisablePlayerRestrictions = 0;
	GetPrivateProfileString("PedSpeech", "DisablePlayerRestrictions", "0", value, sizeof(value), modulePath);
	sscanf(value, "%i", &DisablePlayerRestrictions);

	if (DisablePlayerRestrictions > 0)
		PatchTommy();
}

BOOL APIENTRY DllMain (HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	//	TURN OFF DLL_THREAD_ATTACH & DLL_THREAD_DETACH
	DisableThreadLibraryCalls( hModule );
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (GetEXEVersion() == -1) return FALSE;
		Patch(hModule);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

