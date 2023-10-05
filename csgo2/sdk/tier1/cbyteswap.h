//========= Copyright ?1996-2006, Valve LLC, All rights reserved. ============
//
// Purpose: Low level byte swapping routines.
//
// $NoKeywords: $
//=============================================================================
#ifndef BYTESWAP_H
#define BYTESWAP_H
#if defined(_WIN32)
#pragma once
#endif

#include "../sdk.h"

typedef enum _fieldtypes {
    FIELD_VOID = 0,    // No type or value
    FIELD_FLOAT,       // Any floating point value
    FIELD_STRING,      // A string ID (return from ALLOC_STRING)
    FIELD_VECTOR,      // Any vector, QAngle, or AngularImpulse
    FIELD_QUATERNION,  // A quaternion
    FIELD_INTEGER,     // Any integer or enum
    FIELD_BOOLEAN,  // boolean, implemented as an int, I may use this as a hint
                    // for compression
    FIELD_SHORT,    // 2 byte integer
    FIELD_CHARACTER,  // a byte
    FIELD_COLOR32,    // 8-bit per channel r,g,b,a (32bit color)
    FIELD_EMBEDDED,  // an embedded object with a datadesc, recursively traverse
                     // and embedded class/structure based on an additional
                     // typedescription
    FIELD_CUSTOM,    // special type that contains function pointers to it's
                     // read/write/parse functions

    FIELD_CLASSPTR,  // CBaseEntity *
    FIELD_EHANDLE,   // Entity handle

    FIELD_POSITION_VECTOR,  // A world coordinate (these are fixed up across
                            // level transitions automagically)
    FIELD_TIME,       // a floating point time (these are fixed up automatically
                      // too!)
    FIELD_TICK,       // an integer tick count( fixed up similarly to time)
    FIELD_SOUNDNAME,  // Engine string that is a sound name (needs precache)

    FIELD_INPUT,     // a list of inputed data fields (all derived from
                     // CMultiInputVar)
    FIELD_FUNCTION,  // A class function pointer (Think, Use, etc)

    FIELD_VMATRIX,  // a vmatrix (output coords are NOT worldspace)

    // NOTE: Use float arrays for local transformations that don't need to be
    // fixed up.
    FIELD_VMATRIX_WORLDSPACE,  // A VMatrix that maps some local space to world
                               // space (translation is fixed up on level
                               // transitions)
    FIELD_MATRIX3X4_WORLDSPACE,  // matrix3x4_t that maps some local space to
                                 // world space (translation is fixed up on
                                 // level transitions)

    FIELD_INTERVAL,  // a start and range floating point interval (
                     // e.g., 3.2->3.6 == 3.2 and 0.4 )
    FIELD_UNUSED,

    FIELD_VECTOR2D,   // 2 floats
    FIELD_INTEGER64,  // 64bit integer

    FIELD_VECTOR4D,  // 4 floats

    FIELD_RESOURCE,

    FIELD_TYPEUNKNOWN,

    FIELD_CSTRING,
    FIELD_HSCRIPT,
    FIELD_VARIANT,
    FIELD_UINT64,
    FIELD_FLOAT64,
    FIELD_POSITIVEINTEGER_OR_NULL,
    FIELD_HSCRIPT_NEW_INSTANCE,
    FIELD_UINT,
    FIELD_UTLSTRINGTOKEN,
    FIELD_QANGLE,
    FIELD_NETWORK_ORIGIN_CELL_QUANTIZED_VECTOR,
    FIELD_HMATERIAL,
    FIELD_HMODEL,
    FIELD_NETWORK_QUANTIZED_VECTOR,
    FIELD_NETWORK_QUANTIZED_FLOAT,
    FIELD_DIRECTION_VECTOR_WORLDSPACE,
    FIELD_QANGLE_WORLDSPACE,
    FIELD_QUATERNION_WORLDSPACE,
    FIELD_HSCRIPT_LIGHTBINDING,
    FIELD_V8_VALUE,
    FIELD_V8_OBJECT,
    FIELD_V8_ARRAY,
    FIELD_V8_CALLBACK_INFO,
    FIELD_UTLSTRING,

    FIELD_NETWORK_ORIGIN_CELL_QUANTIZED_POSITION_VECTOR,
    FIELD_HRENDERTEXTURE,

    FIELD_HPARTICLESYSTEMDEFINITION,
    FIELD_UINT8,
    FIELD_UINT16,
    FIELD_CTRANSFORM,
    FIELD_CTRANSFORM_WORLDSPACE,
    FIELD_HPOSTPROCESSING,
    FIELD_MATRIX3X4,
    FIELD_SHIM,
    FIELD_CMOTIONTRANSFORM,
    FIELD_CMOTIONTRANSFORM_WORLDSPACE,
    FIELD_ATTACHMENT_HANDLE,
    FIELD_AMMO_INDEX,
    FIELD_CONDITION_ID,
    FIELD_AI_SCHEDULE_BITS,
    FIELD_MODIFIER_HANDLE,
    FIELD_ROTATION_VECTOR,
    FIELD_ROTATION_VECTOR_WORLDSPACE,
    FIELD_HVDATA,
    FIELD_SCALE32,
    FIELD_STRING_AND_TOKEN,
    FIELD_ENGINE_TIME,
    FIELD_ENGINE_TICK,
    FIELD_WORLD_GROUP_ID,

    FIELD_TYPECOUNT
} fieldtype_t;
struct inputdata_t;

class ISaveRestoreOps;
typedef void(__fastcall *inputfunc_t)(inputdata_t &data);

enum {
    PC_NON_NETWORKED_ONLY = 0,
    PC_NETWORKED_ONLY,

    PC_COPYTYPE_COUNT,
    PC_EVERYTHING = PC_COPYTYPE_COUNT,
};

enum {
    TD_OFFSET_NORMAL = 0,
    TD_OFFSET_PACKED = 1,

    // Must be last
    TD_OFFSET_COUNT,
};
struct datarun_t;
struct typedescription_t;
struct datacopyruns_t
{
public:
    CUtlVector< datarun_t > m_vecRuns;
};

struct flattenedoffsets_t
{
    CUtlVector< typedescription_t >	m_Flattened;
    int								m_nPackedSize; // Contiguous memory to pack all of these together for TD_OFFSET_PACKED
    int								m_nPackedStartOffset;
};

struct datamapinfo_t
{
    // Flattened list, with FIELD_EMBEDDED, FTYPEDESC_PRIVATE, 
    //  and FTYPEDESC_OVERRIDE (overridden) fields removed
    flattenedoffsets_t	m_Flat;
    datacopyruns_t		m_CopyRuns;
};

struct optimized_datamap_t
{
    // Optimized info for PC_NON_NETWORKED and PC_NETWORKED data
    datamapinfo_t	m_Info[PC_COPYTYPE_COUNT];
};


struct datamap_t
{
    typedescription_t* dataDesc;
    int					dataNumFields;
    char const* dataClassName;
    datamap_t* baseMap;

    int					m_nPackedSize;
    optimized_datamap_t* m_pOptimizedDataMap;

#if defined( _DEBUG )
    bool				bValidityChecked;
#endif // _DEBUG
};

struct typedescription_t {
    fieldtype_t fieldType;
    const char *fieldName;
    int fieldOffset;  // Local offset value
    unsigned short fieldSize;
    short flags;
    // the name of the variable in the map/fgd data, or the name of the action
    const char *externalName;
    // pointer to the function set for save/restoring of custom data types
    ISaveRestoreOps *pSaveRestoreOps;
    // for associating function with string names
    inputfunc_t inputFunc;
    // For embedding additional datatables inside this one
    datamap_t *td;

    // Stores the actual member variable size in bytes
    int fieldSizeInBytes;

    // FTYPEDESC_OVERRIDE point to first baseclass instance if chains_validated
    // has occurred
    struct typedescription_t *override_field;

    // Used to track exclusion of baseclass fields
    int override_count;

    // Tolerance for field errors for float fields
    float fieldTolerance;

    // For raw fields (including children of embedded stuff) this is the
    // flattened offset
    int flatOffset[TD_OFFSET_COUNT];
    unsigned short flatGroup;
};
struct datarun_t
{
    datarun_t() : m_nStartFlatField(0), m_nEndFlatField(0), m_nLength(0)
    {
        for (int i = 0; i < TD_OFFSET_COUNT; ++i)
        {
            m_nStartOffset[i] = 0;
        }
    }

    // Indices of start/end fields in the flattened typedescription_t list
    int m_nStartFlatField;
    int m_nEndFlatField;

    // Offsets for run in the packed/unpacked data (I think the run starts need to be properly aligned)
    int m_nStartOffset[TD_OFFSET_COUNT];

    int m_nLength;
};

class CByteswap {
   public:
    CByteswap() {
        // Default behavior sets the target endian to match the machine native
        // endian (no swap).
        SetTargetBigEndian(IsMachineBigEndian());
    }

    //-----------------------------------------------------------------------------
    // Write a single field.
    //-----------------------------------------------------------------------------
    void SwapFieldToTargetEndian(void *pOutputBuffer, void *pData,
                                 typedescription_t *pField);

    //-----------------------------------------------------------------------------
    // Write a block of fields.  Works a bit like the saverestore code.
    //-----------------------------------------------------------------------------
    void SwapFieldsToTargetEndian(void *pOutputBuffer, void *pBaseData,
                                  datamap_t *pDataMap);

    // Swaps fields for the templated type to the output buffer.
    template <typename T>
    inline void SwapFieldsToTargetEndian(T *pOutputBuffer, void *pBaseData,
                                         unsigned int objectCount = 1) {
        for (unsigned int i = 0; i < objectCount; ++i, ++pOutputBuffer) {
            SwapFieldsToTargetEndian((void *)pOutputBuffer, pBaseData,
                                     &T::m_DataMap);
            pBaseData = (byte *)pBaseData + sizeof(T);
        }
    }

    // Swaps fields for the templated type in place.
    template <typename T>
    inline void SwapFieldsToTargetEndian(T *pOutputBuffer,
                                         unsigned int objectCount = 1) {
        SwapFieldsToTargetEndian<T>(pOutputBuffer, (void *)pOutputBuffer,
                                    objectCount);
    }

    //-----------------------------------------------------------------------------
    // True if the current machine is detected as big endian.
    // (Endienness is effectively detected at compile time when optimizations
    // are enabled)
    //-----------------------------------------------------------------------------
    static bool IsMachineBigEndian() {
        short nIsBigEndian = 1;

        // if we are big endian, the first byte will be a 0, if little endian,
        // it will be a one.
        return (bool)(0 == *(char *)&nIsBigEndian);
    }

    //-----------------------------------------------------------------------------
    // Sets the target byte ordering we are swapping to or from.
    //
    // Braindead Endian Reference:
    //		x86 is LITTLE Endian
    //		PowerPC is BIG Endian
    //-----------------------------------------------------------------------------
    inline void SetTargetBigEndian(bool bigEndian) {
        m_bBigEndian = bigEndian;
        m_bSwapBytes = IsMachineBigEndian() != bigEndian;
    }

    // Changes target endian
    inline void FlipTargetEndian(void) {
        m_bSwapBytes = !m_bSwapBytes;
        m_bBigEndian = !m_bBigEndian;
    }

    // Forces byte swapping state, regardless of endianess
    inline void ActivateByteSwapping(bool bActivate) {
        SetTargetBigEndian(IsMachineBigEndian() != bActivate);
    }

    //-----------------------------------------------------------------------------
    // Returns true if the target machine is the same as this one in endianness.
    //
    // Used to determine when a byteswap needs to take place.
    //-----------------------------------------------------------------------------
    inline bool IsSwappingBytes(void)  // Are bytes being swapped?
    {
        return m_bSwapBytes;
    }

    inline bool IsTargetBigEndian(void)  // What is the current target endian?
    {
        return m_bBigEndian;
    }

    //-----------------------------------------------------------------------------
    // IsByteSwapped()
    //
    // When supplied with a chunk of input data and a constant or magic number
    // (in native format) determines the endienness of the current machine in
    // relation to the given input data.
    //
    // Returns:
    //		1  if input is the same as nativeConstant.
    //		0  if input is byteswapped relative to nativeConstant.
    //		-1 if input is not the same as nativeConstant and not byteswapped
    // either.
    //
    // ( This is useful for detecting byteswapping in magic numbers in structure
    // headers for example. )
    //-----------------------------------------------------------------------------
    template <typename T>
    inline int SourceIsNativeEndian(T input, T nativeConstant) {
        // If it's the same, it isn't byteswapped:
        if (input == nativeConstant) return 1;

        int output;
        LowLevelByteSwap<T>(&output, &input);
        if (output == nativeConstant) return 0;

        Assert(0);  // if we get here, input is neither a swapped nor unswapped
                    // version of nativeConstant.
        return -1;
    }

    //-----------------------------------------------------------------------------
    // Swaps an input buffer full of type T into the given output buffer.
    //
    // Swaps [count] items from the inputBuffer to the outputBuffer.
    // If inputBuffer is omitted or NULL, then it is assumed to be the same as
    // outputBuffer - effectively swapping the contents of the buffer in place.
    //-----------------------------------------------------------------------------
    template <typename T>
    inline void SwapBuffer(T *outputBuffer, T *inputBuffer = NULL,
                           int count = 1) {
        Assert(count >= 0);
        Assert(outputBuffer);

        // Fail gracefully in release:
        if (count <= 0 || !outputBuffer) return;

        // Optimization for the case when we are swapping in place.
        if (inputBuffer == NULL) {
            inputBuffer = outputBuffer;
        }

        // Swap everything in the buffer:
        for (int i = 0; i < count; i++) {
            LowLevelByteSwap<T>(&outputBuffer[i], &inputBuffer[i]);
        }
    }

    //-----------------------------------------------------------------------------
    // Swaps an input buffer full of type T into the given output buffer.
    //
    // Swaps [count] items from the inputBuffer to the outputBuffer.
    // If inputBuffer is omitted or NULL, then it is assumed to be the same as
    // outputBuffer - effectively swapping the contents of the buffer in place.
    //-----------------------------------------------------------------------------
    template <typename T>
    inline void SwapBufferToTargetEndian(T *outputBuffer, T *inputBuffer = NULL,
                                         int count = 1) {
        Assert(count >= 0);
        Assert(outputBuffer);

        // Fail gracefully in release:
        if (count <= 0 || !outputBuffer) return;

        // Optimization for the case when we are swapping in place.
        if (inputBuffer == NULL) {
            inputBuffer = outputBuffer;
        }

        // Are we already the correct endienness? ( or are we swapping 1 byte
        // items? )
        if (!m_bSwapBytes || (sizeof(T) == 1)) {
            // If we were just going to swap in place then return.
            if (!inputBuffer) return;

            // Otherwise copy the inputBuffer to the outputBuffer:
            if (outputBuffer != inputBuffer)
                memcpy(outputBuffer, inputBuffer, count * sizeof(T));
            return;
        }

        // Swap everything in the buffer:
        for (int i = 0; i < count; i++) {
            LowLevelByteSwap<T>(&outputBuffer[i], &inputBuffer[i]);
        }
    }

   private:
    //-----------------------------------------------------------------------------
    // The lowest level byte swapping workhorse of doom.  output always contains
    // the swapped version of input.  ( Doesn't compare machine to target
    // endianness )
    //-----------------------------------------------------------------------------
    template <typename T>
    static void LowLevelByteSwap(T *output, T *input) {
        T temp = *output;
#if defined(_X360)
        // Intrinsics need the source type to be fixed-point
        DWORD *word = (DWORD *)input;
        switch (sizeof(T)) {
            case 8: {
                __storewordbytereverse(*(word + 1), 0, &temp);
                __storewordbytereverse(*(word + 0), 4, &temp);
            } break;

            case 4:
                __storewordbytereverse(*word, 0, &temp);
                break;

            case 2:
                __storeshortbytereverse(*input, 0, &temp);
                break;

            case 1:
                Q_memcpy(&temp, input, 1);
                break;

            default:
                Assert("Invalid size in CByteswap::LowLevelByteSwap" && 0);
        }
#else
        for (size_t i = 0; i < sizeof(T); i++) {
            ((unsigned char *)&temp)[i] =
                ((unsigned char *)input)[sizeof(T) - (i + 1)];
        }
#endif
        memcpy(output, &temp, sizeof(T));
    }

#if defined(_X360)
    // specialized for void * to get 360 XDK compile working despite changelist
    // 281331
    //-----------------------------------------------------------------------------
    // The lowest level byte swapping workhorse of doom.  output always contains
    // the swapped version of input.  ( Doesn't compare machine to target
    // endianness )
    //-----------------------------------------------------------------------------
    template <>
    static void LowLevelByteSwap(void **output, void **input) {
        AssertMsgOnce(sizeof(void *) == sizeof(unsigned int),
                      "void *'s on this platform are not four bytes!");
        __storewordbytereverse(*reinterpret_cast<unsigned int *>(input), 0,
                               output);
    }
#endif

    unsigned int m_bSwapBytes : 1;
    unsigned int m_bBigEndian : 1;
};

#endif /* !BYTESWAP_H */
