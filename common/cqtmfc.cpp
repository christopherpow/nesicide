#include "cqtmfc.h"
//#include "resource.h"

#include <stdarg.h>

#include <QLinearGradient>
#include <QHeaderView>
#include <QMessageBox>
#include <QPixmap>
#include <QMainWindow>
#include <QFileInfo>
#include <QFontDatabase>

extern CWinApp* ptrToTheApp;

CWinApp* AfxGetApp() 
{ 
   return ptrToTheApp; 
}

CFrameWnd* AfxGetMainWnd() 
{ 
   return ptrToTheApp->m_pMainWnd; 
}

CBrush nullBrush;

HGDIOBJ GetStockObject(
   int fnObject
)
{
   switch ( fnObject )
   {
   case NULL_BRUSH:
      return (HGDIOBJ)nullBrush;
      break;
   }
   return (HGDIOBJ)NULL;
}

extern void qtMfcInitDialogResource(UINT dlgID,CDialog* parent);
extern void qtMfcInitToolBarResource(UINT dlgID,CToolBar* parent);
extern void qtMfcInitMenuResource(UINT menuID,CMenu* parent);

QHash<int,CString> qtMfcStringResources;

CString qtMfcStringResource(int id)
{
   return qtMfcStringResources.value(id);
}

QHash<int,CBitmap*> qtMfcBitmapResources;

CBitmap* qtMfcBitmapResource(int id)
{
   return qtMfcBitmapResources.value(id);
}

QHash<int,QIcon*> qtIconResources;

QIcon* qtIconResource(int id)
{
   return qtIconResources.value(id);
}

QHash<int,CMenu*> qtMfcMenuResources;

CMenu* qtMfcMenuResource(int id)
{
   return qtMfcMenuResources.value(id);
}

int AfxMessageBox(
   LPCTSTR lpszText,
   UINT nType,
   UINT nIDHelp 
)
{
#if UNICODE
   QString text = QString::fromWCharArray(lpszText);
#else
   QString text = lpszText;
#endif
   switch ( nType )
   {
   case MB_ICONERROR:
      QMessageBox::critical(0,"Error!",text);
      break;
   case MB_ICONEXCLAMATION:
      QMessageBox::warning(0,"Warning",text);
      break;
   case MB_ICONQUESTION:
      QMessageBox::question(0,"Did you mean?",text);
      break;
   default:
      QMessageBox::information(0,"Information...",text);
      break;
   }
   return QMessageBox::Ok;
}

int AFXAPI AfxMessageBox(
   UINT nIDPrompt,
   UINT nType,
   UINT nIDHelp
)
{
#if UNICODE
   QString text = QString::fromWCharArray(qtMfcStringResource(nIDPrompt));
#else
   QString text = qtMfcStringResource(nIDPrompt);
#endif
   switch ( nType )
   {
   case MB_ICONERROR:
      QMessageBox::critical(0,"Error!",text);
      break;
   case MB_ICONEXCLAMATION:
      QMessageBox::warning(0,"Warning",text);
      break;
   case MB_ICONQUESTION:
      QMessageBox::question(0,"Did you mean?",text);
      break;
   default:
      QMessageBox::information(0,"Information...",text);
      break;
   }
   return QMessageBox::Ok;
}


HCURSOR WINAPI SetCursor(
   HCURSOR hCursor
)
{
   return (HCURSOR)0;
}

BOOL WINAPI GetWindowRect(
   HWND hWnd,
   LPRECT lpRect
)
{
   CWnd* pWnd = (CWnd*)hWnd;
   pWnd->GetWindowRect(lpRect);
   return TRUE;
}

UINT WINAPI RegisterClipboardFormat(
   LPCTSTR lpszFormat
)
{
   return 0xC000; // From MS info for RegisterClipboardFormat...we don't really need it.
}

DWORD WINAPI GetFileAttributes(
   LPCTSTR lpFileName
)
{
   QFileInfo fileInfo;
   DWORD attribs = 0;
#if UNICODE
   fileInfo.setFile(QString::fromWCharArray(lpFileName));
#else
   fileInfo.setFile(QString::fromAscii(lpFileName));
#endif
   attribs |= (fileInfo.isDir()?FILE_ATTRIBUTE_DIRECTORY:0);
   attribs |= (fileInfo.isHidden()?FILE_ATTRIBUTE_HIDDEN:0);
   return attribs;
}

LONG WINAPI GetWindowLong(
   HWND hWnd,
   int nIndex
)
{
   CWnd* pWnd = (CWnd*)hWnd;
   LONG ret = 0;
   switch ( nIndex )
   {
   case GWL_STYLE:
      ret = pWnd->GetStyle();
   }
   return ret;
}

int WINAPI GetKeyNameText(
   LONG lParam,
   LPTSTR lpString,
   int cchSize
)
{
   QKeySequence key((lParam>>16));
   QString keyString = key.toString();
#if UNICODE
   wcsncpy(lpString,keyString.unicode(),cchSize);
   return wcslen(lpString);
#else
   strncpy(lpString,keyString.toAscii().constData(),cchSize);
   return strlen(lpString);
#endif
}

#if !defined(Q_WS_WIN) && !defined(Q_WS_WIN32)
HMODULE WINAPI LoadLibrary(
   LPCTSTR lpFileName
)
{
   QLibrary* pLib = new QLibrary;
#if UNICODE
   pLib->setFileName(QString::fromWCharArray(lpFileName));   
#else
   pLib->setFileName(QString::fromAscii(lpFileName));   
#endif
   if ( !pLib->load() )
   {
      delete pLib;
      pLib = NULL;
   }
   return (HMODULE)pLib;
}

FARPROC WINAPI GetProcAddress(
   HMODULE hModule,
   LPCSTR lpProcName
)
{
   QLibrary* pLib = (QLibrary*)hModule;
#if UNICODE
   return (FARPROC)pLib->resolve(QString::fromWCharArray(lpProcName).toAscii().constData());
#else
   return (FARPROC)pLib->resolve(QString::fromAscii(lpProcName).toAscii().constData());
#endif
}

BOOL WINAPI FreeLibrary(
   HMODULE hModule
)
{
   QLibrary* pLib = (QLibrary*)hModule;   
   return pLib->unload();
}
#endif

HANDLE WINAPI CreateEvent(
   LPSECURITY_ATTRIBUTES lpEventAttributes,
   BOOL bManualReset,
   BOOL bInitialState,
   LPCTSTR lpName
)
{
   return (HANDLE)new CEvent(bInitialState,bManualReset,lpName,lpEventAttributes);
}

BOOL WINAPI SetEvent(
   HANDLE hEvent
)
{
   CEvent* pEvent = (CEvent*)hEvent;
   return pEvent->SetEvent();
}

BOOL WINAPI ResetEvent(
   HANDLE hEvent
)
{
   CEvent* pEvent = (CEvent*)hEvent;
   return pEvent->ResetEvent();
}

BOOL WINAPI PulseEvent(
   HANDLE hEvent
)
{
   CEvent* pEvent = (CEvent*)hEvent;
   return pEvent->PulseEvent();   
}

BOOL WINAPI GetCursorPos(
   LPPOINT lpPoint
)
{
   QPoint pos = QCursor::pos();
   lpPoint->x = pos.x();
   lpPoint->y = pos.y();
   return TRUE;
}

DWORD WINAPI GetModuleFileName(
   HMODULE hModule,
   LPTSTR lpFilename,
   DWORD nSize
)
{
#if UNICODE
   wcsncpy(lpFilename,(LPWSTR)QCoreApplication::applicationFilePath().unicode(),nSize);
   return wcslen(lpFilename);
#else
   strncpy(lpFilename,QCoreApplication::applicationFilePath().toAscii().constData(),nSize);
   return strlen(lpFilename);
#endif
}

BOOL PathRemoveFileSpec(
   LPTSTR pszPath
)
{
   int len;
#if UNICODE
   len = wcslen(pszPath);
#else
   len = strlen(pszPath);
#endif
   for ( ; len > 0; len-- )
   {
      if ( (pszPath[len] == '/') || 
           (pszPath[len] == '\\') )
      {
         pszPath[len] = 0;
         break;
      }
   }
   return TRUE;
}

BOOL PathAppend(
   LPTSTR pszPath,
   LPCTSTR pszMore
)
{
   int len;
#if UNICODE
   len = wcslen(pszPath);
#else
   len = strlen(pszPath);
#endif
   if ( (pszPath[len-1] == '/') || 
        (pszPath[len-1] == '\\') )
   {
#if UNICODE
      wcscat(pszPath,_T("/"));
#else
      strcat(pszPath,"/");
#endif
   }
#if UNICODE
   wcscat(pszPath,pszMore);
#else
   strcat(pszPath,pszMore);
#endif
   return TRUE;
}

#if UNICODE
TCHAR* A2T(char* str)
{
   char* dup = strdup(str);
   TCHAR* ret = (TCHAR*)str;
   int i;
   for ( i = 0; i < strlen(dup); i++ )
   {
      ret[i] = (TCHAR)dup[i];
   }
   ret[i] = 0;
   free(dup);
   return ret;
}

char* T2A(TCHAR* str)
{
   TCHAR* dup = wcsdup(str);
   char* ret = (char*)str;
   int i;
   for ( i = 0; i < wcslen(str); i++ )
   {
      ret[i] = (char)dup[i];
   }
   ret[i] = 0;
   free(dup);
   return ret;
}
#else
TCHAR* A2T(char* str)
{
   return str;
}

char* T2A(TCHAR* str)
{
   return str;
}
#endif

int MulDiv(
  int nNumber,
  int nNumerator,
  int nDenominator
)
{
   long long intermediate = nNumber*nNumerator;
   return intermediate/nDenominator;
}

static QElapsedTimer tickTimer;
DWORD WINAPI GetTickCount(void)
{
   return tickTimer.elapsed();
}

DWORD WINAPI GetSysColor(
  int nIndex
)
{
   switch ( nIndex )
   {
   case COLOR_3DFACE:
      return 0xf0f0f0;
      break;
   case COLOR_BTNHIGHLIGHT:
      return 0xb0b0b0;
      break;
   case COLOR_APPWORKSPACE:
      return 0xe0e0e0;
      break;
   }
   return 0xffffff;
}

int WINAPI GetSystemMetrics(
  int nIndex
)
{
   static QScrollBar* sb = NULL;
   if ( !sb )
      sb = new QScrollBar(Qt::Vertical);
   switch ( nIndex )
   {
   case SM_CXVSCROLL:
      sb->setOrientation(Qt::Vertical);
      return sb->sizeHint().width();
      break;
   case SM_CYHSCROLL:
      sb->setOrientation(Qt::Horizontal);
      return sb->sizeHint().height();
      break;
   }
   return 0;
}

QMimeData* gpClipboardMimeData = NULL;
BOOL WINAPI OpenClipboard(
//  HWND hWndNewOwner
)
{
   if ( !gpClipboardMimeData )
   {
      gpClipboardMimeData = new QMimeData;
   }
   return TRUE;
}

BOOL WINAPI EmptyClipboard(void)
{
   QApplication::clipboard()->clear();
   return TRUE;
}

BOOL WINAPI CloseClipboard(void)
{
   return TRUE;
}

HANDLE WINAPI SetClipboardData(
  UINT uFormat,
  HANDLE hMem
)
{
   QSharedMemory* pMem = (QSharedMemory*)hMem;
   QByteArray value = QString::number((int)pMem,16).toAscii();
   gpClipboardMimeData->setData("application/x-qt-windows-mime;value=\"FamiTracker\"",value);
   QApplication::clipboard()->setMimeData(gpClipboardMimeData);
   return hMem;
}

BOOL WINAPI IsClipboardFormatAvailable(
  UINT format
)
{
   QStringList formats = QApplication::clipboard()->mimeData()->formats();
   
   if ( formats.count() && formats.contains("application/x-qt-windows-mime;value=\"FamiTracker\"") )
      return TRUE;
   return FALSE;
}

HANDLE WINAPI GetClipboardData(
  UINT uFormat
)
{
   QByteArray value = QApplication::clipboard()->mimeData()->data("application/x-qt-windows-mime;value=\"FamiTracker\"");
   return (HANDLE)value.toInt(0,16);
}

HGLOBAL WINAPI GlobalAlloc(
  UINT uFlags,
  SIZE_T dwBytes
)
{
   QSharedMemory* pMem = new QSharedMemory("FamiTracker");
   pMem->create(dwBytes);
   return pMem;
}

LPVOID WINAPI GlobalLock(
  HGLOBAL hMem
)
{
   QSharedMemory* pMem = (QSharedMemory*)hMem;
   
   pMem->lock();
   return pMem->data();
}

BOOL WINAPI GlobalUnlock(
  HGLOBAL hMem
)
{
   QSharedMemory* pMem = (QSharedMemory*)hMem;
   
   return pMem->unlock();
}

SIZE_T WINAPI GlobalSize(
  HGLOBAL hMem
)
{
   QSharedMemory* pMem = (QSharedMemory*)hMem;
   
   return pMem->size();
}

ACCEL* _acceleratorTbl = NULL;

HACCEL WINAPI CreateAcceleratorTable(
  LPACCEL lpaccl,
  int cEntries
)
{
   _acceleratorTbl = new ACCEL[cEntries];
   memcpy(_acceleratorTbl,lpaccl,cEntries*sizeof(ACCEL));
   return (HACCEL)_acceleratorTbl;
}

int WINAPI TranslateAccelerator(
  HWND hWnd,
  HACCEL hAccTable,
  LPMSG lpMsg
)
{
   qDebug("TranslateAccelerator");
}

BOOL WINAPI DestroyAcceleratorTable(
  HACCEL hAccel
)
{
   delete [] _acceleratorTbl;
   return TRUE;
}

UINT WINAPI MapVirtualKey(
  UINT uCode,
  UINT uMapType
)
{
   switch ( uMapType )
   {
   case MAPVK_VK_TO_VSC:
      qDebug("MapVirtualKey...not sure what to do here yet uCode=%x, uMapType=%d.",uCode,uMapType);
      switch ( uCode )
      {
      case VK_DIVIDE:
         return Qt::Key_Slash;
         break;
      }

      return uCode;
      break;
   default:
      qDebug("MapVirtualKey case %d not supported",uMapType);
      break;
   }
   return 0;
}

/*
 *  Class CString
 */

BOOL operator==(const CString& s1, const LPCTSTR s2)
{
#if UNICODE
   return (wcscmp(s1.GetString(),s2)==0);
#else
   return (strcmp(s1.GetString(),s2)==0);
#endif
}

BOOL operator!=(const CString& s1, const LPCTSTR s2)
{
#if UNICODE
   return (wcscmp(s1.GetString(),s2)!=0);
#else
   return (strcmp(s1.GetString(),s2)!=0);
#endif
}

BOOL operator <( const CString& s1, const CString& s2 )
{
#if UNICODE
   return (wcscmp(s1.GetString(),s2)<0);
#else
   return (strcmp(s1.GetString(),s2)<0);
#endif
}

CString::CString()
{
   _qstr.clear();
   _qstrn = QByteArray(_qstr.toLatin1());
   UpdateScratch();
}

CString::CString(LPCTSTR str)
{
   _qstr.clear();
#if UNICODE
   _qstr = QString::fromWCharArray(str);
#else
   _qstr = QString::fromAscii(str);
#endif
   UpdateScratch();
}

CString::CString(QString str)
{
   _qstr.clear();
   _qstr = str;
   UpdateScratch();
}

CString::CString(const CString &ref)
{
   _qstr.clear();
   _qstr = ref._qstr;
   UpdateScratch();
}

CString::~CString()
{
   _qstr.clear();
   UpdateScratch(); // BTODO: remove this later
   _qstr.clear();
}

BOOL CString::LoadString( UINT nID )
{
   _qstr.clear();
#if UNICODE
   _qstr.append(QString::fromWCharArray(qtMfcStringResource(nID).GetString()));
#else
   _qstr.append(QString::fromAscii(qtMfcStringResource(nID).GetString()));
#endif   
   UpdateScratch();
   return TRUE;   
}

void CString::UpdateScratch()
{
    _qstrn = QByteArray(_qstr.toLatin1());
    _qstrn.data();
}

void CString::Format( UINT nFormatID, ... )
{
   CString fmt = qtMfcStringResource(nFormatID);
   va_list argptr;
   va_start(argptr,nFormatID);
   FormatV(fmt,argptr);
   va_end(argptr);
}

void CString::Format(LPCTSTR fmt, ...)
{
   va_list argptr;
   va_start(argptr,fmt);
   FormatV(fmt,argptr);
   va_end(argptr);
}

void CString::FormatV(LPCTSTR fmt, va_list ap)
{
   // CPTODO: UN-HACK!!!
   TCHAR local[2048];
   int c, d, n;
#if UNICODE
   n = vswprintf(local,fmt,ap);
   // CPTODO: UN-HACK!!!  [This 'converts' a wchar_t string to a char string...
   for ( c = n-1; c >= 0; c-- )
   {
      if ( !(*(local+c)) )
      {
         for ( d = c; d < n; d++ )
         {
            *(local+d) = *(local+d+1);
         }
      }
   }
   _qstr.clear();
   _qstr = QString::fromWCharArray(local);
#else
   vsprintf(local,fmt,ap);
   _qstr.clear();
   _qstr = QString(local);
#endif
   UpdateScratch();
}

CString& CString::Append(LPCSTR str)
{
   _qstr.append(QString(str));
   UpdateScratch();
   return *this;
}

CString& CString::Append(LPWSTR str)
{
   _qstr.append(QString::fromWCharArray(str));
   UpdateScratch();
   return *this;
}

void CString::AppendFormat(LPCTSTR fmt, ...)
{
   va_list argptr;
   va_start(argptr,fmt);
   AppendFormatV(fmt,argptr);
   va_end(argptr);
}

void CString::AppendFormatV(LPCTSTR fmt, va_list ap)
{
   // CPTODO: UN-HACK!!!
   TCHAR local[2048];
#if UNICODE
   wvsprintf(local,fmt,ap);
   _qstr += QString::fromWCharArray(local);
#else
   vsprintf(local,fmt,ap);
   _qstr += QString(local);
#endif
//   _qstr.vsprintf((const char*)fmt,ap);
   UpdateScratch();
}

void CString::Truncate(int length)
{
   _qstr.truncate(length);
   UpdateScratch();
}

int CString::ReverseFind( TCHAR ch ) const
{
#if UNICODE
   return _qstr.lastIndexOf(QString::fromWCharArray(&ch));
#else
   return _qstr.lastIndexOf(QString(ch));
#endif
}

int CString::Compare( LPCTSTR lpsz ) const
{
#if UNICODE
   return _qstr.compare(QString::fromWCharArray(lpsz));
#else
   return _qstr.compare(lpsz);
#endif
}

CString& CString::operator=(const CString& str)
{
   _qstr.clear();
   _qstr = str._qstr;
   UpdateScratch();
   return *this;
}

CString& CString::operator=(LPCTSTR str)
{
   _qstr.clear();
#if UNICODE
   _qstr = QString::fromWCharArray(str);
#else
   _qstr = QString::fromAscii(str);
#endif
   UpdateScratch();
   return *this;
}

CString& CString::operator=(QString str)
{
   _qstr.clear();
   _qstr = str;
   UpdateScratch();
   return *this;
}

CString& CString::operator+=(const CString& str)
{
   _qstr.append(str._qstr);
   UpdateScratch();
   return *this;
}

CString& CString::operator+=(LPCTSTR str)
{
#if UNICODE
   _qstr.append(QString::fromWCharArray(str));
#else
   _qstr.append(QString::fromAscii(str));
#endif
   UpdateScratch();
   return *this;
}

CString& CString::operator+=(QString str)
{
   _qstr.append(str);
   UpdateScratch();
   return *this;
}

CString& CString::operator+(const CString& str)
{
   _qstr += str._qstr;
   UpdateScratch();
   return *this;
}

CString& CString::operator+(LPTSTR str)
{
#if UNICODE
   _qstr += QString::fromWCharArray(str);
#else
   _qstr += QString::fromAscii(str);
#endif
   UpdateScratch();
   return *this;
}

CString& CString::operator+(LPCTSTR str)
{
#if UNICODE
   _qstr += QString::fromWCharArray(str);
#else
   _qstr += QString::fromAscii(str);
#endif
   UpdateScratch();
   return *this;
}

CString& CString::operator+(QString str)
{
   _qstr += str;
   UpdateScratch();
   return *this;
}

CString::operator QString() const
{
   return _qstr;
}

CString::operator const TCHAR*() const
{
   return GetString();
}

void CString::Empty() 
{ 
   _qstr.clear(); 
   UpdateScratch();
}

LPCTSTR CString::GetString() const
{
#if UNICODE
   return (LPCTSTR)_qstr.unicode();
#else
   return _qstrn.constData();
#endif
}

LPTSTR CString::GetBuffer( int nMinBufLength )
{
   _qstr.reserve(nMinBufLength+1); // Space for null-terminator.
   UpdateScratch();
#if UNICODE
   return (LPTSTR)_qstr.unicode();
#else
   return (LPTSTR)_qstrn.data();
#endif
}

void CString::ReleaseBuffer( int nNewLength )
{
   if ( nNewLength >= 0 )
   {
      // Append null.
      _qstr[nNewLength] = 0;
   }
}

int CString::Find( TCHAR ch ) const
{
#if UNICODE
   return _qstr.indexOf(ch);
#else
   return _qstr.indexOf(ch);
#endif
}

int CString::Find( LPCTSTR lpszSub ) const
{
#if UNICODE
   return _qstr.indexOf(QString::fromWCharArray(lpszSub));
#else
   return _qstr.indexOf(QString::fromAscii(lpszSub));
#endif
}

int CString::Find( TCHAR ch, int nStart ) const
{
#if UNICODE
   return _qstr.indexOf(ch,nStart);
#else
   return _qstr.indexOf(ch,nStart);
#endif
}

int CString::Find( LPCTSTR pstr, int nStart ) const
{
#if UNICODE
   return _qstr.indexOf(QString::fromWCharArray(pstr),nStart);
#else
   return _qstr.indexOf(QString::fromAscii(pstr),nStart);
#endif
}

CString CString::Left( int nCount ) const
{
   return CString(_qstr.left(nCount));
}

CString CString::Right( int nCount ) const
{
   return CString(_qstr.right(nCount));
}

CString CString::Mid( int nFirst ) const
{
   return CString(_qstr.mid(nFirst));
}

CString CString::Mid( int nFirst, int nCount ) const
{
   return CString(_qstr.mid(nFirst,nCount));
}

CString CString::MakeUpper( )
{
   return CString(_qstr.toUpper());
}

CString CString::MakeLower( )
{
   return CString(_qstr.toLower());
}

int CString::GetLength() const
{
   return _qstr.length();
}

int CString::CompareNoCase( LPCTSTR lpsz ) const
{
#if UNICODE
   return _qstr.compare(QString::fromWCharArray(lpsz),Qt::CaseInsensitive);
#else
   return _qstr.compare(QString(lpsz),Qt::CaseInsensitive);
#endif
}

TCHAR CString::GetAt( int nIndex ) const
{
   return _qstr.at(nIndex).toAscii();
}

CStringA::CStringA(CString str)
{ 
   _qstr = str.GetString(); 
   UpdateScratch();
}

CStringA::operator char*() const
{
   return _qstr.toAscii().data();
}

INT_PTR CStringArray::Add( LPCTSTR newElement )
{
   _qlist.append(CString(newElement));
   return _qlist.count()-1;
}

void CStringArray::RemoveAll( )
{
   _qlist.clear();
}

CString CStringArray::GetAt(int idx) const
{ 
   return _qlist.at(idx); 
}

void CStringArray::SetAt(int idx, CString str)
{ 
   _qlist.replace(idx,str); 
}

INT_PTR CStringArray::GetCount( ) const 
{ 
   return _qlist.count(); 
}

CString CStringArray::operator []( INT_PTR nIndex )
{
   return GetAt(nIndex);
}

CString CStringArray::operator []( INT_PTR nIndex ) const
{
   return GetAt(nIndex);
}

BOOL CStringArray::IsEmpty( ) const
{
   return _qlist.isEmpty();
}

/*
 *  Class CFile
 */

CFile::CFile()
{
}

CFile::CFile(
   LPCTSTR lpszFileName,
   UINT nOpenFlags 
)
{
   QFile::OpenMode flags;
#if UNICODE
   _qfile.setFileName(QString::fromWCharArray(lpszFileName));
#else
   _qfile.setFileName(lpszFileName);
#endif
   if ( nOpenFlags&modeRead ) flags = QIODevice::ReadOnly;
   if ( nOpenFlags&modeWrite ) flags = QIODevice::WriteOnly;
   if ( nOpenFlags&modeCreate ) flags |= QIODevice::Truncate;
   _qfile.open(flags);
}

CFile::~CFile()
{
   if ( _qfile.isOpen() )
      _qfile.close();
}

BOOL CFile::Open(
   LPCTSTR lpszFileName,
   UINT nOpenFlags,
   CFileException* pError
)
{
   QFile::OpenMode flags;
#if UNICODE
   _qfile.setFileName(QString::fromWCharArray(lpszFileName));
#else
   _qfile.setFileName(lpszFileName);
#endif
   if ( nOpenFlags&modeRead ) flags = QIODevice::ReadOnly;
   if ( nOpenFlags&modeWrite ) flags = QIODevice::WriteOnly;
   if ( nOpenFlags&modeCreate ) flags |= QIODevice::Truncate;
   return _qfile.open(flags);
}

void CFile::Write(
   const void* lpBuf,
   UINT nCount
)
{
   if ( _qfile.isOpen() )
      _qfile.write((const char*)lpBuf,nCount);
}

UINT CFile::Read(
   void* lpBuf,
   UINT nCount
)
{
   if ( _qfile.isOpen() )
      return _qfile.read((char*)lpBuf,nCount);
   else
      return 0;
}

ULONGLONG CFile::Seek(
   LONGLONG lOff,
   UINT nFrom 
)
{
   if ( _qfile.isOpen() )
   {
      switch ( nFrom )
      {
      case begin:
         return _qfile.seek(lOff);
         break;
      case current:
         return _qfile.seek(_qfile.pos()+lOff);
         break;
      case end:
         return _qfile.seek(_qfile.size()+lOff);
         break;
      }
      return 0;
   }
   else
      return 0;
}

ULONGLONG CFile::GetPosition( ) const
{
   return _qfile.pos();
}

ULONGLONG CFile::GetLength( ) const
{
   if ( _qfile.isOpen() )
      return _qfile.size();
   else
      return 0;
}

void CFile::Close()
{
   if ( _qfile.isOpen() )
      _qfile.close();
}

CRect::CRect( )
{
   top = 0;
   bottom = 0;
   left = 0;
   right = 0;
}

CRect::CRect( 
   int l, 
   int t, 
   int r, 
   int b  
)
{
   top = t;
   bottom = b;
   left = l;
   right = r;
}

CRect::CRect( 
   const RECT& srcRect  
)
{
   top = srcRect.top;
   bottom = srcRect.bottom;
   left = srcRect.left;
   right = srcRect.right;
}

CRect::CRect( 
   LPCRECT lpSrcRect  
)
{
   top = lpSrcRect->top;
   bottom = lpSrcRect->bottom;
   left = lpSrcRect->left;
   right = lpSrcRect->right;
}

CRect::CRect( 
   POINT point, 
   SIZE size  
)
{
   top = point.y;
   bottom = point.y+size.cy;
   left = point.x;
   right = point.x+size.cx;
}

CRect::CRect( 
   POINT topLeft, 
   POINT bottomRight  
)
{
   top = topLeft.y;
   bottom = bottomRight.y;
   left = topLeft.x;
   right = bottomRight.x;
}

void CRect::MoveToX(
   int x
)
{
   int width = Width();
   right = x+width;
   left = x;
}

void CRect::MoveToY(
   int y
)
{
   int height = Height();
   bottom = y+height;
   top = y;
}

void CRect::MoveToXY(
   int x,
   int y 
)
{
   int width = Width();
   right = x+width;
   left = x;
   int height = Height();
   bottom = y+height;
   top = y;
}

void CRect::MoveToXY(
   POINT point 
)
{
   int width = Width();
   right = point.x+width;
   left = point.x;
   int height = Height();
   bottom = point.y+height;
   top = point.y;
}

void CRect::DeflateRect( 
   int x, 
   int y  
)
{
   left += x;
   right -= x;
   top += y;
   bottom -= y;
}

void CRect::DeflateRect( 
   SIZE size  
)
{
   left += size.cx;
   right -= size.cx;
   top += size.cy;
   bottom -= size.cy;
}

void CRect::DeflateRect( 
   LPCRECT lpRect  
)
{
   left += lpRect->left;
   right -= lpRect->right;
   top += lpRect->top;
   bottom -= lpRect->bottom;
}

void CRect::DeflateRect( 
   int l, 
   int t, 
   int r, 
   int b  
)
{
   left += l;
   right -= r;
   top += t;
   bottom -= b;
}

void CRect::InflateRect( 
   int x, 
   int y  
)
{
   left -= x;
   right += x;
   top -= y;
   bottom += y;
}

void CRect::InflateRect( 
   SIZE size  
)
{
   left -= size.cx;
   right += size.cx;
   top -= size.cy;
   bottom += size.cy;
}

void CRect::InflateRect( 
   LPCRECT lpRect  
)
{
   left -= lpRect->left;
   right += lpRect->right;
   top -= lpRect->top;
   bottom += lpRect->bottom;
}

void CRect::InflateRect( 
   int l, 
   int t, 
   int r, 
   int b  
)
{
   left -= l;
   right += r;
   top -= t;
   bottom += b;
}

/*
 *  CDC object classes
 */

CPen::CPen()
{
}

CPen::CPen(
   int nPenStyle,
   int nWidth,
   COLORREF crColor 
)
{
   QColor color(GetRValue(crColor),GetGValue(crColor),GetBValue(crColor));
   _qpen.setWidth(nWidth);
   _qpen.setColor(color);
   switch ( nPenStyle )
   {   
   case PS_SOLID:      
      _qpen.setStyle(Qt::SolidLine);
      break;
   case PS_DASH:
      _qpen.setStyle(Qt::DashLine);
      break;
   case PS_DOT:
      _qpen.setStyle(Qt::DotLine);
      break;
   case PS_DASHDOT:
      _qpen.setStyle(Qt::DashDotLine);
      break;
   case PS_DASHDOTDOT:
      _qpen.setStyle(Qt::DashDotDotLine);
      break;
   case PS_NULL:
      _qpen.setStyle(Qt::NoPen);
      break;
   case PS_INSIDEFRAME:
      qDebug("PS_INSIDEFRAME not supported");
      break;
   }
}
CPen::CPen(
   int nPenStyle,
   int nWidth,
   const LOGBRUSH* pLogBrush,
   int nStyleCount,
   const DWORD* lpStyle 
)
{
   QBrush brush;
   QColor color(GetRValue(pLogBrush->lbColor),GetGValue(pLogBrush->lbColor),GetBValue(pLogBrush->lbColor));
   
   switch ( pLogBrush->lbStyle )
   {
   case BS_SOLID:
      brush.setColor(color);
      break;
   default:
      qDebug("CPen: brush style %d not supported",pLogBrush->lbStyle);
   }

   _qpen.setBrush(brush);
   _qpen.setWidth(nWidth);
   switch ( nPenStyle )
   {   
   case PS_SOLID:      
      _qpen.setStyle(Qt::SolidLine);
      break;
   case PS_DASH:
      _qpen.setStyle(Qt::DashLine);
      break;
   case PS_DOT:
      _qpen.setStyle(Qt::DotLine);
      break;
   case PS_DASHDOT:
      _qpen.setStyle(Qt::DashDotLine);
      break;
   case PS_DASHDOTDOT:
      _qpen.setStyle(Qt::DashDotDotLine);
      break;
   case PS_NULL:
      _qpen.setStyle(Qt::NoPen);
      break;
   case PS_INSIDEFRAME:
      qDebug("PS_INSIDEFRAME not supported");
      break;
   }
}

BOOL CPen::CreatePen(
   int nPenStyle,
   int nWidth,
   COLORREF crColor 
)
{
   QColor color(GetRValue(crColor),GetGValue(crColor),GetBValue(crColor));
   _qpen.setWidth(nWidth);
   _qpen.setColor(color);
   switch ( nPenStyle )
   {   
   case PS_SOLID:      
      _qpen.setStyle(Qt::SolidLine);
      break;
   case PS_DASH:
      _qpen.setStyle(Qt::DashLine);
      break;
   case PS_DOT:
      _qpen.setStyle(Qt::DotLine);
      break;
   case PS_DASHDOT:
      _qpen.setStyle(Qt::DashDotLine);
      break;
   case PS_DASHDOTDOT:
      _qpen.setStyle(Qt::DashDotDotLine);
      break;
   case PS_NULL:
      _qpen.setStyle(Qt::NoPen);
      break;
   case PS_INSIDEFRAME:
      qDebug("PS_INSIDEFRAME not supported");
      break;
   }
   return TRUE;
}

CBrush::CBrush( )
{
   _qbrush.setStyle(Qt::SolidPattern);
}

CBrush::CBrush(
   COLORREF crColor 
)
{
   QColor color(GetRValue(crColor),GetGValue(crColor),GetBValue(crColor));
   _qbrush.setStyle(Qt::SolidPattern);
   _qbrush.setColor(color);
}

CBrush::CBrush(
   int nIndex,
   COLORREF crColor 
)
{
   QColor color(GetRValue(crColor),GetGValue(crColor),GetBValue(crColor));
   _qbrush.setColor(color);
   switch ( nIndex )
   {
   case HS_BDIAGONAL:
      _qbrush.setStyle(Qt::BDiagPattern);
      break;
   case HS_CROSS:
      _qbrush.setStyle(Qt::CrossPattern);
      break;
   case HS_DIAGCROSS:
      _qbrush.setStyle(Qt::DiagCrossPattern);
      break;
   case HS_FDIAGONAL:
      _qbrush.setStyle(Qt::FDiagPattern);
      break;
   case HS_HORIZONTAL:
      _qbrush.setStyle(Qt::HorPattern);
      break;
   case HS_VERTICAL:
      _qbrush.setStyle(Qt::VerPattern);
      break;
   }
}

CBrush::CBrush(
   CBitmap* pBitmap 
)
{
   QPixmap pixmap = (QPixmap)*pBitmap->toQPixmap();
   _qbrush.setTextureImage(pixmap.toImage());
}

BOOL CBrush::CreateSolidBrush(
   COLORREF crColor 
)
{
   QColor color(GetRValue(crColor),GetGValue(crColor),GetBValue(crColor));
   _qbrush.setColor(color);
   return TRUE;
}

BOOL CFont::CreateFont(
   int nHeight,
   int nWidth,
   int nEscapement,
   int nOrientation,
   int nWeight,
   BYTE bItalic,
   BYTE bUnderline,
   BYTE cStrikeOut,
   BYTE nCharSet,
   BYTE nOutPrecision,
   BYTE nClipPrecision,
   BYTE nQuality,
   BYTE nPitchAndFamily,
   LPCTSTR lpszFacename 
)
{
#if UNICODE
   _qfont.setFamily(QString::fromWCharArray(lpszFacename));
#else
   _qfont.setFamily(lpszFacename);
#endif

   _qfont.setPointSize(abs(nHeight)*.75);
   _qfont.setItalic(bItalic);
   _qfont.setUnderline(bUnderline);
   _qfont.setStrikeOut(cStrikeOut);
   _qfont.setBold(nWeight>=FW_BOLD);
   return TRUE;
}

BOOL CFont::CreateFontIndirect(
   const LOGFONT* lpLogFont 
)
{
#if UNICODE
   _qfont.setFamily(QString::fromWCharArray(lpLogFont->lfFaceName));
#else
   _qfont.setFamily(lpLogFont->lfFaceName);
#endif
   _qfont.setPointSize(abs(lpLogFont->lfHeight)*.75);
   _qfont.setItalic(lpLogFont->lfItalic);
   _qfont.setBold(lpLogFont->lfWeight>=FW_BOLD);
   return TRUE;
}

CBitmap::CBitmap()
{
   _qpixmap = new QPixmap(1,1);
   _owned = true;
}

CBitmap::CBitmap(QString resource)
{
   _qpixmap = new QPixmap(resource);
   _owned = false;
}

CBitmap::~CBitmap()
{
   if ( _owned )
      delete _qpixmap;
}

BOOL CBitmap::CreateCompatibleBitmap(
   CDC* pDC,
   int nWidth,
   int nHeight 
)
{
   if ( _owned )
      delete _qpixmap;
   _qpixmap = new QPixmap(nWidth,nHeight);
   _owned = true;
   return TRUE;
}

BOOL CBitmap::CreateBitmap(
   int nWidth,
   int nHeight,
   UINT nPlanes,
   UINT nBitcount,
   const void* lpBits 
)
{
   if ( _owned )
      delete _qpixmap;
   _qpixmap = new QPixmap(nWidth,nHeight);
   _qpixmap->loadFromData((const uchar*)lpBits,nWidth*nHeight*sizeof(short int));
   _owned = true;
   return TRUE;
}

CSize CBitmap::SetBitmapDimension(
   int nWidth,
   int nHeight 
)
{
   CSize origSize;
   origSize = _qpixmap->size();
   if ( _owned )
      delete _qpixmap;
   _qpixmap = new QPixmap(nWidth,nHeight);
   _owned = true;
   return origSize;
}

CSize CBitmap::GetBitmapDimension( ) const
{
   return CSize(_qpixmap->size());
}

BOOL CBitmap::LoadBitmap(
   UINT nIDResource 
)
{
   BOOL result = FALSE;
   if ( _owned )
      delete _qpixmap;
   _qpixmap = qtMfcBitmapResource(nIDResource)->toQPixmap();
   _owned = false;
   result = TRUE;
   return result;
}

/*
 *  Class CDC
 */

CDC::CDC()
{
   m_hDC = NULL;
   _qwidget = NULL;
   _qpainter = NULL;
   _qpixmap = NULL;
   _pen = NULL;
   _brush = NULL;
   _font = NULL;
   _bitmap = NULL;
   _bitmapSize = QSize(-1,-1);
   _rgn = NULL;   
   _gdiobject = NULL;
   _object = NULL;
   _lineOrg.x = 0;
   _lineOrg.y = 0;
   _bkColor = QColor(0,0,0);
   _bkMode = 0;
   _textColor = QColor(0,0,0);
   _windowOrg.x = 0;
   _windowOrg.y = 0;
   attached = false;
   _doFlush = false;
}

CDC::CDC(CWnd* parent)
{
   m_hDC = (HDC)parent->toQWidget();
   _qwidget = parent->toQWidget();
   _qpainter = NULL;
   _qpixmap = NULL;
   _pen = NULL;
   _brush = NULL;
   _font = NULL;
   _bitmap = NULL;
   _bitmapSize = QSize(-1,-1);
   _rgn = NULL;   
   _gdiobject = NULL;
   _object = NULL;
   _lineOrg.x = 0;
   _lineOrg.y = 0;
   _bkColor = QColor(0,0,0);
   _bkMode = 0;
   _textColor = QColor(0,0,0);
   _windowOrg.x = 0;
   _windowOrg.y = 0;
   attached = false;
   _doFlush = false;
   
   attach(parent->toQWidget());
}

CDC::~CDC()
{
   flush();
   detach();
}

void CDC::flush()
{
   if ( _qwidget && _doFlush )
   {
      QPainter p;
      p.begin(_qwidget);
      p.drawPixmap(0,0,*_qpixmap);
      p.end();
   }   
}

void CDC::attach()
{
   _qpixmap = new QPixmap(1,1);
   _qpainter = new QPainter(_qpixmap);
   m_hDC = (HDC)_qpixmap;
   attached = true;
}

void CDC::attach(QWidget* parent)
{
   _qwidget = parent;
   _qpixmap = new QPixmap(_qwidget->size());
   _qpixmap->fill(_qwidget,0,0); // CP: hack to initialize pixmap with widget's background color.
   _qpainter = new QPainter(_qpixmap);
   m_hDC = (HDC)_qpixmap;
   attached = true;
   _doFlush = true;
}

void CDC::detach()
{   
   if ( attached )
   {
      if ( _qpainter )
      {
         if ( _qpainter->isActive() )
            _qpainter->end();
         delete _qpainter;
         _qpainter = NULL;
      }
      delete _qpixmap;
   }
   attached = false;
}

BOOL CDC::CreateCompatibleDC(
   CDC* pDC 
)
{
   if ( pDC->widget() )
      attach(pDC->widget());
   else
      attach();
   return TRUE;
}

BOOL CDC::DeleteDC( )
{
   detach();
   m_hDC = NULL;
   return TRUE;
}

HGDIOBJ CDC::SelectObject(
   HGDIOBJ obj
)
{
   CGdiObject* pObj = (CGdiObject*)obj;
   CPen* pPen = dynamic_cast<CPen*>(pObj);
   if ( pPen )
      return SelectObject(pPen);
   CBrush* pBrush = dynamic_cast<CBrush*>(pObj);
   if ( pBrush )
      return SelectObject(pBrush);
   CFont* pFont = dynamic_cast<CFont*>(pObj);
   if ( pFont )
      return SelectObject(pFont);
   CBitmap* pBitmap = dynamic_cast<CBitmap*>(pObj);
   if ( pBitmap )
      return SelectObject(pBitmap);
   return NULL;
}

CPen* CDC::SelectObject(
   CPen* pPen 
)
{
   CPen* temp = _pen;
   _pen = pPen;
   if ( _pen )
      _qpainter->setPen((QPen)(*_pen));
   return temp;
}

CBrush* CDC::SelectObject(
   CBrush* pBrush 
)
{
   CBrush* temp = _brush;
   _brush = pBrush;
   if ( _brush )      
      _qpainter->setBrush((QBrush)(*_brush));
   return temp;
}

CFont* CDC::SelectObject(
   CFont* pFont 
)
{
   CFont* temp = _font;
   _font = pFont;
   if ( _font )
      _qpainter->setFont((QFont)(*_font));
   return temp;
}

CBitmap* CDC::SelectObject(
   CBitmap* pBitmap 
)
{
   CBitmap* temp = _bitmap;
   _bitmap = pBitmap;
   if ( _bitmap )
   {
      _qpainter->drawPixmap(0,0,*_bitmap->toQPixmap());
      _bitmapSize = _bitmap->toQPixmap()->size();
   }
   else
   {
      _bitmapSize = QSize(-1,-1);
   }
   return temp;
}   

CGdiObject* CDC::SelectObject(
   CGdiObject* pObject
)
{
   CGdiObject* temp = _gdiobject;
   _gdiobject = pObject;
   return temp;
}   

CObject* CDC::SelectObject(
   CObject* pObject
)
{
   CObject* temp = _object;
   _object = pObject;
   return temp;
}   

int CDC::GetDeviceCaps(
   int nIndex 
) const
{
   switch ( nIndex )
   {
   case LOGPIXELSX:
      return 96; // DEFAULT_DPI is unfortunately static const...
      break;
   case LOGPIXELSY:
      return 96; // DEFAULT_DPI is unfortunately static const...
      break;
   }
   return 0;
}

COLORREF CDC::GetPixel(
   int x,
   int y 
) const
{
   QImage image = _qpixmap->toImage();
   QColor col = image.pixel(x,y);
   COLORREF ret = RGB(col.red(),col.green(),col.blue());
   return ret;
}

COLORREF CDC::GetPixel(
   POINT point 
) const
{
   return GetPixel(point.x,point.y);
}

BOOL CDC::BitBlt(
   int x,
   int y,
   int nWidth,
   int nHeight,
   CDC* pSrcDC,
   int xSrc,
   int ySrc,
   DWORD dwRop 
)
{
   QPixmap* pixmap = pSrcDC->pixmap();
   QSize pixmapSize = pSrcDC->pixmapSize();
   pixmapSize = pixmapSize.boundedTo(QSize(nWidth,nHeight));
   if ( pixmap && (pixmapSize.width() >= 0) )
      _qpainter->drawPixmap(x,y,pixmapSize.width(),pixmapSize.height(),*pixmap,xSrc,ySrc,pixmapSize.width(),pixmapSize.height());
   else
      _qpainter->drawPixmap(x,y,nWidth,nHeight,*pixmap,xSrc,ySrc,nWidth,nHeight);
   return TRUE;
}

int StretchDIBits(
  CDC& dc,
  int XDest,
  int YDest,
  int nDestWidth,
  int nDestHeight,
  int XSrc,
  int YSrc,
  int nSrcWidth,
  int nSrcHeight,
  const VOID *lpBits,
  const BITMAPINFO *lpBitsInfo,
  UINT iUsage,
  DWORD dwRop
)
{
   QImage image((const uchar*)lpBits,nSrcWidth,nSrcHeight,QImage::Format_RGB32);
   image = image.scaled(nDestWidth,nDestHeight);
   dc.painter()->drawImage(XDest,YDest,image);
   dc.flush();
   return 0;
}

BOOL CDC::DrawEdge(
   LPRECT lpRect,
   UINT nEdge,
   UINT nFlags 
)
{
   QRect rect(lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top);
   _qpainter->drawRect(rect);
   qDebug("CDC::DrawEdge not implemented!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
   return TRUE;
}

BOOL CDC::Rectangle(
   int x1,
   int y1,
   int x2,
   int y2 
)
{
   QRect rect(x1,y1,x2-x1,y2-y1);
   _qpainter->drawRect(rect);
   return TRUE;
}

BOOL CDC::Rectangle(
   LPCRECT lpRect 
)
{
   return Rectangle(lpRect->left,lpRect->top,lpRect->right,lpRect->bottom);
}

void CDC::Draw3dRect( int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight )
{
   QPen tlc(QColor(GetRValue(clrTopLeft),GetGValue(clrTopLeft),GetBValue(clrTopLeft)));
   QPen brc(QColor(GetRValue(clrBottomRight),GetGValue(clrBottomRight),GetBValue(clrBottomRight)));
   QPen origPen = _qpainter->pen();
   x -= _windowOrg.x;
   y -= _windowOrg.y;
   _qpainter->setPen(tlc);
   _qpainter->drawLine(x,y,x+cx-1,y);
   _qpainter->drawLine(x,y,x,y+cy-1);
   _qpainter->setPen(brc);
   _qpainter->drawLine(x+cx-1,y+cy-1,x,y+cy-1);
   _qpainter->drawLine(x+cx-1,y+cy-1,x+cx-1,y);
   _qpainter->setPen(origPen);
}
int CDC::DrawText(
   const CString& str,
   LPRECT lpRect,
   UINT nFormat 
)
{
   QRect rect(lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top);
#if UNICODE
   QString qstr = QString::fromWCharArray((LPCTSTR)str);
#else
   QString qstr = (LPCTSTR)str;
#endif
   QPen origPen = _qpainter->pen();
   _qpainter->setPen(QPen(_textColor));
//   _qpainter->setFont((QFont)*_font);
   _qpainter->drawText(rect,qstr.toLatin1().constData());
   return _tcslen((LPCTSTR)str);   
   _qpainter->setPen(origPen);
}
int CDC::DrawText(
   LPCTSTR lpszString,
   int nCount,
   LPRECT lpRect,
   UINT nFormat 
)
{
   QRect rect(lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top);
#if UNICODE
   QString qstr = QString::fromWCharArray(lpszString);
#else
   QString qstr(lpszString);
#endif
   QPen origPen = _qpainter->pen();
   _qpainter->setPen(QPen(_textColor));
//   _qpainter->setFont((QFont)*_font);
   _qpainter->drawText(rect,qstr.left(nCount).toLatin1().constData());
   _qpainter->setPen(origPen);
   return 0; // CP: should be text height  
}

void CDC::FillSolidRect(
   LPCRECT lpRect,
   COLORREF clr 
)
{
   QRect rect(lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top);
   rect.translate(-QPoint(_windowOrg.x,_windowOrg.y));
   QColor color(GetRValue(clr),GetGValue(clr),GetBValue(clr));
   _qpainter->fillRect(rect,color);
}

void CDC::FillSolidRect(
   int x,
   int y,
   int cx,
   int cy,
   COLORREF clr 
)
{
   QRect rect(x,y,cx,cy);
   rect.translate(-QPoint(_windowOrg.x,_windowOrg.y));
   QColor color(GetRValue(clr),GetGValue(clr),GetBValue(clr));
   _qpainter->fillRect(rect,color);
}

BOOL CDC::GradientFill( 
   TRIVERTEX* pVertices, 
   ULONG nVertices, 
   void* pMesh, 
   ULONG nMeshElements, 
   DWORD dwMode  
)
{
   QRect rect;
   GRADIENT_RECT* grect = (GRADIENT_RECT*)pMesh;
   ULONG el;
   
   for ( el = 0; el < nMeshElements; el++ )
   {
      rect = QRect(QPoint(pVertices[grect[el].UpperLeft].x,pVertices[grect[el].UpperLeft].y),QPoint(pVertices[grect[el].LowerRight].x-1,pVertices[grect[el].LowerRight].y-1));
      rect.translate(-QPoint(_windowOrg.x,_windowOrg.y));

      QLinearGradient gradient;
      QGradientStops gstops;
      gradient.setStart(rect.topLeft());
      gradient.setFinalStop(rect.bottomLeft());
      gradient.setColorAt(0,QColor(pVertices[grect[el].UpperLeft].Red>>8,pVertices[grect[el].UpperLeft].Green>>8,pVertices[grect[el].UpperLeft].Blue>>8,pVertices[grect[el].UpperLeft].Alpha>>8));
      gradient.setColorAt(1,QColor(pVertices[grect[el].LowerRight].Red>>8,pVertices[grect[el].LowerRight].Green>>8,pVertices[grect[el].LowerRight].Blue>>8,pVertices[grect[el].LowerRight].Alpha>>8));
      QBrush brush(gradient);
   
      _qpainter->fillRect(rect,brush);
   }
   return TRUE;
}

BOOL CDC::LineTo( 
   int x, 
   int y  
)
{
   _qpainter->drawLine(_lineOrg.x,_lineOrg.y,x,y);
   _lineOrg.x = x;
   _lineOrg.y = y;
   return TRUE;
}

BOOL CDC::Polygon(
   LPPOINT lpPoints,
   int nCount 
)
{
   int pt;
   QPainterPath path;
   QPolygon poly;
   for ( pt = 0; pt < nCount; pt++ )
   {
      poly.append(QPoint(lpPoints[pt].x,lpPoints[pt].y));
   }
   poly.append(QPoint(lpPoints[0].x,lpPoints[0].y));
   path.addPolygon(poly);
   _qpainter->fillPath(path,(QBrush)*_brush);
   _qpainter->drawPath(path);
   return TRUE;
}

int CDC::SelectObject(
   CRgn* pRgn 
)
{
   return TRUE;
}

COLORREF CDC::SetPixel( int x, int y, COLORREF crColor )
{
   return TRUE;
}

BOOL CDC::TextOut(
   int x,
   int y,
   LPCTSTR lpszString,
   int nCount 
)
{
#if UNICODE
   QString qstr = QString::fromWCharArray(lpszString);
#else
   QString qstr(lpszString);
#endif
   QFontMetrics fontMetrics((QFont)*_font);
   QPen origPen = _qpainter->pen();
   _qpainter->setPen(QPen(_textColor));
//   _qpainter->setFont((QFont)*_font);
   x += -_windowOrg.x;
   y += -_windowOrg.y;
   y += fontMetrics.ascent();
   _qpainter->drawText(x,y,qstr.left(nCount));
   _qpainter->setPen(origPen);
   return TRUE;
}

BOOL CDC::TextOut(
   int x,
      int y,
      const CString& str 
)
{
   QFontMetrics fontMetrics((QFont)*_font);
   QPen origPen = _qpainter->pen();
   _qpainter->setPen(QPen(_textColor));
//   _qpainter->setFont((QFont)*_font);
   x += -_windowOrg.x;
   y += -_windowOrg.y;
   y += fontMetrics.ascent();
   _qpainter->drawText(x,y,(const QString&)str);
   _qpainter->setPen(origPen);
   return TRUE;
}

CComboBox::CComboBox(CWnd *parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   _qt = new QComboBox(parent->toQWidget());

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QComboBox*>(_qt);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(currentIndexChanged(int)),this,SIGNAL(currentIndexChanged(int)));
}

CComboBox::~CComboBox()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CComboBox::Create(
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   _qtd->setGeometry(rect.left,rect.top,rect.right-rect.left,_qtd->sizeHint().height());
   _qtd->setVisible(dwStyle&WS_VISIBLE);
   
   QFontMetrics fm(_qtd->font());
   
   _qtd->setMaxVisibleItems((rect.bottom-rect.top)/fm.height());
   
   return TRUE;
}

void CComboBox::SetWindowText(
   LPCTSTR lpszString 
)
{
#if UNICODE
   _qtd->lineEdit()->setText(QString::fromWCharArray(lpszString));
#else
   _qtd->lineEdit()->setText(lpszString);
#endif
}

void CComboBox::ResetContent()
{
   _qtd->blockSignals(true);
   _qtd->clear();
   _qtd->blockSignals(false);
}

int CComboBox::AddString(
   LPCTSTR lpszString 
)
{
   _qtd->blockSignals(true); // Don't cause CbnSelchange yet...
#if UNICODE
   _qtd->addItem(QString::fromWCharArray(lpszString));
#else
   _qtd->addItem(lpszString);
#endif
   _qtd->blockSignals(false); // Don't cause CbnSelchange yet...
   
   return _qtd->count()-1;
}

void CComboBox::SetCurSel(int sel)
{
   _qtd->setCurrentIndex(sel);
}

int CComboBox::GetCurSel( ) const
{
   return _qtd->currentIndex();
}

int CComboBox::GetLBText(
   int nIndex,
   LPTSTR lpszText 
) const
{
   QString lbText = _qtd->itemText(nIndex);
   int length = CB_ERR;
#if UNICODE
   wcscpy(lpszText,(LPWSTR)lbText.unicode());
   length = wcslen(lpszText);
#else
   strcpy(lpszText,lbText.toAscii().constData());
   length = strlen(lpszText);
#endif
   return length;
}

#if UNICODE
int CComboBox::GetLBText(
   int nIndex,
   char* lpszText 
) const
{
   QString lbText = _qtd->itemText(nIndex);
   int length = CB_ERR;
   strcpy(lpszText,lbText.toAscii().constData());
   length = strlen(lpszText);
   return length;
}
#endif

void CComboBox::GetLBText(
   int nIndex,
   CString& rString 
) const
{
   rString = _qtd->itemText(nIndex);
}

int CComboBox::SelectString(
   int nStartAfter,
   LPCTSTR lpszString 
)
{
   int item;
   int index = -1;
#if UNICODE
   QString string = QString::fromWCharArray(lpszString);
#else
   QString string = lpszString;
#endif
   for ( item = nStartAfter; item < _qtd->count(); item++ )
   {
      if ( _qtd->itemText(item) == string )
      {
         _qtd->setCurrentIndex(item);
         index = item;
         break;
      }
   }
   return index;
}

void CComboBox::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned 
)
{
   _qtd->setEditText(QString::number(nValue));
}

UINT CComboBox::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   return _qtd->currentText().toInt();
}

void CComboBox::SetDlgItemText(
   int nID,
   LPCTSTR lpszString 
)
{
#if UNICODE
   _qtd->setEditText(QString::fromWCharArray(lpszString));
#else
   _qtd->setEditText(QString::fromAscii(lpszString));
#endif
}

int CComboBox::GetDlgItemText(
   int nID,
   CString& rString 
) const
{
   rString = _qtd->currentText();
   return _qtd->currentText().length();
}

int CComboBox::GetDlgItemText(
   int nID,
   LPTSTR lpStr,
   int nMaxCount 
) const
{
#if UNICODE
   wcsncpy(lpStr,(LPWSTR)_qtd->currentText().unicode(),nMaxCount);
#else
   strncpy(lpStr,_qtd->currentText().toAscii().constData(),nMaxCount);
#endif   
   return _qtd->currentText().length();
}

CListBox::CListBox(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   _qt = new QListWidget(parent->toQWidget());
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QListWidget*>(_qt);
      
   _qtd->setFont(QFont("MS Shell Dlg",8));
   _qtd->setEditTriggers(QAbstractItemView::NoEditTriggers);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(itemSelectionChanged()),this,SIGNAL(itemSelectionChanged()));
   QObject::connect(_qtd,SIGNAL(itemClicked(QListWidgetItem*)),this,SIGNAL(itemClicked(QListWidgetItem*)));
   QObject::connect(_qtd,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SIGNAL(itemDoubleClicked(QListWidgetItem*)));
}

CListBox::~CListBox()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CListBox::Create(
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   _qtd->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   _qtd->setVisible(dwStyle&WS_VISIBLE);

   return TRUE;
}

CCheckListBox::CCheckListBox(CWnd* parent)
   : CListBox(parent)
{
}

CCheckListBox::~CCheckListBox()
{
}

CListCtrl::CListCtrl(CWnd* parent)
   : CWnd(parent),
     _qtd_table(NULL),
     _qtd_list(NULL),
     m_pImageList(NULL),
     _dwStyle(0)
{
}

CListCtrl::~CListCtrl()
{
   if ( _qtd_table )
      delete _qtd_table;
   _qtd_table = NULL;
   if ( _qtd_list )
      delete _qtd_list;
   _qtd_list = NULL;
   _qt = NULL;
}

QModelIndex CListCtrl::currentIndex () const 
{ 
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      return _qtd_table->currentIndex(); 
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      return _qtd_list->currentIndex(); 
   }
}

BOOL CListCtrl::Create(
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   _dwStyle = dwStyle;
   
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   if ( (dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      if ( pParentWnd )
         _qt = new QTableWidget(pParentWnd->toQWidget());
      else
         _qt = new QTableWidget;      
      
      // Downcast to save having to do it all over the place...
      _qtd_table = dynamic_cast<QTableWidget*>(_qt);
         
      _qtd_table->setFont(QFont("MS Shell Dlg",8));
      _qtd_table->horizontalHeader()->setStretchLastSection(true);
      _qtd_table->verticalHeader()->hide();
      _qtd_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
      
      // Pass-through signals
      QObject::connect(_qtd_table,SIGNAL(itemSelectionChanged()),this,SIGNAL(itemSelectionChanged()));
      QObject::connect(_qtd_table,SIGNAL(cellClicked(int,int)),this,SIGNAL(cellClicked(int,int)));
      QObject::connect(_qtd_table,SIGNAL(cellDoubleClicked(int,int)),this,SIGNAL(cellDoubleClicked(int,int)));

      _qtd_table->verticalHeader()->setVisible(false);
      _qtd_table->horizontalHeader()->setSortIndicatorShown(true);
      if ( dwStyle&LVS_SINGLESEL )
      {
         _qtd_table->setSelectionMode(QAbstractItemView::SingleSelection);
         _qtd_table->setSelectionBehavior(QAbstractItemView::SelectRows);
      }
   //   if ( (dwStyle&LVS_SORTASCENDING)
   //        (dwStyle&LVS_SORTDESCENDING) )
   //   {
   //      _qtd_table->setSortingEnabled(true);
   //   }
      if ( dwStyle&LVS_NOCOLUMNHEADER )
      {
         _qtd_table->horizontalHeader()->setVisible(false);
      }
      if ( dwStyle&LVS_NOSORTHEADER )
      {
         _qtd_table->horizontalHeader()->setSortIndicatorShown(false);
      }
      _qtd_table->horizontalHeader()->setStretchLastSection(true);
      
      _qtd_table->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
      _qtd_table->setVisible(dwStyle&WS_VISIBLE);
   }
   else if ( (dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      if ( pParentWnd )
         _qt = new QListWidget(pParentWnd->toQWidget());
      else
         _qt = new QListWidget;      
      
      // Downcast to save having to do it all over the place...
      _qtd_list = dynamic_cast<QListWidget*>(_qt);
         
      _qtd_list->setFont(QFont("MS Shell Dlg",8));
      _qtd_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
      
      // Pass-through signals
      QObject::connect(_qtd_list,SIGNAL(itemSelectionChanged()),this,SIGNAL(itemSelectionChanged()));

      _qtd_list->setFlow(QListView::TopToBottom);
      _qtd_list->setWrapping(true);
      if ( dwStyle&LVS_SINGLESEL )
      {
         _qtd_list->setSelectionMode(QAbstractItemView::SingleSelection);
         _qtd_list->setSelectionBehavior(QAbstractItemView::SelectRows);
      }
   //   if ( (dwStyle&LVS_SORTASCENDING)
   //        (dwStyle&LVS_SORTDESCENDING) )
   //   {
   //      _qtd_list->setSortingEnabled(true);
   //   }
      
      _qtd_list->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
      _qtd_list->setVisible(dwStyle&WS_VISIBLE);
   }   

   return TRUE;
}

CImageList* CListCtrl::SetImageList(
   CImageList* pImageList,
   int nImageListType 
)
{
   CImageList* oldList = m_pImageList;
   m_pImageList = pImageList;
   return oldList;
}

LRESULT CListCtrl::SendMessage(
   UINT message,
   WPARAM wParam,
   LPARAM lParam 
)
{
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      switch ( message )
      {
      case LVM_SETEXTENDEDLISTVIEWSTYLE:
         if ( !wParam )
         {
            switch ( lParam )
            {
            case LVS_EX_FULLROWSELECT:            
               _qtd_table->setSelectionMode(QAbstractItemView::SingleSelection);
               _qtd_table->setSelectionBehavior(QAbstractItemView::SelectRows);
               break;
            case LVS_EX_CHECKBOXES:
               qDebug("LVS_EX_CHECKBOXES?");
               break;
            }
         }
         break;
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      switch ( message )
      {
      case LVM_SETEXTENDEDLISTVIEWSTYLE:
         if ( !wParam )
         {
            switch ( lParam )
            {
            case LVS_EX_FULLROWSELECT:            
               _qtd_list->setSelectionMode(QAbstractItemView::SingleSelection);
               _qtd_list->setSelectionBehavior(QAbstractItemView::SelectRows);
               break;
            case LVS_EX_CHECKBOXES:
               qDebug("LVS_EX_CHECKBOXES?");
               break;
            }
         }
         break;
      }
   }
   return 0; // CP: not sure this matters...much
}

DWORD CListCtrl::SetExtendedStyle(
   DWORD dwNewStyle 
)
{
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      if ( dwNewStyle&LVS_EX_FULLROWSELECT )
      {
         _qtd_table->setSelectionBehavior(QAbstractItemView::SelectRows);
      }
      if ( dwNewStyle&LVS_EX_GRIDLINES )
      {
         _qtd_table->setShowGrid(true);
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      if ( dwNewStyle&LVS_EX_FULLROWSELECT )
      {
         _qtd_list->setSelectionBehavior(QAbstractItemView::SelectRows);
      }
   }

   return 0; // CP: not sure this matters...much
}

UINT CListCtrl::GetSelectedCount( ) const
{
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      return _qtd_table->selectedItems().count();
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      return _qtd_list->selectedItems().count();
   }
}

int CListCtrl::GetSelectionMark( )
{
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      if ( _qtd_table->selectedItems().count() )
         return _qtd_table->selectedItems().at(0)->row();
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      if ( _qtd_list->selectedItems().count() )
         return _qtd_list->row(_qtd_list->selectedItems().at(0));
   }
   return -1;
}

int CListCtrl::GetNextItem(
   int nItem,
   int nFlags 
) const
{
   QList<QTableWidgetItem*> twis;
   QList<QListWidgetItem*> lwis;
   int item;
   int nextItemRow = -1;

   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      twis = _qtd_table->selectedItems();
      
      switch ( nFlags )
      {
      case LVNI_SELECTED:
         for ( item = 0; item < twis.count(); item++ )
         {
            if ( twis.at(item)->row() == nItem )
            {
               if ( item < (twis.count()-1) )
               {
                  nextItemRow = twis.at(item+1)->row();
               }
            }
         }
         break;
      default:
         qDebug("CListCtrl::GetNextItem called with unsupported nFlags");
         break;
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwis = _qtd_list->selectedItems();
      
      switch ( nFlags )
      {
      case LVNI_SELECTED:
         for ( item = 0; item < lwis.count(); item++ )
         {
            if ( _qtd_list->row(lwis.at(item)) == nItem )
            {
               if ( item < (lwis.count()-1) )
               {
                  nextItemRow = _qtd_list->row(lwis.at(item+1));
               }
            }
         }
         break;
      default:
         qDebug("CListCtrl::GetNextItem called with unsupported nFlags");
         break;
      }
   }
   return nextItemRow;
}

CString CListCtrl::GetItemText(
   int nItem,
   int nSubItem 
) const
{
   QTableWidgetItem* twi;
   QListWidgetItem* lwi;
   CString str;

   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      twi = _qtd_table->item(nItem,nSubItem);
      
      if ( twi )
      {
         str = twi->text();
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem);
      
      if ( lwi )
      {
         str = lwi->text();
      }
   }
   return str;
}

int CListCtrl::GetItemText(
   int nItem,
   int nSubItem,
   LPTSTR lpszText,
   int nLen 
) const
{
   QTableWidgetItem* twi;
   QListWidgetItem* lwi;
   int length = 0;

   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      twi = _qtd_table->item(nItem,nSubItem);
      
      if ( twi )
      {
#if UNICODE
         wcscpy(lpszText,(LPWSTR)twi->text().unicode());
         length = wcslen(lpszText);
#else
         strcpy(lpszText,twi->text().toAscii().constData());
         length = strlen(lpszText);
#endif
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem);
      
      if ( lwi )
      {
#if UNICODE
         wcscpy(lpszText,(LPWSTR)lwi->text().unicode());
         length = wcslen(lpszText);
#else
         strcpy(lpszText,lwi->text().toAscii().constData());
         length = strlen(lpszText);
#endif
      }
   }
   return length;
}

#if UNICODE
int CListCtrl::GetItemText(
   int nItem,
   int nSubItem,
   char* lpszText,
   int nLen 
) const
{
   QTableWidgetItem* twi;
   QListWidgetItem* lwi;
   int length = 0;
   
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      twi = _qtd_table->item(nItem,nSubItem);
      
      if ( twi )
      {
         strcpy(lpszText,twi->text().toAscii().constData());
         length = strlen(lpszText);
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem,nSubItem);
      
      if ( lwi )
      {
         strcpy(lpszText,lwi->text().toAscii().constData());
         length = strlen(lpszText);
      }
   }
   return length;
}
#endif

int CListCtrl::InsertColumn(
   int nCol,
   LPCTSTR lpszColumnHeading,
   int nFormat,
   int nWidth,
   int nSubItem
)
{   
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      _qtd_table->insertColumn(nCol);
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
//      _qtd_list->insertColumn(nCol);
   }

   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      QTableWidgetItem* twi = new QTableWidgetItem;
   
#if UNICODE
      twi->setText(QString::fromWCharArray(lpszColumnHeading));
#else
      twi->setText(lpszColumnHeading);
#endif
      
      _qtd_table->setColumnWidth(nCol,nWidth);
      _qtd_table->setHorizontalHeaderItem(nCol,twi);
      return _qtd_table->columnCount()-1;
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
//      QListWidgetItem* lwi = new QListWidgetItem;
   
//#if UNICODE
//      lwi->setText(QString::fromWCharArray(lpszColumnHeading));
//#else
//      lwi->setText(lpszColumnHeading);
//#endif
      
//      _qtd_list->setColumnWidth(nCol,nWidth);
//      return _qtd_list->columnCount()-1;
   }
   return 0;
}

int CListCtrl::InsertItem(
   int nItem,
   LPCTSTR lpszItem
)
{
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      QTableWidgetItem* twi = new QTableWidgetItem;
      
#if UNICODE
      twi->setText(QString::fromWCharArray(lpszItem));
#else
      twi->setText(lpszItem);
#endif
      
      _qtd_table->blockSignals(true);
      _qtd_table->insertRow(nItem);
      _qtd_table->setItem(nItem,0,twi);
      _qtd_table->blockSignals(false);
      _qtd_table->resizeRowToContents(nItem);
      _qtd_table->resizeColumnsToContents();
      return _qtd_table->rowCount()-1;
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      QListWidgetItem* lwi = new QListWidgetItem;
      
#if UNICODE
      lwi->setText(QString::fromWCharArray(lpszItem));
#else
      lwi->setText(lpszItem);
#endif
      
      _qtd_list->blockSignals(true);
      _qtd_list->insertItem(nItem,lwi);
      _qtd_list->blockSignals(false);
      return _qtd_list->count()-1;
   }
   return 0;
}

int CListCtrl::InsertItem(
   int nItem,
   LPCTSTR lpszItem,
   int nImage 
)
{
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      QTableWidgetItem* twi = new QTableWidgetItem;
      
#if UNICODE
      twi->setText(QString::fromWCharArray(lpszItem));
#else
      twi->setText(lpszItem);
#endif
      if ( m_pImageList )
      {
         twi->setIcon(*(QIcon*)m_pImageList->ExtractIcon(nImage));
      }
      
      _qtd_table->blockSignals(true);
      _qtd_table->insertRow(nItem);
      _qtd_table->setItem(nItem,0,twi);
      _qtd_table->blockSignals(false);
      _qtd_table->resizeRowToContents(nItem);
      _qtd_table->resizeColumnsToContents();
      return _qtd_table->rowCount()-1;
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      QListWidgetItem* lwi = new QListWidgetItem;
      
#if UNICODE
      lwi->setText(QString::fromWCharArray(lpszItem));
#else
      lwi->setText(lpszItem);
#endif
      if ( m_pImageList )
      {
         lwi->setIcon(*(QIcon*)m_pImageList->ExtractIcon(nImage));
      }      
      _qtd_list->blockSignals(true);
      _qtd_list->insertItem(nItem,lwi);
      _qtd_list->blockSignals(false);
      return _qtd_list->count()-1;
   }
   return 0;
}

int CListCtrl::SetSelectionMark(
   int iIndex 
)
{
   int selection;
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      selection = _qtd_table->selectionModel()->currentIndex().row();
      _qtd_table->selectRow(iIndex);
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      selection = _qtd_list->selectionModel()->currentIndex().row();
      _qtd_list->setCurrentRow(iIndex);
   }
   return selection;
}

BOOL CListCtrl::SetCheck(
   int nItem,
   BOOL fCheck
)
{
   QTableWidgetItem* twi;  
   QListWidgetItem* lwi;  
   bool add = false;

   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      twi = _qtd_table->item(nItem,0);
      
      if ( !twi )
      {
         add = true;
         twi = new QTableWidgetItem;
      }
      
      twi->setCheckState(fCheck?Qt::Checked:Qt::Unchecked);
   
      if ( add )
         _qtd_table->setItem(nItem,0,twi);
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem);
      
      if ( !twi )
      {
         add = true;
         lwi = new QListWidgetItem;
      }
      
      lwi->setCheckState(fCheck?Qt::Checked:Qt::Unchecked);
   
      if ( add )
         _qtd_list->insertItem(nItem,lwi);
   }
   
   return TRUE;
}

BOOL CListCtrl::GetCheck(
   int nItem 
) const
{
   QTableWidgetItem* twi;  
   QListWidgetItem* lwi;  
   
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      twi = _qtd_table->item(nItem,0);
      
      if ( !twi )
      {
         return twi->checkState()==Qt::Checked;
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem);
      
      if ( !lwi )
      {
         return lwi->checkState()==Qt::Checked;
      }
   }
   return FALSE;
}

BOOL CListCtrl::SetItemText(
   int nItem,
   int nSubItem,
   char* lpszText 
)
{
   QTableWidgetItem* twi;
   QListWidgetItem* lwi;
   bool add = false;
   
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      twi = _qtd_table->item(nItem,nSubItem);
      
      if ( !twi )
      {
         add = true;
         twi = new QTableWidgetItem;
      }
   
      twi->setText(lpszText);
      
      if ( add )
         _qtd_table->setItem(nItem,nSubItem,twi);
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem);
      
      if ( !lwi )
      {
         add = true;
         lwi = new QListWidgetItem;
      }
   
      lwi->setText(lpszText);
      
      if ( add )
         _qtd_list->insertItem(nItem,lwi);
   }

   return TRUE;
}

BOOL CListCtrl::SetItemText(
   int nItem,
   int nSubItem,
   LPCTSTR lpszText 
)
{
   QTableWidgetItem* twi;
   QListWidgetItem* lwi;
   bool add = false;
   
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      twi = _qtd_table->item(nItem,nSubItem);
      
      if ( !twi )
      {
         add = true;
         twi = new QTableWidgetItem;
      }
      
#if UNICODE
      twi->setText(QString::fromWCharArray(lpszText));
#else
      twi->setText(lpszText);
#endif
         
      if ( add )
         _qtd_table->setItem(nItem,nSubItem,twi);
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem);
      
      if ( !lwi )
      {
         add = true;
         lwi = new QListWidgetItem;
      }
      
#if UNICODE
      lwi->setText(QString::fromWCharArray(lpszText));
#else
      lwi->setText(lpszText);
#endif
         
      if ( add )
         _qtd_list->insertItem(nItem,lwi);
   }
   
   return TRUE;
}

BOOL CListCtrl::SetItemState(
   int nItem,
   UINT nState,
   UINT nMask 
)
{
   nState &= nMask;
   
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      if ( nState&LVIS_SELECTED )
      {
         _qtd_table->selectRow(nItem);
      }
      if ( nState&LVIS_FOCUSED )
      {
         _qtd_table->selectRow(nItem);
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      if ( nState&LVIS_SELECTED )
      {
         _qtd_list->setCurrentRow(nItem);
      }
      if ( nState&LVIS_FOCUSED )
      {
         _qtd_list->setCurrentRow(nItem);
      }
   }
   
   return TRUE;
}

int CListCtrl::FindItem(
   LVFINDINFO* pFindInfo,
   int nStart
) const
{
   int index = -1;
   Qt::MatchFlags flags;
   if ( pFindInfo->flags&LVFI_PARTIAL )
      flags |= Qt::MatchStartsWith;
   else
      flags |= Qt::MatchExactly;
   if ( pFindInfo->flags&LVFI_WRAP )
      flags |= Qt::MatchWrap;
   
   QList<QTableWidgetItem*> twis;
   QList<QListWidgetItem*> lwis;
   
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
#if UNICODE
      twis = _qtd_table->findItems(QString::fromWCharArray(pFindInfo->psz),flags);
#else
      twis = _qtd_table->findItems(pFindInfo->psz,flags);
#endif
      
      if ( twis.count() )
      {
         foreach ( QTableWidgetItem* twi, twis )
         {
            if ( twi->row() > nStart )
            {
               index = twi->row();
               break;
            }
         }
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
#if UNICODE
      lwis = _qtd_list->findItems(QString::fromWCharArray(pFindInfo->psz),flags);
#else
      lwis = _qtd_list->findItems(pFindInfo->psz,flags);
#endif
      
      if ( lwis.count() )
      {
         foreach ( QListWidgetItem* lwi, lwis )
         {
            if ( _qtd_list->row(lwi) > nStart )
            {
               index = _qtd_list->row(lwi);
               break;
            }
         }
      }
   }
   return index;
}

BOOL CListCtrl::SetBkColor(
   COLORREF cr 
)
{
   QString color;
   color.sprintf("#%02X%02X%02X",GetRValue(cr),GetGValue(cr),GetBValue(cr));
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      _qtd_table->setStyleSheet(_qtd_table->styleSheet()+"QTableWidget { background: "+color+" }");
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      _qtd_list->setStyleSheet(_qtd_list->styleSheet()+"QListWidget { background: "+color+" }");
   }
   return TRUE;
}

BOOL CListCtrl::SetTextBkColor(
   COLORREF cr 
)
{
   QString color;
   color.sprintf("#%02X%02X%02X",GetRValue(cr),GetGValue(cr),GetBValue(cr));
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      _qtd_table->setStyleSheet(_qtd_table->styleSheet()+"QTableWidget { background: "+color+" }");
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      _qtd_list->setStyleSheet(_qtd_list->styleSheet()+"QListWidget { background: "+color+" }");
   }
   return TRUE;
}

BOOL CListCtrl::SetTextColor(
   COLORREF cr 
)
{
   QString color;
   color.sprintf("#%02X%02X%02X",GetRValue(cr),GetGValue(cr),GetBValue(cr));
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      _qtd_table->setStyleSheet(_qtd_table->styleSheet()+"QTableWidget { color: "+color+" }");
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      _qtd_list->setStyleSheet(_qtd_list->styleSheet()+"QListWidget { color: "+color+" }");
   }
   return TRUE;
}

BOOL CListCtrl::DeleteAllItems()
{
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      _qtd_table->blockSignals(true);
      _qtd_table->clearContents();
      _qtd_table->blockSignals(false);
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      _qtd_list->blockSignals(true);
      _qtd_list->clear();
      _qtd_list->blockSignals(false);
   }
   return TRUE;
}

BOOL CListCtrl::DeleteItem(
   int nItem 
)
{
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      if ( nItem < _qtd_list->count() )
      {
         _qtd_table->removeRow(nItem);
         return TRUE;
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      if ( nItem < _qtd_list->count() )
      {
         QListWidgetItem* lwi = _qtd_list->takeItem(nItem);
         delete lwi;
         return TRUE;
      }
   }
   return FALSE;
}

int CListCtrl::GetItemCount( ) const
{
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      return _qtd_table->rowCount();
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      return _qtd_list->count();
   }
}

DWORD_PTR CListCtrl::GetItemData( 
   int nItem  
) const
{
   QTableWidgetItem* twi;
   QListWidgetItem* lwi;
   
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      twi = _qtd_table->item(nItem,0);
      
      if ( twi )
      {
         return twi->data(Qt::UserRole).toInt();
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem);
      
      if ( lwi )
      {
         return lwi->data(Qt::UserRole).toInt();
      }
   }
}

BOOL CListCtrl::SetItemData(
   int nItem,
      DWORD_PTR dwData 
)
{
   QTableWidgetItem* twi;
   QListWidgetItem* lwi;
   
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      twi = _qtd_table->item(nItem,0);
      
      if ( twi )
      {
         twi->setData(Qt::UserRole,QVariant((int)dwData));
         return TRUE; 
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem);
      
      if ( lwi )
      {
         lwi->setData(Qt::UserRole,QVariant((int)dwData));
         return TRUE; 
      }
   }
   return TRUE;
}

BOOL CListCtrl::EnsureVisible(
   int nItem,
   BOOL bPartialOK 
)
{
   QTableWidgetItem* twi;
   QListWidgetItem* lwi;
   
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      twi = _qtd_table->item(nItem,0);
      
      if ( twi )
      {
         _qtd_table->scrollToItem(twi);
         return TRUE;
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem);
      
      if ( lwi )
      {
         _qtd_list->scrollToItem(lwi);
         return TRUE;
      }
   }
   return FALSE;
}

CTreeCtrl::CTreeCtrl(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   _qt = new QTreeWidget(parent->toQWidget());
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QTreeWidget*>(_qt);
      
   _qtd->setFont(QFont("MS Shell Dlg",8));
   _qtd->setEditTriggers(QAbstractItemView::NoEditTriggers);
   _qtd->setHeaderHidden(true);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(itemSelectionChanged()),this,SIGNAL(itemSelectionChanged()));
   QObject::connect(_qtd,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SIGNAL(itemClicked(QTreeWidgetItem*,int)));
   QObject::connect(_qtd,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)));
}

CTreeCtrl::~CTreeCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CTreeCtrl::Create(
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   if ( dwStyle&TVS_LINESATROOT )
   {
      _qtd->setRootIsDecorated(true);
   }
   if ( dwStyle&TVS_HASLINES )
   {
      _qtd->setLineWidth(1);
   }
   
   _qtd->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   _qtd->setVisible(dwStyle&WS_VISIBLE);

   return TRUE;
}

HTREEITEM CTreeCtrl::InsertItem(
   LPCTSTR lpszItem,
   HTREEITEM hParent,
   HTREEITEM hInsertAfter 
)
{
   QTreeWidgetItem* twi = new QTreeWidgetItem;
   QTreeWidgetItem* parent = hParent;
#if UNICODE
   twi->setText(0,QString::fromWCharArray(lpszItem));
#else
   twi->setText(0,lpszItem);
#endif
   _qtd->blockSignals(true);
   if ( hInsertAfter != TVI_LAST )
   {
      qDebug("CTreeCtrl::InsertItem !TVI_LAST not supported.");
   }
   if ( hParent == TVI_ROOT )
   {      
      _qtd->insertTopLevelItem(_qtd->topLevelItemCount(),twi);
   }
   else
   {
      parent->insertChild(parent->childCount(),twi);
   }
   _qtd->blockSignals(false);
   return twi;
}

BOOL CTreeCtrl::SortChildren(
   HTREEITEM hItem 
)
{
   QTreeWidgetItem* twi = hItem;
   if ( twi )
   {
      twi->sortChildren(0,Qt::AscendingOrder);
      return TRUE;
   }
   return FALSE;
}

HTREEITEM CTreeCtrl::GetRootItem( ) const
{
   return _qtd->topLevelItem(0);
}

HTREEITEM CTreeCtrl::GetNextItem(
   HTREEITEM hItem,
   UINT nCode 
) const
{
   QTreeWidgetItem* twi = hItem;
   QTreeWidgetItem* parent;
   if ( twi )
   {
      switch ( nCode )
      {
      case TVGN_CARET:
      case TVGN_DROPHILITE:
      case TVGN_FIRSTVISIBLE:
      case TVGN_LASTVISIBLE:
      case TVGN_NEXTVISIBLE:
      case TVGN_PREVIOUSVISIBLE:
         qDebug("CTreeCtrl::GetNextItem nCode(%d) not supported",nCode);
         break;
      case TVGN_CHILD:
         return twi->child(0);
         break;
      case TVGN_NEXT:
         return _qtd->itemBelow(twi);
         break;
      case TVGN_PARENT:
         return twi->parent();
         break;
      case TVGN_PREVIOUS:
         return _qtd->itemAbove(twi);
         break;
      case TVGN_ROOT:
         do
         {
            parent = twi;
            twi = twi->parent();
         } while ( twi );
         return parent;
         break;
      }
   }
   return NULL;
}

HTREEITEM CTreeCtrl::GetParentItem(
   HTREEITEM hItem 
) const
{
   QTreeWidgetItem* twi = hItem;
   return twi->parent();
}

HTREEITEM CTreeCtrl::GetSelectedItem( ) const
{
   if ( _qtd->selectedItems().count() )
      return _qtd->selectedItems().at(0);
   else
      return 0;
}

BOOL CTreeCtrl::ItemHasChildren(
   HTREEITEM hItem 
) const
{
   QTreeWidgetItem* twi = hItem;
   if ( twi )
   {
      return twi->childCount()!=0;
   }
   return FALSE;
}

DWORD_PTR CTreeCtrl::GetItemData(
   HTREEITEM hItem 
) const
{
   QTreeWidgetItem* twi = hItem;
   if ( twi )
   {
      return twi->data(0,Qt::UserRole).toInt();
   }
   return -1;
}

BOOL CTreeCtrl::SetItemData(
   HTREEITEM hItem,
   DWORD_PTR dwData 
)
{
   QTreeWidgetItem* twi = hItem;
   if ( twi )
   {
      twi->setData(0,Qt::UserRole,QVariant((int)dwData));
      return TRUE;
   }
   return FALSE;
}

BOOL CTreeCtrl::DeleteItem(
   HTREEITEM hItem 
)
{
   QTreeWidgetItem* twi = hItem;
   if ( twi )
   {
      QTreeWidgetItem* parent = twi->parent();
      if ( parent )
      {
         parent->removeChild(twi);
      }
      else
      {
         _qtd->removeItemWidget(twi,0);
      }
      return TRUE;
   }
   return FALSE;
}

BOOL CTreeCtrl::Expand(
   HTREEITEM hItem,
   UINT nCode 
)
{
   QTreeWidgetItem* twi = hItem;
   if ( twi )
   {
      switch ( nCode )
      {
      case TVE_COLLAPSE:
         _qtd->collapseItem(twi);
         break;
      case TVE_COLLAPSERESET:
         _qtd->collapseItem(twi);
         break;
      case TVE_EXPAND:
         _qtd->expandItem(twi);
         break;
      case TVE_TOGGLE:
         if ( twi->isExpanded() )
            _qtd->collapseItem(twi);
         else
            _qtd->expandItem(twi);
         break;
      }
   }
   return FALSE;
}

CString CTreeCtrl::GetItemText(
   HTREEITEM hItem 
) const
{
   QTreeWidgetItem* twi = hItem;
   if ( twi )
   {
      return twi->text(0);
   }
   return CString();
}

CScrollBar::CScrollBar(CWnd *parent)
   : CWnd(parent),
     _orient(Qt::Vertical)
{
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   if ( parent )
      _qt = new QScrollBar(parent->toQWidget());
   else
      _qt = new QScrollBar;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QScrollBar*>(_qt);   
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(actionTriggered(int)),this,SIGNAL(actionTriggered(int)));
}

CScrollBar::~CScrollBar()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CScrollBar::Create(
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   _qtd->setOrientation(_orient);

   QRect myRect(QPoint(rect.left,rect.top),QPoint(rect.right,rect.bottom));
   _qtd->setParent(pParentWnd->toQWidget());
   if ( dwStyle&SBS_VERT )
   {
      _qtd->setOrientation(Qt::Vertical);
      if ( dwStyle&SBS_RIGHTALIGN )
      {
         myRect.setLeft(myRect.right() - _qtd->sizeHint().width());
      }
      else if ( dwStyle&SBS_LEFTALIGN )
      {
         myRect.setRight(myRect.left() + _qtd->sizeHint().width());
      }
   }
   else
   {
      // CP: SBS_VERT is 1.  SBS_HORZ is 0.
      _qtd->setOrientation(Qt::Horizontal);
      if ( dwStyle&SBS_BOTTOMALIGN )
      {
         myRect.setTop(myRect.bottom() - _qtd->sizeHint().height());
      }
      else if ( dwStyle&SBS_TOPALIGN )
      {
         myRect.setBottom(myRect.top() + _qtd->sizeHint().height());
      }
   }
   _qtd->setGeometry(myRect);
   _qtd->setVisible(dwStyle&WS_VISIBLE);
      
   return TRUE;
}

BOOL CScrollBar::SetScrollInfo(
   LPSCROLLINFO lpScrollInfo,
   BOOL bRedraw 
)
{
   if ( lpScrollInfo->fMask&SIF_RANGE )
   {
      _qtd->setMinimum(lpScrollInfo->nMin);
      _qtd->setMaximum(lpScrollInfo->nMax);
   }
   if ( lpScrollInfo->fMask&SIF_POS )
   {
      _qtd->setValue(lpScrollInfo->nPos);
   }
   if ( lpScrollInfo->fMask&SIF_TRACKPOS )
   {
      _qtd->setValue(lpScrollInfo->nTrackPos);
   }
   if ( lpScrollInfo->fMask&SIF_PAGE )
   {
      _qtd->setPageStep(lpScrollInfo->nPage);
   }
   return TRUE;
}

int CScrollBar::SetScrollPos(
   int nPos,
   BOOL bRedraw 
)
{
   int pos = _qtd->value();
   _qtd->setValue(nPos);
   return pos;
}

void CScrollBar::SetScrollRange(
   int nMinPos,
   int nMaxPos,
   BOOL bRedraw
)
{
   _qtd->setMinimum(nMinPos);
   _qtd->setMaximum(nMaxPos);
}   

void CScrollBar::ShowScrollBar(
   BOOL bShow
)
{
   _qtd->setVisible(bShow);
}   

BOOL CScrollBar::EnableScrollBar(
   UINT nArrowFlags
)
{
   _qtd->setEnabled(nArrowFlags==ESB_ENABLE_BOTH?true:false);
   return 1;
}

BOOL CCmdTarget::OnCmdMsg(
   UINT nID,
   int nCode,
   void* pExtra,
   AFX_CMDHANDLERINFO* pHandlerInfo 
)
{
   return TRUE;
}

CWnd* CWnd::focusWnd = NULL;
CFrameWnd* CWnd::m_pFrameWnd = NULL;

CWnd::CWnd(CWnd *parent) 
   : m_pParentWnd(parent),
     mfcVerticalScrollBar(NULL),
     mfcHorizontalScrollBar(NULL),
     m_hWnd((HWND)NULL),
     _grid(NULL),
     _myDC(NULL)
{
   if ( parent )
   {
      _qt = new QFrame(parent->toQWidget());
   }
   else
   {
      _qt = new QFrame;
   }
   _grid = new QGridLayout;
   _grid->setContentsMargins(0,0,0,0);
   _grid->setSpacing(0);
   _qt->setLayout(_grid);

   _myDC = new CDC(this);
   _myDC->doFlush(false);
   
   _qt->setMouseTracking(true);
   _qt->installEventFilter(this);
   _qt->setFont(QFont("MS Shell Dlg",8));
   
   _qtd = dynamic_cast<QFrame*>(_qt);
}

CWnd::~CWnd()
{
   if ( mfcVerticalScrollBar )
      delete mfcVerticalScrollBar;
   if ( mfcHorizontalScrollBar )
      delete mfcHorizontalScrollBar;
   mfcVerticalScrollBar = NULL;
   mfcHorizontalScrollBar = NULL;

   delete _myDC;

   if ( _qt )
      delete _qt;
   _qt = NULL;   
   _qtd = NULL;
//   if ( _grid )
//      delete _grid;
}

void CWnd::SetOwner(
   CWnd* pOwnerWnd 
)
{
}

BOOL CWnd::EnableToolTips(
   BOOL bEnable
)
{
   // nothing to do here...
   return TRUE;
}

CDC* CWnd::GetDC() 
{ 
   return _myDC; 
}

void CWnd::ReleaseDC(CDC* pDC) 
{ 
}

LRESULT CWnd::SendMessage(
   UINT message,
   WPARAM wParam,
   LPARAM lParam 
)
{
   return 0;
}

void CWnd::SendMessageToDescendants(
   UINT message,
   WPARAM wParam,
   LPARAM lParam,
   BOOL bDeep,
   BOOL bOnlyPerm 
)
{
   foreach ( CWnd* pWnd, mfcToQtWidget )
   {
      pWnd->SendMessage(message,wParam,lParam);
   }
}

void CWnd::subclassWidget(int nID,CWnd* widget)
{
   mfcToQtWidget.remove(nID);
   mfcToQtWidget.insert(nID,widget);
}

bool CWnd::eventFilter(QObject *object, QEvent *event)
{
   if ( event->type() == QEvent::Show )
   {
      showEvent(dynamic_cast<QShowEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::ShowToParent )
   {
      showEvent(dynamic_cast<QShowEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::Hide )
   {
      hideEvent(dynamic_cast<QHideEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::Move )
   {
      moveEvent(dynamic_cast<QMoveEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::Paint )
   {
      paintEvent(dynamic_cast<QPaintEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::FocusIn )
   {
      focusInEvent(dynamic_cast<QFocusEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::FocusOut )
   {
      focusOutEvent(dynamic_cast<QFocusEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::MouseButtonPress )
   {
      mousePressEvent(dynamic_cast<QMouseEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::MouseButtonRelease )
   {
      mouseReleaseEvent(dynamic_cast<QMouseEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::MouseButtonDblClick )
   {
      mouseDoubleClickEvent(dynamic_cast<QMouseEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::MouseMove )
   {
      mouseMoveEvent(dynamic_cast<QMouseEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::Wheel )
   {
      wheelEvent(dynamic_cast<QWheelEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::Resize )
   {
      resizeEvent(dynamic_cast<QResizeEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::KeyPress )
   {
      keyPressEvent(dynamic_cast<QKeyEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::KeyRelease )
   {
      keyReleaseEvent(dynamic_cast<QKeyEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::ContextMenu )
   {
      contextMenuEvent(dynamic_cast<QContextMenuEvent*>(event));
      return true;
   }
//   qDebug("eventFilter: unhandled %d object %s", event->type(), object->objectName().toAscii().constData());
   return false;
}

BOOL CWnd::IsWindowVisible( ) const
{
   return _qt->isVisible();
}

BOOL CWnd::EnableWindow(
   BOOL bEnable
)
{
   BOOL state = _qt->isEnabled();
   _qt->setEnabled(bEnable);
   return state;
}

BOOL CWnd::CreateEx(
   DWORD dwExStyle,
   LPCTSTR lpszClassName,
   LPCTSTR lpszWindowName,
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID,
   LPVOID lpParam
)
{
   CREATESTRUCT createStruct;

   m_hWnd = (HWND)this;
   _id = nID;
   
   createStruct.dwExStyle = dwExStyle;
   createStruct.style = dwStyle;
   createStruct.x = rect.left;
   createStruct.y = rect.top;
   createStruct.cx = rect.right-rect.left;
   createStruct.cy = rect.bottom-rect.top;
   // For widgets that aren't added to a layout...
   m_pParentWnd = pParentWnd;
   if ( pParentWnd )
      _qt->setParent(pParentWnd->toQWidget());
   else
      _qt->setParent(NULL);
   PreCreateWindow(createStruct);
   _qtd->setLineWidth(0);
   _qtd->setMidLineWidth(0);
   if ( createStruct.dwExStyle&WS_EX_STATICEDGE )
   {
      _qtd->setFrameShape(QFrame::StyledPanel);
      _qtd->setFrameShadow(QFrame::Sunken);
      _qtd->setLineWidth(1);
      createStruct.cx += 2;
      createStruct.cy += 2;
   }
   if ( createStruct.style&SS_SUNKEN )
   {
      _qtd->setFrameShape(QFrame::StyledPanel);
      _qtd->setFrameShadow(QFrame::Sunken);
      _qtd->setLineWidth(1);
      createStruct.cx += 2;
      createStruct.cy += 2;
   }
   if ( createStruct.style&WS_VSCROLL )
   {
      mfcVerticalScrollBar = new CScrollBar(this);
      mfcVerticalScrollBar->Create(SBS_VERT | SBS_RIGHTALIGN | WS_CHILD | WS_VISIBLE, rect, this, 0);
      _grid->addWidget(mfcVerticalScrollBar->toQWidget(),0,1);
   }
   if ( createStruct.style&WS_HSCROLL )
   {
      mfcHorizontalScrollBar = new CScrollBar(this);
      mfcHorizontalScrollBar->Create(SBS_HORZ | SBS_BOTTOMALIGN | WS_CHILD | WS_VISIBLE, rect, this, 0);
      _grid->addWidget(mfcHorizontalScrollBar->toQWidget(),1,0);
   }
   _qt->setGeometry(createStruct.x,createStruct.y,createStruct.cx,createStruct.cy);
   _qt->setFixedSize(createStruct.cx,createStruct.cy);
   OnCreate(&createStruct);
   return TRUE;
}

int CWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{   
   return 0;
}

void CWnd::OnDestroy( )
{
}

void CWnd::UpdateDialogControls(
   CCmdTarget* pTarget,
   BOOL bDisableIfNoHndler 
)
{
}

void CWnd::RepositionBars(
   UINT nIDFirst,
   UINT nIDLast,
   UINT nIDLeftOver,
   UINT nFlag,
   LPRECT lpRectParam,
   LPCRECT lpRectClient,
   BOOL bStretch 
)
{
   AFX_SIZEPARENTPARAMS layout;
   CWnd* pWndExtra = GetDlgItem(nIDLeftOver);
   
   layout.bStretch = bStretch;
	layout.sizeTotal.cx = layout.sizeTotal.cy = 0;
//   if ( lpRectParam )
//   {
//      layout.rect = *lpRectParam;
//   }
//   else
   {
      GetClientRect(&layout.rect);
   }
   foreach ( CWnd* pWnd, mfcToQtWidget )
   {
      if ( pWnd != pWndExtra )
      {
         pWnd->SendMessage(WM_SIZEPARENT,0,(LPARAM)&layout);
      }
   }
   // Hack to resize the view...
   if ( pWndExtra )
      pWndExtra->MoveWindow(&layout.rect);
}

void CWnd::SetFont(
   CFont* pFont,
   BOOL bRedraw
)
{
   _qt->setFont((QFont)*pFont);
}

void CWnd::MoveWindow(int x, int y, int cx, int cy)
{
   MoveWindow(CRect(CPoint(x,y),CSize(cx,cy)));   
}

void CWnd::MoveWindow(LPCRECT lpRect, BOOL bRepaint)
{
   setGeometry(lpRect->left,lpRect->top,(lpRect->right-lpRect->left)+1,(lpRect->bottom-lpRect->top)+1);
   setFixedSize((lpRect->right-lpRect->left)+1,(lpRect->bottom-lpRect->top)+1);
   if ( bRepaint )
      repaint();
}

void CWnd::DragAcceptFiles(
   BOOL bAccept 
)
{
   _qtd->setAcceptDrops(bAccept);
}

BOOL CWnd::PostMessage(
   UINT message,
   WPARAM wParam,
   LPARAM lParam
)
{
   MSG msg;
   msg.message = message;
   msg.wParam = wParam;
   msg.lParam = lParam;
   
   BOOL handled = PreTranslateMessage(&msg);
   return handled;
}

CWnd* CWnd::GetDlgItem(
   int nID 
) const
{
   return mfcToQtWidget.value(nID);
}

void CWnd::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned
)
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nID));
   if ( pUIE )
      pUIE->SetDlgItemInt(nID,nValue,bSigned);
}

UINT CWnd::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nID));
   if ( pUIE )
      return pUIE->GetDlgItemInt(nID,lpTrans,bSigned);
   else
      return 0;
}

void CWnd::SetDlgItemText(
   int nID,
   LPCTSTR lpszString 
)
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nID));
   if ( pUIE )
      pUIE->SetDlgItemText(nID,lpszString);
}

int CWnd::GetDlgItemText(
   int nID,
   CString& rString 
) const
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nID));
   if ( pUIE )
      return pUIE->GetDlgItemText(nID,rString);
   else
      return 0;
}

int CWnd::GetDlgItemText(
   int nID,
   LPTSTR lpStr,
   int nMaxCount 
) const
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nID));
   if ( pUIE )
      return pUIE->GetDlgItemText(nID,lpStr,nMaxCount);
   else
      return 0;
}

void CWnd::CheckDlgButton( 
   int nIDButton, 
   UINT nCheck  
)
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nIDButton));
   if ( pUIE )
      pUIE->CheckDlgButton(nIDButton,nCheck);
}

UINT CWnd::IsDlgButtonChecked(
   int nIDButton
) const
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nIDButton));
   if ( pUIE )
      return pUIE->IsDlgButtonChecked(nIDButton);
   else
      return 0;
}

BOOL CWnd::SubclassDlgItem(
   UINT nID,
   CWnd* pParent 
)
{
   CWnd* pWndSrc = pParent->GetDlgItem(nID);

   if ( pWndSrc )
   {
      SetParent(pParent);
      setParent(pParent->toQWidget());
      setGeometry(pWndSrc->geometry());
      pParent->subclassWidget(nID,this);
      pWndSrc->setParent(NULL);
      delete pWndSrc;
      return TRUE;
   }
   return FALSE;
}

void CWnd::MapWindowPoints(
   CWnd* pwndTo,
   LPRECT lpRect 
) const
{
}

void CWnd::MapWindowPoints(
   CWnd* pwndTo,
   LPPOINT lpPoint,
   UINT nCount 
) const
{
}

CScrollBar* CWnd::GetScrollBarCtrl(
   int nBar 
) const
{
   switch ( nBar )
   {
   case SB_VERT:
      return mfcVerticalScrollBar;
      break;
   case SB_HORZ:
      return mfcHorizontalScrollBar;
      break;
   }
   return NULL;
}

BOOL CWnd::SetScrollInfo(
   int nBar,
   LPSCROLLINFO lpScrollInfo,
   BOOL bRedraw
)
{
   if ( ((nBar==SB_VERT) && (!mfcVerticalScrollBar)) ||
        ((nBar==SB_HORZ) && (!mfcHorizontalScrollBar)) )
   {
      return FALSE;
   }
   if ( lpScrollInfo->fMask&SIF_RANGE )
   {
      switch ( nBar )
      {
      case SB_HORZ:
         mfcHorizontalScrollBar->setMinimum(lpScrollInfo->nMin);
         mfcHorizontalScrollBar->setMaximum(lpScrollInfo->nMax);
         break;
      case SB_VERT:
         mfcVerticalScrollBar->setMinimum(lpScrollInfo->nMin);
         mfcVerticalScrollBar->setMaximum(lpScrollInfo->nMax);
         break;
      }
   }
   if ( lpScrollInfo->fMask&SIF_POS )
   {
      switch ( nBar )
      {
      case SB_HORZ:
         mfcHorizontalScrollBar->setValue(lpScrollInfo->nPos);
         break;
      case SB_VERT:
         mfcVerticalScrollBar->setValue(lpScrollInfo->nPos);
         break;
      }
   }
   if ( lpScrollInfo->fMask&SIF_TRACKPOS )
   {
      switch ( nBar )
      {
      case SB_HORZ:
         mfcHorizontalScrollBar->setValue(lpScrollInfo->nTrackPos);
         break;
      case SB_VERT:
         mfcVerticalScrollBar->setValue(lpScrollInfo->nTrackPos);
         break;
      }
   }
   if ( lpScrollInfo->fMask&SIF_PAGE )
   {
      switch ( nBar )
      {
      case SB_HORZ:
         mfcHorizontalScrollBar->setPageStep(lpScrollInfo->nPage);
         break;
      case SB_VERT:
         mfcVerticalScrollBar->setPageStep(lpScrollInfo->nPage);
         break;
      }
   }
   return TRUE;
}

void CWnd::SetScrollRange(
   int nBar,
   int nMinPos,
   int nMaxPos,
   BOOL bRedraw 
)
{
   switch ( nBar )
   {
   case SB_HORZ:
      mfcHorizontalScrollBar->setMinimum(nMinPos);
      mfcHorizontalScrollBar->setMaximum(nMaxPos);
      break;
   case SB_VERT:
      mfcVerticalScrollBar->setMinimum(nMinPos);
      mfcVerticalScrollBar->setMaximum(nMaxPos);
      break;
   }
}

int CWnd::SetScrollPos(
   int nBar,
   int nPos,
   BOOL bRedraw
)
{
   int pos;
   switch ( nBar )
   {
   case SB_HORZ:
      pos = mfcHorizontalScrollBar->sliderPosition();
      mfcHorizontalScrollBar->setValue(nPos);
      break;
   case SB_VERT:
      pos = mfcVerticalScrollBar->sliderPosition();
      mfcVerticalScrollBar->setValue(nPos);
      break;
   }
   return pos;
}

UINT CWnd::SetTimer(UINT id, UINT interval, void*)
{
   if ( mfcToQtTimer.contains((int)id) )
   {
      killTimer(mfcToQtTimer.value((int)id));
      qtToMfcTimer.remove(mfcToQtTimer.value((int)id));
      mfcToQtTimer.remove((int)id);
   }
   int qtId = startTimer(interval);
   mfcToQtTimer.insert((int)id,qtId);
   qtToMfcTimer.insert(qtId,(int)id);
   return (UINT)id;
}

void CWnd::KillTimer(UINT id)
{
   if ( mfcToQtTimer.contains((int)id) )
   {
      killTimer(mfcToQtTimer.value((int)id));
      qtToMfcTimer.remove(mfcToQtTimer.value((int)id));
      mfcToQtTimer.remove((int)id);
   }
}

int CWnd::GetWindowTextLength( ) const
{
   return _qt->windowTitle().length();
}

void CWnd::GetWindowText(
   CString& rString 
) const
{
   rString = _qt->windowTitle();
}

int CWnd::GetWindowText(
   LPTSTR lpszStringBuf,
   int nMaxCount 
) const
{
#if UNICODE
   wcsncpy(lpszStringBuf,(LPTSTR)_qt->windowTitle().unicode(),nMaxCount);
   return wcslen(lpszStringBuf);
#else
   strncpy(lpszStringBuf,(LPTSTR)_qt->windowTitle().toAscii().constData(),nMaxCount);
   return strlen(lpszStringBuf);
#endif
}

void CWnd::SetWindowText(
   LPCTSTR lpszString 
)
{
}

void CWnd::GetWindowRect(
   LPRECT lpRect 
) const
{
   lpRect->left = geometry().left();
   lpRect->right = geometry().right();
   lpRect->top = geometry().top();
   lpRect->bottom = geometry().bottom();
}

void CWnd::GetClientRect(
   LPRECT lpRect 
) const
{
   lpRect->left = 0;
   lpRect->right = rect().right();
   lpRect->top = 0;
   lpRect->bottom = rect().bottom();
   if ( _grid )
   {
      if ( _grid->columnCount() > 1 )
      {
         lpRect->right -= GetSystemMetrics(SM_CXVSCROLL);
      }
      if ( _grid->rowCount() > 1 )
      {
         lpRect->bottom -= GetSystemMetrics(SM_CYHSCROLL);
      }
   }
}

void CWnd::ShowWindow(int code)
{
   switch ( code )
   {
   case SW_SHOW:
      foreach ( CWnd* pWnd, mfcToQtWidget ) pWnd->blockSignals(false);
      setVisible(true);
      break;
   case SW_HIDE:
      foreach ( CWnd* pWnd, mfcToQtWidget ) pWnd->blockSignals(true);
      setVisible(false);
      break;
   }
}

CFrameWnd::CFrameWnd(CWnd *parent)
   : CWnd(parent),
     m_pView(NULL),
     m_pDocument(NULL),
     m_bInRecalcLayout(FALSE)
{
   m_pFrameWnd = this;
   ptrToTheApp->m_pMainWnd = this;
   
   QWidget* centralWidget = _qt;
   QGridLayout* gridLayout = _grid;
   
   gridLayout->setSpacing(0);
   gridLayout->setContentsMargins(0, 0, 0, 0);
   gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
   
   cbrsBottom = new QVBoxLayout();
   cbrsBottom->setSpacing(0);
   cbrsBottom->setContentsMargins(0,0,0,0);
   cbrsBottom->setObjectName(QString::fromUtf8("cbrsBottom"));

   gridLayout->addLayout(cbrsBottom, 2, 0, 1, -1);
   gridLayout->setRowMinimumHeight(2,0);
   gridLayout->setRowStretch(2,0);

   cbrsTop = new QVBoxLayout();
   cbrsTop->setSpacing(0);
   cbrsTop->setContentsMargins(0,0,0,0);
   cbrsTop->setObjectName(QString::fromUtf8("cbrsTop"));

   gridLayout->addLayout(cbrsTop, 0, 0, 1, -1);
   gridLayout->setRowMinimumHeight(0,0);
   gridLayout->setRowStretch(0,0);

   cbrsLeft = new QHBoxLayout();
   cbrsLeft->setSpacing(0);
   cbrsLeft->setContentsMargins(0,0,0,0);
   cbrsLeft->setObjectName(QString::fromUtf8("cbrsLeft"));

   gridLayout->addLayout(cbrsLeft, 1, 0, 1, 1);
   gridLayout->setColumnMinimumWidth(0,0);
   gridLayout->setColumnStretch(0,0);

   cbrsRight = new QHBoxLayout();
   cbrsRight->setSpacing(0);
   cbrsRight->setContentsMargins(0,0,0,0);
   cbrsRight->setObjectName(QString::fromUtf8("cbrsRight"));

   gridLayout->addLayout(cbrsRight, 1, 2, 1, 1);
   gridLayout->setColumnMinimumWidth(2,0);
   gridLayout->setColumnStretch(2,0);
   
   realCentralWidget = new QWidget;
//   realCentralWidget->setSpacing(0);
//   realCentralWidget->setContentsMargins(0,0,0,0);
   realCentralWidget->setObjectName(QString::fromUtf8("realCentralWidget"));

   gridLayout->addWidget(realCentralWidget, 1, 1, 1, 1);

   gridLayout->setRowStretch(1,1);
   gridLayout->setColumnStretch(1,1);
   
   centralWidget->setLayout(gridLayout);
   
   m_pMenuBar = new QMenuBar;
//   m_pMenuBar->addMenu(qtMfcMenuResource(128).toQMenu());
//   ptrToTheApp->qtMainWindow->setMenuBar(m_pMenuBar);
}

CFrameWnd::~CFrameWnd()
{
   delete m_pMenuBar;
   delete m_pMenu;
}

void CFrameWnd::addControlBar(int area, QWidget *bar)
{
   switch ( area )
   {
   case CBRS_TOP:
      cbrsTop->insertWidget(cbrsTop->count(),bar);
      break;
   case CBRS_LEFT:
      cbrsLeft->insertWidget(cbrsLeft->count(),bar);
      break;
   case CBRS_BOTTOM:
      cbrsBottom->insertWidget(cbrsBottom->count(),bar);
      break;
   case CBRS_RIGHT:
      cbrsRight->insertWidget(cbrsRight->count(),bar);
      break;
   }
   RecalcLayout();
}

void CFrameWnd::GetMessageString(
   UINT nID,
   CString& rMessage 
) const
{
   rMessage = qtMfcStringResource(nID);
}

void CFrameWnd::InitialUpdateFrame(
   CDocument* pDoc,
   BOOL bMakeVisible 
)
{
   // send initial update to all views (and other controls) in the frame
   SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);
}

void CFrameWnd::SetMessageText(LPCTSTR fmt,...)
{
   CString message;
   va_list args;
   va_start(args,fmt);
   message.FormatV(fmt,args);
   va_end(args);
#if UNICODE
   ptrToTheApp->qtMainWindow->statusBar()->showMessage(QString::fromWCharArray(message));
#else
   ptrToTheApp->qtMainWindow->statusBar()->showMessage(QString::fromAscii(message));
#endif
}

void CFrameWnd::SetMessageText(
   UINT nID 
)
{
   CString message = qtMfcStringResource(nID);
   SetMessageText(message);
}

BOOL CFrameWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
// CP: probably don't need this...
//	CPushRoutingFrame push(this);

	// pump through current view FIRST
	CView* pView = GetActiveView();
	if (pView != NULL && pView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// then pump through frame
	if (CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// last but not least, pump through app
	CWinApp* pApp = AfxGetApp();
	if (pApp != NULL && pApp->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	return FALSE;
}

void CFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);    // important for MDI Children
	if (nType != SIZE_MINIMIZED)
		RecalcLayout();
}

void CFrameWnd::RecalcLayout(
   BOOL bNotify
)
{
   if (m_bInRecalcLayout)
		return;

	m_bInRecalcLayout = TRUE;
//	// clear idle flags for recalc layout if called elsewhere
//	if (m_nIdleFlags & idleNotify)
//		bNotify = TRUE;
//	m_nIdleFlags &= ~(idleLayout|idleNotify);

//#ifndef _AFX_NO_OLE_SUPPORT
//	// call the layout hook -- OLE support uses this hook
//	if (bNotify && m_pNotifyHook != NULL)
//		m_pNotifyHook->OnRecalcLayout();
//#endif

//	// reposition all the child windows (regardless of ID)
//	if (GetStyle() & FWS_SNAPTOBARS)
//	{
//		CRect rect(0, 0, 32767, 32767);
//		RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery,
//			&rect, &rect, FALSE);
//		RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposExtra,
//			&m_rectBorder, &rect, TRUE);
//		CalcWindowRect(&rect);
//		SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(),
//			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
//	}
//	else
		RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposExtra, &m_rectBorder);
	m_bInRecalcLayout = FALSE;
}

CView::CView(CWnd* parent) 
   : CWnd(parent), 
     m_pDocument(NULL) 
{
}

CView::~CView()
{
}

CSize CControlBar::CalcFixedLayout(
   BOOL bStretch,
   BOOL bHorz 
)
{
   return CSize(0,0);
}

void CControlBar::SetBarStyle(
   DWORD dwStyle 
)
{
   qDebug("CControlBar::SetBarStyle");
}

BOOL CControlBar::IsVisible() const
{
   return _qtd->isVisible();
}

BOOL CReBarCtrl::Create( 
   DWORD dwStyle, 
   const RECT& rect, 
   CWnd* pParentWnd, 
   UINT nID  
)
{   
   m_hWnd = (HWND)this;
   _id = nID;

   _dwStyle = dwStyle;

   if ( _qt )
      delete _qt;
   
   if ( pParentWnd )
      _qt = new QToolBar(pParentWnd->toQWidget());
   else
      _qt = new QToolBar;

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QToolBar*>(_qt);
   
   _qtd->setMovable(false);
   CRect clientRect;
   pParentWnd->GetClientRect(&clientRect);
   _qtd->setGeometry(clientRect.left,clientRect.top,clientRect.right-clientRect.left,clientRect.bottom-clientRect.top);
   return TRUE;
}

BOOL CReBarCtrl::InsertBand( 
   UINT uIndex, 
   REBARBANDINFO* prbbi  
)
{
   CWnd* pWnd = (CWnd*)prbbi->hwndChild;
   if ( _qtd->actions().count() )
   {
      _qtd->addSeparator();
   }
   if ( dynamic_cast<QToolBar*>(pWnd->toQWidget()) )
   {
      QToolBar* toolBar = dynamic_cast<QToolBar*>(pWnd->toQWidget());
      _qtd->addActions(toolBar->actions());
      _qtd->setIconSize(toolBar->iconSize());
      pWnd->toQWidget()->setVisible(false);
   }
   else
   {
      _qtd->addWidget(pWnd->toQWidget());
   }
   return TRUE;
}

void CReBarCtrl::MinimizeBand(
   UINT uBand 
)
{
   qDebug("CReBarCtrl::MinimizeBand");
}

void CReBarCtrl::toolBarAction_triggered()
{
   emit toolBarAction_triggered(_toolBarActions.indexOf(sender()));
}

CReBar::CReBar()
{   
   m_pReBarCtrl = new CReBarCtrl;
}

CReBar::~CReBar()
{
   delete m_pReBarCtrl;
}

BOOL CReBar::Create(
   CWnd* pParentWnd,
   DWORD dwCtrlStyle,
   DWORD dwStyle,
   UINT nID
)
{
   m_hWnd = (HWND)this;
   _id = nID;
   
   CRect rect;
   pParentWnd->GetClientRect(&rect);
   m_pReBarCtrl->Create(dwStyle,rect,pParentWnd,nID);
   
   ptrToTheApp->qtMainWindow->addToolBar(dynamic_cast<QToolBar*>(m_pReBarCtrl->toQWidget()));
   return TRUE;
}

CToolBar::CToolBar(CWnd* parent)
{   
   _dwStyle = 0;
   
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QToolBar(parent->toQWidget());
   else
      _qt = new QToolBar;

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QToolBar*>(_qt);
   
   _qtd->setMovable(false);
}

CToolBar::~CToolBar()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CToolBar::CreateEx(
   CWnd* pParentWnd,
   DWORD dwCtrlStyle,
   DWORD dwStyle,
   CRect rcBorders,
   UINT nID
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   _dwStyle = dwStyle;

   m_pParentWnd = pParentWnd;
   if ( pParentWnd )
      _qt->setParent(pParentWnd->toQWidget());
   else
      _qt->setParent(NULL);
   
   pParentWnd->mfcToQtWidgetMap()->insert(nID,this);
   
   return TRUE;
}

LRESULT CToolBar::SendMessage(
   UINT message,
   WPARAM wParam,
   LPARAM lParam 
)
{
   return 0;
}

BOOL CToolBar::LoadToolBar(
   UINT nIDResource 
)
{
   qtMfcInitToolBarResource(nIDResource,this);
   return TRUE;
}

void CToolBar::SetButtonStyle(
   int nIndex,
   UINT nStyle 
)
{
   qDebug("CToolBar::SetButtonStyle");
}

void CToolBar::toolBarAction_triggered()
{
   emit toolBarAction_triggered(_toolBarActions.indexOf(sender()));
}

CStatusBar::CStatusBar(CWnd* parent)
{
   _dwStyle = 0;
   
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QStatusBar(parent->toQWidget());
   else
      _qt = new QStatusBar;

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QStatusBar*>(_qt);
}

CStatusBar::~CStatusBar()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CStatusBar::Create(
   CWnd* pParentWnd,
   DWORD dwStyle,
   UINT nID
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   _dwStyle = dwStyle;
   
   pParentWnd->mfcToQtWidgetMap()->insert(nID,this);

   ptrToTheApp->qtMainWindow->setStatusBar(_qtd);
   
   // Pass-through signals

   return TRUE;
}

LRESULT CStatusBar::SendMessage(
   UINT message,
   WPARAM wParam,
   LPARAM lParam 
)
{
   return 0;
}

void CStatusBar::SetWindowText(
   LPCTSTR lpszString 
)
{
#if UNICODE
   _qtd->showMessage(QString::fromWCharArray(lpszString));
#else
   _qtd->showMessage(lpszString);
#endif
}

BOOL CStatusBar::SetIndicators(
   const UINT* lpIDArray,
   int nIDCount 
)
{
   int pane;
   
   for ( pane = 0; pane < nIDCount; pane++ )
   {
      CStatic* newPane = new CStatic;
      _panes.insert(pane,newPane);
      _qtd->addPermanentWidget(newPane->toQWidget());
      CString lpszText = qtMfcStringResource(lpIDArray[pane]);
#if UNICODE
      ((QLabel*)newPane->toQWidget())->setText(QString::fromWCharArray(lpszText));
#else
      ((QLabel*)newPane->toQWidget())->setText(QString::fromAscii(lpszText));
#endif
      if ( lpIDArray[pane] != ID_SEPARATOR )
      {
         mfcToQtWidget.insert(lpIDArray[pane],newPane);
      }
   }
   return TRUE;
}

BOOL CStatusBar::SetPaneText(
   int nIndex,
   LPCTSTR lpszNewText,
   BOOL bUpdate 
)
{
   CStatic* pane = _panes.value(nIndex);
   if ( pane )
   {
#if UNICODE
      ((QLabel*)pane->toQWidget())->setText(QString::fromWCharArray(lpszNewText));
#else
      ((QLabel*)pane->toQWidget())->setText(QString::fromAscii(lpszNewText));
#endif
      return TRUE;
   }
   return FALSE;
}

CDialogBar::CDialogBar()
{
   _mfcd = new CDialog;
   _mfcd->setGeometry(0,0,100,100);
   
   _qt->installEventFilter(this);
}

CDialogBar::~CDialogBar()
{
   if ( _mfcd )
      delete _mfcd;
}

LRESULT CDialogBar::SendMessage(
   UINT message,
   WPARAM wParam,
   LPARAM lParam 
)
{
   AFX_SIZEPARENTPARAMS* pLayout = (AFX_SIZEPARENTPARAMS*)lParam;
   QRect myRect;
   switch ( message )
   {
   case WM_SIZEPARENT:
      if ( _qt->isVisible() )
      {
         if ( _nStyle&CBRS_TOP )
         {            
            pLayout->rect.top += rect().height();
            myRect.setHeight(rect().height());
            myRect.setWidth(pLayout->rect.right-pLayout->rect.left);
            pLayout->sizeTotal.cx = pLayout->rect.right-pLayout->rect.left;
            pLayout->sizeTotal.cy += rect().height();
         }
         else if ( _nStyle&CBRS_LEFT )
         {
            pLayout->rect.left += rect().width();
            myRect.setHeight(pLayout->rect.bottom-pLayout->rect.top);
            myRect.setWidth(rect().width());
            pLayout->sizeTotal.cx += rect().width();
            pLayout->sizeTotal.cy = pLayout->rect.bottom-pLayout->rect.top;
         }
         else if ( _nStyle&CBRS_BOTTOM )
         {            
            pLayout->rect.bottom -= rect().height();
            myRect.setHeight(rect().height());
            myRect.setWidth(pLayout->rect.right-pLayout->rect.left);
            pLayout->sizeTotal.cx = pLayout->rect.right-pLayout->rect.left;
            pLayout->sizeTotal.cy += rect().height();
         }
         else if ( _nStyle&CBRS_RIGHT )
         {
            pLayout->rect.right -= rect().width();
            myRect.setHeight(pLayout->rect.bottom-pLayout->rect.top);
            myRect.setWidth(rect().width());
            pLayout->sizeTotal.cx += rect().width();
            pLayout->sizeTotal.cy = pLayout->rect.bottom-pLayout->rect.top;
         }
         else
         {
            myRect.setHeight(rect().height());
            myRect.setWidth(rect().width());
         }
      }
      break;
   }
}

BOOL CDialogBar::Create(
   CWnd* pParentWnd,
   UINT nIDTemplate,
   UINT nStyle,
   UINT nID 
)
{ 
   m_hWnd = (HWND)this;
   _id = nID;

   _nStyle = nStyle;
   
   _mfcd->Create(nIDTemplate,this);
   
   _qt->setParent(pParentWnd->toQWidget()); 
   
   // This is a container.
   foreach ( UINT key, _mfcd->mfcToQtWidgetMap()->keys() )
   {
      mfcToQtWidget.insert(key,_mfcd->mfcToQtWidgetMap()->value(key));
   }
   
   pParentWnd->mfcToQtWidgetMap()->insertMulti(nID,this);
   
   if ( nStyle&CBRS_TOP )
   {
      _qt->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
      _qt->setFixedHeight(_mfcd->rect().height());
      m_pFrameWnd->addControlBar(CBRS_TOP,toQWidget());
   }
   else if ( nStyle&CBRS_LEFT )
   {
      _qt->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::MinimumExpanding));
      _qt->setFixedWidth(_mfcd->rect().width());
      m_pFrameWnd->addControlBar(CBRS_LEFT,toQWidget());
   }
   else if ( nStyle&CBRS_BOTTOM )
   {
      _qt->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
      _qt->setFixedHeight(_mfcd->rect().height());
      m_pFrameWnd->addControlBar(CBRS_BOTTOM,toQWidget());
   }
   else if ( nStyle&CBRS_RIGHT )
   {
      _qt->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::MinimumExpanding));
      _qt->setFixedWidth(_mfcd->rect().width());
      m_pFrameWnd->addControlBar(CBRS_RIGHT,toQWidget());
   }
   else
   {
      _qt->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
      _qt->setFixedSize(_mfcd->rect().width(),_mfcd->rect().height());
   }
   ShowWindow(SW_SHOW);
//   setVisible(true);
   
   return TRUE;
}

CSize CDialogBar::CalcFixedLayout(
   BOOL bStretch,
   BOOL bHorz 
)
{
   if (bStretch) // if not docked stretch to fit
		return CSize(bHorz ? 32767 : m_sizeDefault.cx,
			bHorz ? m_sizeDefault.cy : 32767);
	else
		return m_sizeDefault;
}

CDialog::CDialog()
{
   if ( _qt )
      delete _qt;
   
   _qt = new QDialog;
   
   _qtd = dynamic_cast<QDialog*>(_qt);
   _inited = false;
   
   _qt->installEventFilter(this);
   
   // Pass-through signals
}

CDialog::CDialog(int dlgID, CWnd *parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   if ( parent )
      _qt = new QDialog(parent);
   else
      _qt = new QDialog;
   
   _qtd = dynamic_cast<QDialog*>(_qt);
   _inited = false;
   _id = dlgID;
   
   _qt->installEventFilter(this);
   
   // Pass-through signals
}

CDialog::~CDialog()
{
   if ( _qt )
   {
      if ( _qtd )
         delete _qtd;
      _qtd = NULL;
      _qt = NULL;
   }
}

BOOL CDialog::Create(
   UINT nIDTemplate,
   CWnd* pParentWnd
)
{ 
   m_hWnd = (HWND)this;
   _id = nIDTemplate;

   qtMfcInitDialogResource(nIDTemplate,this);
   
   if ( pParentWnd )
      _qt->setParent(pParentWnd->toQWidget()); 
   else
      _qt->setParent(NULL);
   SetParent(pParentWnd); 
   if ( pParentWnd == m_pFrameWnd )
      _qt->setParent(NULL);
   
   foreach ( CWnd* pWnd, mfcToQtWidget ) pWnd->blockSignals(true);
   BOOL result = OnInitDialog(); 
   _inited = true;
   return result;
}

INT_PTR CDialog::DoModal()
{ 
   if ( !_inited )
   {
      qtMfcInitDialogResource(_id,this);
      
      OnInitDialog();
   }
   _inited = true;
   
   SetFocus();
   
   INT_PTR result = _qtd->exec();
   if ( result == QDialog::Accepted )
      return 1;
   else
      return 0;
}

void CDialog::SetWindowText(
   LPCTSTR lpszString 
)
{
#if UNICODE
   _qt->setWindowTitle(QString::fromWCharArray(lpszString));
#else
   _qt->setWindowTitle(lpszString);
#endif
}

void CDialog::ShowWindow(int code)
{
   switch ( code )
   {
   case SW_SHOW:
      foreach ( CWnd* pWnd, mfcToQtWidget ) pWnd->blockSignals(false);
      _qtd->setVisible(true);
      _qtd->setFocus();
      break;
   case SW_HIDE:
      foreach ( CWnd* pWnd, mfcToQtWidget ) pWnd->blockSignals(true);
      _qtd->setVisible(false);
      break;
   }
}

void CDialog::MapDialogRect( 
   LPRECT lpRect  
) const
{
   QFontMetrics sysFontMetrics(QFont("MS Shell Dlg",8));
 
   int baseunitX = sysFontMetrics.averageCharWidth()+1;
   int baseunitY = sysFontMetrics.height();
   
   lpRect->left   = MulDiv(lpRect->left,   baseunitX, 4);
   lpRect->right  = MulDiv(lpRect->right,  baseunitX, 4);
   lpRect->top    = MulDiv(lpRect->top,    baseunitY, 8);
   lpRect->bottom = MulDiv(lpRect->bottom, baseunitY, 8);
}

void CDialog::EndDialog(
   int nResult 
)
{
   _qtd->setResult(nResult);
   _qtd->close();
}

CCommonDialog::CCommonDialog(CWnd *pParentWnd)
 : CDialog(0,pParentWnd)
{
}

CCommonDialog::~CCommonDialog()
{
}

CWinThread::CWinThread()
{
   m_hThread = (HANDLE)this;
   m_nThreadID = (DWORD)QThread::currentThreadId();
   
   InitInstance();
}

CWinThread::~CWinThread()
{
}

BOOL CWinThread::CreateThread(
   DWORD dwCreateFlags,
   UINT nStackSize,
   LPSECURITY_ATTRIBUTES lpSecurityAttrs 
)
{
   return TRUE;
}

DWORD CWinThread::ResumeThread( )
{
   start(QThread::InheritPriority);
   return 0;
}

BOOL CWinThread::SetThreadPriority(
   int nPriority 
)
{
   QThread::Priority priority = QThread::currentThread()->priority();
   switch ( nPriority )
   {
   case THREAD_PRIORITY_TIME_CRITICAL:
      priority = QThread::TimeCriticalPriority;
      break;
   case THREAD_PRIORITY_HIGHEST:
      priority = QThread::HighestPriority;
      break;
   case THREAD_PRIORITY_ABOVE_NORMAL:
      priority = QThread::HighPriority;
      break;
   case THREAD_PRIORITY_NORMAL:
      priority = QThread::NormalPriority;
      break;
   case THREAD_PRIORITY_BELOW_NORMAL:
      priority = QThread::LowPriority;
      break;
   case THREAD_PRIORITY_LOWEST:
      priority = QThread::LowestPriority;
      break;
   case THREAD_PRIORITY_IDLE:
      priority = QThread::IdlePriority;
      break;
   }
   setPriority(priority);
   return TRUE;
}

BOOL CWinThread::PostThreadMessage(
   UINT message ,
   WPARAM wParam,
   LPARAM lParam 
      )
{
   emit postThreadMessage(message,wParam,lParam); 
   return TRUE;
}

void CDocument::SetTitle(CString title )
{
   m_docTitle = title;
//#if UNICODE
//   emit documentTitleChanged(QString::fromWCharArray((LPCTSTR)title));
//#else
//   emit documentTitleChanged(QString::fromAscii((LPCTSTR)title));
//#endif
}

POSITION CDocument::GetFirstViewPosition() const 
{ 
   POSITION pos = NULL;
   if ( _views.count() )
   {
      pos = new int; 
      (*pos) = 0; 
   }
   return pos; 
}

CView* CDocument::GetNextView(POSITION pos) const 
{ 
   if ( (*pos) == -1 ) 
   {
      delete pos;
      return NULL; // Choker for end-of-list
   }
   CView* pView = _views.at((*pos)++); 
   if ( (*pos) >= _views.count() ) 
   { 
      (*pos) = -1; 
   } 
   return pView; 
}

CDocTemplate::CDocTemplate(UINT f,CDocument* pDoc,CFrameWnd* pFrameWnd,CView* pView)
{
   m_pDoc = pDoc;
   m_pFrameWnd = pFrameWnd;
   m_pView = pView;
   
   m_pDoc->OnNewDocument();
   
   // Create linkages...
   m_pDoc->privateSetDocTemplate(this);
   m_pDoc->privateAddView(m_pView);
   m_pView->privateSetDocument(m_pDoc);
   m_pView->privateSetParentFrame(m_pFrameWnd);
   m_pFrameWnd->privateSetActiveView(m_pView);
   m_pFrameWnd->privateSetActiveDocument(m_pDoc);
   m_pFrameWnd->mfcToQtWidgetMap()->insert(AFX_IDW_PANE_FIRST,m_pView);
}

void CDocTemplate::InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc,
   BOOL bMakeVisible)
{
   // just delagate to implementation in CFrameWnd
   pFrame->InitialUpdateFrame(pDoc, bMakeVisible);
}

BOOL CDocTemplate::GetDocString(
   CString& rString,
   enum DocStringIndex index 
) const
{
   switch ( index )
   {
   case windowTitle:
      break;
      
   case docName:
      break;
      
   case fileNewName:
      break;
      
   case filterName:
      break;
      
   case filterExt:
      break;
      
   case regFileTypeId:
      break;
      
   case regFileTypeName:
      break;
   }
}


CSingleDocTemplate::CSingleDocTemplate(UINT f,CDocument* pDoc,CFrameWnd* pFrameWnd,CView* pView)
   : CDocTemplate(f,pDoc,pFrameWnd,pView)
{
   CRect rect;
   pFrameWnd->GetClientRect(&rect);
   pView->CreateEx(0,NULL,_T(""),WS_VISIBLE|WS_VSCROLL|WS_HSCROLL,rect,pFrameWnd,0);
}

POSITION CSingleDocTemplate::GetFirstDocPosition( ) const
{
   POSITION pos = NULL;

   pos = new int; 
   (*pos) = 0; 
   
   return pos; 
}

CDocument* CSingleDocTemplate::GetNextDoc(
   POSITION& rPos 
) const
{
   if ( *rPos == -1 ) 
   {
      delete rPos;
      return NULL; // Choker for end-of-list
   }
   CDocument* pDoc = m_pDoc; 

   (*rPos) = -1;
   
   return pDoc; 
}

CDocument* CSingleDocTemplate::OpenDocumentFile(
   LPCTSTR lpszPathName,
   BOOL bMakeVisible 
)
{
   if ( lpszPathName )
   {
      m_pDoc->OnOpenDocument(lpszPathName);
      m_pDoc->SetTitle(lpszPathName);
   }
   else
   {
      m_pDoc->OnNewDocument();
   }
   InitialUpdateFrame(m_pFrameWnd,m_pDoc);
   return m_pDoc;
}

CCommandLineInfo::CCommandLineInfo( )
{
   _args = QCoreApplication::arguments();
}

void CCommandLineInfo::ParseParam( 
   const TCHAR* pszParam,  
   BOOL bFlag, 
   BOOL bLast
)
{
   qDebug("CCommandLineInfo::ParseParam");
}

void CWinApp::ParseCommandLine(
   CCommandLineInfo& rCmdInfo 
)
{
   int arg;
   QString localArg;
   BOOL bLast = FALSE;
   BOOL bFlag;
   for ( arg = 0; arg < rCmdInfo._args.count(); arg++ )
   {
      if ( arg == rCmdInfo._args.count()-1 ) bLast = TRUE;
      localArg = rCmdInfo._args.at(arg);
      bFlag = FALSE;
      if ( localArg.startsWith("//") ||
           localArg.startsWith("-") )
      {
         localArg = localArg.right(localArg.length()-1);
         bFlag = TRUE;
      }
      rCmdInfo.ParseParam(CString(localArg),bFlag,bLast);
   }
}

BOOL CWinApp::ProcessShellCommand( 
   CCommandLineInfo& rCmdInfo  
)
{
   qDebug("ProcessShellCommand");
   return TRUE;
}

BOOL CWinApp::PreTranslateMessage(
   MSG* pMsg 
)
{
   return TRUE;
}

POSITION CWinApp::GetFirstDocTemplatePosition( ) const
{
   POSITION pos = NULL;
   if ( _docTemplates.count() )
   {
      pos = new int; 
      (*pos) = 0; 
   }
   return pos; 
}

CDocTemplate* CWinApp::GetNextDocTemplate(
   POSITION& pos 
) const
{
   if ( (*pos) == -1 ) 
   {
      delete pos;
      return NULL; // Choker for end-of-list
   }
   CDocTemplate* pDocTemplate = _docTemplates.at((*pos)++); 
   if ( (*pos) >= _docTemplates.count() ) 
   { 
      (*pos) = -1; 
   } 
   return pDocTemplate; 
}

void CWinApp::AddDocTemplate(CDocTemplate* pDocTemplate) 
{ 
   _docTemplates.append(pDocTemplate);
}

CDocument* CWinApp::OpenDocumentFile(
   LPCTSTR lpszFileName 
)
{
   POSITION pos;
   CDocTemplate* pDocTemplate = NULL;
   CDocument* pDoc = NULL;
   
   pos = GetFirstDocTemplatePosition();
   if ( pos )
   {
      // SDI...only need to get/tell the first document template.
      pDocTemplate = GetNextDocTemplate(pos);
   }
   if ( pDocTemplate )
   {
      pDoc = pDocTemplate->OpenDocumentFile(lpszFileName);
   }
   return pDoc;
}

HICON CWinApp::LoadIcon(
   UINT nIDResource 
) const
{
   return (HICON)qtIconResource(nIDResource);
}

BOOL CWinApp::InitInstance()
{
   return TRUE;
}

void CWinApp::OnFileNew()
{
   OpenDocumentFile(NULL);
}

void CWinApp::OnFileOpen()
{
}

HCURSOR CWinApp::LoadStandardCursor( 
   LPCTSTR lpszCursorName  
) const
{
   qDebug("LoadStandardCursor needs work...");
//   setCursor()
}

CMenu::CMenu()
   : m_hMenu(NULL)
{
   _qtd = new QMenu;
   m_hMenu = (HMENU)_qtd;
}

BOOL CMenu::LoadMenu(
   UINT nIDResource 
)
{
   _cmenu.clear();
   _cmenu.append(qtMfcMenuResource(nIDResource));
   return TRUE;
}

CMenu* CMenu::GetSubMenu(
   int nPos 
) const
{
   return _cmenu.value(nPos);
}

BOOL CMenu::CreatePopupMenu()
{
   _cmenu.clear();
   return TRUE;
}

BOOL CMenu::AppendMenu(
   UINT nFlags,
   UINT_PTR nIDNewItem,
   LPCTSTR lpszNewItem
)
{
   if ( nFlags&MF_POPUP )
   {
      LoadMenu(nIDNewItem);
   }

   if ( nFlags&MF_SEPARATOR )
   {
      _qtd->addSeparator();
   }
   else
   {
#if UNICODE
      QAction* action = _qtd->addAction(QString::fromWCharArray(lpszNewItem)); // CP: Add slots later
#else
      QAction* action = _qtd->addAction(lpszNewItem); // CP: Add slots later
#endif
      if ( action->text().contains("\t") )
      {
         action->setShortcut(QKeySequence(action->text().split("\t").at(1)));
      }
      if ( nFlags&MF_CHECKED )
      {
         action->setCheckable(true);
         action->setChecked(true);         
      }
      if ( nFlags&MF_ENABLED )
      {
         action->setEnabled(true);
      }
      if ( (nFlags&MF_DISABLED) ||
           (nFlags&MF_GRAYED) )
      {
         action->setDisabled(true);
      }
      mfcToQtMenu.insert(nIDNewItem,action);
      qtToMfcMenu.insert(action,nIDNewItem);
   }
   
   return TRUE;
}

UINT CMenu::GetMenuItemCount( ) const
{
   return _qtd->actions().count();
}

BOOL CMenu::SetDefaultItem(
   UINT uItem,
   BOOL fByPos
)
{
   QAction* action;
   BOOL result = FALSE;
   
   if ( fByPos )
   {
      action = _qtd->actions().at(uItem);
   }
   else
   {
      action = mfcToQtMenu.value(uItem);
   }
   if ( action )
   {
      _qtd->setDefaultAction(action);
      result = TRUE;
   }
   return result;
}

UINT CMenu::CheckMenuItem(
   UINT nIDCheckItem,
   UINT nCheck 
)
{
   QAction* action = mfcToQtMenu.value(nIDCheckItem);
   UINT prevState = (UINT)-1;
   if ( action )
   {
      prevState = action->isChecked();
      action->setCheckable(true);
      action->setChecked(nCheck);
   }
   return prevState;
}

BOOL CMenu::TrackPopupMenu(
   UINT nFlags,
   int x,
   int y,
   CWnd* pWnd,
   LPCRECT lpRect
)
{
   QAction* action = _qtd->exec(QCursor::pos());
   int result = 0;
   if ( action && (nFlags&TPM_RETURNCMD) )
   {
      result = qtToMfcMenu.value(action);
   }
   else if ( action )
   {
      result = 1;
   }
      
   return result;
}

UINT CMenu::EnableMenuItem(
   UINT nIDEnableItem,
   UINT nEnable 
)
{
   QAction* action = mfcToQtMenu.value(nIDEnableItem);
   if ( action )
   {
      bool enabled = action->isEnabled();
      action->setEnabled(nEnable);
      return enabled;
   }
   return -1;
}

BOOL CMenu::DestroyMenu( )
{
   if ( _qtd )
      delete _qtd;
   return TRUE;
}

CTabCtrl::CTabCtrl(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   _qt = new QTabWidget(parent->toQWidget());

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QTabWidget*>(_qt);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(currentChanged(int)),this,SIGNAL(currentChanged(int)));
}

CTabCtrl::~CTabCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

LONG CTabCtrl::InsertItem(
  int nItem,
  LPCTSTR lpszItem 
)
{
   _qtd->blockSignals(true); // Don't cause TcnSelchange yet...
#if UNICODE
   _qtd->insertTab(nItem,new QWidget(),QString::fromWCharArray(lpszItem));
#else
   _qtd->insertTab(nItem,new QWidget(),lpszItem);
#endif
   _qtd->blockSignals(false);
   return nItem;
}

int CTabCtrl::SetCurSel(
  int nItem 
)
{
   int oldSel = _qtd->currentIndex();
   _qtd->setCurrentIndex(nItem);
   return oldSel;
}

int CTabCtrl::GetCurSel() const
{
   return _qtd->currentIndex();
}

BOOL CTabCtrl::DeleteAllItems( )
{
   int tab;
   _qtd->blockSignals(true);
   for ( tab = _qtd->count()-1; tab >= 0; tab-- )
      _qtd->removeTab(tab);
   _qtd->blockSignals(false);
   return TRUE;
}

CEdit::CEdit(CWnd* parent)
   : CWnd(parent),
     _qtd_ptedit(NULL),
     _qtd_ledit(NULL),
     _dwStyle(0)
{
}

CEdit::~CEdit()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CEdit::Create(
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   m_hWnd = (HWND)this;
   _id = nID;
   
   _dwStyle = dwStyle;
   
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   if ( dwStyle&ES_MULTILINE )
   {
      if ( pParentWnd )
         _qt = new QPlainTextEdit(pParentWnd->toQWidget());
      else
         _qt = new QPlainTextEdit;
      
      // Downcast to save having to do it all over the place...
      _qtd_ptedit = dynamic_cast<QPlainTextEdit*>(_qt);
      
      // Pass-through signals
      QObject::connect(_qtd_ptedit,SIGNAL(textChanged(QString)),this,SIGNAL(textChanged(QString)));
   
      _qtd_ptedit->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
      _qtd_ptedit->setReadOnly(dwStyle&ES_READONLY);
      _qtd_ptedit->setVisible(dwStyle&WS_VISIBLE);
   }
   else
   {
      if ( pParentWnd )
         _qt = new QLineEdit(pParentWnd->toQWidget());
      else
         _qt = new QLineEdit;
      
      // Downcast to save having to do it all over the place...
      _qtd_ledit = dynamic_cast<QLineEdit*>(_qt);
      
      // Pass-through signals
      QObject::connect(_qtd_ledit,SIGNAL(textChanged(QString)),this,SIGNAL(textChanged(QString)));
   
      _qtd_ledit->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
      _qtd_ledit->setReadOnly(dwStyle&ES_READONLY);
      _qtd_ledit->setVisible(dwStyle&WS_VISIBLE);
   }
   
   return TRUE;
}

LRESULT CEdit::SendMessage(
   UINT message,
   WPARAM wParam,
   LPARAM lParam 
)
{
   if ( _dwStyle&ES_MULTILINE )
   {
      switch ( message )
      {
      case EM_SETREADONLY:
         _qtd_ptedit->setReadOnly(wParam);
         break;
      }
   }
   else
   {
      switch ( message )
      {
      case EM_SETREADONLY:
         _qtd_ledit->setReadOnly(wParam);
         break;
      }
   }
   return 0; // CP: not sure this matters...much
}

int CEdit::GetWindowTextLength( ) const
{
   if ( _dwStyle&ES_MULTILINE )
   {
      return _qtd_ptedit->toPlainText().length();
   }
   else
   {
      return _qtd_ledit->text().length();
   }
}

void CEdit::GetWindowText(
   CString& rString 
) const
{
   if ( _dwStyle&ES_MULTILINE )
   {
      rString = _qtd_ptedit->toPlainText();
   }
   else
   {
      rString = _qtd_ledit->text();
   }
}

int CEdit::GetWindowText(
   LPTSTR lpszStringBuf,
   int nMaxCount 
) const
{
   if ( _dwStyle&ES_MULTILINE )
   {
#if UNICODE
      wcsncpy(lpszStringBuf,(LPTSTR)_qtd_ptedit->toPlainText().unicode(),nMaxCount);
      return wcslen(lpszStringBuf);
#else
      strncpy(lpszStringBuf,(LPTSTR)_qtd_ptedit->toPlainText().toAscii().constData(),nMaxCount);
      return strlen(lpszStringBuf);
#endif
   }
   else
   {
#if UNICODE
      wcsncpy(lpszStringBuf,(LPTSTR)_qtd_ledit->text().unicode(),nMaxCount);
      return wcslen(lpszStringBuf);
#else
      strncpy(lpszStringBuf,(LPTSTR)_qtd_ledit->text().toAscii().constData(),nMaxCount);
      return strlen(lpszStringBuf);
#endif
   }
}

void CEdit::SetWindowText(
   LPCTSTR lpszString 
)
{
   if ( _dwStyle&ES_MULTILINE )
   {
#if UNICODE
   _qtd_ptedit->setPlainText(QString::fromWCharArray(lpszString));
#else
   _qtd_ptedit->setPlainText(lpszString);
#endif
   }
   else
   {
#if UNICODE
   _qtd_ledit->setText(QString::fromWCharArray(lpszString));
#else
   _qtd_ledit->setText(lpszString);
#endif
   }
}

DWORD CEdit::GetStyle() const
{
   LONG ret = 0;
   if ( _dwStyle&ES_MULTILINE )
   {
      ret |= _qtd_ptedit->isReadOnly()?ES_READONLY:0;
   }
   else
   {
      ret |= _qtd_ledit->isReadOnly()?ES_READONLY:0;
   }
   return ret;
}

void CEdit::SetSel(
   DWORD dwSelection,
   BOOL bNoScroll
)
{
   SetSel(dwSelection>>16,dwSelection&0xFFFF,bNoScroll);
}

void CEdit::SetSel(
   int nStartChar,
   int nEndChar,
   BOOL bNoScroll
)
{
   if ( _dwStyle&ES_MULTILINE )
   {
      QTextCursor textCursor(_qtd_ptedit->document());
      if ( nEndChar < nStartChar )
      {
         int temp;
         temp = nEndChar;
         nEndChar = nStartChar;
         nStartChar = temp;
      }   
      textCursor.setPosition(nStartChar);
      textCursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,nEndChar-nStartChar);
      _qtd_ptedit->setTextCursor(textCursor);
   }
   else
   {
      _qtd_ledit->setSelection(nStartChar,nEndChar);
   }
}

void CEdit::ReplaceSel(
   LPCTSTR lpszNewText,
      BOOL bCanUndo
)
{
   if ( _dwStyle&ES_MULTILINE )
   {
      QTextCursor textCursor = _qtd_ptedit->textCursor();
      textCursor.removeSelectedText();
#if UNICODE
      textCursor.insertText(QString::fromWCharArray(lpszNewText));
#else
      textCursor.insertText(lpszNewText);
#endif
      _qtd_ptedit->setTextCursor(textCursor);
   }
   else
   {
#if UNICODE
      _qtd_ledit->insert(QString::fromWCharArray(lpszNewText));
#else
      _qtd_ledit->insert(lpszNewText);
#endif
   }
}

#if UNICODE
void CEdit::ReplaceSel(
   LPCSTR lpszNewText,
      BOOL bCanUndo
)
{
   if ( _dwStyle&ES_MULTILINE )
   {
      QTextCursor textCursor = _qtd_ptedit->textCursor();
      textCursor.removeSelectedText();
      textCursor.insertText(lpszNewText);
      _qtd_ptedit->setTextCursor(textCursor);
   }
   else
   {
      _qtd_ledit->insert(lpszNewText);
   }
}
#endif

BOOL CEdit::EnableWindow(
   BOOL bEnable
)
{
   if ( _dwStyle&ES_MULTILINE )
   {
      BOOL state = _qtd_ptedit->isEnabled();
      _qtd_ptedit->setEnabled(bEnable);
      return state;
   }
   else
   {
      BOOL state = _qtd_ledit->isEnabled();
      _qtd_ledit->setEnabled(bEnable);
      return state;
   }
}

void CEdit::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned 
)
{
   if ( _dwStyle&ES_MULTILINE )
   {
      _qtd_ptedit->setPlainText(QString::number(nValue));
   }
   else
   {
      _qtd_ledit->setText(QString::number(nValue));
   }
}

UINT CEdit::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   if ( _dwStyle&ES_MULTILINE )
   {
      return _qtd_ptedit->toPlainText().toInt();
   }
   else
   {
      return _qtd_ledit->text().toInt();
   }
}

void CEdit::SetDlgItemText(
   int nID,
   LPCTSTR lpszString 
)
{
   if ( _dwStyle&ES_MULTILINE )
   {
#if UNICODE
      _qtd_ptedit->setPlainText(QString::fromWCharArray(lpszString));
#else
      _qtd_ptedit->setPlainText(lpszString);
#endif   
   }
   else
   {
#if UNICODE
      _qtd_ledit->setText(QString::fromWCharArray(lpszString));
#else
      _qtd_ledit->setText(lpszString);
#endif   
   }   
}

int CEdit::GetDlgItemText(
   int nID,
   CString& rString 
) const
{
   if ( _dwStyle&ES_MULTILINE )
   {
      rString = _qtd_ptedit->toPlainText();
      return _qtd_ptedit->toPlainText().length();
   }
   else
   {
      rString = _qtd_ledit->text();
      return _qtd_ledit->text().length();
   }
}

int CEdit::GetDlgItemText(
   int nID,
   LPTSTR lpStr,
   int nMaxCount 
) const
{
   if ( _dwStyle&ES_MULTILINE )
   {
#if UNICODE
      wcsncpy(lpStr,(LPWSTR)_qtd_ptedit->toPlainText().unicode(),nMaxCount);
#else
      strncpy(lpStr,_qtd_ptedit->toPlainText().toAscii().constData(),nMaxCount);
#endif   
      return _qtd_ptedit->toPlainText().length();
   }
   else
   {
#if UNICODE
      wcsncpy(lpStr,(LPWSTR)_qtd_ledit->text().unicode(),nMaxCount);
#else
      strncpy(lpStr,_qtd_ledit->text().toAscii().constData(),nMaxCount);
#endif   
      return _qtd_ledit->text().length();
   }
}

CButton::CButton(CWnd* parent)
   : CWnd(parent),
     _qtd_push(NULL),
     _qtd_radio(NULL),
     _qtd_check(NULL),
     _qtd(NULL)
{
}

CButton::~CButton()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CButton::Create(
   LPCTSTR lpszCaption,
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   DWORD buttonType = dwStyle&0x000F;
   DWORD buttonStyle = dwStyle&0xFFF0;
   
   if ( _qt )
      delete _qt;

   _grid = NULL;
   
   if ( buttonType == BS_AUTOCHECKBOX )
   {
      _qt = new QCheckBox(pParentWnd->toQWidget());
      
      // Downcast to save having to do it all over the place...
      _qtd_check = dynamic_cast<QCheckBox*>(_qt);
      _qtd_check->setCheckable(true);
   }
   else if ( buttonType == BS_AUTO3STATE )
   {
      _qt = new QCheckBox(pParentWnd->toQWidget());
      
      // Downcast to save having to do it all over the place...
      _qtd_check = dynamic_cast<QCheckBox*>(_qt);
      _qtd_check->setCheckable(true);
      _qtd_check->setTristate(true);
   }
   else if ( buttonType == BS_AUTORADIOBUTTON )
   {
      _qt = new QRadioButton(pParentWnd->toQWidget());
      
      // Downcast to save having to do it all over the place...
      _qtd_radio = dynamic_cast<QRadioButton*>(_qt);
      _qtd_radio->setCheckable(true);
   }
   else if ( (buttonType == BS_PUSHBUTTON) || 
             (buttonType == BS_DEFPUSHBUTTON) )
   {
      _qt = new QPushButton(pParentWnd->toQWidget());
      
      // Downcast to save having to do it all over the place...
      _qtd_push = dynamic_cast<QPushButton*>(_qt);
      
      _qtd_push->setDefault(buttonType==BS_DEFPUSHBUTTON);
   }
   
   _qtd = dynamic_cast<QAbstractButton*>(_qt);

#if UNICODE
   _qtd->setText(QString::fromWCharArray(lpszCaption));
#else
   _qtd->setText(lpszCaption);
#endif
   
   _qtd->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   _qtd->setVisible(dwStyle&WS_VISIBLE);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(clicked()),this,SIGNAL(clicked()));
   
   return TRUE;
}

HBITMAP CButton::SetBitmap(
   HBITMAP hBitmap 
)
{
   CBitmap* pBitmap = (CBitmap*)hBitmap;
   _qtd->setIcon(QIcon(*pBitmap->toQPixmap()));
}

void CButton::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned 
)
{
   _qtd->setText(QString::number(nValue));
}

UINT CButton::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   return _qtd->text().toInt();
}

void CButton::SetDlgItemText(
   int nID,
   LPCTSTR lpszString 
)
{
#if UNICODE
   _qtd->setText(QString::fromWCharArray(lpszString));
#else
   _qtd->setText(lpszString);
#endif
}

int CButton::GetDlgItemText(
   int nID,
   CString& rString 
) const
{
   rString = _qtd->text();
   return _qtd->text().length();
}

int CButton::GetDlgItemText(
   int nID,
   LPTSTR lpStr,
   int nMaxCount 
) const
{
#if UNICODE
   wcsncpy(lpStr,(LPWSTR)_qtd->text().unicode(),nMaxCount);
#else
   strncpy(lpStr,_qtd->text().toAscii().constData(),nMaxCount);
#endif   
   return _qtd->text().length();
}

void CButton::CheckDlgButton( 
   int nIDButton, 
   UINT nCheck  
)
{
   _qtd->setChecked(nCheck);
}

UINT CButton::IsDlgButtonChecked( 
   int nIDButton 
) const
{
   return _qtd->isChecked();
}


CBitmapButton::CBitmapButton(CWnd* parent)
   : CButton(parent),
     _qtd(NULL)
{
}

CBitmapButton::~CBitmapButton()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CBitmapButton::Create(
   LPCTSTR lpszCaption,
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   DWORD buttonType = dwStyle&0x000F;
   DWORD buttonStyle = dwStyle&0xFFF0;
   
   if ( _qt )
      delete _qt;

   _grid = NULL;
   
   _qt = new QToolButton(pParentWnd->toQWidget());
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QToolButton*>(_qt);

#if UNICODE
   _qtd->setText(QString::fromWCharArray(lpszCaption));
#else
   _qtd->setText(lpszCaption);
#endif
   
   _qtd->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   _qtd->setVisible(dwStyle&WS_VISIBLE);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(clicked()),this,SIGNAL(clicked()));
   
   return TRUE;
}

CSpinButtonCtrl::CSpinButtonCtrl(CWnd* parent)
   : CWnd(parent),
     _oldValue(0)
{
}

CSpinButtonCtrl::~CSpinButtonCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CSpinButtonCtrl::Create(
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   _qt = new QSpinBox_MFC(pParentWnd->toQWidget());
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QSpinBox_MFC*>(_qt);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(valueChanged(int)),this,SLOT(control_edited()));
   QObject::connect(_qtd,SIGNAL(editingFinished()),this,SLOT(control_edited()));

   _qtd->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   _qtd->setFrame(false);
   _qtd->setVisible(dwStyle&WS_VISIBLE);
   
   return TRUE;
}

void CSpinButtonCtrl::control_edited()
{
   emit valueChanged(_oldValue,_qtd->value());
   _oldValue = _qtd->value();
}

int CSpinButtonCtrl::SetPos(
   int nPos 
)
{
   int pos = _qtd->value();
   _qtd->blockSignals(true);
   _qtd->setValue(nPos);
   _qtd->blockSignals(false);
   return pos;
}

int CSpinButtonCtrl::GetPos( ) const
{
   return _qtd->value();
}

void CSpinButtonCtrl::SetRange(
   short nLower,
   short nUpper 
)
{
   _qtd->setRange(nLower,nUpper);
}

void CSpinButtonCtrl::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned 
)
{
   _qtd->setValue(nValue);
}

UINT CSpinButtonCtrl::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   return _qtd->value();
}

void CSpinButtonCtrl::SetDlgItemText(
   int nID,
   LPCTSTR lpszString 
)
{
   QString val;
#if UNICODE
   val = QString::fromWCharArray(lpszString);
#else
   val = QString::fromAscii(lpszString);
#endif
   _qtd->setValue(val.toInt());
}

int CSpinButtonCtrl::GetDlgItemText(
   int nID,
   CString& rString 
) const
{
   rString = _qtd->text();
   return _qtd->text().length();
}

int CSpinButtonCtrl::GetDlgItemText(
   int nID,
   LPTSTR lpStr,
   int nMaxCount 
) const
{
#if UNICODE
   wcsncpy(lpStr,(LPWSTR)_qtd->text().unicode(),nMaxCount);
#else
   strncpy(lpStr,_qtd->text().toAscii().constData(),nMaxCount);
#endif   
   return _qtd->text().length();
}

CSliderCtrl::CSliderCtrl(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   if ( parent )
      _qt = new QSlider(parent->toQWidget());
   else
      _qt = new QSlider;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QSlider*>(_qt);
   
   // Not sure if there's vertical sliders in MFC...
   _qtd->setOrientation(Qt::Horizontal);
   _qtd->setTickPosition(QSlider::TicksBelow);
   _qtd->setTickInterval(1);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(valueChanged(int)),this,SIGNAL(valueChanged(int)));
}

CSliderCtrl::~CSliderCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CSliderCtrl::Create(
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   if ( dwStyle&TBS_NOTICKS )
   {
      _qtd->setTickPosition(QSlider::NoTicks);
   }
   else if ( dwStyle&TBS_BOTH )
   {
      _qtd->setTickPosition(QSlider::TicksBothSides);
   }
   else if ( dwStyle&TBS_LEFT )
   {
      _qtd->setTickPosition(QSlider::TicksLeft);
   }
   else
   {
      _qtd->setTickPosition(QSlider::TicksRight);
   }
   if ( dwStyle&TBS_VERT )
   {
      _qtd->setOrientation(Qt::Vertical);
      _qtd->setInvertedAppearance(true);
   }
   
   _qtd->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   _qtd->setVisible(dwStyle&WS_VISIBLE);

   return TRUE;
}

void CSliderCtrl::SetRange(
   short nLower,
   short nUpper 
)
{
   _qtd->setRange(nLower,nUpper);
}

void CSliderCtrl::SetRangeMax(
   int nMax,
   BOOL bRedraw
)
{
   _qtd->setMaximum(nMax);
}

void CSliderCtrl::SetPos(
   int nPos 
)
{
   _qtd->blockSignals(true);
   _qtd->setValue(nPos);
   _qtd->blockSignals(false);
}

int CSliderCtrl::GetPos( ) const
{
   return _qtd->value();
}

void CSliderCtrl::SetTicFreq(
   int nFreq 
)
{
   _qtd->setTickInterval(nFreq);
}

CProgressCtrl::CProgressCtrl(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   _qt = new QProgressBar(parent->toQWidget());
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QProgressBar*>(_qt);
   
   // Not sure if there's vertical sliders in MFC...
   _qtd->setOrientation(Qt::Horizontal);
   
   // Pass-through signals
}

CProgressCtrl::~CProgressCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CProgressCtrl::SetRange(
   short nLower,
   short nUpper 
)
{
   _qtd->setRange(nLower,nUpper);
}

void CProgressCtrl::SetPos(
   int nPos 
)
{
   _qtd->blockSignals(true);
   _qtd->setValue(nPos);
   _qtd->blockSignals(false);
}

int CProgressCtrl::GetPos( ) const
{
   return _qtd->value();
}

CStatic::CStatic(CWnd *parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   if ( parent )
      _qt = new QLabel(parent->toQWidget());
   else
      _qt = new QLabel;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QLabel*>(_qt);      
}

CStatic::~CStatic()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CStatic::Create(
   LPCTSTR lpszText,
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID
)
{
   m_hWnd = (HWND)this;
   _id = nID;

   _qtd->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   _qtd->setVisible(dwStyle&WS_VISIBLE);
   
#if UNICODE
   _qtd->setText(QString::fromWCharArray(lpszText));
#else
   _qtd->setText(lpszText);
#endif
   
   // Pass-through signals
   
   return TRUE;
}

void CStatic::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned 
)
{
   _qtd->setText(QString::number(nValue));
}

UINT CStatic::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   return _qtd->text().toInt();
}

void CStatic::SetDlgItemText(
   int nID,
   LPCTSTR lpszString 
)
{
#if UNICODE
   _qtd->setText(QString::fromWCharArray(lpszString));
#else
   _qtd->setText(lpszString);
#endif
}

int CStatic::GetDlgItemText(
   int nID,
   CString& rString 
) const
{
   rString = _qtd->text();
   return _qtd->text().length();
}

int CStatic::GetDlgItemText(
   int nID,
   LPTSTR lpStr,
   int nMaxCount 
) const
{
#if UNICODE
   wcsncpy(lpStr,(LPWSTR)_qtd->text().unicode(),nMaxCount);
#else
   strncpy(lpStr,_qtd->text().toAscii().constData(),nMaxCount);
#endif   
   return _qtd->text().length();
}

CGroupBox::CGroupBox(CWnd *parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   _qt = new QGroupBox(parent->toQWidget());
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QGroupBox*>(_qt);
   
   _qtd->setContentsMargins(0,0,0,0);
      
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(clicked()),this,SIGNAL(clicked()));
}

CGroupBox::~CGroupBox()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

BOOL CGroupBox::Create(
   LPCTSTR lpszCaption,
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   m_hWnd = (HWND)this;
   _id = nID;

#if UNICODE
   _qtd->setTitle(QString::fromWCharArray(lpszCaption));
#else
   _qtd->setTitle(lpszCaption);
#endif
   
   _qtd->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   _qtd->setVisible(dwStyle&WS_VISIBLE);
   
   return TRUE;
}

void CGroupBox::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned 
)
{
   _qtd->setTitle(QString::number(nValue));
}

UINT CGroupBox::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   return _qtd->title().toInt();
}

void CGroupBox::SetDlgItemText(
   int nID,
   LPCTSTR lpszString 
)
{
#if UNICODE
   _qtd->setTitle(QString::fromWCharArray(lpszString));
#else
   _qtd->setTitle(lpszString);
#endif
}

int CGroupBox::GetDlgItemText(
   int nID,
   CString& rString 
) const
{
   rString = _qtd->title();
   return _qtd->title().length();
}

int CGroupBox::GetDlgItemText(
   int nID,
   LPTSTR lpStr,
   int nMaxCount 
) const
{
#if UNICODE
   wcsncpy(lpStr,(LPWSTR)_qtd->title().unicode(),nMaxCount);
#else
   strncpy(lpStr,_qtd->title().toAscii().constData(),nMaxCount);
#endif   
   return _qtd->title().length();
}

CFileDialog::CFileDialog(
   BOOL bOpenFileDialog,
   LPCTSTR lpszDefExt,
   LPCTSTR lpszFileName,
   DWORD dwFlags,
   LPCTSTR lpszFilter,
   CWnd* pParentWnd,
   DWORD dwSize
)
   : CCommonDialog(pParentWnd)
{
   int seg;

   m_pOFN = &m_ofn;
   
   if ( _qt )
      delete _qt;
   
   if ( pParentWnd )
      _qt = new QFileDialog(pParentWnd->toQWidget());
   else
      _qt = new QFileDialog;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QFileDialog*>(_qt);
   
   // Pass-through signals
   
   _qtd->hide();
   
#if UNICODE
   qDebug(QString::fromWCharArray(lpszFilter).toAscii().constData());
   QStringList filtersList = QString::fromWCharArray(lpszFilter).split("|");
#else
   QStringList filtersList = QString(lpszFilter).split("|");
#endif
   
   QString filters;
   
   // Take out odd-numbered splits between ||;s and convert |'s to ;;'s.
   for ( seg = filtersList.count()-1; seg >= 0; seg -= 2 )
   {
      filtersList.removeAt(seg);
   }
   for ( seg = 0; seg < filtersList.count(); seg++ )
   {
      if ( !filtersList.at(seg).compare("all files",Qt::CaseInsensitive) )
      {
         filtersList.replace(seg,"All files (*.*)");
      }
   }
   filters = filtersList.join(";;");
   if ( filters.endsWith(";;") )
   {
      filters = filters.left(filters.length()-2);
   }
   
#if UNICODE
   _qtd->setDefaultSuffix(QString::fromWCharArray(lpszDefExt));
   _qtd->selectFile(QString::fromWCharArray(lpszFileName));
   _qtd->setFilter(filters);
#else
   _qtd->setDefaultSuffix(lpszDefExt);
   _qtd->selectFile(lpszFileName);
   _qtd->setFilter(filters);
#endif
   qDebug("CFileDialog::CFileDialog...need dwFlags impl");
   switch ( dwFlags )
   {
   case OFN_HIDEREADONLY:
      break;
   case OFN_OVERWRITEPROMPT:
      break;
   case OFN_ALLOWMULTISELECT:
      _qtd->setFileMode(QFileDialog::ExistingFiles);
      break;
   }
   if ( bOpenFileDialog )
   {
      _qtd->setAcceptMode(QFileDialog::AcceptOpen);
   }
   else
   {
      _qtd->setAcceptMode(QFileDialog::AcceptSave);
   }
}

CFileDialog::~CFileDialog()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

INT_PTR CFileDialog::DoModal()
{ 
   INT_PTR result = _qtd->exec();
   if ( result == QDialog::Accepted )
      return 1;
   else
      return 0;
}

POSITION CFileDialog::GetStartPosition( ) const
{
   QStringList files = _qtd->selectedFiles();
   POSITION pos = NULL;
   if ( files.count() )
   {
      pos = new int; 
      (*pos) = 0; 
   }
   return pos; 
}

CString CFileDialog::GetNextPathName(
   POSITION& pos 
) const
{
   if ( (*pos) == -1 ) 
   {
      delete pos;
      return CString(); // Choker for end-of-list
   }
   QStringList files = _qtd->selectedFiles();
   CString file = files.at((*pos)++); 
   if ( (*pos) >= files.count() ) 
   { 
      (*pos) = -1;
   } 
   return file; 
}

CString CFileDialog::GetFileExt( ) const
{
   QStringList files = selectedFiles();
   if ( files.count() == 1 )
   {
      QString file = files.at(0);
      QFileInfo fileInfo(file);
      return CString(fileInfo.suffix());
   }
   return CString();
}

CString CFileDialog::GetFileName( ) const
{
   QStringList files = selectedFiles();
   if ( files.count() == 1 )
   {
      QString file = files.at(0);
      QFileInfo fileInfo(file);
      return CString(fileInfo.fileName());
   }
   return CString();
}

CString CFileDialog::GetPathName( ) const
{
   QStringList files = selectedFiles();
   if ( files.count() == 1 )
   {
      return CString(files.at(0));
   }
   return CString();
}

CColorDialog::CColorDialog(
   COLORREF clrInit,
   DWORD dwFlags,
   CWnd* pParentWnd 
)
   : CCommonDialog(pParentWnd)
{
   _color = clrInit;
   
   if ( _qt )
      delete _qt;
   
   if ( pParentWnd )
      _qt = new QColorDialog(pParentWnd->toQWidget());
   else
      _qt = new QColorDialog;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QColorDialog*>(_qt);
   
   // Pass-through signals
   
   _qtd->hide();
}

CColorDialog::~CColorDialog()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

INT_PTR CColorDialog::DoModal()
{ 
   QColor color(GetRValue(_color),GetGValue(_color),GetBValue(_color));
   INT_PTR result;
   _qtd->setCurrentColor(color);
   if ( m_cc.Flags&CC_RGBINIT )
   {
      color.setRgb(GetRValue(m_cc.rgbResult),GetGValue(m_cc.rgbResult),GetBValue(m_cc.rgbResult));
      _qtd->setCurrentColor(color);
   }
   result = _qtd->exec();
   if ( result == QDialog::Accepted )
      return 1;
   else
      return 0;
}

COLORREF CColorDialog::GetColor( ) const
{
   QColor col = _qtd->selectedColor();
   COLORREF ret = RGB(col.red(),col.green(),col.blue());
   return ret;
}

CMutex::CMutex(
   BOOL bInitiallyOwn,
   LPCTSTR lpszName,
   LPSECURITY_ATTRIBUTES lpsaAttribute
)
{
   _qtd = new QMutex;
   if ( bInitiallyOwn )
      _qtd->lock();
}

CMutex::~CMutex()
{
   delete _qtd;
}

BOOL CMutex::Lock(
   DWORD dwTimeout 
)
{
   return _qtd->tryLock(dwTimeout);
}

BOOL CMutex::Unlock( )
{
   _qtd->unlock();
   return TRUE;
}

CCriticalSection::CCriticalSection()
{
   _qtd = new QMutex;
}

CCriticalSection::~CCriticalSection()
{
   delete _qtd;
}

BOOL CCriticalSection::Lock(
   DWORD dwTimeout 
)
{
   return _qtd->tryLock(dwTimeout);
}

BOOL CCriticalSection::Unlock( )
{
   _qtd->unlock();
   return TRUE;
}

CEvent::CEvent(
   BOOL bInitiallyOwn,
   BOOL bManualReset,
   LPCTSTR lpszName,
   LPSECURITY_ATTRIBUTES lpsaAttribute 
)
{
}

CEvent::~CEvent()
{
}

BOOL CEvent::SetEvent()
{
}

BOOL CEvent::ResetEvent()
{
}

BOOL CEvent::PulseEvent()
{
}

BOOL CFileFind::FindFile(
   LPCTSTR pstrName,
   DWORD dwUnused 
)
{
   QFileInfo fileInfo;
   QString path;
   QString filter;
   QString full;
   if ( pstrName )
   {
#if UNICODE
      full = QString::fromWCharArray(pstrName);
#else
      full = QString::fromAscii(pstrName);
#endif
      fileInfo.setFile(full);
      path = fileInfo.path();
      filter = fileInfo.fileName();
      _qdir.setPath(path);
   }
   else
   {
      _qdir = QDir::homePath();
      filter = "*.*";
   }
   _qfiles = _qdir.entryInfoList(QStringList(filter));
   _idx = 0;
   if ( _qfiles.count() )
      return TRUE;
   return FALSE;
}

BOOL CFileFind::FindNextFile( )
{
   BOOL ret = TRUE;
   if ( _idx == _qfiles.count()-1 ) // need to return false for last file.
   {
      ret = FALSE;
   }
   _idx++;
   return ret;
}

CString CFileFind::GetFileName( ) const
{
   return CString(_qfiles.at(_idx-1).fileName());
}

CString CFileFind::GetFilePath( ) const
{
   return CString(_qfiles.at(_idx-1).filePath());
}

CString CFileFind::GetFileTitle( ) const
{
   return CString(_qfiles.at(_idx-1).completeBaseName());
}

BOOL CFileFind::IsDirectory( ) const
{
   return _qfiles.at(_idx-1).isDir();
}

BOOL CFileFind::IsHidden( ) const
{
   return _qfiles.at(_idx-1).isHidden();
}

BOOL CFileFind::IsDots( ) const
{
   return _qfiles.at(_idx-1).fileName().startsWith("..");
}

CImageList::CImageList()
{
}

BOOL CImageList::Create(
   int cx,
   int cy,
   UINT nFlags,
   int nInitial,
   int nGrow 
)
{
   // Nothing to do here really...
   return TRUE;
}

int CImageList::Add(
   CBitmap* pbmImage,
   CBitmap* pbmMask 
)
{
   _images.append(pbmImage);
   // Not sure what to do with mask yet.
}

int CImageList::Add(
   CBitmap* pbmImage,
   COLORREF crMask 
)
{
   _images.append(pbmImage);
   // Not sure what to do with mask yet.
}

int CImageList::Add(
   HICON hIcon 
)
{
   CBitmap* pBitmap = (CBitmap*)hIcon;
   _images.append(pBitmap);
}

HICON CImageList::ExtractIcon(
   int nImage
)
{
   return (HICON)_images.at(nImage);
}

CPropertySheet::CPropertySheet(
   UINT nIDCaption,
   CWnd* pParentWnd,
   UINT iSelectPage
)
{
   _commonConstruct(pParentWnd,iSelectPage);
}

CPropertySheet::CPropertySheet(
   LPCTSTR pszCaption,
   CWnd* pParentWnd,
   UINT iSelectPage 
)
{
   _commonConstruct(pParentWnd,iSelectPage);
}

CPropertySheet::CPropertySheet(
   UINT nIDCaption,
   CWnd* pParentWnd,
   UINT iSelectPage,
   HBITMAP hbmWatermark,
   HPALETTE hpalWatermark,
   HBITMAP hbmHeader 
)
{
   _commonConstruct(pParentWnd,iSelectPage);
}

CPropertySheet::CPropertySheet(
   LPCTSTR pszCaption,
   CWnd* pParentWnd,
   UINT iSelectPage,
   HBITMAP hbmWatermark,
   HPALETTE hpalWatermark,
   HBITMAP hbmHeader 
)
{
   _commonConstruct(pParentWnd,iSelectPage);
}

void CPropertySheet::_commonConstruct(CWnd* parent,UINT selectedPage)
{
   if ( _qt )
      delete _qt;
   
   _qt = new QDialog;
   
   _qtd = dynamic_cast<QDialog*>(_qt);
   
   _selectedPage = selectedPage;
   
   _grid = new QGridLayout(_qtd);
   _qtabwidget = new QTabWidget;
   QObject::connect(_qtabwidget,SIGNAL(currentChanged(int)),this,SLOT(tabWidget_currentChanged(int)));
   _grid->addWidget(_qtabwidget,0,0,1,1);
   _qbuttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel|QDialogButtonBox::Apply,Qt::Horizontal);
   QObject::connect(_qbuttons->button(QDialogButtonBox::Ok),SIGNAL(clicked()),this,SLOT(ok_clicked()));
   QObject::connect(_qbuttons->button(QDialogButtonBox::Cancel),SIGNAL(clicked()),this,SLOT(cancel_clicked()));
   QObject::connect(_qbuttons->button(QDialogButtonBox::Apply),SIGNAL(clicked()),this,SLOT(apply_clicked()));
   _grid->addWidget(_qbuttons,1,0,1,1);
   
   // Pass-through signals
}

void CPropertySheet::tabWidget_currentChanged(int idx)
{
   if ( (idx >= 0) && (idx < _qtabwidget->count()) )
   {
      _pages.at(idx)->OnSetActive();
   }
}

void CPropertySheet::ok_clicked()
{
   // Apply changes and exit dialog.
   apply_clicked();
   _qtd->accept();
}

void CPropertySheet::cancel_clicked()
{
   _qtd->reject();
}

void CPropertySheet::apply_clicked()
{
   int idx;
   
   // Apply changes from all tabs.
   for ( idx = 0; idx < _qtabwidget->count(); idx++ )
   {
      _pages.at(idx)->OnApply();
   }
}

void CPropertySheet::AddPage(
   CPropertyPage *pPage 
)
{
   qtMfcInitDialogResource(pPage->GetDlgCtrlID(),pPage);
   
   _qtabwidget->blockSignals(true);
   CString windowText;
   pPage->GetWindowText(windowText);
#if UNICODE
   _qtabwidget->addTab(pPage->toQWidget(),QString::fromWCharArray((LPCTSTR)windowText));
#else
   _qtabwidget->addTab(pPage->toQWidget(),QString::fromAscii((LPCTSTR)windowText));
#endif
   _qtabwidget->blockSignals(false);
   _pages.append(pPage);
   
   pPage->OnInitDialog();
}

INT_PTR CPropertySheet::DoModal( )
{
   INT_PTR result;
   _qtabwidget->setCurrentIndex(_selectedPage);
   _pages.at(_selectedPage)->OnSetActive();
   SetFocus();
   result = _qtd->exec();
   if ( result == QDialog::Accepted )
      return 1;
   else
      return 0;
}

CPropertyPage::CPropertyPage( 
   UINT nIDTemplate, 
   UINT nIDCaption, 
   DWORD dwSize 
)
{
   m_hWnd = (HWND)this;
   _id = nIDTemplate;
   
   if ( _qt )
      delete _qt;
   
   _grid = NULL;
   
   _qt = new QDialog;
   
   _qtd = dynamic_cast<QDialog*>(_qt);
   _inited = false;
   
   _qt->installEventFilter(this);
   
   // Pass-through signals
}

CPropertyPage::~CPropertyPage()
{
}

void CPropertyPage::SetModified(
   BOOL bChanged 
)
{
   qDebug("CPropertyPage::SetModified");
}

BOOL CPropertyPage::OnApply( )
{
   qDebug("CPropertyPage::OnApply");
   return TRUE;
}

BOOL CPropertyPage::OnSetActive( )
{
   qDebug("CPropertyPage::OnSetActive");
   return TRUE;
}

CToolTipCtrl::CToolTipCtrl( )
{
   // nothing to do here...
}

BOOL CToolTipCtrl::Create(
   CWnd* pParentWnd,
      DWORD dwStyle
)
{
   // nothing to do here...
}

void CToolTipCtrl::Activate( 
   BOOL bActivate  
)
{
   // nothing to do here...
}

BOOL CToolTipCtrl::AddTool(
   CWnd* pWnd,
   UINT nIDText,
   LPCRECT lpRectTool,
   UINT_PTR nIDTool 
)
{
   _tippers.append(pWnd);
   pWnd->toQWidget()->setToolTip(qtMfcStringResource(nIDText));
}

BOOL CToolTipCtrl::AddTool(
   CWnd* pWnd,
   LPCTSTR lpszText,
   LPCRECT lpRectTool,
   UINT_PTR nIDTool
)
{
   _tippers.append(pWnd);
   pWnd->toQWidget()->setToolTip(lpszText);
}

void CToolTipCtrl::RelayEvent(
   LPMSG lpMsg 
)
{
   // nothing to do here...
}


int EnumFontFamiliesEx(
   HDC hdc,
   LPLOGFONT lpLogfont,
   FONTENUMPROC lpEnumFontFamExProc,
   LPARAM lParam,
   DWORD dwFlags
)
{
   QFontDatabase database;
   ENUMLOGFONTEX elfe;
   NEWTEXTMETRICEX ntme;
   
   foreach ( QString family, database.families() )
   {
      memset(&ntme,0,sizeof(ntme));
      memset(&elfe,0,sizeof(ntme));
      strcpy((char*)elfe.elfFullName,family.toAscii().constData());
      lpEnumFontFamExProc((LOGFONT*)&elfe,NULL,0,lParam);
   }
}

void openFile(QString fileName)
{
   if ( fileName.isEmpty() )
   {
      ptrToTheApp->OpenDocumentFile(NULL);
   }
   else
   {
      ptrToTheApp->OpenDocumentFile(CString(fileName));
   }
}
