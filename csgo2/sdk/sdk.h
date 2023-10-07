#pragma once
#include <string>
#include <array>
#include <Windows.h>
#include <Psapi.h>
#include <thread>
#include <unordered_map>
#include <map>
#include <shared_mutex>
#include <sstream>
#include <filesystem>
#include <limits.h>
#include <cstdint>
#define CORRECT_PATH_SEPARATOR '\\'
#define CORRECT_PATH_SEPARATOR_S "\\"
#define INCORRECT_PATH_SEPARATOR '/'
#define INCORRECT_PATH_SEPARATOR_S "/"
#define FMTFUNCTION(a, b)

enum EStringConvertErrorPolicy {
    _STRINGCONVERTFLAG_SKIP = 1,
    _STRINGCONVERTFLAG_FAIL = 2,
    _STRINGCONVERTFLAG_ASSERT = 4,

    STRINGCONVERT_REPLACE = 0,
    STRINGCONVERT_SKIP = _STRINGCONVERTFLAG_SKIP,
    STRINGCONVERT_FAIL = _STRINGCONVERTFLAG_FAIL,

    STRINGCONVERT_ASSERT_REPLACE =
        _STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_REPLACE,
    STRINGCONVERT_ASSERT_SKIP = _STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_SKIP,
    STRINGCONVERT_ASSERT_FAIL = _STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_FAIL,
};

enum EUniverse {
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
#define RESTRICT __restrict
#define RESTRICT_FUNC __declspec(restrict)
#define FORCEINLINE_TEMPLATE __forceinline
#define PAD_NUMBER(number, boundary) \
    (((number) + ((boundary)-1)) / (boundary)) * (boundary)
typedef __int64 intp;
typedef unsigned __int64 uintp;  // (ie, sizeof(intp) >= sizeof(int) &&
                                 // sizeof(intp) >= sizeof(void *)
typedef unsigned char uint8;
typedef signed char int8;
typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;
typedef unsigned int uint;
#define Assert
template <class T>
inline T* Construct(T* pMemory) {
    return ::new (pMemory) T;
}

template <class T>
inline void Destruct(T* pMemory) {
    pMemory->~T();

#ifdef _DEBUG
    memset((void*)pMemory, 0xDD, sizeof(T));
#endif
}
template <typename T>
inline T AlignValue(T val, uintptr_t alignment) {
    return (T)(((uintptr_t)val + alignment - 1) & ~(alignment - 1));
}


#include "public/Vector_Sdk.h"
#include "public/color.h"

#include "player/playerslot.h"

#include "public/mathlib.h"
#include "public/string_t.h"
#include "tier1/UtlMemory.hpp"
#include "tier1/utlfixedmemory.h"
#include "tier1/utlblockmemory.h"
#include "tier1/UtlString.hpp"
#include "tier1/UtlVector.hpp"
#include "tier1/bufferstring.h"
#include "tier1/cbyteswap.h"

#include "tier1/utlbuffer.h"

#include "public/eiface.h"
#include "public/bitvec.h"
#include "public/iserver.h"
#include "public/utlrbtree.h"
#include "public/utlmap.h"
#include "public/IRecipientFilter.h"
#include "public/recipientfilters.h"
#include "interfaces/interfaces.h"
#include "gameevent/IGameEvent.h"
#include "convar/convar.hpp"
#include "public/icvar.h"
#include "gameevent/IGameEvent.h"
#include "tier1/bufferstring.h"
#include "public/eiface.h"
