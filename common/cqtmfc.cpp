#include "cqtmfc.h"
#include "resource.h"

#include <stdarg.h>

#include <QLinearGradient>
#include <QHeaderView>
#include <QMessageBox>
#include <QPixmap>

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

size_t strlen(const wchar_t* str)
{
   int len = 0;
   if ( str )
   {
      while ( *(str+len) ) { ++len; }
   }
   return len;
}

int _tstoi(TCHAR* str)
{
   QString tmp = QString::fromWCharArray(str);
   return tmp.toInt();
}

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
      return 0xd0d0d0;
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
   QScrollBar sb(Qt::Vertical);
   switch ( nIndex )
   {
   case SM_CXVSCROLL:
      return sb.sizeHint().width();
      break;
   }
   return 0;
}

BOOL WINAPI OpenClipboard(
//  HWND hWndNewOwner
)
{
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
   QMimeData mimeData;
   QSharedMemory* pMem = (QSharedMemory*)hMem;
   mimeData.setData("application/x-qt-windows-mime;value=\"FamiTracker\"",QByteArray((const char*)pMem,::GlobalSize(hMem)));
   QApplication::clipboard()->setMimeData(&mimeData);
   return hMem;
}

BOOL WINAPI IsClipboardFormatAvailable(
  UINT format
)
{
   QStringList formats = QApplication::clipboard()->mimeData()->formats();
   
   if ( !formats.at(0).compare("application/x-qt-windows-mime;value=\"FamiTracker\"") )
      return TRUE;
   return FALSE;
//   return QApplication::clipboard()->mimeData()->hasFormat(formats.at(0));
}

HANDLE WINAPI GetClipboardData(
  UINT uFormat
)
{
   return QApplication::clipboard()->mimeData()->data("application/x-qt-windows-mime;value=\"FamiTracker\"").data();
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
   
   return pMem->unlock();;
}

SIZE_T WINAPI GlobalSize(
  HGLOBAL hMem
)
{
   QSharedMemory* pMem = (QSharedMemory*)hMem;
   
   return pMem->size();
}

/*
 *  Class CString
 */

bool operator==(const CString& s1, const LPCTSTR s2)
{
#if UNICODE
   return (!wcscmp(s1.GetString(),s2));
#else
   return (!strcmp(s1.GetString(),s2));
#endif
}

CString::CString()
{
   _qstr.clear();
   _qstrn = QByteArray(_qstr.toLatin1());
   UpdateScratch();
}

CString::CString(LPCSTR str)
{
   _qstr.clear();
   _qstr = str;
   UpdateScratch();
}

CString::CString(LPCWSTR str)
{
   _qstr.clear();
   _qstr = QString::fromWCharArray(str);
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
   _qstr.append(QString::fromWCharArray(qtMfcStringResource(nID).GetString()));
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
}

int CString::ReverseFind( TCHAR ch ) const
{
#if UNICODE
   return _qstr.lastIndexOf(QString::fromWCharArray(&ch));
#else
   return _qstr.lastIndexOf(QString(ch));
#endif
}

const CString& CString::operator=(const CString& str)
{
   _qstr.clear();
   _qstr = str._qstr;
   UpdateScratch();
   return *this;
}

const CString& CString::operator+=(const CString& str)
{
   _qstr.append(str._qstr);
   UpdateScratch();
   return *this;
}

const CString& CString::operator=(LPSTR str)
{
   _qstr.clear();
   _qstr = QString(str);
   UpdateScratch();
   return *this;
}

const CString& CString::operator+=(LPSTR str)
{
   _qstr.append(QString(str));
   UpdateScratch();
   return *this;
}

const CString& CString::operator=(LPCSTR str)
{
   _qstr.clear();
   _qstr = QString(str);
   UpdateScratch();
   return *this;
}

const CString& CString::operator+=(LPCSTR str)
{
   _qstr.append(QString(str));
   UpdateScratch();
   return *this;
}

const CString& CString::operator=(LPWSTR str)
{
   _qstr.clear();
   _qstr = QString::fromWCharArray(str);
   UpdateScratch();
   return *this;
}

const CString& CString::operator+=(LPWSTR str)
{
   _qstr.append(QString::fromWCharArray(str));
   UpdateScratch();
   return *this;
}

const CString& CString::operator=(LPCWSTR str)
{
   _qstr.clear();
   _qstr = QString::fromWCharArray(str);
   UpdateScratch();
   return *this;
}

const CString& CString::operator+=(LPCWSTR str)
{
   _qstr.append(QString::fromWCharArray(str));
   UpdateScratch();
   return *this;
}

const CString& CString::operator=(QString str)
{
   _qstr.clear();
   _qstr = str;
   UpdateScratch();
   return *this;
}

const CString& CString::operator+=(QString str)
{
   _qstr.append(str);
   UpdateScratch();
   return *this;
}

CString::operator LPCTSTR() const
{
    return GetString();
}

CString::operator const QString&() const
{
   return _qstr;
}

void CString::Empty() 
{ 
   _qstr.clear(); 
}

LPCTSTR CString::GetString() const
{
#if UNICODE
   return (LPCWSTR)_qstr.unicode();
#else
   return _qstrn.constData();
#endif
}

LPCTSTR CString::GetBuffer() const
{
#if UNICODE
   return (LPWSTR)_qstr.unicode();
#else
   return _qstrn.constData();
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

void CRect::MoveToXY(
   int x,
   int y 
)
{
   bottom = y + Height();
   top = y;
   right = x + Width();
   left = x;
}

void CRect::MoveToY(
      int y
)
{
   bottom = y + Height();
   top = y;
}

void CRect::MoveToXY(
   POINT point 
)
{
   bottom = point.y + Height();
   top = point.y;
   right = point.x + Width();
   left = point.x;
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

   _qfont.setPointSize(nHeight);
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
   _qfont.setPointSize(lpLogFont->lfHeight);
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

BOOL CBitmap::LoadBitmap(
   UINT nIDResource 
)
{
   BOOL result = FALSE;
   if ( _owned )
      delete _qpixmap;
   _qpixmap = qtMfcBitmapResource(nIDResource).toQPixmap();
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
   
   attach(parent->toQWidget());
}

CDC::~CDC()
{
   flush();
   detach();
}

void CDC::flush()
{
   if ( _qwidget )
   {
      QPainter p;
      p.begin(_qwidget);
      p.setBackgroundMode(Qt::TransparentMode);
      p.drawPixmap(0,0,*_qpixmap);
      p.end();
   }   
}

void CDC::attach()
{
   _qpixmap = new QPixmap(1,1);
   _qpainter = new QPainter(_qpixmap);
//   _qpainter->setBackgroundMode(Qt::TransparentMode);
   m_hDC = (HDC)_qpixmap;
   attached = true;
}

void CDC::attach(QWidget* parent)
{
   _qwidget = parent;
   _qpixmap = new QPixmap(_qwidget->size());
   _qpixmap->fill(_qwidget,0,0); // CP: hack to initialize pixmap with widget's background color.
   _qpainter = new QPainter(_qpixmap);
   _qpainter->setBackgroundMode(Qt::TransparentMode);
   m_hDC = (HDC)_qpixmap;
   attached = true;
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

COLORREF CDC::GetPixel(
   int x,
   int y 
) const
{
   qDebug("GetPixel not supported yet.");
   COLORREF ref = 0xbadf00d;
   return ref;
}

COLORREF CDC::GetPixel(
   POINT point 
) const
{
   qDebug("GetPixel not supported yet.");
   COLORREF ref = 0xbadf00d;
   return ref;
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
   if ( pixmap && (pSrcDC->pixmapSize().width() >= 0) )
      _qpainter->drawPixmap(x,y,pSrcDC->pixmapSize().width(),pSrcDC->pixmapSize().height(),*pixmap,xSrc,ySrc,pSrcDC->pixmapSize().width(),pSrcDC->pixmapSize().height());
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
   QString qstr = QString::fromWCharArray(str.GetBuffer());
#else
   QString qstr(str.GetBuffer());
#endif
   QPen origPen = _qpainter->pen();
   _qpainter->setPen(QPen(_textColor));
//   _qpainter->setFont((QFont)*_font);
   _qpainter->drawText(rect,qstr.toLatin1().constData());
   return strlen(str.GetBuffer());   
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
   
   if ( parent )
      _qt = new QComboBox(parent->toQWidget());
   else
      _qt = new QComboBox;

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

void CComboBox::ResetContent()
{
   _qtd->clear();
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
   wcscpy(lpszText,(wchar_t*)lbText.unicode());
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
   int index = -1;
#if UNICODE
   QString string = QString::fromWCharArray(lpszString);
#else
   QString string = lpszString;
#endif
   do
   {
      index = _qtd->findText(string);
   } while ( (index != -1) && (index >= nStartAfter) );
}

CListCtrl::CListCtrl(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QTableWidget(parent->toQWidget());
   else
      _qt = new QTableWidget;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QTableWidget*>(_qt);
      
   _qtd->setFont(QFont("MS Shell Dlg",8));
   _qtd->horizontalHeader()->setStretchLastSection(true);
   _qtd->verticalHeader()->hide();
   _qtd->setEditTriggers(QAbstractItemView::NoEditTriggers);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(itemSelectionChanged()),this,SIGNAL(itemSelectionChanged()));
   QObject::connect(_qtd,SIGNAL(cellClicked(int,int)),this,SIGNAL(cellClicked(int,int)));
   QObject::connect(_qtd,SIGNAL(cellDoubleClicked(int,int)),this,SIGNAL(cellDoubleClicked(int,int)));
}

CListCtrl::~CListCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

UINT CListCtrl::GetSelectedCount( ) const
{
   return _qtd->selectedItems().count();
}

int CListCtrl::GetSelectionMark( )
{
   if ( _qtd->selectedItems().count() )
      return _qtd->selectedItems().at(0)->row();
   return -1;
}

int CListCtrl::GetNextItem(
   int nItem,
   int nFlags 
) const
{
   QList<QTableWidgetItem*> items = _qtd->selectedItems();
   int item;
   int nextItemRow = -1;
   
   switch ( nFlags )
   {
   case LVNI_SELECTED:
      for ( item = 0; item < items.count(); item++ )
      {
         if ( items.at(item)->row() == nItem )
         {
            if ( item < (items.count()-1) )
            {
               nextItemRow = items.at(item+1)->row();
            }
         }
      }
      break;
   default:
      qDebug("CListCtrl::GetNextItem called with unsupported nFlags");
      break;
   }
   return nextItemRow;
}

CString CListCtrl::GetItemText(
   int nItem,
   int nSubItem 
) const
{
   QTableWidgetItem* twi = _qtd->item(nItem,nSubItem);
   CString str;
   if ( twi )
   {
      str = twi->text();
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
   QTableWidgetItem* twi = _qtd->item(nItem,nSubItem);
   int length = 0;
   if ( twi )
   {
#if UNICODE
      wcscpy(lpszText,(wchar_t*)twi->text().unicode());
      length = wcslen(lpszText);
#else
      strcpy(lpszText,twi->text().toAscii().constData());
      length = strlen(lpszText);
#endif
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
   QTableWidgetItem* twi = _qtd->item(nItem,nSubItem);
   int length = 0;
   if ( twi )
   {
      strcpy(lpszText,twi->text().toAscii().constData());
      length = strlen(lpszText);
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
   _qtd->insertColumn(nCol);
   QTableWidgetItem* twi = new QTableWidgetItem;
#if UNICODE
   twi->setText(QString::fromWCharArray(lpszColumnHeading));
#else
   twi->setText(lpszColumnHeading);
#endif
   _qtd->setColumnWidth(nCol,nWidth);
   _qtd->setHorizontalHeaderItem(nCol,twi);
   return _qtd->columnCount()-1;
}

int CListCtrl::InsertItem(
   int nItem,
   LPCTSTR lpszItem
)
{
   QTableWidgetItem* twi = new QTableWidgetItem;
#if UNICODE
   twi->setText(QString::fromWCharArray(lpszItem));
#else
   twi->setText(lpszItem);
#endif
   _qtd->blockSignals(true);
   _qtd->insertRow(nItem);
   _qtd->setItem(nItem,0,twi);
   _qtd->blockSignals(false);
   _qtd->resizeRowToContents(nItem);
   _qtd->resizeColumnsToContents();
   return _qtd->rowCount()-1;
}

int CListCtrl::InsertItem(
   int nItem,
   LPCTSTR lpszItem,
   int nImage 
)
{
   QTableWidgetItem* twi = new QTableWidgetItem;
#if UNICODE
   twi->setText(QString::fromWCharArray(lpszItem));
#else
   twi->setText(lpszItem);
#endif
   _qtd->blockSignals(true);
   _qtd->insertRow(nItem);
   _qtd->setItem(nItem,0,twi);
   _qtd->blockSignals(false);
   _qtd->resizeRowToContents(nItem);
   _qtd->resizeColumnsToContents();
   return _qtd->rowCount()-1;
}

int CListCtrl::SetSelectionMark(
   int iIndex 
)
{
   int selection = _qtd->selectionModel()->currentIndex().row();
   _qtd->selectRow(iIndex);
   return selection;
}

BOOL CListCtrl::SetCheck(
   int nItem,
   BOOL fCheck
)
{
   QTableWidgetItem* twi = _qtd->item(nItem,0);  
   bool add = false;
   if ( !twi )
   {
      add = true;
      twi = new QTableWidgetItem;
   }
   
   twi->setCheckState(fCheck?Qt::Checked:Qt::Unchecked);

   if ( add )
      _qtd->setItem(nItem,0,twi);

   return TRUE;
}

BOOL CListCtrl::SetItemText(
   int nItem,
   int nSubItem,
   char* lpszText 
)
{
   QTableWidgetItem* twi = _qtd->item(nItem,nSubItem);
   bool add = false;
   if ( !twi )
   {
      add = true;
      twi = new QTableWidgetItem;
   }

   
   twi->setText(lpszText);
   
   if ( add )
      _qtd->setItem(nItem,nSubItem,twi);
   
   return TRUE;
}

BOOL CListCtrl::SetItemText(
   int nItem,
   int nSubItem,
   LPCTSTR lpszText 
)
{
   QTableWidgetItem* twi = _qtd->item(nItem,nSubItem);
   bool add = false;
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
      _qtd->setItem(nItem,nSubItem,twi);
   
   return TRUE;
}

BOOL CListCtrl::SetItemState(
   int nItem,
   UINT nState,
   UINT nMask 
)
{
   nState &= nMask;
   if ( nState&LVIS_SELECTED )
   {
      _qtd->selectRow(nItem);
   }
   if ( nState&LVIS_FOCUSED )
   {
      _qtd->selectRow(nItem);
   }
   return TRUE;
}

BOOL CListCtrl::DeleteAllItems()
{
   _qtd->clearContents();
   return TRUE;
}

int CListCtrl::GetItemCount( ) const
{
   return _qtd->rowCount();
}

CScrollBar::CScrollBar(CWnd *parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QScrollBar(parent->toQWidget());
   else
      _qt = new QScrollBar;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QScrollBar*>(_qt);
   _qtd->setOrientation(Qt::Vertical);
      
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(actionTriggered(int)),this,SIGNAL(actionTriggered(int)));
}

CScrollBar::CScrollBar(Qt::Orientation orient,CWnd *parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QScrollBar(parent->toQWidget());
   else
      _qt = new QScrollBar;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QScrollBar*>(_qt);   
   _qtd->setOrientation(orient);
   
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
   if ( dwStyle&WS_VISIBLE )
   {
      _qtd->setVisible(true);
   }
   m_hWnd = (HWND)_qt;
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

CWnd* CWnd::focusWnd = NULL;
CFrameWnd* CWnd::m_pFrameWnd = NULL;

CWnd::CWnd(CWnd *parent) 
   : m_pParentWnd(parent),
     mfcVerticalScrollBar(NULL),
     mfcHorizontalScrollBar(NULL),
     m_hWnd(NULL)
{
   if ( parent )
   {
      _qt = new QWidget(parent->toQWidget());
   }
   else
   {
      _qt = new QWidget;
   }
      
   _qt->installEventFilter(this);
}

CWnd::~CWnd()
{
   if ( mfcVerticalScrollBar )
      delete mfcVerticalScrollBar;
   if ( mfcHorizontalScrollBar )
      delete mfcHorizontalScrollBar;
   mfcVerticalScrollBar = NULL;
   mfcHorizontalScrollBar = NULL;
   if ( _qt )
      delete _qt;
   _qt = NULL;
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
   _qt->setFixedSize(rect.right-rect.left,rect.bottom-rect.top);
   _qt->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   m_hWnd = (HWND)_qt;
   PreCreateWindow(createStruct);
   OnCreate(&createStruct);
   return TRUE;
}

int CWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   QGridLayout* grid = dynamic_cast<QGridLayout*>(_qt->layout());
   
   if ( lpCreateStruct->style&WS_VSCROLL )
   {
      mfcVerticalScrollBar = new CScrollBar(Qt::Vertical);
      grid->addWidget(mfcVerticalScrollBar->toQWidget(),0,1);
   }
   if ( lpCreateStruct->style&WS_HSCROLL )
   {
      mfcHorizontalScrollBar = new CScrollBar(Qt::Horizontal);
      grid->addWidget(mfcHorizontalScrollBar->toQWidget(),1,0);   
   }
   return 0;
}

void CWnd::MoveWindow(int x, int y, int cx, int cy)
{
   MoveWindow(CRect(CPoint(x,y),CSize(cx,cy)));   
}

void CWnd::MoveWindow(LPCRECT lpRect, BOOL bRepaint)
{
//   _qt->setGeometry(lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top);
   _qt->move(lpRect->left,lpRect->top);
}

BOOL CWnd::PostMessage(
   UINT message,
   WPARAM wParam,
   LPARAM lParam 
)
{
   qDebug("CWnd::PostMessage");
   return FALSE;
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
      setParent((QDialog*)(pParent->toQWidget()));
      setGeometry(pWndSrc->geometry());
      pParent->subclassWidget(nID,this);
      pWndSrc->setParent(NULL);
      delete pWndSrc;
      return TRUE;
   }
   return FALSE;
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
   int pos = mfcHorizontalScrollBar->sliderPosition();
   switch ( nBar )
   {
   case SB_HORZ:
      mfcHorizontalScrollBar->setValue(nPos);
      break;
   case SB_VERT:
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

void CWnd::GetWindowText(
   CString& rString 
) const
{
   rString = _qt->windowTitle();
}

void CWnd::SetWindowText(
   LPCTSTR lpszString 
)
{
#if UNICODE
   _qt->setWindowTitle(QString::fromWCharArray(lpszString));
#else
   _qt->setWindowTitle(lpszString);
#endif
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
     m_pDocument(NULL)     
{
}

CFrameWnd::~CFrameWnd()
{
}

CView::CView(CWnd* parent) 
   : CWnd(parent), 
     m_pDocument(NULL) 
{
}

CView::~CView()
{
}

CDialog::CDialog(int dlgID, CWnd *parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QDialog(parent);
   else
      _qt = new QDialog;
   
   _qtd = dynamic_cast<QDialog*>(_qt);
   _inited = false;
   
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
   if ( pParentWnd )
      _qt->setParent(pParentWnd->toQWidget()); 
   else
      _qt->setParent(NULL);
   if ( pParentWnd == m_pFrameWnd )
      _qt->setParent(NULL);
   SetParent(pParentWnd); 
   foreach ( CWnd* pWnd, mfcToQtWidget ) pWnd->blockSignals(true);
   BOOL result = OnInitDialog(); 
   _inited = true;
   return result;
}

void CDialog::ShowWindow(int code)
{
   switch ( code )
   {
   case SW_SHOW:
      foreach ( CWnd* pWnd, mfcToQtWidget ) pWnd->blockSignals(false);
      _qtd->setVisible(true);
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

INT_PTR CDialog::DoModal()
{ 
   if ( !_inited )
      OnInitDialog();
   _inited = true;
   
   INT_PTR result = _qtd->exec();
   if ( result == QDialog::Accepted )
      return 1;
   else
      return 0;
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
   start(QThread::InheritPriority);
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

POSITION CDocument::GetFirstViewPosition() const 
{ 
   POSITION pos = NULL;
   if ( m_pViews.count() )
   {
      pos = new int; 
      (*pos) = 0; 
   }
   return pos; 
}

CView* CDocument::GetNextView(POSITION pos) const 
{ 
   if ( !pos ) return NULL; // Choker for end-of-list
   CView* pView = m_pViews.at((*pos)++); 
   if ( (*pos) >= m_pViews.count() ) 
   { 
      delete pos; 
      pos = 0; 
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
}

CSingleDocTemplate::CSingleDocTemplate(UINT f,CDocument* pDoc,CFrameWnd* pFrameWnd,CView* pView)
   : CDocTemplate(f,pDoc,pFrameWnd,pView)
{
   CREATESTRUCT cs;
   if ( pView->PreCreateWindow(cs) )
   {
      pView->OnCreate(&cs);
   }
}

CDocument* CSingleDocTemplate::OpenDocumentFile(
   LPCTSTR lpszPathName,
   BOOL bMakeVisible 
)
{
   m_pDoc->OnNewDocument();
   return m_pDoc;
}

BOOL CWinApp::InitInstance()
{
   return TRUE;
}

HCURSOR CWinApp::LoadStandardCursor( 
   LPCTSTR lpszCursorName  
) const
{
   qDebug("LoadStandardCursor needs work...");
//   setCursor()
}

CMenu::CMenu()
   : _qtd(NULL)
{
}

BOOL CMenu::LoadMenu(
   UINT nIDResource 
)
{
   if ( _qtd )
      delete _qtd;
   _qtd = qtMfcMenuResource(nIDResource).toQMenu();
   return TRUE;
}

CMenu* CMenu::GetSubMenu(
   int nPos 
) const
{
   qDebug("CMenu::GetSubMenu() not supported yet.");
   return NULL;
}

BOOL CMenu::CreatePopupMenu()
{
   if ( _qtd )
      delete _qtd;
   _qtd = new QMenu;
   return TRUE;
}

BOOL CMenu::AppendMenu(
   UINT nFlags,
   UINT_PTR nIDNewItem,
   char* lpszNewItem
)
{
   if ( nFlags&MF_POPUP )
   {
      _qtd->addMenu(qtMfcMenuResource(nIDNewItem).toQMenu());
   }
   if ( nFlags&MF_MENUBARBREAK )
   {
      _qtd->addSeparator();
   }
   if ( nFlags&MF_STRING )
   {
      QAction* action = _qtd->addAction(lpszNewItem); // CP: Add slots later
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

BOOL CMenu::AppendMenu(
   UINT nFlags,
   UINT_PTR nIDNewItem,
   LPCTSTR lpszNewItem
)
{
   if ( nFlags&MF_POPUP )
   {
      _qtd->addMenu(qtMfcMenuResource(nIDNewItem).toQMenu());
   }
   if ( nFlags&MF_MENUBARBREAK )
   {
      _qtd->addSeparator();
   }
   if ( nFlags&MF_STRING )
   {
#if UNICODE
      QAction* action = _qtd->addAction(QString::fromWCharArray(lpszNewItem)); // CP: Add slots later
#else
      QAction* action = _qtd->addAction(lpszNewItem); // CP: Add slots later
#endif
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
   QAction* action = _qtd->exec(QPoint(x,y));
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
   
   if ( parent )
      _qt = new QTabWidget(parent->toQWidget());
   else
      _qt = new QTabWidget;

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
   for ( tab = _qtd->count()-1; tab >= 0; tab-- )
      _qtd->removeTab(tab);
   return TRUE;
}

CEdit::CEdit(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QLineEdit(parent->toQWidget());
   else
      _qt = new QLineEdit;

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QLineEdit*>(_qt);
      
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(textChanged(QString)),this,SIGNAL(textChanged(QString)));
}

CEdit::~CEdit()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CEdit::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned 
)
{
   _qtd->blockSignals(true);
   _qtd->setText(QString::number(nValue));
   _qtd->blockSignals(false);
}

UINT CEdit::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   return _qtd->text().toInt();
}

void CEdit::SetDlgItemText(
   int nID,
   LPCTSTR lpszString 
)
{
   _qtd->blockSignals(true);
#if UNICODE
   _qtd->setText(QString::fromWCharArray(lpszString));
#else
   _qtd->setText(lpszString);
#endif
   _qtd->blockSignals(false);
}

int CEdit::GetDlgItemText(
   int nID,
   CString& rString 
) const
{
   rString = _qtd->text();
   return _qtd->text().length();
}

CButton::CButton(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QPushButton(parent->toQWidget());
   else
      _qt = new QPushButton;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QPushButton*>(_qt);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(clicked()),this,SIGNAL(clicked()));
}

CButton::~CButton()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
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

void CButton::CheckDlgButton( 
   int nIDButton, 
   UINT nCheck  
)
{
   _qtd->blockSignals(true);
   _qtd->setChecked(nCheck);
   _qtd->blockSignals(false);
}

UINT CButton::IsDlgButtonChecked( 
   int nIDButton 
) const
{
   return _qtd->isChecked();
}

CCheckBox::CCheckBox(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QCheckBox(parent->toQWidget());
   else
      _qt = new QCheckBox;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QCheckBox*>(_qt);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(clicked()),this,SIGNAL(clicked()));
}

CCheckBox::~CCheckBox()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CCheckBox::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned 
)
{
   _qtd->setText(QString::number(nValue));
}

UINT CCheckBox::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   return _qtd->text().toInt();
}

void CCheckBox::SetDlgItemText(
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

int CCheckBox::GetDlgItemText(
   int nID,
   CString& rString 
) const
{
   rString = _qtd->text();
   return _qtd->text().length();
}

void CCheckBox::CheckDlgButton( 
   int nIDButton, 
   UINT nCheck  
)
{
   _qtd->blockSignals(true);
   _qtd->setChecked(nCheck);
   _qtd->blockSignals(false);
}

UINT CCheckBox::IsDlgButtonChecked( 
   int nIDButton 
) const
{
   return _qtd->isChecked();
}

CSpinButtonCtrl::CSpinButtonCtrl(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QSpinBox(parent->toQWidget());
   else
      _qt = new QSpinBox;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QSpinBox*>(_qt);
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(valueChanged(int)),this,SIGNAL(valueChanged(int)));
}

CSpinButtonCtrl::~CSpinButtonCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CSpinButtonCtrl::SetRange(
   short nLower,
   short nUpper 
)
{
   _qtd->setRange(nLower,nUpper);
}

CSliderCtrl::CSliderCtrl(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QSlider(parent->toQWidget());
   else
      _qt = new QSlider;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QSlider*>(_qt);
   
   // Not sure if there's vertical sliders in MFC...
   _qtd->setOrientation(Qt::Horizontal);
   _qtd->setTickPosition(QSlider::TicksBelow);
   
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

CStatic::CStatic(CWnd *parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QLabel(parent->toQWidget());
   else
      _qt = new QLabel;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QLabel*>(_qt);
   
   // Pass-through signals
}

CStatic::~CStatic()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
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

CGroupBox::CGroupBox(CWnd *parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;
   
   if ( parent )
      _qt = new QGroupBox(parent->toQWidget());
   else
      _qt = new QGroupBox;
   
   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QGroupBox*>(_qt);
   
   _qtd->setContentsMargins(0,0,0,0);
   
   // Pass-through signals
}

CGroupBox::~CGroupBox()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
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
   if ( !pos ) return CString(); // Choker for end-of-list
   QStringList files = _qtd->selectedFiles();
   CString file = files.at((*pos)++); 
   if ( (*pos) >= files.count() ) 
   { 
      delete pos; 
      pos = 0; 
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

QMap<int,CString> qtMfcStringResources;

CString qtMfcStringResource(int id)
{
   return qtMfcStringResources.value(id);
}

QMap<int,CMenu> qtMfcMenuResources;

CMenu qtMfcMenuResource(int id)
{
   return qtMfcMenuResources.value(id);
}

QMap<int,CBitmap> qtMfcBitmapResources;

CBitmap qtMfcBitmapResource(int id)
{
   return qtMfcBitmapResources.value(id);
}

