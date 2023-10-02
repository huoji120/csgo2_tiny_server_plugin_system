#pragma once
#include <limits.h>
#include <cstdint>
#define CORRECT_PATH_SEPARATOR '\\'
#define CORRECT_PATH_SEPARATOR_S "\\"
#define INCORRECT_PATH_SEPARATOR '/'
#define INCORRECT_PATH_SEPARATOR_S "/"
#define FMTFUNCTION( a, b )
enum EStringConvertErrorPolicy
{
	_STRINGCONVERTFLAG_SKIP = 1,
	_STRINGCONVERTFLAG_FAIL = 2,
	_STRINGCONVERTFLAG_ASSERT = 4,

	STRINGCONVERT_REPLACE = 0,
	STRINGCONVERT_SKIP = _STRINGCONVERTFLAG_SKIP,
	STRINGCONVERT_FAIL = _STRINGCONVERTFLAG_FAIL,

	STRINGCONVERT_ASSERT_REPLACE = _STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_REPLACE,
	STRINGCONVERT_ASSERT_SKIP = _STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_SKIP,
	STRINGCONVERT_ASSERT_FAIL = _STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_FAIL,
};

enum EUniverse
{
	k_EUniverseInvalid = 0,
	k_EUniversePublic = 1,
	k_EUniverseBeta = 2,
	k_EUniverseInternal = 3,
	k_EUniverseDev = 4,
	// k_EUniverseRC = 5,				// no such universe anymore
	k_EUniverseMax
};

#define Plat_FastMemset memset
#define Plat_FastMemcpy memcpy
#define RESTRICT				__restrict
#define RESTRICT_FUNC			__declspec(restrict)
#define FORCEINLINE_TEMPLATE	__forceinline
#define PAD_NUMBER(number, boundary) \
	( ((number) + ((boundary)-1)) / (boundary) ) * (boundary)
typedef __int64 intp;
#ifndef Assert
#define Assert
#endif // !Assert
template <class T>
inline T* Construct(T* pMemory)
{
	return ::new(pMemory) T;
}

template <class T>
inline void Destruct(T* pMemory)
{
	pMemory->~T();

#ifdef _DEBUG
	memset((void*)pMemory, 0xDD, sizeof(T));
#endif
}
#include "../head.h"