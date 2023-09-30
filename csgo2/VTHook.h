#pragma once
#include "pch.h"
class VTHook
{
public:
	VTHook()
	{
		memset( this, 0, sizeof( VTHook) );
	}

	VTHook( PDWORD64* ppdwClassBase )
	{
		bInitialize( ppdwClassBase );
	}

	~VTHook()
	{
		UnHook();
	}
	void ClearClassBase()
	{
		m_ClassBase = NULL;
	}
	bool bInitialize( PDWORD64* ppdwClassBase )
	{
		m_ClassBase = ppdwClassBase;
		m_OldVT = *ppdwClassBase;
		m_VTSize = GetVTCount( *ppdwClassBase );
		m_NewVT = new DWORD64[ m_VTSize ];
		memcpy( m_NewVT, m_OldVT, sizeof( DWORD64) * m_VTSize );
		*ppdwClassBase = m_NewVT;
		return true;
	}

	bool bInitialize( PDWORD64** pppdwClassBase ) // fix for pp
	{
		return bInitialize( *pppdwClassBase );
	}

	void UnHook()
	{
		if( m_ClassBase )
		{
			*m_ClassBase = m_OldVT;
		}
	}

	void ReHook()
	{
		if( m_ClassBase )
		{
			*m_ClassBase = m_NewVT;
		}
	}

	int iGetFuncCount()
	{
		return ( int )m_VTSize;
	}

	DWORD64 GetFuncAddress( int Index )
	{
		if( Index >= 0 && Index <= ( int )m_VTSize && m_OldVT != NULL )
		{
			return m_OldVT[ Index ];
		}
		return NULL;
	}

	PDWORD64 GetOldVT()
	{
		return m_OldVT;
	}

	DWORD64 HookFunction( DWORD64 dwNewFunc, unsigned int iIndex )
	{
		if( m_NewVT && m_OldVT && iIndex <= m_VTSize && iIndex >= 0 )
		{
			m_NewVT[ iIndex ] = dwNewFunc;
			return m_OldVT[ iIndex ];
		}

		return NULL;
	}

private:
	DWORD64 GetVTCount( PDWORD64 pdwVMT )
	{
		DWORD64 dwIndex = 0;

		while (IsBadCodePtr((FARPROC)pdwVMT[dwIndex]) == false) {
			dwIndex++;
		}
		return dwIndex;
	}

	PDWORD64* m_ClassBase;
	PDWORD64 m_NewVT, m_OldVT;
	DWORD64 m_VTSize;
};
