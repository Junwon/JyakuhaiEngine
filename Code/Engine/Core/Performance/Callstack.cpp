#include "Engine/Core/Performance/Callstack.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#pragma warning( disable : 4091 ) //  warning C4091: 'typedef ': ignored on left of '' when no variable is declared
#include <DbgHelp.h>



#define MAX_SYMBOL_NAME_LENGTH 128
#define MAX_FILENAME_LENGTH 1024
#define MAX_DEPTH 128


// SymInitialize()
typedef BOOL(__stdcall *SymInitialize_T)(IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess);
typedef BOOL(__stdcall *SymCleanup_T)(IN HANDLE hProcess);
typedef BOOL(__stdcall *SymFromAddr_T)(IN HANDLE hProcess, IN DWORD64 Address, OUT PDWORD64 Displacement, OUT PSYMBOL_INFO Symbol);
typedef BOOL(__stdcall *SymGetLine_T)(IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Symbol);


static HMODULE gDebugHelp;
static HANDLE gProcess;
static SYMBOL_INFO  *gSymbol;

static SymInitialize_T LSymInitialize;
static SymCleanup_T LSymCleanup;
static SymFromAddr_T LSymFromAddr;
static SymGetLine_T LSymGetLineFromAddr64;

static int gCallstackCount = 0;

//------------------------------------------------------------------------
Callstack::Callstack()
	: hash(0)
	, frameCount(0) 
{}


//------------------------------------------------------------------------
bool CallstackSystemInit()
{
	// Load the dll, similar to OpenGL function fecthing.
	// This is where these functions will come from.
	gDebugHelp = LoadLibraryA("dbghelp.dll");
	if (gDebugHelp == nullptr) {
		return false;
	}

	// Get pointers to the functions we want from the loded library.
	LSymInitialize = (SymInitialize_T)GetProcAddress(gDebugHelp, "SymInitialize");
	LSymCleanup = (SymCleanup_T)GetProcAddress(gDebugHelp, "SymCleanup");
	LSymFromAddr = (SymFromAddr_T)GetProcAddress(gDebugHelp, "SymFromAddr");
	LSymGetLineFromAddr64 = (SymGetLine_T)GetProcAddress(gDebugHelp, "SymGetLineFromAddr64");

	// Initialize the system using the current process [see MSDN for details]
	gProcess = ::GetCurrentProcess();
	LSymInitialize(gProcess, NULL, TRUE);

	// Preallocate some memory for loading symbol information. 
	gSymbol = (SYMBOL_INFO *) ::malloc(sizeof(SYMBOL_INFO) + (MAX_FILENAME_LENGTH * sizeof(char)));
	gSymbol->MaxNameLen = MAX_FILENAME_LENGTH;
	gSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	return true;
}

//------------------------------------------------------------------------
void CallstackSystemDeinit()
{
	// cleanup after ourselves
	free(gSymbol);
	gSymbol = nullptr;

	LSymCleanup(gProcess);

	FreeLibrary(gDebugHelp);
	gDebugHelp = NULL;
}

//------------------------------------------------------------------------
// Can not be static - called when
// the callstack is freed.
void DestroyCallstack(Callstack* ptr)
{
	gCallstackCount--;
	::free(ptr);
}

//------------------------------------------------------------------------
Callstack* CreateCallstack(unsigned int skipFrames)
{
	// Capture the callstack frames - uses a windows call
	void* stack[MAX_DEPTH];
	DWORD hash;

	// skip_frames:  number of frames to skip [starting at the top - so don't return the frames for "CreateCallstack" (+1), plus "skip_frame_" layers.
	// max_frames to return
	// memory to put this information into.
	// out pointer to back trace hash.
	uint32_t frames = CaptureStackBackTrace(1 + skipFrames, MAX_DEPTH, stack, &hash);

	// create the callstack using an untracked allocation
	Callstack* cs = (Callstack*) ::malloc(sizeof(Callstack));

	// force call the constructor (new in-place)
	cs = new (cs) Callstack();

	// copy the frames to our callstack object
	unsigned int frameCount = min(MAX_FRAMES_PER_CALLSTACK, frames);
	cs->frameCount = frameCount;
	memcpy(cs->frames, stack, sizeof(void*) * frameCount);

	cs->hash = hash;

	gCallstackCount++;
	return cs;
}

//------------------------------------------------------------------------
// Fills lines with human readable data for the given callstack
// Fills from top to bottom (top being most recently called, with each next one being the calling function of the previous)
//
// Additional features you can add;
// [ ] If a file exists in yoru src directory, clip the filename
// [ ] Be able to specify a list of function names which will cause this trace to stop.
unsigned int CallstackGetLines(CallstackLine_T* lineBuffer, unsigned int const maxLines, Callstack* cs)
{
	IMAGEHLP_LINE64 line_info;
	DWORD lineOffset = 0; // Displacement from the beginning of the line 
	line_info.SizeOfStruct = sizeof(IMAGEHLP_LINE64);


	unsigned int count = min(maxLines, cs->frameCount);
	unsigned int idx = 0;

	for (unsigned int i = 0; i < count; ++i) {
		CallstackLine_T *line = &(lineBuffer[idx]);
		DWORD64 ptr = (DWORD64)(cs->frames[i]);
		if (FALSE == LSymFromAddr(gProcess, ptr, 0, gSymbol)) {
			continue;
		}

		//strcpy_s(line->functionName, 128, gSymbol->Name);
		
		if (strlen(gSymbol->Name) > 128) {
			std::string truncatedName = std::string(gSymbol->Name, gSymbol->Name + 124) + "...";
			strcpy_s(line->functionName, 128, truncatedName.c_str());
		}
		else {
			strcpy_s(line->functionName, 128, gSymbol->Name);
		}

		BOOL bRet = LSymGetLineFromAddr64(
			GetCurrentProcess(),	// Process handle of the current process 
			ptr,					// Address 
			&lineOffset,			// Displacement will be stored here by the function 
			&line_info);			// File name / line information will be stored here 

		if (bRet) {
			line->line = line_info.LineNumber;

			strcpy_s(line->filename, 128, line_info.FileName);
			line->offset = lineOffset;

		}
		else {
			// no information
			line->line = 0;
			line->offset = 0;
			strcpy_s(line->filename, 128, "N/A");
		}

		++idx;
	}

	return idx;
}