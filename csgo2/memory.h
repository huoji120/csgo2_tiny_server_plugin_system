#pragma once
#include "pch.h"
namespace Memory {
	template<class T>
	void write(uintptr_t address, T value)
	{
		WriteProcessMemory(GetCurrentProcess(), (void*)address, &value, sizeof(T), 0);
	}

	template<class T>
	T read(uintptr_t address)
	{
		T buffer{};
		ReadProcessMemory(GetCurrentProcess(), (void*)address, &buffer, sizeof(T), 0);
		return buffer;
	}

};