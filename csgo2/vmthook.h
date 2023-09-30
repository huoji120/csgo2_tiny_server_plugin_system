#pragma once
//form https://www.unknowncheats.me/forum/c-and-c-/188449-vmt-hooking-class.html

#include "pch.h"

class VMTHook
{
public:

	void** vmt = nullptr; // Pointer to the VMT, we're using it more as an array of void*

	VMTHook(void* vmt); // Hook original VMT by it's address
	VMTHook(DWORD64* vmt_ptr); // Create Shadow VMT from VMT pointer ( Not implemented here )
	~VMTHook(); // Destructor, removes all hooks

	void* Hook(int index, void* hk);
	void ClearHooks();
private:
	std::map<int, void*>::iterator func_iterator; // Iterator so we can iterate the map below
	std::map<int, void*> hooked_funcs; // std::map which holds the index hooked and the original function's address
};
