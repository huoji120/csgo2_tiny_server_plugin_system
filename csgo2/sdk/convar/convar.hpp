#pragma once
#include "../sdk.h"
class CCommandContext;
class ConCommandHandle;
class CCommand;
class ConCommandRefAbstract;
class ICommandCallback
{
public:
    virtual void CommandCallback(const CCommandContext& context, const CCommand& command) = 0;
};

class ICommandCompletionCallback
{
public:
    virtual int  CommandCompletionCallback(const char* pPartial, CUtlVector< CUtlString >& commands) = 0;
};
//-----------------------------------------------------------------------------
// Called when a ConCommand needs to execute
//-----------------------------------------------------------------------------
typedef void (*FnCommandCallback_t)(const CCommandContext& context, const CCommand& command);
typedef void (*FnCommandCallbackNoContext_t)(const CCommand& command);
typedef void (*FnCommandCallbackVoid_t)();
typedef int(*FnCommandCompletionCallback)(const char* partial, CUtlVector< CUtlString >& commands);

//-----------------------------------------------------------------------------
// Purpose: The base console invoked command/cvar interface
//-----------------------------------------------------------------------------
class ConCommandBase
{
    friend class CCvar;
    friend class ConCommand;

protected:
    ConCommandBase(void);
public:

    ~ConCommandBase(void);
    // Check flag
    bool				IsFlagSet(int64 flag) const;
    // Set flag
    void				AddFlags(int64 flags);
    // Clear flag
    void				RemoveFlags(int64 flags);

    int64				GetFlags() const;

    // Return name of cvar
    const char* GetName(void) const;

    // Return help text for cvar
    const char* GetHelpText(void) const;

private:
    // Static data
    const char* m_pszName;
    const char* m_pszHelpString;

    // ConVar flags
    int64						m_nFlags;
};
class ConCommandHandle
{
public:
    bool IsValid() { return value != kInvalidConCommandHandle; }
    uint16 Get() { return value; }
    void Set(uint16 _value) { value = _value; }
    void Reset() { value = kInvalidConCommandHandle; }

    bool HasCallback() const;
    void Dispatch(const CCommandContext& context, const CCommand& command);

    void Unregister();

private:
    uint16_t value = kInvalidConCommandHandle;

private:
    static const uint16 kInvalidConCommandHandle = 0xFFFF;
};

class ConCommandRefAbstract
{
public:
    ConCommandHandle handle;
};


//-----------------------------------------------------------------------------
// Purpose: The console invoked command
//-----------------------------------------------------------------------------
class ConCommand : public ConCommandBase
{
    friend class CCvar;
    friend class ConCommandHandle;

public:
    typedef ConCommandBase BaseClass;

    ConCommand(ConCommandRefAbstract* pReferenceOut, const char* pName, FnCommandCallback_t callback,
        const char* pHelpString = 0, int64 flags = 0, FnCommandCompletionCallback completionFunc = 0);
    ConCommand(ConCommandRefAbstract* pReferenceOut, const char* pName, FnCommandCallbackVoid_t callback,
        const char* pHelpString = 0, int64 flags = 0, FnCommandCompletionCallback completionFunc = 0);
    ConCommand(ConCommandRefAbstract* pReferenceOut, const char* pName, FnCommandCallbackNoContext_t callback,
        const char* pHelpString = 0, int64 flags = 0, FnCommandCompletionCallback completionFunc = 0);
    ConCommand(ConCommandRefAbstract* pReferenceOut, const char* pName, ICommandCallback* pCallback,
        const char* pHelpString = 0, int64 flags = 0, ICommandCompletionCallback* pCommandCompletionCallback = 0);

    ~ConCommand(void);

    // Used internally by OneTimeInit to initialize/shutdown
    void Init();
    void Shutdown();

    void Create(const char* pName, const char* pHelpString = 0,
        int64 flags = 0);

    int AutoCompleteSuggest(const char* partial, CUtlVector< CUtlString >& commands);

    bool CanAutoComplete(void);

    inline ConCommandRefAbstract* GetRef(void) const
    {
        return m_pReference;
    }

    inline void SetHandle(ConCommandHandle hndl)
    {
        m_pReference->handle = hndl;
    }

private:
    // Call this function when executing the command
    class CallbackInfo_t
    {
    public:
        union {
            FnCommandCallback_t m_fnCommandCallback;
            FnCommandCallbackVoid_t m_fnVoidCommandCallback;
            FnCommandCallbackNoContext_t m_fnContextlessCommandCallback;
            ICommandCallback* m_pCommandCallback;
        };

        bool m_bUsingCommandCallbackInterface : 1;
        bool m_bHasVoidCommandCallback : 1;
        bool m_bHasContextlessCommandCallback : 1;
    };

    CallbackInfo_t m_Callback;

    // NOTE: To maintain backward compat, we have to be very careful:
    // All public virtual methods must appear in the same order always
    // since engine code will be calling into this code, which *does not match*
    // in the mod code; it's using slightly different, but compatible versions
    // of this class. Also: Be very careful about adding new fields to this class.
    // Those fields will not exist in the version of this class that is instanced
    // in mod code.

    union
    {
        FnCommandCompletionCallback	m_fnCompletionCallback;
        ICommandCompletionCallback* m_pCommandCompletionCallback;
    };

    bool m_bHasCompletionCallback : 1;
    bool m_bUsingCommandCompletionInterface : 1;

    ConCommandRefAbstract* m_pReference;
};

struct characterset_t {
    char set[256];
};


class CCommand {
   public:
    CCommand();
    CCommand(int nArgC, const char** ppArgV);
    virtual bool Tokenize(const char* pCommand,
                          characterset_t* pBreakSet = NULL);
    virtual void Reset();

    int ArgC() const { return m_Args.Count(); }
    const char** ArgV() const;
    const char* ArgS()
        const;  // All args that occur after the 0th arg, in string form
    const char* GetCommandString() const {
        return ArgC() ? m_ArgSBuffer.Base() : "";
    }
    const char* operator[](int nIndex) const;  // Gets at arguments
    const char* Arg(int nIndex) const {
        // FIXME: Many command handlers appear to not be particularly careful
        // about checking for valid argc range. For now, we're going to
        // do the extra check and return an empty string if it's out of range
        if (nIndex < 0 || nIndex >= ArgC()) return "";
        return m_Args[nIndex];
    }

    // Helper functions to parse arguments to commands.
    //
    // Returns index of argument, or -1 if no such argument.
    int FindArg(const char* pName) const;

    int FindArgInt(const char* pName, int nDefaultVal) const;

    static int MaxCommandLength() { return COMMAND_MAX_LENGTH - 1; }
    static characterset_t* DefaultBreakSet();

   private:
    enum {
        COMMAND_MAX_ARGC = 64,
        COMMAND_MAX_LENGTH = 512,
    };

    int m_nArgv0Size;
    CUtlVectorFixedGrowable<char, COMMAND_MAX_LENGTH> m_ArgSBuffer;
    CUtlVectorFixedGrowable<char, COMMAND_MAX_LENGTH> m_ArgvBuffer;
    CUtlVectorFixedGrowable<char*, COMMAND_MAX_ARGC> m_Args;
};
struct CSplitScreenSlot
{
    CSplitScreenSlot(int index)
    {
        m_Data = index;
    }

    int Get() const
    {
        return m_Data;
    }

    int m_Data;
};

class ConVarHandle
{
public:
    bool IsValid() { return value != kInvalidConVarHandle; }
    uint32 Get() { return value; }
    void Set(uint32 _value) { value = _value; }

private:
    uint32 value = kInvalidConVarHandle;

private:
    static const uint32 kInvalidConVarHandle = 0xFFFFFFFF;
};



enum EConVarType : short
{
    EConVarType_Invalid = -1,
    EConVarType_Bool,
    EConVarType_Int16,
    EConVarType_UInt16,
    EConVarType_Int32,
    EConVarType_UInt32,
    EConVarType_Int64,
    EConVarType_UInt64,
    EConVarType_Float32,
    EConVarType_Float64,
    EConVarType_String,
    EConVarType_Color,
    EConVarType_Vector2,
    EConVarType_Vector3,
    EConVarType_Vector4,
    EConVarType_Qangle
};
class Vector;
union CVValue_t
{
    bool		m_bValue;
    short		m_i16Value;
    uint16		m_u16Value;
    int			m_i32Value;
    uint		m_u32Value;
    int64		m_i64Value;
    uint64		m_u64Value;
    float		m_flValue;
    double		m_dbValue;
    const char* m_szValue;
    Color		m_clrValue;
    Vector2D	m_vec2Value;
    Vector_SimPleSdk		m_vec3Value;
    Vector4D	m_vec4Value;
    Vector_SimPleSdk		m_angValue;
};

//-----------------------------------------------------------------------------
// Purpose: A console variable
//-----------------------------------------------------------------------------
class ConVar
{
    friend class CCvar;
    friend class ConVarRef;
    friend class SplitScreenConVarRef;

public:
    const char* m_pszName;
    CVValue_t* m_cvvDefaultValue;
    CVValue_t* m_cvvMinValue;
    CVValue_t* m_cvvMaxValue;
    const char* m_pszHelpString;
    EConVarType m_eVarType;

    // This gets copied from the ConVarDesc_t on creation
    short unk1;

    unsigned int timesChanged;
    int64 flags;
    unsigned int callback_index;

    // Used when setting default, max, min values from the ConVarDesc_t
    // although that's not the only place of usage
    // flags seems to be:
    // (1 << 0) Skip setting value to split screen slots and also something keyvalues related
    // (1 << 1) Skip setting default value
    // (1 << 2) Skip setting min/max values
    int allocation_flag_of_some_sort;

    CVValue_t** values;
};

class ConVarRefAbstract
{
public:
    // High-speed method to read convar data
    ConVarHandle m_Handle;
    ConVar* m_pConVarState;
};

