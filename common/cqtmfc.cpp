#include "cqtmfc.h"

#include <stdarg.h>

#include <QLinearGradient>

size_t strlen(const wchar_t* str)
{
   int len = 0;
   if ( str )
   {
      while ( *(str+len) ) { ++len; }
   }
   return len;
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
}

BOOL WINAPI OpenClipboard(
  HWND hWndNewOwner
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

CString::CString(const CString& ref)
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

void CString::UpdateScratch()
{
    _qstrn = QByteArray(_qstr.toLatin1());
    _qstrn.data();
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
#if UNICODE
   wvsprintf(local,fmt,ap);
   _qstr.clear();
   _qstr = QString::fromWCharArray(local);
//   _qstr.vsprintf((const char*)fmt,ap);
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
#ifdef UNICODE
   wvsprintf(local,fmt,ap);
   _qstr += QString::fromWCharArray(local);
#else
   vsprintf(local,fmt,ap);
   _qstr += QString(local);
#endif
//   _qstr.vsprintf((const char*)fmt,ap);
   UpdateScratch();
}

CString& CString::operator=(LPSTR str)
{
   _qstr.clear();
   _qstr = QString(str);
   UpdateScratch();
   return *this;
}

CString& CString::operator+=(LPSTR str)
{
   _qstr.append(QString(str));
   UpdateScratch();
   return *this;
}

CString& CString::operator=(LPCSTR str)
{
   _qstr.clear();
   _qstr = QString(str);
   UpdateScratch();
   return *this;
}

CString& CString::operator+=(LPCSTR str)
{
   _qstr.append(QString(str));
   UpdateScratch();
   return *this;
}

CString& CString::operator=(LPWSTR str)
{
   _qstr.clear();
   _qstr = QString::fromWCharArray(str);
   UpdateScratch();
   return *this;
}

CString& CString::operator+=(LPWSTR str)
{
   _qstr.append(QString::fromWCharArray(str));
   UpdateScratch();
   return *this;
}

CString& CString::operator=(LPCWSTR str)
{
   _qstr.clear();
   _qstr = QString::fromWCharArray(str);
   UpdateScratch();
   return *this;
}

CString& CString::operator+=(LPCWSTR str)
{
   _qstr.append(QString::fromWCharArray(str));
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

CString& CString::operator+=(QString str)
{
   _qstr.append(str);
   UpdateScratch();
   return *this;
}

CString& CString::operator=(CString str)
{
   _qstr.clear();
   _qstr = str._qstr;
   UpdateScratch();
   return *this;
}

CString& CString::operator+=(CString str)
{
   _qstr.append(str._qstr);
   UpdateScratch();
   return *this;
}

bool CString::operator==(const CString& str) const
{
   return _qstr == str._qstr;
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
#ifdef UNICODE
   return (LPCWSTR)_qstr.unicode();
#else
   return _qstrn.constData();
#endif
}

LPCTSTR CString::GetBuffer() const
{
#ifdef UNICODE
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
#ifdef UNICODE
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

CFile::CFile(CString& lpszFileName, int nOpenFlags)
{
   QFile::OpenMode flags;
   _qfile.setFileName(CString(lpszFileName.GetString()));
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

BOOL CFile::Open(
   LPCTSTR lpszFileName,
   UINT nOpenFlags,
   CFileException* pError
)
{
   QFile::OpenMode flags;
   _qfile.setFileName((char*)lpszFileName);
   if ( nOpenFlags&modeRead ) flags = QIODevice::ReadOnly;
   if ( nOpenFlags&modeWrite ) flags = QIODevice::WriteOnly;
   if ( nOpenFlags&modeCreate ) flags |= QIODevice::Truncate;
   return _qfile.open(flags);
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
   QBitmap bitmap = (QBitmap)*pBitmap;
   _qbrush.setTextureImage(bitmap.toImage());
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
#ifdef UNICODE
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
#ifdef UNICODE
   _qfont.setFamily(QString::fromWCharArray(lpLogFont->lfFaceName));
#else
   _qfont.setFamily(lpLogFont->lfFaceName);
#endif
   _qfont.setPointSize(lpLogFont->lfHeight);
   _qfont.setItalic(lpLogFont->lfItalic);
   _qfont.setBold(lpLogFont->lfWeight>=FW_BOLD);
   return TRUE;
}

/*
 *  Class CDC
 */

CDC::~CDC()
{
   if ( _qpainter )
   {
      _qpainter->end();
      delete _qpainter;
      _qpainter = NULL;
   }
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
}

void CDC::Draw3dRect( int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight )
{
   QPen tlc(QColor(GetRValue(clrTopLeft),GetGValue(clrTopLeft),GetBValue(clrTopLeft)));
   QPen brc(QColor(GetRValue(clrBottomRight),GetGValue(clrBottomRight),GetBValue(clrBottomRight)));
   QPen orig = _qpainter->pen();
   x -= _windowOrg.x;
   y -= _windowOrg.y;
   _qpainter->setPen(tlc);
   _qpainter->drawLine(x,y,x+cx-1,y);
   _qpainter->drawLine(x,y,x,y+cy-1);
   _qpainter->setPen(brc);
   _qpainter->drawLine(x+cx-1,y+cy-1,x,y+cy-1);
   _qpainter->drawLine(x+cx-1,y+cy-1,x+cx-1,y);
   _qpainter->setPen(orig);
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
   _qpainter->setPen(QPen(_textColor));
//   _qpainter->setFont((QFont)*_font);
   _qpainter->drawText(rect,qstr.toLatin1().constData());
   return strlen(str.GetBuffer());   
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
   _qpainter->setPen(QPen(_textColor));
//   _qpainter->setFont((QFont)*_font);
   _qpainter->drawText(rect,qstr.left(nCount).toLatin1().constData());
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
   int el;
   
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
   _qpainter->setPen(QPen(_textColor));
//   _qpainter->setFont((QFont)*_font);
   x += -_windowOrg.x;
   y += -_windowOrg.y;
   y += fontMetrics.ascent();
   _qpainter->drawText(x,y,qstr.left(nCount));
   return TRUE;
}
BOOL CDC::TextOut(
   int x,
      int y,
      const CString& str 
)
{
   QFontMetrics fontMetrics((QFont)*_font);
   _qpainter->setPen(QPen(_textColor));
//   _qpainter->setFont((QFont)*_font);
   x += -_windowOrg.x;
   y += -_windowOrg.y;
   y += fontMetrics.ascent();
   _qpainter->drawText(x,y,(const QString&)str);
   return TRUE;
}

void CComboBox::ResetContent()
{
   clear();
}

int CComboBox::AddString(CString& text)
{
   addItem(text);
}

void CComboBox::SetCurSel(int sel)
{
   setCurrentIndex(sel);
}

int CListCtrl::InsertColumn(
   int nCol,
   LPCTSTR lpszColumnHeading,
   int nFormat,
   int nWidth,
   int nSubItem
)
{
   insertColumn(nCol);
   QTableWidgetItem twi;
   twi.setText(QString::fromWCharArray(lpszColumnHeading));
   setHorizontalHeaderItem(nCol,&twi);
}

int CListCtrl::InsertItem(
   int nItem,
   LPCTSTR lpszItem,
   int nImage 
)
{
   QTableWidgetItem twi;
   twi.setText(QString::fromWCharArray(lpszItem));
   insertRow(nItem);
   setItem(nItem,3,&twi);
}

BOOL CListCtrl::SetCheck(
   int nItem,
   BOOL fCheck
)
{
   qDebug("CListCtrl::SetCheck!");
}

BOOL CListCtrl::SetItemText(
   int nItem,
   int nSubItem,
   LPCTSTR lpszText 
)
{
   QTableWidgetItem twi;
   twi.setText(QString::fromWCharArray(lpszText));
   setItem(nItem,nSubItem,&twi);
}

BOOL SetItemState(
   int nItem,
   UINT nState,
   UINT nMask 
)
{
   qDebug("CListCtrl::SetItemState!");
}

BOOL CScrollBar::SetScrollInfo(
   LPSCROLLINFO lpScrollInfo,
   BOOL bRedraw 
)
{
   if ( lpScrollInfo->fMask&SIF_RANGE )
   {
      setMinimum(lpScrollInfo->nMin);
      setMaximum(lpScrollInfo->nMax);
   }
   if ( lpScrollInfo->fMask&SIF_POS )
   {
      setValue(lpScrollInfo->nPos);
   }
   if ( lpScrollInfo->fMask&SIF_TRACKPOS )
   {
      setValue(lpScrollInfo->nTrackPos);
   }
   if ( lpScrollInfo->fMask&SIF_PAGE )
   {
      setPageStep(lpScrollInfo->nPage);
   }
}

int CScrollBar::SetScrollPos(
   int nPos,
   BOOL bRedraw 
)
{
   int pos = value();
   setValue(nPos);
   return pos;
}

void CScrollBar::SetScrollRange(
   int nMinPos,
   int nMaxPos,
   BOOL bRedraw
)
{
   setMinimum(nMinPos);
   setMaximum(nMaxPos);
}   

void CScrollBar::ShowScrollBar(
   BOOL bShow
)
{
   setVisible(bShow);
}   

BOOL CScrollBar::EnableScrollBar(
   UINT nArrowFlags
)
{
   setEnabled(nArrowFlags==ESB_ENABLE_BOTH?true:false);
}

CWnd* CWnd::focusWnd = NULL;

CWnd::CWnd(QWidget *parent) 
   : QWidget(parent), 
     m_pFrameWnd(NULL),
     verticalScrollBar(NULL),
     horizontalScrollBar(NULL)
{
}

CWnd::~CWnd()
{
   if ( verticalScrollBar && !verticalScrollBar->parent())
      delete verticalScrollBar;
   if ( horizontalScrollBar && !horizontalScrollBar->parent() )
      delete horizontalScrollBar;
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
   setParent(pParentWnd);
   setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   OnCreate(&createStruct);
   return TRUE;
}

int CWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   QGridLayout* grid = dynamic_cast<QGridLayout*>(layout());
   
   if ( lpCreateStruct->style&WS_VSCROLL )
   {
      verticalScrollBar = new CScrollBar(Qt::Vertical);
      grid->addWidget(verticalScrollBar,0,1);
   }
   if ( lpCreateStruct->style&WS_HSCROLL )
   {
      horizontalScrollBar = new CScrollBar(Qt::Horizontal);
      grid->addWidget(horizontalScrollBar,1,0);   
   }
   return 0;
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
   qDebug("CWnd::GetDlgItem");
   return NULL;
}

void CWnd::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned
)
{
   CWnd* pWnd = GetDlgItem(nID);
   qDebug("SetDlgItemInt");
}

UINT CWnd::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   CWnd* pWnd = GetDlgItem(nID);
   qDebug("GetDlgItemInt");
   return 0;
}

int CWnd::GetDlgItemText(
   int nID,
   CString& rString 
) const
{
   CWnd* pWnd = GetDlgItem(nID);
   qDebug("GetDlgItemText");
   return 0;
}

void CWnd::SetDlgItemText(
   int nID,
   LPCTSTR lpszString 
)
{
   CWnd* pWnd = GetDlgItem(nID);
   qDebug("SetDlgItemText");
}

BOOL CScrollBar::Create(
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID 
)
{
   if ( dwStyle&SBS_VERT )
   {
      setOrientation(Qt::Vertical);
   }
   if ( dwStyle&SBS_HORZ )
   {
      setOrientation(Qt::Horizontal);
   }
   setParent(pParentWnd);
   setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
}

CScrollBar* CWnd::GetScrollBarCtrl(
   int nBar 
) const
{
   switch ( nBar )
   {
   case SB_VERT:
      return verticalScrollBar;
      break;
   case SB_HORZ:
      return horizontalScrollBar;
      break;
   }
}

BOOL CWnd::SetScrollInfo(
   int nBar,
   LPSCROLLINFO lpScrollInfo,
   BOOL bRedraw
)
{
   if ( ((nBar==SB_VERT) && (!verticalScrollBar)) ||
        ((nBar==SB_HORZ) && (!horizontalScrollBar)) )
   {
      return FALSE;
   }
   if ( lpScrollInfo->fMask&SIF_RANGE )
   {
      switch ( nBar )
      {
      case SB_HORZ:
         horizontalScrollBar->setMinimum(lpScrollInfo->nMin);
         horizontalScrollBar->setMaximum(lpScrollInfo->nMax);
         break;
      case SB_VERT:
         verticalScrollBar->setMinimum(lpScrollInfo->nMin);
         verticalScrollBar->setMaximum(lpScrollInfo->nMax);
         break;
      }
   }
   if ( lpScrollInfo->fMask&SIF_POS )
   {
      switch ( nBar )
      {
      case SB_HORZ:
         horizontalScrollBar->setValue(lpScrollInfo->nPos);
         break;
      case SB_VERT:
         verticalScrollBar->setValue(lpScrollInfo->nPos);
         break;
      }
   }
   if ( lpScrollInfo->fMask&SIF_TRACKPOS )
   {
      switch ( nBar )
      {
      case SB_HORZ:
         horizontalScrollBar->setValue(lpScrollInfo->nTrackPos);
         break;
      case SB_VERT:
         verticalScrollBar->setValue(lpScrollInfo->nTrackPos);
         break;
      }
   }
   if ( lpScrollInfo->fMask&SIF_PAGE )
   {
      switch ( nBar )
      {
      case SB_HORZ:
         horizontalScrollBar->setPageStep(lpScrollInfo->nPage);
         break;
      case SB_VERT:
         verticalScrollBar->setPageStep(lpScrollInfo->nPage);
         break;
      }
   }
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
      horizontalScrollBar->setMinimum(nMinPos);
      horizontalScrollBar->setMaximum(nMaxPos);
      break;
   case SB_VERT:
      verticalScrollBar->setMinimum(nMinPos);
      verticalScrollBar->setMaximum(nMaxPos);
      break;
   }
}

int CWnd::SetScrollPos(
   int nBar,
   int nPos,
   BOOL bRedraw
)
{
   switch ( nBar )
   {
   case SB_HORZ:
      horizontalScrollBar->setValue(nPos);
      break;
   case SB_VERT:
      verticalScrollBar->setValue(nPos);
      break;
   }
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

void CWnd::GetWindowRect(
   LPRECT lpRect 
) const
{
   lpRect->left = rect().left();
   lpRect->right = rect().right();
   lpRect->top = rect().top();
   lpRect->bottom = rect().bottom();
}

void CWnd::GetClientRect(
   LPRECT lpRect 
) const
{
   lpRect->left = rect().left();
   lpRect->right = rect().right();
   lpRect->top = rect().top();
   lpRect->bottom = rect().bottom();
}

void CWnd::ShowWindow(int code)
{
   switch ( code )
   {
   case SW_SHOW:
      setVisible(true);
      break;
   case SW_HIDE:
      setVisible(false);
      break;
   }
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
}

BOOL CMenu::AppendMenu(
   UINT nFlags,
   UINT_PTR nIDNewItem,
   LPCTSTR lpszNewItem
)
{
   if ( nFlags == MF_STRING )
   {
      QAction* action = addAction(QString::fromWCharArray(lpszNewItem)); // CP: Add slots later
      mfcToQtMenu.insert(nIDNewItem,action);
   }
}

UINT CMenu::CheckMenuItem(
   UINT nIDCheckItem,
   UINT nCheck 
)
{
   QAction* action = mfcToQtMenu.value(nIDCheckItem);
   if ( action )
   {
      action->setChecked(nCheck);
   }
}

BOOL CMenu::TrackPopupMenu(
   UINT nFlags,
   int x,
   int y,
   CWnd* pWnd,
   LPCRECT lpRect
)
{
   popup(QPoint(x,y));
}
