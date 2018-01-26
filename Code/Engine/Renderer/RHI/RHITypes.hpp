#pragma once

enum eRHIOutputMode : unsigned int
{
	RENDEROUTPUT_WINDOWED,
	RENDEROUTPUT_BORDERLESS,
	RENDEROUTPUT_FULLSCREEN_WINDOW,     // Requires Window Optional and ability to resize SwapChain
	RENDEROUTPUT_FULLSCREEN_DEDICATED,  // Dedicated Fullscreen Mode.  
};

enum eBufferUsage : unsigned int
{
	BUFFERUSAGE_GPU,     // GPU Read/Write, CPU Full Writes        (RenderTargets?)
	BUFFERUSAGE_STATIC,  // GPU Read, Written only at creation     (TEXTURES, STATIC MESHES)
	BUFFERUSAGE_DYNAMIC, // GPU Read, CPU Write (Partial or Full)  (DYNAMIC MESHES/DYNAMIC TEXTURES)
	BUFFERUSAGE_STAGING, // GPU Read/Write, CPU Read/Write         Slowest - image composition/manipulation
};

//------------------------------------------------------------------------
enum ePrimitiveType : unsigned int
{
	PRIMITIVE_NONE,
	PRIMITIVE_POINTS,
	PRIMITIVE_LINES,
	PRIMITIVE_LINESTRIP,
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_TRIANGLESTRIP,
	PRIMITIVE_QUADS
};