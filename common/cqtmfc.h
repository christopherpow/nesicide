#ifndef CQTMFC_H
#define CQTMFC_H

#include <QApplication>
#include <QDialogButtonBox>
#include <QMainWindow>
#include <QShortcut>
#include <QAction>
#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QPen>
#include <QBitmap>
#include <QBrush>
#include <QSize>
#include <QStatusBar>
#include <QToolBar>
#include <QPixmap>
#include <QFont>
#include <QRegion>
#include <QFrame>
#include <QAbstractButton>
#include <QRadioButton>
#include <QComboBox>
#include <QProgressBar>
#include <QClipboard>
#include <QScrollBar>
#include <QEvent>
#include <QList>
#include <QHash>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimerEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QObject>
#include <QThread>
#include <QFile>
#include <QMutex>
#include <QByteArray>
#include <QGridLayout>
#include <QMimeData>
#include <QElapsedTimer>
#include <QSharedMemory>
#include <QSettings>
#include <QMutex>
#include <QSemaphore>
#include <QListWidget>
#include <QDialog>
#include <QMenu>
#include <QListWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QMenuBar>
#include <QToolButton>
#include <QLibrary>
#include <QToolTip>

#ifndef QT_NO_DEBUG
//#define _DEBUG
#endif

// Define resources here that are "hidden under the hood" of MFC...
enum
{
   __UNDER_THE_HOOD_START = 0x8000000,
   
   AFX_IDS_ALLFILTER,
   AFX_IDS_OPENFILE,
   
   WM_SIZEPARENT,
   WM_INITIALUPDATE,
   
   IDC_STATIC,

   AFX_IDW_PANE_FIRST,
   AFX_IDW_STATUS_BAR,
   AFX_IDW_TOOLBAR,
   AFX_IDW_REBAR,
   
   ID_SEPARATOR,
   
//   STRINGTABLE 
//   BEGIN
   AFX_IDS_APP_TITLE       ,
   AFX_IDS_IDLEMESSAGE     ,
   AFX_IDS_HELPMODEMESSAGE,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_INDICATOR_EXT        ,
   ID_INDICATOR_CAPS       ,
   ID_INDICATOR_NUM        ,
   ID_INDICATOR_SCRL       ,
   ID_INDICATOR_OVR        ,
   ID_INDICATOR_REC        ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_FILE_NEW             ,
   ID_FILE_OPEN            ,
   ID_FILE_CLOSE           ,
   ID_FILE_SAVE            ,
   ID_FILE_SAVE_AS         ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_APP_ABOUT            ,
   ID_APP_EXIT             ,
   ID_HELP_INDEX           ,
   ID_HELP_FINDER          ,
   ID_HELP_USING           ,
   ID_CONTEXT_HELP         ,
   ID_HELP                 ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_FILE_MRU_FILE1       ,
   ID_FILE_MRU_FILE2       ,
   ID_FILE_MRU_FILE3       ,
   ID_FILE_MRU_FILE4       ,
   ID_FILE_MRU_FILE5       ,
   ID_FILE_MRU_FILE6       ,
   ID_FILE_MRU_FILE7       ,
   ID_FILE_MRU_FILE8       ,
   ID_FILE_MRU_FILE9       ,
   ID_FILE_MRU_FILE10      ,
   ID_FILE_MRU_FILE11      ,
   ID_FILE_MRU_FILE12      ,
   ID_FILE_MRU_FILE13      ,
   ID_FILE_MRU_FILE14      ,
   ID_FILE_MRU_FILE15      ,
   ID_FILE_MRU_FILE16      ,
//   END

//   STRINGTABLE 
//   BEGIN
   ID_NEXT_PANE            ,
   ID_PREV_PANE            ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_WINDOW_SPLIT         ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_EDIT_CLEAR           ,
   ID_EDIT_CLEAR_ALL       ,
   ID_EDIT_COPY            ,
   ID_EDIT_CUT             ,
   ID_EDIT_FIND            ,
   ID_EDIT_PASTE           ,
   ID_EDIT_REPEAT          ,
   ID_EDIT_REPLACE         ,
   ID_EDIT_SELECT_ALL      ,
   ID_EDIT_UNDO            ,
   ID_EDIT_REDO            ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_VIEW_TOOLBAR         ,
   ID_VIEW_STATUS_BAR      ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   AFX_IDS_SCSIZE          ,
   AFX_IDS_SCMOVE          ,
   AFX_IDS_SCMINIMIZE      ,
   AFX_IDS_SCMAXIMIZE      ,
   AFX_IDS_SCNEXTWINDOW    ,
   AFX_IDS_SCPREVWINDOW    ,
   AFX_IDS_SCCLOSE         ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   AFX_IDS_SCRESTORE       ,
   AFX_IDS_SCTASKLIST      ,
//   END
};

// Releasing pointers
#define SAFE_RELEASE(p) \
        if (p != NULL) { \
                delete p;       \
                p = NULL;       \
        }       \

#define SAFE_RELEASE_ARRAY(p) \
        if (p != NULL) { \
                delete [] p;    \
                p = NULL;       \
        }       \


#if defined(Q_WS_WIN) || defined(Q_WS_WIN32)
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) > (b)) ? (b) : (a))
#endif

// workaround to force ignore ms_abi errors, not needed as long as we don't link with other mfc implementations
// also g++ doesn't have __has_attribute()
#if !defined(Q_WS_WIN) && !defined(Q_WS_WIN32) && !defined(__GNUC__) && !defined(__GNUG__)
#if !__has_attribute(ms_abi)
#define ms_abi
#endif
#endif

#include <windows.h>
#include <prsht.h>

#define stricmp strcasecmp

#if !defined(Q_WS_WIN) && !defined(Q_WS_WIN32)
#define _MAX_PATH 256
#endif

// special struct for WM_SIZEPARENT
struct AFX_SIZEPARENTPARAMS
{
//    HDWP hDWP;       // handle for DeferWindowPos
    RECT rect;       // parent client rectangle (trim as appropriate)
    SIZE sizeTotal;  // total size on each side as layout proceeds
    BOOL bStretch;   // should stretch to fill all space
};

#if UNICODE
typedef LPCWSTR LPCTSTR; 
typedef wchar_t TCHAR;
#define _T(x) L##x
#else
typedef LPCSTR LPCTSTR;
typedef char TCHAR;
#define _T(x) x
#endif 
#if !defined(TRACE0)
#define TRACE0(x) { QString str; str.sprintf("TRACE0: %s(%d): %s",__FILE__,__LINE__, (x)); qDebug(str.toLatin1().constData()); }
#endif
#if !defined(TRACE)
#define TRACE(x) { QString str; str.sprintf("TRACE0: %s(%d): %s",__FILE__,__LINE__, (x)); qDebug(str.toLatin1().constData()); }
#endif
#if !defined(ATLTRACE2)
#define ATLTRACE2(a,b,str,q...) qDebug(str,##q)
#endif

#define VERIFY(x) x
#define ENSURE(x) x

TCHAR* A2T(char* str);
char* T2A(TCHAR* str);

typedef int* POSITION;

#define DECLARE_DYNCREATE(x) 
#define IMPLEMENT_DYNCREATE(x,y)
#define DECLARE_DYNAMIC(x)
#define IMPLEMENT_DYNAMIC(x,y)
#define DECLARE_MESSAGE_MAP() QHash<int,void*> _messageMap;
   
#define BEGIN_MESSAGE_MAP(dc,bc)
#define END_MESSAGE_MAP() 

#define RUNTIME_CLASS(x) new x

#define AFX_MSG_CALL 

class CCmdTarget;
typedef struct
{
//   void( AFX_MSG_CALL CCmdTarget::* )( void ) 	pmf;
   CCmdTarget* 	pTarget;
} AFX_CMDHANDLERINFO;

#define afx_msg

#if UNICODE
#define _tcscpy_s(d,l,s) wcsncpy((char*)d,(const char*)s,l)
#else
#define _tcscpy_s(d,l,s) strncpy((char*)d,(const char*)s,l)
#endif
#define strcpy_s(d,l,s) strncpy((char*)d,(const char*)s,l)
#define vsprintf_s(b,n,f,v) vsprintf(b,f,v)
#if UNICODE
#define _ttoi _wtoi
#define _tstoi _wtoi
#define _tcslen wcslen
#define _tcscmp wcscmp
#define _tcsicmp wcsicmp
#define _stscanf wscanf
#else
#define _ttoi atoi
#define _tstoi atoi
#define _tcslen strlen
#define _tcscmp strcmp
#define _tcsicmp stricmp
#define _stscanf sscanf
#endif
#ifdef QT_NO_DEBUG
#define ASSERT(y)
#define ASSERT_VALID(y)
#else
#define ASSERT(y) { if (!(y)) { QString str; str.sprintf("ASSERT: %s(%d)",__FILE__,__LINE__); qFatal(str.toLatin1().constData()); } }
#define ASSERT_VALID(y) { if (!(y)) { QString str; str.sprintf("ASSERT: %s(%d)",__FILE__,__LINE__); qFatal(str.toLatin1().constData()); } }
#endif

#define ENSURE_VALID(x)

#define AFXAPI

int AfxMessageBox(
   LPCTSTR lpszText,
   UINT nType = MB_OK,
   UINT nIDHelp = 0 
);

int AFXAPI AfxMessageBox(
   UINT nIDPrompt,
   UINT nType = MB_OK,
   UINT nIDHelp = (UINT) -1 
);

HCURSOR WINAPI SetCursor(
   HCURSOR hCursor
);

BOOL WINAPI GetWindowRect(
   HWND hWnd,
   LPRECT lpRect
);

UINT WINAPI RegisterClipboardFormat(
   LPCTSTR lpszFormat
);

DWORD WINAPI GetFileAttributes(
   LPCTSTR lpFileName
);

LONG WINAPI GetWindowLong(
   HWND hWnd,
   int nIndex
);

int WINAPI GetKeyNameText(
   LONG lParam,
   LPTSTR lpString,
   int cchSize
);

#if !defined(Q_WS_WIN) && !defined(Q_WS_WIN32)
HMODULE WINAPI LoadLibrary(
   LPCTSTR lpFileName
);

FARPROC WINAPI GetProcAddress(
   HMODULE hModule,
   LPCSTR lpProcName
);

BOOL WINAPI FreeLibrary(
   HMODULE hModule
);
#endif

HANDLE WINAPI CreateEvent(
   LPSECURITY_ATTRIBUTES lpEventAttributes,
   BOOL bManualReset,
   BOOL bInitialState,
   LPCTSTR lpName
);

BOOL WINAPI SetEvent(
   HANDLE hEvent
);

BOOL WINAPI ResetEvent(
   HANDLE hEvent
);

BOOL WINAPI PulseEvent(
   HANDLE hEvent
);

BOOL WINAPI GetCursorPos(
   LPPOINT lpPoint
);

DWORD WINAPI GetModuleFileName(
   HMODULE hModule,
   LPTSTR lpFilename,
   DWORD nSize
);

BOOL PathRemoveFileSpec(
   LPTSTR pszPath
);

BOOL PathAppend(
   LPTSTR pszPath,
   LPCTSTR pszMore
);

int MulDiv(
  int nNumber,
  int nNumerator,
  int nDenominator
);
DWORD WINAPI GetTickCount(void);
DWORD WINAPI GetSysColor(
  int nIndex
);
int WINAPI GetSystemMetrics(
  int nIndex
);
BOOL WINAPI IsClipboardFormatAvailable(
  UINT format
);
BOOL WINAPI OpenClipboard(
//  HWND hWndNewOwner = 0
);
BOOL WINAPI EmptyClipboard(void);
BOOL WINAPI CloseClipboard(void);
HANDLE WINAPI SetClipboardData(
  UINT uFormat,
  HANDLE hMem
);
HANDLE WINAPI GetClipboardData(
  UINT uFormat
);
HGLOBAL WINAPI GlobalAlloc(
  UINT uFlags,
  SIZE_T dwBytes
);
LPVOID WINAPI GlobalLock(
  HGLOBAL hMem
);
BOOL WINAPI GlobalUnlock(
  HGLOBAL hMem
);
SIZE_T WINAPI GlobalSize(
  HGLOBAL hMem
);

extern ACCEL* _acceleratorTbl;

HACCEL WINAPI CreateAcceleratorTable(
  LPACCEL lpaccl,
  int cEntries
);

int WINAPI TranslateAccelerator(
  HWND hWnd,
  HACCEL hAccTable,
  LPMSG lpMsg
);

BOOL WINAPI DestroyAcceleratorTable(
  HACCEL hAccel
);

#define MAPVK_VK_TO_VSC    0
#define MAPVK_VSC_TO_VK    1
#define MAPVK_VK_TO_CHAR   2
#define MAPVK_VSC_TO_VK_EX 3

UINT WINAPI MapVirtualKey(
  UINT uCode,
  UINT uMapType
);

class CDumpContext
{
};

class CObject
{
public:
   CObject() {}
   virtual ~CObject() {}
   virtual void DeleteObject() {}
};

class CCmdTarget : public CObject
{
public:
   CCmdTarget() {}
   virtual ~CCmdTarget() {}
   virtual BOOL OnCmdMsg(
      UINT nID,
      int nCode,
      void* pExtra,
      AFX_CMDHANDLERINFO* pHandlerInfo 
   );
};

class CSyncObject
{
public:
   CSyncObject() : m_hObject(this) {}
   virtual ~CSyncObject() {}
   virtual BOOL Lock(
      DWORD dwTimeout = INFINITE 
   ) { return TRUE; }
   virtual BOOL Unlock( )
   { return TRUE; }
   virtual BOOL Unlock(
      LONG lCount,
      LPLONG lpPrevCount = NULL 
   ) { return TRUE; }
   HANDLE m_hObject;
};

class CCriticalSection : public CSyncObject
{
public:
   CCriticalSection();
   virtual ~CCriticalSection();
   BOOL Lock(
      DWORD dwTimeout = INFINITE 
   );
   BOOL Unlock( ); 
protected:
   QMutex* _qtd;
};

class CMutex : public CSyncObject
{
public:
   CMutex(
      BOOL bInitiallyOwn = FALSE,
      LPCTSTR lpszName = NULL,
      LPSECURITY_ATTRIBUTES lpsaAttribute = NULL 
   );
   virtual ~CMutex();
   BOOL Lock(
      DWORD dwTimeout = INFINITE 
   );
   BOOL Unlock( ); 
protected:
   QMutex* _qtd;
};

class CSemaphore : public CSyncObject
{
};

class CEvent : public CSyncObject
{
public:
   CEvent(
      BOOL bInitiallyOwn = FALSE,
      BOOL bManualReset = FALSE,
      LPCTSTR lpszName = NULL,
      LPSECURITY_ATTRIBUTES lpsaAttribute = NULL 
   );
   virtual ~CEvent();
   BOOL SetEvent();
   BOOL ResetEvent();
   BOOL PulseEvent();
};

class CString;
BOOL operator==(const CString& s1, const LPCTSTR s2);
BOOL operator!=(const CString& s1, const LPCTSTR s2);
BOOL operator <( const CString& s1, const CString& s2 );

class CString
{
public:
   CString();
   CString(const CString& ref);
   CString(QString str);
   CString(LPCTSTR str);
   virtual ~CString();
   
   BOOL LoadString( UINT nID );
   
   void UpdateScratch();

   CString& Append(LPCSTR str);
   CString& Append(LPWSTR str);
   void AppendFormat(LPCTSTR fmt, ...);
   void AppendFormatV(LPCTSTR fmt, va_list ap);
   void Format( UINT nFormatID, ... );
   void Format(LPCTSTR fmt, ...);
   void FormatV(LPCTSTR fmt, va_list ap);
   void Truncate(int length);
   int ReverseFind( TCHAR ch ) const;
   int Compare( LPCTSTR lpsz ) const;
 
   CString& operator=(const CString& str);
   CString& operator=(LPCTSTR str);
   CString& operator=(QString str);
   CString& operator+(const CString& str);
   CString& operator+(LPTSTR str);
   CString& operator+(LPCTSTR str);
   CString& operator+(QString str);
   CString& operator+=(const CString& str);
   CString& operator+=(LPCTSTR str);
   CString& operator+=(QString str);
   operator QString() const;
   operator const TCHAR*() const;
   
   void Empty();
   LPCTSTR GetString() const;
   LPTSTR GetBuffer( int nMinBufLength = 0 );
   void ReleaseBuffer( int nNewLength = -1 );
   int Find( TCHAR ch ) const;   
   int Find( LPCTSTR lpszSub ) const;
   int Find( TCHAR ch, int nStart ) const;
   int Find( LPCTSTR pstr, int nStart ) const;
   CString Left( int nCount ) const;
   CString Right( int nCount ) const;
   CString Mid( int nFirst ) const;  
   CString Mid( int nFirst, int nCount ) const;
   CString MakeUpper( );
   CString MakeLower( );
   int GetLength() const;
   int CompareNoCase( LPCTSTR lpsz ) const;
   TCHAR GetAt( int nIndex ) const;
   
protected:
   QString _qstr;
   QByteArray _qstrn;
};

class CStringA : public CString
{
public:
   CStringA(CString str);
   operator char*() const;
};

class CStringArray
{
public:
   void RemoveAll( );
   INT_PTR Add( LPCTSTR newElement );
   CString GetAt(int idx) const;
   void SetAt(int idx, CString str);
   INT_PTR GetCount( ) const;
   CString operator []( INT_PTR nIndex );  
   CString operator []( INT_PTR nIndex ) const; 
   BOOL IsEmpty( ) const;
private:
   QList<CString> _qlist;
};

class CException
{
public:
   virtual BOOL GetErrorMessage(
      LPTSTR lpszError,
      UINT nMaxError,
      PUINT pnHelpContext = NULL
   ) const
   {
      lpszError[0] = 0;
      return false;
   }
};

class CFileException : public CException
{
};

class CFile : public CCmdTarget
{
public:
   enum
   {
      modeCreate = 0x01,
      modeRead = 0x02,
      modeWrite = 0x04,
      shareDenyWrite = 0x08
   };
   enum
   {
      begin = 1,
      current = 2,
      end = 3
   };
   CFile();
   CFile(
      LPCTSTR lpszFileName,
      UINT nOpenFlags 
   );
   virtual ~CFile();

   virtual BOOL Open(
      LPCTSTR lpszFileName,
      UINT nOpenFlags,
      CFileException* pError = NULL
   );
   virtual ULONGLONG GetLength( ) const;
   virtual void Write(
      const void* lpBuf,
      UINT nCount
   );
   virtual ULONGLONG GetPosition( ) const;
   virtual UINT Read(
      void* lpBuf,
      UINT nCount
   );
   virtual ULONGLONG Seek(
      LONGLONG lOff,
      UINT nFrom 
   );
   virtual void Close();

private:
   QFile _qfile;
};

class CPoint : public tagPOINT
{
public:
   CPoint()
   {
      x = 0;
      y = 0;
   }
   CPoint(QPoint point)
   {
      x = point.x();
      y = point.y();
   }
   CPoint(int _x, int _y)
   {
      x = _x;
      y = _y;
   }
   void SetPoint(int _x, int _y)
   {
      x = _x;
      y = _y;
   }
   bool operator==(CPoint p)
   {
      return ((p.x == x) && (p.y == y));
   }
};

class CSize : public tagSIZE
{
public:
   CSize( ) { cx = 0; cy = 0; } 
   CSize( 
      int initCX, 
      int initCY  
   ) { cx = initCX; cy = initCY; }
   CSize( 
      SIZE initSize  
   ) { cx = initSize.cx; cy = initSize.cy; }
   CSize( 
      POINT initPt  
   ) { cx = initPt.x; cy = initPt.y; }
   CSize( 
      DWORD dwSize  
   ) { cx = dwSize&0xFFFF; cy = (dwSize>>16); }
   CSize(QSize qSize) { cx = qSize.width(); cy = qSize.height(); }
};

class CRect : public tagRECT
{
public:
   CRect( ); 
   CRect( 
      int l, 
      int t, 
      int r, 
      int b  
   );
   CRect( 
      const RECT& srcRect  
   );
   CRect( 
      LPCRECT lpSrcRect  
   );
   CRect( 
      POINT point, 
      SIZE size  
   );
   CRect( 
      POINT topLeft, 
      POINT bottomRight  
   );
   int Width() const { return right-left; }
   int Height() const { return bottom-top; }
   void MoveToX( 
      int x  
   );
   void MoveToY(
      int y
   );
   void MoveToXY(
      int x,
      int y 
   );
   void MoveToXY(
      POINT point 
   );   
   void DeflateRect( 
      int x, 
      int y  
   );
   void DeflateRect( 
      SIZE size  
   );
   void DeflateRect( 
      LPCRECT lpRect  
   );
   void DeflateRect( 
      int l, 
      int t, 
      int r, 
      int b  
   );
   void InflateRect( 
      int x, 
      int y  
   );
   void InflateRect( 
      SIZE size  
   );
   void InflateRect( 
      LPCRECT lpRect  
   );
   void InflateRect( 
      int l, 
      int t, 
      int r, 
      int b  
   );
   operator LPRECT() const
   {
      return (RECT*)this;
   }
   operator LPCRECT() const
   {
      return (const RECT*)this;
   }
   operator QRect() const
   {
      return QRect(left,top,right-left,bottom-top);
   }
};

class CGdiObject : public CObject
{
public:
   CGdiObject() : m_hObject((HGDIOBJ)this) {}
   virtual ~CGdiObject() {}
   operator HGDIOBJ() const
   {
      return m_hObject;
   }
   HGDIOBJ m_hObject;
};

class CPen : public CGdiObject
{
public:
   CPen( );
   CPen(
      int nPenStyle,
      int nWidth,
      COLORREF crColor 
   );
   CPen(
      int nPenStyle,
      int nWidth,
      const LOGBRUSH* pLogBrush,
      int nStyleCount = 0,
      const DWORD* lpStyle = NULL 
   );
   virtual ~CPen() {}
   BOOL CreatePen(
      int nPenStyle,
      int nWidth,
      COLORREF crColor 
   );
   operator QPen() const
   {
      return _qpen;
   }
   operator HPEN() const
   {
      return (HPEN)this;
   }

private:
   QPen _qpen;
};

class CDC;
class CBitmap : public CGdiObject
{
   // Qt interfaces
public:
   CBitmap(QString resource);
   QPixmap* toQPixmap() { return _qpixmap; }
   
   // MFC interfaces
public:
   CBitmap();
   virtual ~CBitmap();
   BOOL LoadBitmap(
      UINT nIDResource 
   );
   BOOL CreateBitmap(
      int nWidth,
      int nHeight,
      UINT nPlanes,
      UINT nBitcount,
      const void* lpBits 
   );
   BOOL CreateCompatibleBitmap(
      CDC* pDC,
      int nWidth,
      int nHeight 
   );
   CSize SetBitmapDimension(
      int nWidth,
      int nHeight 
   );
   CSize GetBitmapDimension( ) const;
   operator QPixmap() const
   {
      return *_qpixmap;
   }
   operator HBITMAP() const
   {
      return (HBITMAP)this;
   }
private:
   QPixmap* _qpixmap;
   bool     _owned;
};

class CBrush : public CGdiObject
{
public:
   CBrush( );
   CBrush(
      COLORREF crColor 
   );
   CBrush(
      int nIndex,
      COLORREF crColor 
   );
   explicit CBrush(
      CBitmap* pBitmap 
   );
   virtual ~CBrush() {}
   BOOL CreateSolidBrush(
      COLORREF crColor 
   );
   operator QBrush() const
   {
      return _qbrush;
   }
   operator HBRUSH() const
   {
      return (HBRUSH)this;
   }
private:
   QBrush _qbrush;
};

class CFont : public CGdiObject
{
public:
   CFont() {}
   virtual ~CFont() {}
   BOOL CreateFont(
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
   );
   BOOL CreateFontIndirect(
      const LOGFONT* lpLogFont 
   );
   operator QFont() const
   {
      return _qfont;
   }
   operator HFONT() const
   {
      return (HFONT)this;
   }
private:
   QFont _qfont;
};

class CRgn : public CGdiObject
{
public:
   CRgn() {}
   virtual ~CRgn() {}
   operator QRegion() const
   {
      return _qregion;
   }
   operator HRGN() const
   {
      return (HRGN)this;
   }
private:
   QRegion _qregion;
};

class CWnd;

class CDC : public CObject
{
public:
   CDC();
   CDC(CWnd* parent);
   virtual ~CDC();

   void attach();
   void attach(QWidget* parent);
   void detach();
   void flush();
   void doFlush(bool doIt) { _doFlush = doIt; }
   QPainter* painter() { return _qpainter; }
   QPixmap* pixmap() { return _qpixmap; }
   QSize pixmapSize() { return _bitmapSize; }
   QWidget* widget() { return _qwidget; }
   QPixmap* bitmap() { return _bitmap?_bitmap->toQPixmap():NULL; }
   
   BOOL CreateCompatibleDC(
      CDC* pDC 
   );
   BOOL DeleteDC( );
   BOOL DrawEdge(
      LPRECT lpRect,
      UINT nEdge,
      UINT nFlags 
   );
   BOOL BitBlt(
      int x,
      int y,
      int nWidth,
      int nHeight,
      CDC* pSrcDC,
      int xSrc,
      int ySrc,
      DWORD dwRop 
   );
   COLORREF GetPixel(
      int x,
      int y 
   ) const;
   COLORREF GetPixel(
      POINT point 
   ) const;
   BOOL Rectangle(
      int x1,
      int y1,
      int x2,
      int y2 
   );
   BOOL Rectangle(
      LPCRECT lpRect 
   );
   void Draw3dRect( LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight )
   {
      Draw3dRect(lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top,clrTopLeft,clrBottomRight);
   }
   void Draw3dRect( int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight );
   virtual int DrawText(
      LPCTSTR lpszString,
      int nCount,
      LPRECT lpRect,
      UINT nFormat 
   );
   int DrawText(
      const CString& str,
      LPRECT lpRect,
      UINT nFormat 
   );
   void FillSolidRect(
      LPCRECT lpRect,
      COLORREF clr 
   );
   void FillSolidRect(
      int x,
      int y,
      int cx,
      int cy,
      COLORREF clr 
   );
   int GetDeviceCaps(
      int nIndex 
   ) const;
   BOOL GradientFill( 
      TRIVERTEX* pVertices, 
      ULONG nVertices, 
      void* pMesh, 
      ULONG nMeshElements, 
      DWORD dwMode  
   );   
   BOOL LineTo( 
      int x, 
      int y  
   ); 
   BOOL LineTo( 
      POINT point  
   )
   {
      return LineTo(point.x,point.y);
   }

   CPoint MoveTo( 
      int x, 
      int y  
   )
   {
      CPoint old = _lineOrg;
      _lineOrg.x = x;
      _lineOrg.y = y;
      return old;
   }
   CPoint MoveTo( 
      POINT point  
   )
   {
      return MoveTo(point.x,point.y);
   }

   CPoint OffsetWindowOrg( 
      int nWidth, 
      int nHeight  
   )
   {
      CPoint old = _windowOrg;
      _windowOrg.x += nWidth;
      _windowOrg.y += nHeight;
      return old;
   }
   BOOL Polygon(
      LPPOINT lpPoints,
      int nCount 
   );   
   
   HGDIOBJ SelectObject(
      HGDIOBJ obj
   );
   CPen* SelectObject(
      CPen* pPen 
   );
   CBrush* SelectObject(
      CBrush* pBrush 
   );
   virtual CFont* SelectObject(
      CFont* pFont 
   );
   CBitmap* SelectObject(
      CBitmap* pBitmap 
   );
   int SelectObject(
      CRgn* pRgn 
   );
   CGdiObject* SelectObject(
      CGdiObject* pObject
   );
   CObject* SelectObject(
      CObject* pObject
   );
   COLORREF SetBkColor( 
      COLORREF crColor  
   )
   {
      COLORREF old = _bkColor.red()|(_bkColor.green()<<8)|_bkColor.blue()<<16;
      _bkColor = QColor(GetRValue(crColor),GetGValue(crColor),GetBValue(crColor));
      return old;
   }
   int SetBkMode( 
      int nBkMode  
   )
   {
      int old = _bkMode;
      _bkMode = nBkMode;
      return old;
   }
   COLORREF SetPixel( int x, int y, COLORREF crColor );
   COLORREF SetPixel( POINT point, COLORREF crColor )
   {
      return SetPixel(point.x,point.y,crColor);
   }

   COLORREF SetTextColor( 
      COLORREF crColor  
   )
   {
      COLORREF old = _textColor.red()|(_textColor.green()<<8)|_textColor.blue()<<16;
      _textColor = QColor(GetRValue(crColor),GetGValue(crColor),GetBValue(crColor));
      return old;
   }
   virtual CPoint SetViewportOrg( 
      int x, 
      int y  
   )
   {
      CPoint old = _viewportOrg;
      _viewportOrg.x = x;
      _viewportOrg.y = y;
      return old;
   }
   CPoint SetViewportOrg( 
      POINT point  
   )
   {
      return SetViewportOrg(point.x,point.y);
   }
   CPoint SetWindowOrg(
      int x,
      int y 
   )
   {
      CPoint old = _windowOrg;
      _windowOrg.x = x;
      _windowOrg.y = y;
      return old;
   }
   CPoint SetWindowOrg(
      POINT point 
   )
   {
      return SetWindowOrg(point.x,point.y);
   }

   BOOL TextOut(
      int x,
      int y,
      LPCTSTR lpszString,
      int nCount 
   );
   BOOL TextOut(
      int x,
         int y,
         const CString& str 
   );
   
public:
   HDC         m_hDC;
   
private:
   CDC(CDC& orig);
   bool attached;
   bool _doFlush;
   QWidget*    _qwidget;
   QPixmap*    _qpixmap;
   QPainter*   _qpainter;
   CPen*       _pen;
   CBrush*     _brush;
   CFont*      _font;
   CBitmap*    _bitmap;
   QSize       _bitmapSize;
   CRgn*       _rgn;   
   CGdiObject* _gdiobject;
   CObject*    _object;
   CPoint      _lineOrg;
   QColor      _bkColor;
   int         _bkMode;
   QColor      _textColor;
   CPoint      _windowOrg;
   CPoint      _viewportOrg;
};

class CPaintDC : public CDC
{
public:
   CPaintDC(CWnd* parent) : CDC(parent) {}
};

class CDataExchange
{
};

class QtUIElement
{
public:
   virtual void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   ) {}
   virtual UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const { return 0; }
   virtual void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   ) {}
   virtual int GetDlgItemText(
      int nID,
      CString& rString 
   ) const { return 0; }
   virtual int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const { return 0; }
   virtual void CheckDlgButton( 
      int nIDButton, 
      UINT nCheck  
   ) {}
   virtual UINT IsDlgButtonChecked( 
      int nIDButton
   ) const { return 0; }
   virtual void SetWindowText(
      LPCTSTR lpszString 
   ) {}
   virtual int GetWindowTextLength( ) const { return 0; }
   virtual void GetWindowText(
      CString& rString 
   ) const {}
   virtual int GetWindowText(
      LPTSTR lpszStringBuf,
      int nMaxCount 
   ) const { return 0; }
};

class CFrameWnd;
class CScrollBar;
class CMenu;

class CWnd : public QWidget, public CCmdTarget, public QtUIElement
{
   Q_OBJECT
   // MFC interfaces
public:
   CWnd(CWnd* parent=0);
   virtual ~CWnd();
   operator HWND() { return m_hWnd; }
   DWORD GetStyle() const { return 0; }
   void SetOwner(
      CWnd* pOwnerWnd 
   );
   CMenu* GetMenu( ) const { return NULL; }
   BOOL EnableToolTips(
      BOOL bEnable = TRUE
   );
   virtual LRESULT SendMessage(
      UINT message,
      WPARAM wParam = 0,
      LPARAM lParam = 0 
   );
   void SendMessageToDescendants(
      UINT message,
      WPARAM wParam = 0,
      LPARAM lParam = 0,
      BOOL bDeep = TRUE,
      BOOL bOnlyPerm = FALSE 
   );
   BOOL IsWindowVisible( ) const;
   virtual BOOL EnableWindow(
      BOOL bEnable = TRUE 
   );
   virtual BOOL PreCreateWindow(
      CREATESTRUCT& cs 
   ) { return TRUE; }
   enum
   {
      reposDefault,
      reposQuery,
      reposExtra
   };
   void UpdateDialogControls(
      CCmdTarget* pTarget,
      BOOL bDisableIfNoHndler 
   );
   void RepositionBars(
      UINT nIDFirst,
      UINT nIDLast,
      UINT nIDLeftOver,
      UINT nFlag = reposDefault,
      LPRECT lpRectParam = NULL,
      LPCRECT lpRectClient = NULL,
      BOOL bStretch = TRUE 
   );
   void DragAcceptFiles(
      BOOL bAccept = TRUE 
   );
   virtual BOOL CreateEx(
      DWORD dwExStyle,
      LPCTSTR lpszClassName,
      LPCTSTR lpszWindowName,
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID,
      LPVOID lpParam = NULL
   );
   virtual BOOL PreTranslateMessage(
      MSG* pMsg 
   ) { return FALSE; }
   void MapWindowPoints(
      CWnd* pwndTo,
      LPRECT lpRect 
   ) const;
   void MapWindowPoints(
      CWnd* pwndTo,
      LPPOINT lpPoint,
      UINT nCount 
   ) const;
   virtual CScrollBar* GetScrollBarCtrl(
      int nBar 
   ) const;
   BOOL SetScrollInfo(
      int nBar,
      LPSCROLLINFO lpScrollInfo,
      BOOL bRedraw = TRUE 
   );
   void SetScrollRange(
      int nBar,
      int nMinPos,
      int nMaxPos,
      BOOL bRedraw = TRUE 
   );
   int SetScrollPos(
      int nBar,
      int nPos,
      BOOL bRedraw = TRUE 
   );
   virtual afx_msg int OnCreate(
      LPCREATESTRUCT lpCreateStruct 
   );
   void OnDestroy( );
   void OnMouseMove(UINT,CPoint) {}
   void OnNcMouseMove(UINT nHitTest, CPoint point) {}
   void OnNcLButtonUp(
      UINT nHitTest,
      CPoint point 
   ) {}   
   BOOL OnEraseBkgnd(
      CDC* pDC 
   ) { return TRUE; }
   HBRUSH OnCtlColor(
      CDC* pDC,
      CWnd* pWnd,
      UINT nCtlColor 
   ) { return (HBRUSH)NULL; }
   afx_msg void OnPaint( ) {}
   virtual BOOL OnCmdMsg(
      UINT nID,
      int nCode,
      void* pExtra,
      AFX_CMDHANDLERINFO* pHandlerInfo 
   ) { return FALSE; }
   virtual BOOL OnNotify( 
      WPARAM wParam, 
      LPARAM lParam, 
      LRESULT* pResult  
   ) { return FALSE; }
   void OnLButtonDblClk(UINT,CPoint) {}
   void OnLButtonDown(UINT,CPoint) {}
   void OnLButtonUp(UINT,CPoint) {}
   void OnMButtonDblClk(UINT,CPoint) {}
   void OnMButtonDown(UINT,CPoint) {}
   void OnMButtonUp(UINT,CPoint) {}
   void OnRButtonDblClk(UINT,CPoint) {}
   void OnRButtonDown(UINT,CPoint) {}
   void OnRButtonUp(UINT,CPoint) {}
   BOOL OnMouseWheel(UINT,UINT,CPoint) { return TRUE; }
   void OnSize(UINT nType, int cx, int cy) {}
   UINT SetTimer(UINT id, UINT interval, void*);
   void KillTimer(UINT id);
   void OnTimer(UINT timerId) {}
   void OnKeyDown(UINT,UINT,UINT) {}
   void OnSetFocus(CWnd*) {}
   void OnKillFocus(CWnd*) {}
   void OnVScroll(UINT,UINT,CScrollBar*) {}
   void OnHScroll(UINT,UINT,CScrollBar*) {}
   void OnUpdate(CWnd* p=0,UINT hint=0,CObject* o=0) { _qt->update(); }
   void Invalidate(BOOL bErase = TRUE) { /*update();*/ }
   void RedrawWindow(LPCRECT rect=0,CRgn* rgn=0,UINT f=0) { _qt->update(); }
   CWnd* SetFocus() { CWnd* pWnd = focusWnd; _qt->setFocus(); return pWnd; }
   CWnd* GetFocus() { return focusWnd; } 
   void SetCapture(CWnd* p=0) { /* DON'T DO THIS grabMouse(); */ }
   void ReleaseCapture() { /* DON'T DO THIS releaseMouse(); */ }
   CFrameWnd* GetParentFrame( ) const { return m_pFrameWnd; }
   void SetFont(
      CFont* pFont,
      BOOL bRedraw = TRUE 
   );
   void MoveWindow(
      LPCRECT lpRect,
         BOOL bRepaint = TRUE 
   );
   void MoveWindow(int x,int y,int cx, int cy);
   CDC* GetDC();
   void ReleaseDC(CDC* pDC);
   void ShowWindow(int code);
   void UpdateWindow( ) { _qt->update(); }
   virtual BOOL PostMessage(
      UINT message,
      WPARAM wParam = 0,
      LPARAM lParam = 0 
   );
   virtual void DoDataExchange(
      CDataExchange* pDX 
   ) {}   
   int GetWindowTextLength( ) const;
   CWnd* GetParent() { return m_pParentWnd?(CWnd*)m_pParentWnd:(CWnd*)m_pFrameWnd; }
   void SetParent(CWnd* parent) { m_pParentWnd = parent; _qt->setParent(parent->toQWidget()); }
   void GetWindowText(
      CString& rString 
   ) const;
   int GetWindowText(
      LPTSTR lpszStringBuf,
      int nMaxCount 
   ) const;
   void SetWindowText(
      LPCTSTR lpszString 
   );
   void GetWindowRect(
      LPRECT lpRect 
   ) const;
   void GetClientRect(
      LPRECT lpRect 
   ) const;
   int GetDlgCtrlID( ) const { return _id; }
   CWnd* GetDlgItem(
      int nID 
   ) const;
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
   void CheckDlgButton( 
      int nIDButton, 
      UINT nCheck  
   );
   UINT IsDlgButtonChecked( 
      int nIDButton
   ) const;
   BOOL SubclassDlgItem(
      UINT nID,
      CWnd* pParent 
   );
   virtual BOOL DestroyWindow( ) { _qt->close(); return TRUE; }
   virtual void PostNcDestroy( ) {}  
   
   // This method only for Qt glue
   UINT_PTR mfcTimerId(int qtTimerId) { return qtToMfcTimer.value(qtTimerId); }
   // These methods are only to be used in CDocTemplate initialization...
   void privateSetParentFrame(CFrameWnd* pFrameWnd) { m_pFrameWnd = pFrameWnd; }
   
   // MFC-to-Qt conversions
protected:
   QHash<UINT_PTR,int> mfcToQtTimer;
   QHash<int,UINT_PTR> qtToMfcTimer;
   QHash<int,CWnd*> mfcToQtWidget;
   QHash<int,QAction*> mfcToQtAction;
   static CFrameWnd* m_pFrameWnd;
   CWnd* m_pParentWnd;
   static CWnd* focusWnd;
   CScrollBar* mfcVerticalScrollBar;
   CScrollBar* mfcHorizontalScrollBar;
   CDC* _myDC;
   UINT _id;

   // Qt interfaces
public:
   QHash<int,CWnd*>* mfcToQtWidgetMap() { return &mfcToQtWidget; }
   QHash<int,QAction*>* mfcToQtActionMap() { return &mfcToQtAction; }
   void subclassWidget(int nID,CWnd* widget);
   void setParent(QWidget *parent) { _qt->setParent(parent); }
   void setParent(QWidget *parent, Qt::WindowFlags f) { _qt->setParent(parent,f); }   
   void setGeometry(const QRect & rect) { _qt->setGeometry(rect); }
   void setGeometry(int x, int y, int w, int h) { _qt->setGeometry(x,y,w,h); }
   const QRect &	geometry () const { return _qt->geometry(); }
   void setContentsMargins(int left, int top, int right, int bottom) { _qt->setContentsMargins(left,top,right,bottom); }
   void setContentsMargins(const QMargins &margins) { _qt->setContentsMargins(margins); }
   void setFocusPolicy(Qt::FocusPolicy policy) { _qt->setFocusPolicy(policy); }
   void setFocusProxy(QWidget *widget) { _qt->setFocusProxy(widget); }
   void setFixedSize(int w, int h) { _qt->setFixedSize(w,h); }
   virtual void setVisible(bool visible) { _qt->setVisible(visible); }
   virtual void setEnabled(bool enabled) { _qt->setEnabled(enabled); }
   QRect rect() const { return _qt->rect(); }
   virtual QWidget* toQWidget() { return _qt; }
   virtual void setQWidget(QWidget* qt) { _qt = qt; }
public slots:
   void update() { _qt->update(); }
   void repaint() { _qt->repaint(); }
   void setFocus() { _qt->setFocus(); }
   void setFocus(Qt::FocusReason reason) { _qt->setFocus(reason); }
   bool eventFilter(QObject *object, QEvent *event);
protected:
   QWidget* _qt;
   QFrame* _qtd;
   QGridLayout* _grid;
public:
   HWND m_hWnd;
};

#define AFX_DATA 
static AFX_DATA const CRect rectDefault;

class CView;
class CDocument;
class CFrameWnd : public CWnd
{
   // Qt interfaces
public:
   void addControlBar(int area,QWidget* bar);
protected:
   QMenuBar* m_pMenuBar;
   
   // MFC interfaces
public:
   CFrameWnd(CWnd* parent = 0);
   virtual ~CFrameWnd();
   CMenu* GetMenu( ) const { return m_pMenu; }   
   virtual void GetMessageString(
      UINT nID,
      CString& rMessage 
   ) const;
   void InitialUpdateFrame(
      CDocument* pDoc,
      BOOL bMakeVisible 
   );
   virtual BOOL OnCmdMsg(
      UINT nID,
      int nCode,
      void* pExtra,
      AFX_CMDHANDLERINFO* pHandlerInfo 
   );
   void OnSize(UINT nType, int cx, int cy);
   virtual void SetMessageText(LPCTSTR fmt,...);
   void SetMessageText(
      UINT nID 
   );
   CView* GetActiveView( ) const { return m_pView; } // Only one view for SDI
   virtual CDocument* GetActiveDocument( ) { return m_pDocument; }   
   virtual void RecalcLayout(
      BOOL bNotify = TRUE 
   );   
   // These methods are only to be used in CDocTemplate initialization...
   virtual void privateSetActiveView(CView* pView) { m_pView = pView; }
   virtual void privateSetActiveDocument(CDocument* pDocument) { m_pDocument = pDocument; }
   
protected:
   CMenu* m_pMenu;
   CView* m_pView;
   QHBoxLayout* cbrsLeft;
   QHBoxLayout* cbrsRight;
   QVBoxLayout* cbrsTop;
   QVBoxLayout* cbrsBottom;
   QWidget* realCentralWidget;
   CDocument* m_pDocument;
   BOOL m_bInRecalcLayout;
   CRect m_rectBorder;
};

class CDocTemplate;
class CDocument : public CCmdTarget
{
public:
   CDocument() : m_pDocTemplate(NULL) {}
   void AssertValid() const {}
   void Dump(CDumpContext& dc) const {}
   virtual BOOL OnNewDocument() { DeleteContents(); return TRUE; }
   virtual BOOL OnSaveDocument(LPCTSTR lpszPathName) { return TRUE; }
   virtual BOOL OnOpenDocument(LPCTSTR lpszPathName) { return TRUE; }
   virtual void OnCloseDocument() {}
   virtual void DeleteContents() {}
   virtual void SetModifiedFlag(BOOL bModified = 1) {}
   virtual void OnFileSave() {}
   virtual POSITION GetFirstViewPosition() const; 
   virtual CView* GetNextView(POSITION pos) const;
   CDocTemplate* GetDocTemplate() const { return m_pDocTemplate; }
   virtual void SetTitle(CString title );
   
   // These methods are only to be used in CDocTemplate initialization...
   virtual void privateSetDocTemplate(CDocTemplate* pDocTemplate) { m_pDocTemplate = pDocTemplate; }
   virtual void privateAddView(CView* pView) { _views.append(pView); }
   
protected:
   CDocTemplate* m_pDocTemplate;
   QList<CView*> _views;
   CString m_docTitle;
};

class CView : public CWnd
{
public:
   CView(CWnd* parent);
   virtual ~CView();
   virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {}
   virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {}  
   virtual void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {}
   virtual void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {}  
   CDocument* GetDocument() const { return m_pDocument; }
   
   // These methods are only to be used in CDocTemplate initialization...
   void privateSetDocument(CDocument* pDocument) { m_pDocument = pDocument; }

protected:
   CDocument* m_pDocument;
};

class CMenu : public QObject, public CCmdTarget
{
   Q_OBJECT
   // Qt interfaces
public:
   QMenu* toQMenu() { return _qtd; }
   void addSubMenu(CMenu* menu);
   QAction* findMenuItem(UINT id) const;
   UINT findMenuID(QAction* action) const;
   HMENU m_hMenu;
public:
   QHash<UINT_PTR,QAction*>* mfcToQtMenuMap() { return &mfcToQtMenu; }
   QHash<QAction*,UINT_PTR>* qtToMfcMenuMap() { return &qtToMfcMenu; }
public slots:
   void menuAction_triggered();
   void menuAboutToShow();
signals:
   void menuAction_triggered(int id);
   void menuAboutToShow(CMenu* menu);
      
   // MFC interface
public:
   CMenu();
   virtual ~CMenu();
   BOOL CreatePopupMenu();
   BOOL LoadMenu(
      UINT nIDResource 
   );
   BOOL RemoveMenu(
      UINT nPosition,
      UINT nFlags 
   );
   CMenu* GetSubMenu(
      int nPos 
   ) const;
   UINT GetMenuItemCount( ) const;
   UINT GetMenuItemID(
      int nPos 
   ) const;
   UINT GetMenuState(
      UINT nID,
      UINT nFlags 
   ) const;
   int GetMenuString(
      UINT nIDItem,
      LPTSTR lpString,
      int nMaxCount,
      UINT nFlags 
   ) const;
   int GetMenuString(
      UINT nIDItem,
      CString& rString,
      UINT nFlags 
   ) const;
   BOOL ModifyMenu(
      UINT nPosition,
      UINT nFlags,
      UINT_PTR nIDNewItem = 0,
      LPCTSTR lpszNewItem = NULL 
   );
   BOOL AppendMenu(
      UINT nFlags,
      UINT_PTR nIDNewItem = 0,
      LPCTSTR lpszNewItem = NULL 
   );
   BOOL SetDefaultItem(
      UINT uItem,
      BOOL fByPos = FALSE 
   );
   UINT CheckMenuItem(
      UINT nIDCheckItem,
      UINT nCheck 
   );
   UINT EnableMenuItem(
      UINT nIDEnableItem,
      UINT nEnable 
   );
   BOOL TrackPopupMenu(
      UINT nFlags,
      int x,
      int y,
      CWnd* pWnd,
      LPCRECT lpRect = 0
   );
   BOOL DestroyMenu( );
private:
   QMenu* _qtd;
   QList<CMenu*>* _cmenu;
   QHash<UINT_PTR,QAction*> mfcToQtMenu;
   QHash<QAction*,UINT_PTR> qtToMfcMenu;
};

class CDialog : public CWnd
{
   // Qt interfaces
public:
   QDialog* _qtd;
   bool _inited;
protected:
   
   // MFC interfaces
public:
   CDialog( );
   CDialog(int dlgID,CWnd* parent);
   virtual ~CDialog();
   void EndDialog(
      int nResult 
   );
   virtual void OnOK( ) { _qtd->accept(); }
   virtual void OnCancel( ) { _qtd->reject(); }
   void ShowWindow(int code);
   void SetWindowText(
      LPCTSTR lpszString 
   );
   virtual BOOL Create(
      UINT nIDTemplate,
      CWnd* pParentWnd = NULL 
         );
   virtual BOOL OnInitDialog() { return TRUE; }
   virtual INT_PTR DoModal();
   void MapDialogRect( 
      LPRECT lpRect  
   ) const;
};

class CCommonDialog : public CDialog
{
   // Qt interfaces
public:
protected:
   
public:
   explicit CCommonDialog(
      CWnd* pParentWnd 
   );
   virtual ~CCommonDialog();
};

class CFileDialog : public CCommonDialog
{
   // Qt interfaces
public:
   void setDefaultSuffix(const QString & suffix) { _qtd->setDefaultSuffix(suffix); }
   void selectFile(const QString & filename) { _qtd->selectFile(filename); }
   void setFilter(const QString& filter) { _qtd->setFilter(filter); }
   QStringList selectedFiles() const { return _qtd->selectedFiles(); }
   void translateFilters(LPCTSTR lpszFilter);
protected:
   QFileDialog* _qtd;
   
   // MFC interfaces
public:
   explicit CFileDialog(
      BOOL bOpenFileDialog,
      LPCTSTR lpszDefExt = NULL,
      LPCTSTR lpszFileName = NULL,
      DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      LPCTSTR lpszFilter = NULL,
      CWnd* pParentWnd = NULL,
      DWORD dwSize = 0
   );
   virtual ~CFileDialog();
   INT_PTR DoModal();
   virtual void OnFileNameChange( ) {};
   CString GetFileExt( ) const;
   CString GetFileName( ) const;
   CString GetPathName( ) const;
   POSITION GetStartPosition( ) const;
   CString GetNextPathName(
      POSITION& pos 
   ) const;
   OPENFILENAME m_ofn;
   LPOPENFILENAME m_pOFN;
};

class CColorDialog : public CCommonDialog
{
   // Qt interfaces
public:
protected:
   QColorDialog* _qtd;
   COLORREF _color;
   
   // MFC interfaces
public:
   CColorDialog(
      COLORREF clrInit = 0,
      DWORD dwFlags = 0,
      CWnd* pParentWnd = NULL 
   );
   virtual ~CColorDialog();
   INT_PTR DoModal();
   COLORREF GetColor( ) const;
   CHOOSECOLOR m_cc;
};

class CScrollBar : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   int sliderPosition() const { return _qtd->sliderPosition(); }
   void setMinimum(int minimum) { _qtd->setMinimum(minimum); }
   void setMaximum(int maximum) { _qtd->setMaximum(maximum); }
   void setValue(int value) { _qtd->setValue(value); }
   void setPageStep(int pageStep) { _qtd->setPageStep(pageStep); }
protected:
   QScrollBar* _qtd;
   Qt::Orientation _orient;
signals:
   void actionTriggered(int action);
   
   // MFC interfaces
public:
   CScrollBar(CWnd* parent = 0);
   virtual ~CScrollBar();
   BOOL SetScrollInfo(
      LPSCROLLINFO lpScrollInfo,
      BOOL bRedraw = TRUE 
   );
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   int SetScrollPos(
      int nPos,
      BOOL bRedraw = TRUE 
   );
   void SetScrollRange(
      int nMinPos,
      int nMaxPos,
      BOOL bRedraw = TRUE 
   );
   void ShowScrollBar(
      BOOL bShow = TRUE 
   );
   BOOL EnableScrollBar(
      UINT nArrowFlags = ESB_ENABLE_BOTH 
   );
};

typedef struct _NM_UPDOWN {
  NMHDR hdr;
  int   iPos;
  int   iDelta;
} NMUPDOWN, *LPNMUPDOWN;

class CEdit : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   void setText(QString text) { if ( _dwStyle&ES_MULTILINE ) _qtd_ptedit->setPlainText(text); else _qtd_ledit->setText(text); }
protected:
   QPlainTextEdit* _qtd_ptedit;
   QLineEdit* _qtd_ledit;
   DWORD _dwStyle;
signals:
   void textChanged(QString str);
   
   // MFC interfaces
public:
   CEdit(CWnd* parent = 0);
   virtual ~CEdit();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   int GetWindowTextLength( ) const;
   void GetWindowText(
      CString& rString 
   ) const;
   int GetWindowText(
      LPTSTR lpszStringBuf,
      int nMaxCount 
   ) const;
   void SetWindowText(
      LPCTSTR lpszString 
   );
   DWORD GetStyle() const;
   LRESULT SendMessage(
      UINT message,
      WPARAM wParam = 0,
      LPARAM lParam = 0 
   );
   void SetSel(
      DWORD dwSelection,
      BOOL bNoScroll = FALSE 
   );
   void SetSel(
      int nStartChar,
      int nEndChar,
      BOOL bNoScroll = FALSE 
   );
   void ReplaceSel(
      LPCTSTR lpszNewText,
         BOOL bCanUndo = FALSE 
   );
#if UNICODE
   void ReplaceSel(
      LPCSTR lpszNewText,
         BOOL bCanUndo = FALSE 
   );
#endif
   BOOL EnableWindow(BOOL bEnable);
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
};

class CButton : public CWnd
{
   Q_OBJECT
   // Qt interfaces
protected:
   QAbstractButton* _qtd;
   QPushButton* _qtd_push;
   QRadioButton* _qtd_radio;
   QCheckBox* _qtd_check;
signals:
   void clicked();

   // MFC interfaces
public:
   CButton(CWnd* parent = 0);
   virtual ~CButton();
   virtual BOOL Create(
      LPCTSTR lpszCaption,
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   HBITMAP SetBitmap(
      HBITMAP hBitmap 
   );
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
   void CheckDlgButton( 
      int nIDButton, 
      UINT nCheck  
   );
   UINT IsDlgButtonChecked( 
      int nIDButton
   ) const;
};

class CBitmapButton : public CButton
{
   Q_OBJECT
   // Qt interfaces
protected:
   QToolButton* _qtd;
signals:
   void clicked();

   // MFC interfaces
public:
   CBitmapButton(CWnd* parent = 0);
   virtual ~CBitmapButton();
   virtual BOOL Create(
      LPCTSTR lpszCaption,
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
};

// From mingw/include/commctrl.h
#define TBS_AUTOTICKS	1
#define TBS_VERT	2
#define TBS_HORZ	0
#define TBS_TOP	4
#define TBS_BOTTOM	0
#define TBS_LEFT	4
#define TBS_RIGHT	0
#define TBS_BOTH	8
#define TBS_NOTICKS	16
#define TBS_ENABLESELRANGE	32
#define TBS_FIXEDLENGTH	64
#define TBS_NOTHUMB	128

class CSliderCtrl : public CWnd
{
   Q_OBJECT
   // Qt interfaces
protected:
   QSlider* _qtd;
signals:
   void valueChanged(int);
      
   // MFC interfaces
public:
   CSliderCtrl(CWnd* parent = 0);
   virtual ~CSliderCtrl();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   void SetRange(
      short nLower,
      short nUpper 
   );   
   void SetRangeMax(
      int nMax,
      BOOL bRedraw = FALSE 
   );   
   void SetPos(
      int nPos 
   );
   int GetPos( ) const;
   void SetTicFreq(
      int nFreq 
   );
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
};

class CProgressCtrl : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   void setOrientation(Qt::Orientation orient) { _qtd->setOrientation(orient); }
   void setInvertedAppearance(bool inverted) { _qtd->setInvertedAppearance(inverted); }
protected:
   QProgressBar* _qtd;
      
   // MFC interfaces
public:
   CProgressCtrl(CWnd* parent = 0);
   virtual ~CProgressCtrl();
   void SetRange(
      short nLower,
      short nUpper 
   );   
   void SetPos(
      int nPos 
   );
   int GetPos( ) const;
};

#define UDS_WRAP     1
#define UDS_SETBUDDYINT      2
#define UDS_ALIGNRIGHT       4
#define UDS_ALIGNLEFT        8
#define UDS_AUTOBUDDY        16
#define UDS_ARROWKEYS        32
#define UDS_HORZ     64
#define UDS_NOTHOUSANDS      128
#define UDS_HOTTRACK 0x0100

//class QSpinBox_MFC : public QSpinBox
//{
//public:
//   QSpinBox_MFC(QWidget* parent = 0) : QSpinBox(parent) {}
//   virtual ~QSpinBox_MFC() {}
//   QLineEdit* lineEdit() const { return QSpinBox::lineEdit(); }
//};

class CSpinButtonCtrl : public CWnd
{
   Q_OBJECT
   // Qt interfaces
protected:
//   QSpinBox_MFC* _qtd;
   QSpinBox* _qtd;
   int _oldValue;
public slots:
   void control_edited();
signals:
   void valueChanged(int oldValue, int newValue);
   
   // MFC interfaces
public:
   CSpinButtonCtrl(CWnd* parent = 0);
   virtual ~CSpinButtonCtrl();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   int SetPos(
      int nPos 
   );
   int GetPos( ) const; 
   void SetRange(
      short nLower,
      short nUpper 
   );   
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
};

class CComboBox : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
protected:
   QComboBox* _qtd;
signals:
   void currentIndexChanged(int index);
   
   // MFC interfaces
public:
   CComboBox(CWnd* parent = 0);
   virtual ~CComboBox();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   void SetWindowText(
      LPCTSTR lpszString 
   );
   void ResetContent();
   int AddString(
      LPCTSTR lpszString 
   );
   void SetCurSel(int sel);
   int GetCurSel( ) const;
   int GetLBText(
      int nIndex,
      LPTSTR lpszText 
   ) const;
#if UNICODE
   int GetLBText(
      int nIndex,
      char* lpszText 
   ) const;
#endif
   void GetLBText(
      int nIndex,
      CString& rString 
   ) const;
   int SelectString(
      int nStartAfter,
      LPCTSTR lpszString 
   );
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
};

class CStatic : public CWnd
{
   // Qt interfaces
public:
   void setText(const QString & text) { _qtd->setText(text); }
protected:
   QLabel* _qtd;

   // MFC interfaces
public:
   CStatic(CWnd* parent = 0);
   virtual ~CStatic();
   virtual BOOL Create(
      LPCTSTR lpszText,
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID = 0xffff 
   );
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
};

// Qt hack for MFC BS_GROUPBOX style CButton override
class CGroupBox : public CWnd
{
   Q_OBJECT
   // Qt interfaces
protected:
   QGroupBox* _qtd;
signals:
   void clicked();

   // MFC interfaces
public:
   CGroupBox(CWnd* parent = 0);
   virtual ~CGroupBox();
   virtual BOOL Create(
      LPCTSTR lpszCaption,
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
};

class CTabCtrl : public CWnd
{  
   Q_OBJECT
   // Qt interfaces
public:
protected:
   QTabWidget* _qtd;
signals:
   void currentChanged(int);
   
   // MFC interfaces
public:
   CTabCtrl(CWnd* parent = 0);
   virtual ~CTabCtrl();
   LONG InsertItem(
     int nItem,
     LPCTSTR lpszItem 
   );
   BOOL DeleteAllItems( );
   int SetCurSel(
     int nItem 
   );
   int GetCurSel( ) const;
};

#define LVM_FIRST 0x1000
#define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST+54)

#define LVCFMT_LEFT 100

#define LVIS_SELECTED 1
#define LVIS_FOCUSED  2

// CP: No idea...need to find these in the windows headers.
#define LVIF_STATE 100 
#define LVNI_SELECTED 200

#define LVS_ICON	0
#define LVS_REPORT	1
#define LVS_SMALLICON	2
#define LVS_LIST	3
#define LVS_TYPEMASK	3
#define LVS_SINGLESEL	4
#define LVS_SHOWSELALWAYS	8
#define LVS_SORTASCENDING	16
#define LVS_SORTDESCENDING	32
#define LVS_SHAREIMAGELISTS	64
#define LVS_NOLABELWRAP	128
#define LVS_AUTOARRANGE	256
#define LVS_EDITLABELS	512
#define LVS_NOSCROLL	0x2000
#define LVS_TYPESTYLEMASK	0xfc00
#define LVS_ALIGNTOP	0
#define LVS_ALIGNLEFT	0x800
#define LVS_ALIGNMASK	0xc00
#define LVS_OWNERDRAWFIXED	0x400
#define LVS_NOCOLUMNHEADER	0x4000
#define LVS_NOSORTHEADER	0x8000
#define LVS_EX_CHECKBOXES 4
#define LVS_EX_FULLROWSELECT 32
#define LVS_EX_GRIDLINES 1

typedef struct tagNMLISTVIEW {
  NMHDR  hdr;
  int    iItem;
  int    iSubItem;
  UINT   uNewState;
  UINT   uOldState;
  UINT   uChanged;
  POINT  ptAction;
  LPARAM lParam;
} NMLISTVIEW, *LPNMLISTVIEW, NM_LISTVIEW;

typedef struct tagNMITEMACTIVATE {
  NMHDR  hdr;
  int    iItem;
  int    iSubItem;
  UINT   uNewState;
  UINT   uOldState;
  UINT   uChanged;
  POINT  ptAction;
  LPARAM lParam;
  UINT   uKeyFlags;
} NMITEMACTIVATE, *LPNMITEMACTIVATE;

#define LVFI_PARAM   1
#define LVFI_STRING  2
#define LVFI_PARTIAL 8
#define LVFI_WRAP    32
#define LVFI_NEARESTXY       64

typedef struct tagLVFINDINFO {
  UINT flags;
  LPCTSTR psz;
  LPARAM lParam;
  POINT pt;
  UINT vkDirection;
} LVFINDINFO, FAR* LPFINDINFO;

#define LVSIL_NORMAL 0
#define LVSIL_SMALL  1
#define LVSIL_STATE  2

class CImageList;

class CListCtrl : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   QModelIndex currentIndex () const;
protected:
   QTableWidget* _qtd_table;
   QListWidget* _qtd_list;
   DWORD _dwStyle;
signals:
   void itemSelectionChanged();
   void cellClicked(int row, int column);
   void cellDoubleClicked(int row, int column);
   
   // MFC interfaces
public:
   CListCtrl(CWnd* parent = 0);
   virtual ~CListCtrl();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   CImageList* SetImageList(
      CImageList* pImageList,
      int nImageListType 
   );
   LRESULT SendMessage(
      UINT message,
      WPARAM wParam = 0,
      LPARAM lParam = 0 
   );
   DWORD SetExtendedStyle(
      DWORD dwNewStyle 
   );
   int FindItem(
      LVFINDINFO* pFindInfo,
      int nStart = -1 
   ) const;
   BOOL SetBkColor(
      COLORREF cr 
   );
   BOOL SetTextBkColor(
      COLORREF cr 
   );
   BOOL SetTextColor(
      COLORREF cr 
   );
   BOOL DeleteAllItems( );
   BOOL DeleteItem(
      int nItem 
   );
   int InsertColumn(
      int nCol,
      LPCTSTR lpszColumnHeading,
      int nFormat = LVCFMT_LEFT,
      int nWidth = -1,
      int nSubItem = -1 
   );
   UINT GetSelectedCount( ) const;
   int GetSelectionMark( );
   int GetNextItem(
      int nItem,
      int nFlags 
   ) const;
   int GetItemText(
      int nItem,
      int nSubItem,
      LPTSTR lpszText,
      int nLen 
   ) const;
   CString GetItemText(
      int nItem,
      int nSubItem 
   ) const;
#if UNICODE
   int GetItemText(
      int nItem,
      int nSubItem,
      char* lpszText,
      int nLen 
   ) const;
#endif
   int InsertItem(
      int nItem,
      LPCTSTR lpszItem,
      int nImage 
   );
   int InsertItem(
      int nItem,
      LPCTSTR lpszItem 
   );
   int SetSelectionMark(
      int iIndex 
   );
   BOOL SetCheck(
      int nItem,
      BOOL fCheck = TRUE 
   );
   BOOL GetCheck(
      int nItem 
   ) const;
   BOOL SetItemText(
      int nItem,
      int nSubItem,
      LPCTSTR lpszText 
   );
   BOOL SetItemText(
      int nItem,
      int nSubItem,
      char* lpszText 
   );
   BOOL SetItemState(
      int nItem,
      UINT nState,
      UINT nMask 
   );
   int GetItemCount( ) const;
   DWORD_PTR GetItemData( 
      int nItem  
   ) const;
   BOOL SetItemData(
      int nItem,
         DWORD_PTR dwData 
   );
   BOOL EnsureVisible(
      int nItem,
      BOOL bPartialOK 
   );
protected:
   CImageList* m_pImageList;
};

class CListBox : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   void setSelectionMode(QAbstractItemView::SelectionMode mode) { _qtd->setSelectionMode(mode); }
   void setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior) { _qtd->setSelectionBehavior(behavior); }
   QScrollBar* verticalScrollBar() const { return _qtd->verticalScrollBar(); }
   QScrollBar* horizontalScrollBar() const { return _qtd->horizontalScrollBar(); }
   QModelIndex currentIndex () const { return _qtd->currentIndex(); }
protected:
   QListWidget* _qtd;
signals:
   void itemSelectionChanged();
   void itemClicked(QListWidgetItem* lwi);
   void itemDoubleClicked(QListWidgetItem* lwi);
   
   // MFC interfaces
public:
   CListBox(CWnd* parent = 0);
   virtual ~CListBox();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
};

class CCheckListBox : public CListBox
{   
   // MFC interfaces
public:
   CCheckListBox(CWnd* parent = 0);
   virtual ~CCheckListBox();
};

typedef QTreeWidgetItem* HTREEITEM;

#define TVI_ROOT (HTREEITEM)0
#define TVI_LAST (HTREEITEM)0xffffffff

enum
{
   TVGN_CARET,
   TVGN_DROPHILITE,
   TVGN_FIRSTVISIBLE,
   TVGN_LASTVISIBLE,
   TVGN_NEXTVISIBLE,
   TVGN_PREVIOUSVISIBLE,
   TVGN_CHILD,
   TVGN_NEXT,
   TVGN_PARENT,
   TVGN_PREVIOUS,
   TVGN_ROOT
};

enum
{
   TVE_COLLAPSE,
   TVE_COLLAPSERESET,
   TVE_EXPAND,
   TVE_TOGGLE
};

#define TVS_HASBUTTONS	1
#define TVS_HASLINES	2
#define TVS_LINESATROOT	4
#define TVS_EDITLABELS	8
#define TVS_DISABLEDRAGDROP	16
#define TVS_SHOWSELALWAYS	32
#define TVS_CHECKBOXES 256
#define TVS_NOTOOLTIPS 128
#define TVS_RTLREADING 64
#define TVS_TRACKSELECT 512
#define TVS_FULLROWSELECT 4096
#define TVS_INFOTIP 2048
#define TVS_NONEVENHEIGHT 16384
#define TVS_NOSCROLL 8192
#define TVS_SINGLEEXPAND 1024
#define TVS_NOHSCROLL	0x8000

class CTreeCtrl : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   void setSelectionMode(QAbstractItemView::SelectionMode mode) { _qtd->setSelectionMode(mode); }
   void setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior) { _qtd->setSelectionBehavior(behavior); }
   QScrollBar* verticalScrollBar() const { return _qtd->verticalScrollBar(); }
   QScrollBar* horizontalScrollBar() const { return _qtd->horizontalScrollBar(); }
   QModelIndex currentIndex () const { return _qtd->currentIndex(); }
protected:
   QTreeWidget* _qtd;
signals:
   void itemSelectionChanged();
   void itemClicked(QTreeWidgetItem* item, int column);
   void itemDoubleClicked(QTreeWidgetItem* item, int column);
   
   // MFC interfaces
public:
   CTreeCtrl(CWnd* parent = 0);
   virtual ~CTreeCtrl();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   HTREEITEM InsertItem(
      LPCTSTR lpszItem,
      HTREEITEM hParent = TVI_ROOT,
      HTREEITEM hInsertAfter = TVI_LAST 
   );
   BOOL SortChildren(
      HTREEITEM hItem 
   );
   HTREEITEM GetRootItem( ) const;
   HTREEITEM GetNextItem(
      HTREEITEM hItem,
      UINT nCode 
   ) const;
   HTREEITEM GetSelectedItem( ) const;
   BOOL ItemHasChildren(
      HTREEITEM hItem 
   ) const;
   DWORD_PTR GetItemData(
      HTREEITEM hItem 
   ) const;
   BOOL SetItemData(
      HTREEITEM hItem,
      DWORD_PTR dwData 
   );   
   CString GetItemText(
      HTREEITEM hItem 
   ) const;
   BOOL DeleteItem(
      HTREEITEM hItem 
   );
   BOOL Expand(
      HTREEITEM hItem,
      UINT nCode 
   );
   HTREEITEM GetParentItem(
      HTREEITEM hItem 
   ) const;
};

class CWinThread : public QThread, public CCmdTarget
{
   Q_OBJECT
public:
   CWinThread();
   virtual ~CWinThread();
   BOOL CreateThread(
      DWORD dwCreateFlags = 0,
      UINT nStackSize = 0,
      LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL 
   );
   DWORD ResumeThread( );
   BOOL SetThreadPriority(
      int nPriority 
   );
   BOOL PostThreadMessage(
      UINT message ,
      WPARAM wParam,
      LPARAM lParam 
         );
   virtual BOOL InitInstance() { return FALSE; }
   virtual BOOL ExitInstance() { return FALSE; }
public:
   HANDLE m_hThread;
   DWORD m_nThreadID;
signals:
   void postThreadMessage(unsigned int m,unsigned int w,unsigned int l);
public slots:
   void recvThreadMessage(unsigned int m,unsigned int w,unsigned int l) { qDebug("CWinThread::recvThreadMessage"); }
};

class CDocTemplate : public CCmdTarget
{
public:
   CDocTemplate(UINT f,CDocument* pDoc,CFrameWnd* pFrameWnd,CView* pView);
   virtual CDocument* OpenDocumentFile(
      LPCTSTR lpszPathName,
      BOOL bMakeVisible = TRUE 
   ) = 0;
   virtual void InitialUpdateFrame(
      CFrameWnd* pFrame,
      CDocument* pDoc,
      BOOL bMakeVisible = TRUE 
   );
   enum DocStringIndex
   {
      windowTitle,
      docName,
      fileNewName,
      filterName,
      filterExt,
      regFileTypeId,
      regFileTypeName      
   };
   virtual BOOL GetDocString(
      CString& rString,
      enum DocStringIndex index 
   ) const;
   virtual POSITION GetFirstDocPosition( ) const = 0;
   virtual CDocument* GetNextDoc(
      POSITION& rPos 
   ) const = 0;
   CDocument* m_pDoc;
   CView*     m_pView;
   CFrameWnd* m_pFrameWnd;
};

class CSingleDocTemplate : public CDocTemplate
{
public:
   CSingleDocTemplate(UINT f,CDocument* pDoc,CFrameWnd* pFrameWnd,CView* pView);
   virtual CDocument* OpenDocumentFile(
      LPCTSTR lpszPathName,
      BOOL bMakeVisible = TRUE 
   );
   virtual POSITION GetFirstDocPosition( ) const;
   virtual CDocument* GetNextDoc(
      POSITION& rPos 
   ) const;
};

class CCommandLineInfo : public CObject
{
public:
   CCommandLineInfo( );
   virtual void ParseParam( 
      const TCHAR* pszParam,  
      BOOL bFlag, 
      BOOL bLast
   );
   enum
   {   
      FileNew,
      FileOpen,
      FilePrint,
      FilePrintTo,
      FileDDE,
      AppRegister,
      AppUnregister,
      RestartByRestartManager,
      FileNothing = -1
   }; 
   BOOL m_bRunAutomated;
   BOOL m_bRunEmbedded;
   BOOL m_bShowSplash;
   UINT m_nShellCommand;
   CString m_strDriverName;
   CString m_strFileName;
   CString m_strPortName;
   CString m_strPrinterName;
   // Qt interface
   QStringList _args;
};

class CWinApp : public CWinThread
{
public:
   CWinApp() : m_pMainWnd(NULL) {}
   void ParseCommandLine(
      CCommandLineInfo& rCmdInfo 
   );
   BOOL ProcessShellCommand( 
      CCommandLineInfo& rCmdInfo  
   );
   void AddDocTemplate(CDocTemplate* pDocTemplate);
   POSITION GetFirstDocTemplatePosition( ) const;
   CDocTemplate* GetNextDocTemplate(
      POSITION& pos 
   ) const;
   virtual CDocument* OpenDocumentFile(
      LPCTSTR lpszFileName 
   );
   virtual BOOL PreTranslateMessage(
      MSG* pMsg 
   );
   HICON LoadIcon(
      UINT nIDResource 
   ) const;
   virtual BOOL InitInstance();
   HCURSOR LoadStandardCursor( 
      LPCTSTR lpszCursorName  
   ) const;
   virtual CWnd * GetMainWnd( ) { return m_pMainWnd; }
   afx_msg void OnFileNew( );
   afx_msg void OnFileOpen( );
   BOOL ExitInstance() { return TRUE; }
public:
   CFrameWnd* m_pMainWnd;
   // Qt interfaces
   QMainWindow* qtMainWindow;   
   
protected:
   QList<CDocTemplate*> _docTemplates;
};

#define CBRS_TOP      0x0001 // Control bar is at the top of the frame window.
#define CBRS_ALIGN_TOP CBRS_TOP
#define CBRS_BOTTOM   0x0002 //  Control bar is at the bottom of the frame window.
#define CBRS_NOALIGN  0x0000 //   Control bar is not repositioned when the parent is resized.
#define CBRS_LEFT     0x0004 // Control bar is at the left of the frame window.
#define CBRS_RIGHT    0x0008 //  Control bar is at the right of the frame window.
#define CBRS_FLYBY    0x0010
#define CBRS_TOOLTIPS 0x0020 
#define CBRS_SIZE_DYNAMIC 0x2000

class CControlBar : public CWnd
{
public:
   virtual CSize CalcFixedLayout(
      BOOL bStretch,
      BOOL bHorz 
   );
   void SetBarStyle(
      DWORD dwStyle 
   );
   virtual BOOL IsVisible() const;
};

#define TBSTYLE_BUTTON       0
#define TBSTYLE_SEP  1
#define TBSTYLE_CHECK        2
#define TBSTYLE_GROUP        4
#define TBSTYLE_CHECKGROUP   (TBSTYLE_GROUP|TBSTYLE_CHECK)
#define TBSTYLE_DROPDOWN     8
#define TBSTYLE_AUTOSIZE     16
#define TBSTYLE_NOPREFIX     32
#define TBSTYLE_TOOLTIPS     256
#define TBSTYLE_WRAPABLE     512
#define TBSTYLE_ALTDRAG      1024
#define TBSTYLE_FLAT 2048
#define TBSTYLE_LIST 4096
#define TBSTYLE_CUSTOMERASE 8192
#define TBSTYLE_REGISTERDROP 0x4000
#define TBSTYLE_TRANSPARENT  0x8000
#define TBSTYLE_EX_DRAWDDARROWS      0x00000001
#define TBSTYLE_EX_MIXEDBUTTONS 8
#define TBSTYLE_EX_HIDECLIPPEDBUTTONS 16
#define TBSTYLE_EX_DOUBLEBUFFER      0x80

typedef struct {
  UINT     cbSize;
  UINT     fMask;
  UINT     fStyle;
  COLORREF clrFore;
  COLORREF clrBack;
  LPTSTR   lpText;
  UINT     cch;
  int      iImage;
  HWND     hwndChild;
  UINT     cxMinChild;
  UINT     cyMinChild;
  UINT     cx;
  HBITMAP  hbmBack;
  UINT     wID;
#if (_WIN32_IE >= 0x0400)
  UINT     cyChild;
  UINT     cyMaxChild;
  UINT     cyIntegral;
  UINT     cxIdeal;
  LPARAM   lParam;
  UINT     cxHeader;
#endif 
#if (_WIN32_WINNT >= 0x0600)
  RECT     rcChevronLocation;
  UINT     uChevronState;
#endif 
} REBARBANDINFO, *LPREBARBANDINFO;

#define RBS_TOOLTIPS 256
#define RBS_VARHEIGHT 512
#define RBS_BANDBORDERS 1024
#define RBS_FIXEDORDER 2048
#define RBS_REGISTERDROP 4096
#define RBS_AUTOSIZE 8192
#define RBS_VERTICALGRIPPER 16384
#define RBS_DBLCLKTOGGLE  32768
#define RBBS_BREAK   0x0001
#define RBBS_FIXEDSIZE       0x0002
#define RBBS_CHILDEDGE       0x0004
#define RBBS_HIDDEN  0x0008
#define RBBS_NOVERT  0x0010
#define RBBS_FIXEDBMP        0x0020
#define RBBS_VARIABLEHEIGHT  0x0040
#define RBBS_GRIPPERALWAYS   0x0080
#define RBBS_NOGRIPPER       0x0100
#define RBBS_USECHEVRON      0x0200
#define RBBS_HIDETITLE       0x0400
#define RBBS_TOPALIGN        0x0800
#define RBBIM_STYLE 1
#define RBBIM_COLORS 2
#define RBBIM_TEXT 4
#define RBBIM_IMAGE 8
#define RBBIM_CHILD 16
#define RBBIM_CHILDSIZE 32
#define RBBIM_SIZE 64
#define RBBIM_BACKGROUND 128
#define RBBIM_ID 256
#define RBBIM_IDEALSIZE 512
#define RBBIM_LPARAM 1024
#define RBBIM_HEADERSIZE 2048

class CReBar;

class CReBarCtrl : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   QList<QObject*>* toolBarActions() { return &_toolBarActions; }
protected:
   QToolBar* _qtd;
   QList<QObject*> _toolBarActions;
   UINT _dwStyle;
public slots:
   void toolBarAction_triggered();
signals:
   void toolBarAction_triggered(int id);
   
   // MFC interfaces
public:
   virtual BOOL Create( 
      DWORD dwStyle, 
      const RECT& rect, 
      CWnd* pParentWnd, 
      UINT nID  
   );
   BOOL InsertBand( 
      UINT uIndex, 
      REBARBANDINFO* prbbi  
   );
   void MinimizeBand(
      UINT uBand 
   );
};

class CReBar : public CControlBar
{
public:
   CReBar();
   virtual ~CReBar();
   virtual BOOL Create(
      CWnd* pParentWnd,
      DWORD dwCtrlStyle = RBS_BANDBORDERS,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP,
      UINT nID = AFX_IDW_REBAR 
   );
   CReBarCtrl& GetReBarCtrl() const { return *m_pReBarCtrl; }
protected:
   CReBarCtrl* m_pReBarCtrl;
};

#define TBBS_BUTTON     1 //  Standard pushbutton (default)
#define TBBS_SEPARATOR  2 //  Separator
#define TBBS_CHECKBOX   3 //  Auto check-box button
#define TBBS_GROUP      4 //  Marks the start of a group of buttons
#define TBBS_CHECKGROUP 5 //  Marks the start of a group of check-box buttons
#define TBBS_DROPDOWN   6 //  Creates a drop-down list button
#define TBBS_AUTOSIZE   7 //  The button's width will be calculated based on the text of the button, not on the size of the image
#define TBBS_NOPREFIX   8 //  The button text will not have an accelerator prefix associated with it

class CToolBar : public CControlBar
{
   Q_OBJECT
   // Qt interfaces
public:
   QList<QObject*>* toolBarActions() { return &_toolBarActions; }
protected:
   QToolBar* _qtd;
   QList<QObject*> _toolBarActions;
   UINT _dwStyle;
public slots:
   void toolBarAction_triggered();
signals:
   void toolBarAction_triggered(int id);
   
   // MFC interfaces
public:
   CToolBar(CWnd* parent = 0);
   virtual ~CToolBar();
   virtual BOOL CreateEx(
      CWnd* pParentWnd,
      DWORD dwCtrlStyle = TBSTYLE_FLAT,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
      CRect rcBorders = CRect(
      0,
      0,
      0,
      0
   ),
      UINT nID = AFX_IDW_TOOLBAR
   );
   LRESULT SendMessage(
      UINT message,
      WPARAM wParam = 0,
      LPARAM lParam = 0 
   );
   BOOL LoadToolBar(
      UINT nIDResource 
   );
   void SetButtonStyle(
      int nIndex,
      UINT nStyle 
   );
};

class CDialogBar : public CControlBar
{
   Q_OBJECT
   // Qt interfaces
protected:
   CDialog*     _mfcd;
   UINT _nStyle;
public:
   CDialogBar();
   ~CDialogBar();
   virtual BOOL Create(
      CWnd* pParentWnd,
      UINT nIDTemplate,
      UINT nStyle,
      UINT nID 
   );
   LRESULT SendMessage(
      UINT message,
      WPARAM wParam = 0,
      LPARAM lParam = 0 
   );
   virtual CSize CalcFixedLayout(
      BOOL bStretch,
      BOOL bHorz 
   );
public:
   CSize m_sizeDefault;
};

class CStatusBar : public CControlBar
{
   // Qt interfaces
protected:
   QStatusBar* _qtd;
   QHash<int,CStatic*> _panes;
   UINT _dwStyle;
   
   // MFC interfaces
public:
   CStatusBar(CWnd* parent = 0);
   virtual ~CStatusBar();
   virtual BOOL Create(
      CWnd* pParentWnd,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
      UINT nID = AFX_IDW_STATUS_BAR 
   );
   LRESULT SendMessage(
      UINT message,
      WPARAM wParam = 0,
      LPARAM lParam = 0 
   );
   void SetWindowText(
      LPCTSTR lpszString 
   );
   BOOL SetIndicators(
      const UINT* lpIDArray,
      int nIDCount 
   );
   BOOL SetPaneText(
      int nIndex,
      LPCTSTR lpszNewText,
      BOOL bUpdate = TRUE 
   );   
};

#if UNICODE
#define LPSTR_TEXTCALLBACKW  ((LPWSTR)-1)
#define LPSTR_TEXTCALLBACK LPSTR_TEXTCALLBACKW
#else
#define LPSTR_TEXTCALLBACKA  ((LPSTR)-1)
#define LPSTR_TEXTCALLBACK LPSTR_TEXTCALLBACKA
#endif

 class CToolTipCtrl : public CWnd
{
   // Qt interfaces
protected:
   QToolTip* _qtd;
   QList<CWnd*> _tippers;
   // MFC interfaces
public:
   CToolTipCtrl( );
   virtual BOOL Create(
      CWnd* pParentWnd,
         DWORD dwStyle = 0 
   );
   void Activate( 
      BOOL bActivate  
   );
   BOOL AddTool(
      CWnd* pWnd,
      UINT nIDText,
      LPCRECT lpRectTool = NULL,
      UINT_PTR nIDTool = 0 
   );
   BOOL AddTool(
      CWnd* pWnd,
      LPCTSTR lpszText = LPSTR_TEXTCALLBACK,
      LPCRECT lpRectTool = NULL,
      UINT_PTR nIDTool = 0 
   );
   void RelayEvent(
      LPMSG lpMsg 
   );
};

#define CN_COMMAND 0x10101
#define CN_UPDATE_COMMAND_UI 0x10102
 
class CCmdUI
{
public:
   CCmdUI();
   void ContinueRouting( );
   BOOL DoUpdate(CCmdTarget* pTarget, BOOL bDisableIfNoHndler);
   virtual void Enable(
      BOOL bOn = TRUE 
   );
   virtual void SetCheck(
      int nCheck = 1 
   );
   virtual void SetRadio(
      BOOL bOn = TRUE 
   );
   virtual void SetText(
      LPCTSTR lpszText 
   );
   UINT m_nID;
   UINT m_nIndex;
   CMenu* m_pMenu;
   CWnd* m_pOther;
   CMenu* m_pSubMenu;
   BOOL m_bContinueRouting;
   BOOL m_bEnableChanged;
};

template < class TYPE, class ARG_TYPE = const TYPE& > 
class CArray : 
   public CObject
{
public:
   INT_PTR GetCount( ) const
   {
      return _qlist.count();
   }
   
   TYPE& operator[]( 
      INT_PTR nIndex  
   ) 
   {
      return _qlist[nIndex];
   }
   const TYPE& operator[]( 
      INT_PTR nIndex  
   ) const
   {
      return _qlist.at(nIndex);
   }
   void RemoveAll( )
   {
      _qlist.clear();
   }
   void FreeExtra( )
   {
   }
   INT_PTR Add(
      ARG_TYPE newElement 
   )
   {
      _qlist.append(newElement);
      return _qlist.count()-1;
   }
protected:
   QList<TYPE> _qlist;
};

template< class KEY, class ARG_KEY, class VALUE, class ARG_VALUE >
class CMap : 
      public CObject
{
public:
   VALUE& operator[](
         ARG_KEY key 
   )
   {
      return _qmap[key];
   }

protected:
   QMap<KEY,VALUE> _qmap;
};

class CFileFind
{
public:
   virtual BOOL FindFile(
      LPCTSTR pstrName = NULL,
      DWORD dwUnused = 0 
   );
   virtual BOOL FindNextFile( );
   virtual CString GetFileName( ) const;
   virtual CString GetFilePath( ) const;
   virtual CString GetFileTitle( ) const;
   BOOL IsDirectory( ) const;
   BOOL IsHidden( ) const;
   virtual BOOL IsDots( ) const;
protected:
   QDir _qdir;
   QFileInfoList _qfiles;
   int _idx;
};

#define ILC_COLOR 0
#define ILC_COLOR4 4
#define ILC_COLOR8 8
#define ILC_COLOR16 16
#define ILC_COLOR24 24
#define ILC_COLOR32 32
#define ILC_COLORDDB 254

class CImageList : public CObject
{
public:
   CImageList();
   BOOL Create(
      int cx,
      int cy,
      UINT nFlags,
      int nInitial,
      int nGrow 
   );
   int Add(
      CBitmap* pbmImage,
      CBitmap* pbmMask 
   );
   int Add(
      CBitmap* pbmImage,
      COLORREF crMask 
   );
   int Add(
      HICON hIcon 
   );
   HICON ExtractIcon(
      int nImage 
   );
protected:
   QList<CBitmap*> _images;
};

class CPropertyPage : public CDialog
{
   // MFC interfaces
public:
   explicit CPropertyPage( 
      UINT nIDTemplate, 
      UINT nIDCaption = 0, 
      DWORD dwSize = sizeof(PROPSHEETPAGE) 
   ); 
   virtual ~CPropertyPage();
   void SetModified(
      BOOL bChanged = TRUE 
   );
   virtual BOOL OnApply( );
   virtual BOOL OnSetActive( );
};

class CPropertySheet : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   QDialog* _qtd;
   QTabWidget* _qtabwidget;
   QDialogButtonBox* _qbuttons;
   QList<CPropertyPage*> _pages;
   UINT _selectedPage;
public slots:
   void tabWidget_currentChanged(int idx);
   void ok_clicked();
   void cancel_clicked();
   void apply_clicked();
public:
   void _commonConstruct(CWnd* parent,UINT selectedPage);
   explicit CPropertySheet(
      UINT nIDCaption,
      CWnd* pParentWnd = NULL,
      UINT iSelectPage = 0 
   );
   explicit CPropertySheet(
      LPCTSTR pszCaption,
      CWnd* pParentWnd = NULL,
      UINT iSelectPage = 0 
   );
   CPropertySheet(
      UINT nIDCaption,
      CWnd* pParentWnd,
      UINT iSelectPage,
      HBITMAP hbmWatermark,
      HPALETTE hpalWatermark = NULL,
      HBITMAP hbmHeader = NULL 
   );
   CPropertySheet(
      LPCTSTR pszCaption,
      CWnd* pParentWnd,
      UINT iSelectPage,
      HBITMAP hbmWatermark,
      HPALETTE hpalWatermark = NULL,
      HBITMAP hbmHeader = NULL 
   );   
   void AddPage(
      CPropertyPage *pPage 
   );
   virtual INT_PTR DoModal( );
};

class CCreateContext
{
};

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
);

CWinApp* AfxGetApp();
CFrameWnd* AfxGetMainWnd();

HGDIOBJ GetStockObject(
   int fnObject
);

int EnumFontFamiliesEx(
   HDC hdc,
   LPLOGFONT lpLogfont,
   FONTENUMPROC lpEnumFontFamExProc,
   LPARAM lParam,
   DWORD dwFlags
);

CString qtMfcStringResource(int id);

CBitmap* qtMfcBitmapResource(int id);

QIcon* qtIconResource(int id);

void openFile(QString fileName);

#endif // CQTMFC_H
