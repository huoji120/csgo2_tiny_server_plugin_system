#include "UtlString.hpp"

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>
#define _UtlString_assert

//-----------------------------------------------------------------------------
// Base class, containing simple memory management
//-----------------------------------------------------------------------------
CUtlBinaryBlock::CUtlBinaryBlock(int growSize, int initSize) : m_Memory(growSize, initSize)
{
    m_nActualLength = 0;
}

CUtlBinaryBlock::CUtlBinaryBlock(void* pMemory, int nSizeInBytes, int nInitialLength) : m_Memory((unsigned char*)pMemory, nSizeInBytes)
{
    m_nActualLength = nInitialLength;
}

CUtlBinaryBlock::CUtlBinaryBlock(const void* pMemory, int nSizeInBytes) : m_Memory((const unsigned char*)pMemory, nSizeInBytes)
{
    m_nActualLength = nSizeInBytes;
}

CUtlBinaryBlock::CUtlBinaryBlock(const CUtlBinaryBlock& src)
{
    Set(src.Get(), src.Length());
}

void CUtlBinaryBlock::Get(void *pValue, int nLen) const
{
    _UtlString_assert(nLen > 0);
    if(m_nActualLength < nLen) {
        nLen = m_nActualLength;
    }

    if(nLen > 0) {
        memcpy(pValue, m_Memory.Base(), nLen);
    }
}

void CUtlBinaryBlock::SetLength(int nLength)
{
    _UtlString_assert(!m_Memory.IsReadOnly());

    m_nActualLength = nLength;
    if(nLength > m_Memory.NumAllocated()) {
        int nOverFlow = nLength - m_Memory.NumAllocated();
        m_Memory.Grow(nOverFlow);

        // If the reallocation failed, clamp length
        if(nLength > m_Memory.NumAllocated()) {
            m_nActualLength = m_Memory.NumAllocated();
        }
    }

#ifdef _DEBUG
    if(m_Memory.NumAllocated() > m_nActualLength) {
        memset(((char *)m_Memory.Base()) + m_nActualLength, 0xEB, m_Memory.NumAllocated() - m_nActualLength);
    }
#endif
}

void CUtlBinaryBlock::Set(const void *pValue, int nLen)
{
    _UtlString_assert(!m_Memory.IsReadOnly());

    if(!pValue) {
        nLen = 0;
    }

    SetLength(nLen);

    if(m_nActualLength) {
        if(((const char *)m_Memory.Base()) >= ((const char *)pValue) + nLen ||
            ((const char *)m_Memory.Base()) + m_nActualLength <= ((const char *)pValue)) {
            memcpy(m_Memory.Base(), pValue, m_nActualLength);
        } else {
            memmove(m_Memory.Base(), pValue, m_nActualLength);
        }
    }
}


CUtlBinaryBlock &CUtlBinaryBlock::operator=(const CUtlBinaryBlock &src)
{
    _UtlString_assert(!m_Memory.IsReadOnly());
    Set(src.Get(), src.Length());
    return *this;
}


bool CUtlBinaryBlock::operator==(const CUtlBinaryBlock &src) const
{
    if(src.Length() != Length())
        return false;

    return !memcmp(src.Get(), Get(), Length());
}


//-----------------------------------------------------------------------------
// Simple string class. 
//-----------------------------------------------------------------------------
CUtlString::CUtlString()
{
}

CUtlString::CUtlString(const char *pString)
{
    Set(pString);
}

CUtlString::CUtlString(const CUtlString& string)
{
    Set(string.Get());
}
inline const char* CUtlString::Get() const
{
    if (!m_pString)
    {
        return "";
    }
    return m_pString;
}
inline void CUtlString::Set(const char* pValue)
{

}