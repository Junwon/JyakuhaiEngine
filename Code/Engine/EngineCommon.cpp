#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Lifted from;
// http://stackoverflow.com/questions/1128150/win32-api-to-enumerate-dll-export-functions
void ListDLLFunctions(char const *filename)
{
	HMODULE lib = LoadLibraryExA(filename, NULL, DONT_RESOLVE_DLL_REFERENCES);
	if (nullptr == lib) {
		DebuggerPrintlnf("Library %s did not exist.", filename);
	}

	ASSERT_OR_DIE(((PIMAGE_DOS_HEADER)lib)->e_magic == IMAGE_DOS_SIGNATURE, "FAIL");

	PIMAGE_NT_HEADERS header = (PIMAGE_NT_HEADERS)((BYTE *)lib + ((PIMAGE_DOS_HEADER)lib)->e_lfanew);
	ASSERT_OR_DIE(header->Signature == IMAGE_NT_SIGNATURE, "FAIL");
	ASSERT_OR_DIE(header->OptionalHeader.NumberOfRvaAndSizes > 0, "FAIL");

	PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)((BYTE *)lib + header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	ASSERT_OR_DIE(exports->AddressOfNames != NULL, "FAIL");

	BYTE** names = (BYTE**)((uintptr_t)lib + exports->AddressOfNames);
	for (DWORD i = 0; i < exports->NumberOfNames; i++) {
		DebuggerPrintlnf("Export: %s\n", (BYTE*)lib + (uintptr_t)names[i]);
	}
	FreeLibrary(lib);

}