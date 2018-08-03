#pragma once
extern unsigned char ( __cdecl *ReadByte ) ( uintptr_t );
extern int ( __cdecl *ReadInt ) ( uintptr_t );
extern float ( __cdecl *ReadFloat ) ( uintptr_t );
extern double ( __cdecl *ReadDouble ) ( uintptr_t );
extern short ( __cdecl *ReadShort ) ( uintptr_t );
extern void ( __cdecl *WriteByte ) ( uintptr_t , unsigned char );
extern void ( __cdecl *WriteInt ) ( uintptr_t , int );
extern void ( __cdecl *WriteFloat ) ( uintptr_t , float );
extern void ( __cdecl *WriteDouble ) ( uintptr_t , double );
extern void ( __cdecl *WriteShort ) ( uintptr_t , short );

inline void ReadByteArray ( uintptr_t adr , char* dest , int size )
{
	for ( int i = 0; i < size; i++ )
	{
		dest [ i ] = ReadByte ( adr + i );
	}
}

inline void WriteByteArray ( char* dest , char* source , int sourcelength )
{
	for ( int i = 0; i < sourcelength; i++ )
	{
		WriteByte ( ( uintptr_t ) ( dest + i ) , source [ i ] );
	}
}

class CVMTHookManager
{
private:
	PDWORD * m_ppdwClassBase;
	PDWORD	m_pdwNewVMT , m_pdwOldVMT;
	DWORD	m_dwVMTSize;

public:
	CVMTHookManager ( void )
	{
		memset ( this , 0 , sizeof ( CVMTHookManager ) );
	}

	CVMTHookManager ( PDWORD* ppdwClassBase )
	{
		Initialize ( ppdwClassBase );
	}

	~CVMTHookManager ( void )
	{
		UnHook ( );
	}

	bool Initialize ( PDWORD* ppdwClassBase )
	{
		m_ppdwClassBase = ppdwClassBase;
		m_pdwOldVMT = *ppdwClassBase;
		m_dwVMTSize = GetVMTCount ( *ppdwClassBase );
		m_pdwNewVMT = new DWORD [ m_dwVMTSize ];
		memcpy ( m_pdwNewVMT , m_pdwOldVMT , sizeof ( DWORD ) * m_dwVMTSize );
		*ppdwClassBase = m_pdwNewVMT;
		return true;
	}

	bool Initialize ( PDWORD** pppdwClassBase )
	{
		return Initialize ( *pppdwClassBase );
	}

	void UnHook ( void )
	{
		if ( m_ppdwClassBase )
		{
			*m_ppdwClassBase = m_pdwOldVMT;
		}
	}

	void ReHook ( void )
	{
		if ( m_ppdwClassBase )
		{
			*m_ppdwClassBase = m_pdwNewVMT;
		}
	}

	int GetFuncCount ( void )
	{
		return ( int ) m_dwVMTSize;
	}

	DWORD GetMethodAddress ( int Index )
	{
		if ( Index >= 0 && Index <= ( int ) m_dwVMTSize && m_pdwOldVMT != NULL )
		{
			return m_pdwOldVMT [ Index ];
		}
		return NULL;
	}

	PDWORD GetOldVMT ( void )
	{
		return m_pdwOldVMT;
	}

	DWORD HookMethod ( DWORD dwNewFunc , unsigned int iIndex )
	{
		if ( m_pdwNewVMT && m_pdwOldVMT && iIndex <= m_dwVMTSize && iIndex >= 0 )
		{
			m_pdwNewVMT [ iIndex ] = dwNewFunc;
			return m_pdwOldVMT [ iIndex ];
		}
		return NULL;
	}

	template< class T >T GetMethod ( unsigned int nIndex ) const
	{
		return ( T ) m_pdwOldVMT [ nIndex ];
	}

private:
	DWORD GetVMTCount ( PDWORD pdwVMT )
	{
		DWORD dwIndex = 0;

		for ( dwIndex = 0; pdwVMT [ dwIndex ]; dwIndex++ )
		{
			if ( IS_INTRESOURCE ( ( FARPROC ) pdwVMT [ dwIndex ] ) )
			{
				break;
			}
		}
		return dwIndex;
	}
};
