#include "convar.hpp"
#include "../../head.h"
inline const char** CCommand::ArgV() const {
    return ArgC() ? (const char**)m_Args.Base() : NULL;
}

inline const char* CCommand::ArgS() const {
    return m_nArgv0Size ? (m_ArgSBuffer.Base() + m_nArgv0Size) : "";
}

inline const char* CCommand::operator[](int nIndex) const {
    return Arg(nIndex);
}
static characterset_t s_BreakSet;
static bool s_bBuiltBreakSet = false;
void CharacterSetBuild(characterset_t* pSetBuffer, const char* pszSetString) {
    int i = 0;

    // Test our pointers
    if (!pSetBuffer || !pszSetString) return;

    memset(pSetBuffer->set, 0, sizeof(pSetBuffer->set));

    while (pszSetString[i]) {
        pSetBuffer->set[static_cast<size_t>(pszSetString[i])] = 1;
        i++;
    }
}

//-----------------------------------------------------------------------------
// Tokenizer class
//-----------------------------------------------------------------------------
CCommand::CCommand() {
    if (!s_bBuiltBreakSet) {
        s_bBuiltBreakSet = true;
        CharacterSetBuild(&s_BreakSet, "{}()':");
    }

    Reset();
}

CCommand::CCommand(int nArgC, const char** ppArgV) {
    Assert(nArgC > 0);

    if (!s_bBuiltBreakSet) {
        s_bBuiltBreakSet = true;
        CharacterSetBuild(&s_BreakSet, "{}()':");
    }

    Reset();

    char* pBuf = m_ArgvBuffer.Base();
    char* pSBuf = m_ArgSBuffer.Base();
    for (int i = 0; i < nArgC; ++i) {
        m_Args.AddToTail(pBuf);
        int nLen = strlen(ppArgV[i]);
        memcpy(pBuf, ppArgV[i], nLen + 1);
        if (i == 0) {
            m_nArgv0Size = nLen;
        }
        pBuf += nLen + 1;

        bool bContainsSpace = strchr(ppArgV[i], ' ') != NULL;
        if (bContainsSpace) {
            *pSBuf++ = '\"';
        }
        memcpy(pSBuf, ppArgV[i], nLen);
        pSBuf += nLen;
        if (bContainsSpace) {
            *pSBuf++ = '\"';
        }

        if (i != nArgC - 1) {
            *pSBuf++ = ' ';
        }
    }
}
bool CCommand::Tokenize(const char* pCommand, characterset_t* pBreakSet) {
    __debugbreak();
    return false;
}
void CCommand::Reset() {
    m_nArgv0Size = 0;
    m_ArgSBuffer.RemoveAll();
    m_ArgvBuffer.RemoveAll();
    m_Args.RemoveAll();
}

characterset_t* CCommand::DefaultBreakSet() { return &s_BreakSet; }

//-----------------------------------------------------------------------------
// Helper function to parse arguments to commands.
//-----------------------------------------------------------------------------
int CCommand::FindArg(const char* pName) const {
    int nArgC = ArgC();
    for (int i = 1; i < nArgC; i++) {
        if (!_stricmp(Arg(i), pName)) return (i + 1) < nArgC ? i + 1 : -1;
    }
    return -1;
}

int CCommand::FindArgInt(const char* pName, int nDefaultVal) const {
    int idx = FindArg(pName);
    if (idx != -1)
        return atoi(m_Args[idx]);
    else
        return nDefaultVal;
}

//-----------------------------------------------------------------------------
// Default console command autocompletion function
//-----------------------------------------------------------------------------
int DefaultCompletionFunc(const char* partial,
                          CUtlVector<CUtlString>& commands) {
    return 0;
}

//-----------------------------------------------------------------------------
// Statically constructed list of ConCommandBases, 
// used for registering them with the ICVar interface
//-----------------------------------------------------------------------------
static int64 s_nCVarFlag = 0;
static bool s_bRegistered = false;

class ConCommandRegList;
class ConCommandRegList
{
public:
    static void RegisterCommand(ConCommand* pCmd)
    {
        if (s_bConCommandsRegistered)
        {
            ConCommandHandle hndl = Offset::InterFaces::IVEngineCvar->RegisterConCommand(pCmd, s_nCVarFlag);
            if (!hndl.IsValid())
            {
                __debugbreak();
            }

            pCmd->SetHandle(hndl);
        }
        else
        {
            GetCommandRegList()->AddToTail(pCmd);
        }
    }

    static void RegisterAll()
    {
        if (!s_bConCommandsRegistered && Offset::InterFaces::IVEngineCvar)
        {
            s_bConCommandsRegistered = true;

            for (int i = 0; i < GetCommandRegList()->Count(); i++)
            {
                ConCommand* pCmd = GetCommandRegList()->Element(i);
                ConCommandHandle hndl = Offset::InterFaces::IVEngineCvar->RegisterConCommand(pCmd, s_nCVarFlag);
                pCmd->SetHandle(hndl);

                if (!hndl.IsValid())
                {
                    __debugbreak();

                }
            }
        }
    }
private:

    // GAMMACASE: Required to prevent static initialization order problem https://isocpp.org/wiki/faq/ctors#static-init-order
    static CUtlVector<ConCommand*>* GetCommandRegList()
    {
        static CUtlVector<ConCommand*> s_ConCommandRegList;
        return &s_ConCommandRegList;
    }

    static bool s_bConCommandsRegistered;
};
