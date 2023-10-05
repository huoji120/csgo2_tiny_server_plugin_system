#pragma once
#include "../sdk.h"
struct characterset_t {
    char set[256];
};
class ConCommandBase {
    friend class CCvar;
    friend class ConCommand;

   protected:
    ConCommandBase(void);

   public:
    ~ConCommandBase(void);
    // Check flag
    bool IsFlagSet(int64_t flag) const;
    // Set flag
    void AddFlags(int64_t flags);
    // Clear flag
    void RemoveFlags(int64_t flags);

    int64_t GetFlags() const;

    // Return name of cvar
    const char* GetName(void) const;

    // Return help text for cvar
    const char* GetHelpText(void) const;

   private:
    // Static data
    const char* m_pszName;
    const char* m_pszHelpString;

    // ConVar flags
    int64_t m_nFlags;
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
