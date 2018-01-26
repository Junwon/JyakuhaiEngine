#pragma once

typedef unsigned char byte_t;

#define UNUSED(x) (void)(x);

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }
#define SAFE_DELETE(varPointer)	   if ((varPointer) != nullptr) { delete varPointer; varPointer = nullptr; }	

#define GET_VAR_TYPE(var) typeid(var).name()

void ListDLLFunctions(char const *filename);