#include "stdafx.h"
#include "shared.h"
#include "client.h"
#include "version.h"

void CleanupExit() {
	void(*o)();
	*(UINT32*)&o = 0x40EF70;
	o();

	void Sys_Unload();
	Sys_Unload();
}

void Main_UnprotectModule(HMODULE hModule) {
	PIMAGE_DOS_HEADER header = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)hModule + header->e_lfanew);

	// unprotect the entire PE image
	SIZE_T size = ntHeader->OptionalHeader.SizeOfImage;
	DWORD oldProtect;
	VirtualProtect((LPVOID)hModule, size, PAGE_EXECUTE_READWRITE, &oldProtect);
}

bool apply_hooks() {
	// allow alt tab - set dwExStyle from WS_EX_TOPMOST to WS_EX_LEFT (default), which allows minimizing
	XUNLOCK((void*)0x4EEBD1, 1);
	memset((void*)0x4EEBD1, 0x00, 1);

	HMODULE hModule;
	if (SUCCEEDED(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)apply_hooks, &hModule))) {
		Main_UnprotectModule(hModule);
	}

	void patch_opcode_loadlibrary();
	patch_opcode_loadlibrary();
	
	if (codversion != COD_5) return true;

	PATCH_PUSH_STRING_PTR_VALUE(0x4EEC9D, "Call of Duty 1.5x Multiplayer");
	PATCH_PUSH_STRING_PTR_VALUE(0x439F14, "1.5x " BUILD);
	PATCH_PUSH_STRING_PTR_VALUE(0x4147C9, "1.5x"); //cl_updateoldversion
	PATCH_PUSH_STRING_PTR_VALUE(0x439A53, "1.5x");
	PATCH_PUSH_STRING_PTR_VALUE(0x439EEB, "1.5x");
	PATCH_PUSH_STRING_PTR_VALUE(0x4573FB, "1.5x");
	PATCH_PUSH_STRING_PTR_VALUE(0x439EC5, __DATE__);
	PATCH_PUSH_STRING_PTR_VALUE(0x439A49, __DATE__);
	PATCH_PUSH_STRING_PTR_VALUE(0x439EC0, __TIME__);
	PATCH_PUSH_INT_PTR_VALUE(0x439ECA, BUILDNUMBER); //buildnumber
	PATCH_PUSH_STRING_PTR_VALUE(0x4E085A, "") //Too many visible models fix

	__call(0x4684C5, (int)CleanupExit);

	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	__call(0x560F99, (int)WinMain);

	unlock_client_structure(); // make some client cls_ structure members writeable etc

	void CL_FrameStub();
	__call(0x43A6EF, (int)CL_FrameStub);

	void CL_Init();
	__call(0x439FCA, (int)CL_Init);
	__call(0x43A617, (int)CL_Init);

	void R_Init();
	__call(0x512042, (int)R_Init);

	return true;
}