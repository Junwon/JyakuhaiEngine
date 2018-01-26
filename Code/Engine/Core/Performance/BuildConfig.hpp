#pragma once

#define TRACK_MEMORY_NONE		(-1)
#define TRACK_MEMORY_BASIC		(0)
#define TRACK_MEMORY_VERBOSE	(1)

#define PROFILED_ENABLED		(1)
#define PROFILED_DISABLED		(0)

// If not defined, we will not track memory at all
// BASIC will track bytes used, and count
// VERBOSE will track individual callstacks
#if defined(_DEBUG)
	#define TRACK_MEMORY		TRACK_MEMORY_NONE
	//#define TRACK_MEMORY		TRACK_MEMORY_VERBOSE
	//#define TRACK_MEMORY		TRACK_MEMORY_BASIC
	//#define PROFILED_BUILD	PROFILED_ENABLED
	//#define PROFILED_BUILD	PROFILED_DISABLED

#elif defined(FINAL_BUILD)
	// undedfined 
#else
	#define TRACK_MEMORY          TRACK_MEMORY_NONE
#endif
