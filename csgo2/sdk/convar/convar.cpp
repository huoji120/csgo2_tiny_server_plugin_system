#include "convar.hpp"

inline const char** CCommand::ArgV() const
{
	return ArgC() ? (const char**)m_Args.Base() : NULL;
}

inline const char* CCommand::ArgS() const
{
	return m_nArgv0Size ? (m_ArgSBuffer.Base() + m_nArgv0Size) : "";
}



inline const char* CCommand::operator[](int nIndex) const
{
	return Arg(nIndex);
}
