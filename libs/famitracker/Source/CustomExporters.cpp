#include "CustomExporters.h"

CCustomExporters::CCustomExporters( CString PluginPath )
: m_currentExporter( NULL )
{
	FindCustomExporters( PluginPath );
}

CCustomExporters::~CCustomExporters( void )
{

}

void CCustomExporters::GetNames( CStringArray& names ) const
{
	names.RemoveAll();
	for( int i = 0; i < m_customExporters.GetCount(); ++i )
	{
		names.Add( m_customExporters[ i ].getName() );
	}
}

void CCustomExporters::SetCurrentExporter( CString name )
{
	for( int i = 0; i < m_customExporters.GetCount(); ++i )
	{
		if( m_customExporters[ i ].getName() == name )
		{
			m_currentExporter = &m_customExporters[ i ];
			break;
		}
	}
}

CCustomExporter& CCustomExporters::GetCurrentExporter( void ) const
{
	return *m_currentExporter;
}

void CCustomExporters::FindCustomExporters( CString PluginPath )
{
   CFileFind finder;
   
   CString path;
   BOOL bWorking;

#if defined(Q_OS_WIN32)
   // Windows
   path = PluginPath + _T("\\ftmx_*.dll");
   bWorking = finder.FindFile( path );
   while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString fileName = finder.GetFileName();
		CString filePath = finder.GetFilePath();

		CCustomExporter customExporter;

		if( customExporter.load( filePath ) )
		{
			m_customExporters.Add( customExporter );
		}

		//AfxMessageBox(finder.GetFileName());
	}
#elif defined(Q_OS_MAC)
   // Mac OSX
   path = PluginPath + _T("\\*.dylib");
	bWorking = finder.FindFile( path );
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString fileName = finder.GetFileName();
		CString filePath = finder.GetFilePath();

		CCustomExporter customExporter;

		if( customExporter.load( filePath ) )
		{
			m_customExporters.Add( customExporter );
		}

		//AfxMessageBox(finder.GetFileName());
	}
   path = PluginPath + _T("\\*.bundle");
	bWorking = finder.FindFile( path );
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString fileName = finder.GetFileName();
		CString filePath = finder.GetFilePath();

		CCustomExporter customExporter;

		if( customExporter.load( filePath ) )
		{
			m_customExporters.Add( customExporter );
		}

		//AfxMessageBox(finder.GetFileName());
	}
#else
   // Linux
   path = PluginPath + _T("\\*.so");
	bWorking = finder.FindFile( path );
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString fileName = finder.GetFileName();
		CString filePath = finder.GetFilePath();

		CCustomExporter customExporter;

		if( customExporter.load( filePath ) )
		{
			m_customExporters.Add( customExporter );
		}

		//AfxMessageBox(finder.GetFileName());
	}
#endif
}
