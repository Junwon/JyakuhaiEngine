#pragma once

#include <atomic>

#define MAX_FRAMES_PER_CALLSTACK (128)

struct CallstackLine_T
{
	char filename[128];
	char functionName[128];
	uint32_t line;
	uint32_t offset;
};

class Callstack
{
public:
	Callstack();

	uint32_t hash;
	unsigned int frameCount;
	void* frames[MAX_FRAMES_PER_CALLSTACK];
};
/************************************************************************/

bool CallstackSystemInit();
void CallstackSystemDeinit();

// Feel free to reorg this in a way you like - this is very C style.  
// Really, we just want to make sure these callstacks are not allocated on the heap.
// - You could creat them in-place in the meta-data if you prefer (provide memory to fill)
// - You could overload new on the Callstack class, cause "new Callstack(skip_frames)" to call that, a
//   and keeping this functionality.

// As this is - this will create a callstack using malloc (untracked allocation), skipping the first few frames.
Callstack* CreateCallstack(unsigned int skipFrames);

void DestroyCallstack(Callstack* c);

unsigned int CallstackGetLines(CallstackLine_T* lineBuffer, unsigned int const maxLines, Callstack* cs);