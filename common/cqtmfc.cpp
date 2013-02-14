#include "cqtmfc.h"

#include <stdarg.h>

#include <QLinearGradient>

size_t strlen(const TCHAR* str)
{
   return strlen((const char*)str);
}

/*
 *  Class CString
 */

CString::CString()
{
   _qstr.clear();
}

CString::CString(char* str)
{
   _qstr.clear();
   _qstr = str;
}

CString::CString(const char* str)
{
   _qstr.clear();
   _qstr = str;
}

CString::CString(TCHAR* str)
{
   _qstr.clear();
   _qstr = QString::fromWCharArray(str);
}

CString::CString(const TCHAR* str)
{
   _qstr.clear();
   _qstr = QString::fromWCharArray(str);
}

CString::CString(QString str)
{
   _qstr.clear();
   _qstr = str;
}

CString::CString(const CString& ref)
{
   _qstr.clear();
   _qstr = ref._qstr;
}

CString::~CString()
{
   _qstr.clear();
}

void CString::Format(const char* fmt, ...)
{
   va_list argptr;
   va_start(argptr,fmt);
   FormatV(fmt,argptr);
   va_end(argptr);
}

void CString::FormatV(const char* fmt, va_list ap)
{
   // CPTODO: UN-HACK!!!
   char local[2048];
   vsprintf(local,fmt,ap);
   _qstr.clear();
   _qstr = local;
//   _qstr.vsprintf(fmt,ap);
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
   WCHAR local[2048];
   wvsprintf(local,fmt,ap);
   _qstr.clear();
   _qstr = QString::fromWCharArray(local);
//   _qstr.vsprintf((const char*)fmt,ap);
}

void CString::AppendFormat(const char* fmt, ...)
{
   va_list argptr;
   va_start(argptr,fmt);
   AppendFormatV(fmt,argptr);
   va_end(argptr);
}

void CString::AppendFormatV(const char* fmt, va_list ap)
{
   // CPTODO: UN-HACK!!!
   char local[2048];
   vsprintf(local,fmt,ap);
   _qstr += local;
//   _qstr.vsprintf(fmt,ap);
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
   WCHAR local[2048];
   wvsprintf(local,fmt,ap);
   _qstr += QString::fromWCharArray(local);
//   _qstr.vsprintf((const char*)fmt,ap);
}

CString& CString::operator=(const char* str)
{
   _qstr.clear();
   _qstr = str;
   return *this;
}

CString& CString::operator+=(const char* str)
{
   _qstr.append(str);
   return *this;
}

CString& CString::operator=(TCHAR* str)
{
   _qstr.clear();
   _qstr = QString::fromWCharArray(str);
   return *this;
}

CString& CString::operator+=(TCHAR* str)
{
   _qstr.append(QString::fromWCharArray(str));
   return *this;
}

CString& CString::operator=(QString str)
{
   _qstr.clear();
   _qstr = str;
   return *this;
}

CString& CString::operator+=(QString str)
{
   _qstr.append(str);
   return *this;
}

CString& CString::operator=(CString str)
{
   _qstr.clear();
   _qstr = str._qstr;
   return *this;
}

CString& CString::operator+=(CString str)
{
   _qstr.append(str._qstr);
   return *this;
}

bool CString::operator==(const CString& str) const
{
   return _qstr == str._qstr;
}

CString::operator const char*() const
{
   return _qstr.toAscii().constData();
}

CString::operator const TCHAR*() const
{
   return (TCHAR*)_qstr.toAscii().constData();
}

CString::operator const QString&() const
{
   return _qstr;
}

void CString::Empty() 
{ 
   _qstr.clear(); 
}

const char* CString::GetString() const
{
   return _qstr.toAscii().constData();
}

LPTSTR CString::GetBuffer() const
{
   return (TCHAR*)_qstr.unicode();
}

CString CString::Left( int nCount ) const
{
   return CString(_qstr.left(nCount).toAscii().data());
}

CString CString::Right( int nCount ) const
{
   return CString(_qstr.right(nCount).toAscii().data());
}

int CString::GetLength() const
{
   return _qstr.length();
}

int CString::CompareNoCase( LPCTSTR lpsz ) const
{
   return _qstr.compare(QString::fromWCharArray(lpsz),Qt::CaseInsensitive);
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
   _qfile.setFileName((const char*)lpszFileName);
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
   _rect.top = 0;
   _rect.bottom = 0;
   _rect.left = 0;
   _rect.right = 0;
}

CRect::CRect( 
   int l, 
   int t, 
   int r, 
   int b  
)
{
   _rect.top = t;
   _rect.bottom = b;
   _rect.left = l;
   _rect.right = r;
}

CRect::CRect( 
   const RECT& srcRect  
)
{
   _rect.top = srcRect.top;
   _rect.bottom = srcRect.bottom;
   _rect.left = srcRect.left;
   _rect.right = srcRect.right;
}

CRect::CRect( 
   LPCRECT lpSrcRect  
)
{
   _rect.top = lpSrcRect->top;
   _rect.bottom = lpSrcRect->bottom;
   _rect.left = lpSrcRect->left;
   _rect.right = lpSrcRect->right;
}

CRect::CRect( 
   POINT point, 
   SIZE size  
)
{
   _rect.top = point.y;
   _rect.bottom = point.y+size.cy;
   _rect.left = point.x;
   _rect.right = point.x+size.cx;
}

CRect::CRect( 
   POINT topLeft, 
   POINT bottomRight  
)
{
   _rect.top = topLeft.y;
   _rect.bottom = bottomRight.y;
   _rect.left = topLeft.x;
   _rect.right = bottomRight.x;
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
   _qfont.setFamily((char*)lpszFacename);
   _qfont.setPointSize(nHeight);
   _qfont.setItalic(bItalic);
   _qfont.setUnderline(bUnderline);
   _qfont.setStrikeOut(cStrikeOut);
   _qfont.setBold(nWeight>=FW_BOLD);
}

BOOL CFont::CreateFontIndirect(
   const LOGFONT* lpLogFont 
)
{
   _qfont.setFamily((char*)lpLogFont->lfFaceName);
   _qfont.setPointSize(lpLogFont->lfHeight);
   _qfont.setItalic(lpLogFont->lfItalic);
   _qfont.setBold(lpLogFont->lfWeight>=FW_BOLD);
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
   QString qstr = QString::fromWCharArray(str.GetBuffer());
   _qpainter->drawText(rect,qstr.toAscii().constData());
   
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
}
BOOL CDC::LineTo( 
   int x, 
   int y  
)
{
   _qpainter->drawLine(_lineOrg.x,_lineOrg.y,x,y);
   _lineOrg.x = x;
   _lineOrg.y = y;
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
}
int CDC::SelectObject(
   CRgn* pRgn 
)
{
}
COLORREF CDC::SetPixel( int x, int y, COLORREF crColor )
{
}
BOOL CDC::TextOut(
   int x,
   int y,
   LPCTSTR lpszString,
   int nCount 
)
{
   QRect rect;
   QString qstr = QString::fromWCharArray(lpszString);
   QFontMetrics fontMetrics((QFont)*_font);
   rect.setTopLeft(QPoint(x,y));
   rect.setBottomRight(QPoint(x+fontMetrics.size(Qt::TextSingleLine,qstr.left(nCount)).width()+10,y+fontMetrics.height()));
   rect.translate(-QPoint(_windowOrg.x,_windowOrg.y));
   _qpainter->setPen(QPen(_textColor));
   _qpainter->drawText(rect,qstr.left(nCount).toAscii().constData());
}
BOOL CDC::TextOut(
   int x,
      int y,
      const CString& str 
)
{
   QRect rect;
   QFontMetrics fontMetrics((QFont)*_font);
   rect.setTopLeft(QPoint(x,y));
   rect.setBottomRight(QPoint(x+fontMetrics.size(Qt::TextSingleLine,str.GetString()).width()+10,y+fontMetrics.height()));
   rect.translate(-QPoint(_windowOrg.x,_windowOrg.y));
   _qpainter->setPen(QPen(_textColor));
   _qpainter->drawText(rect,QString::fromWCharArray(str.GetBuffer()));
}

void CComboBox::ResetContent()
{
   clear();
}

int CComboBox::AddString(CString& text)
{
   addItem(text.GetString());
}

void CComboBox::SetCurSel(int sel)
{
   setCurrentIndex(sel);
}

UINT CWnd::SetTimer(UINT id, UINT interval, void*)
{
   int qtId = startTimer(interval);
   mfcToQtTimer.insert((int)id,qtId);
   qtToMfcTimer.insert(qtId,(int)id);
   return (UINT)id;
}

void CWnd::KillTimer(UINT id)
{
   killTimer(mfcToQtTimer.value((int)id));
   qtToMfcTimer.remove(mfcToQtTimer.value((int)id));
   mfcToQtTimer.remove((int)id);
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
   qDebug("CreateThread");
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
