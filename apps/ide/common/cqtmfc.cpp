#include "cqtmfc.h"

#include <stdarg.h>

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
   _qstr.append(str);
}

CString::CString(const char* str)
{
   _qstr.clear();
   _qstr.append(str);
}

CString::CString(TCHAR* str)
{
   _qstr.clear();
   _qstr.append((char*)str);
}

CString::CString(QString str)
{
   _qstr.clear();
   _qstr.append(str);
}

CString::CString(const CString& ref)
{
   _qstr.clear();
   _qstr.append(ref._qstr);
}

CString::~CString()
{
   _qstr.clear();
}

void CString::Format(const char* fmt, ...)
{
   qDebug("WARNING: IF THE PRORGRAM JUST CRASHED A QSTRING MAY HAVE BEEN PASSED TO CString::Format");
   va_list argptr;
   va_start(argptr,fmt);
   FormatV(fmt,argptr);
   va_end(argptr);
}

void CString::FormatV(const char* fmt, va_list ap)
{
   // CPTODO: UN-HACK!!!
   char local[2048];
   _qstr.clear();
   vsprintf(local,fmt,ap);
   _qstr.append(local);
//   _qstr.vsprintf(fmt,ap);
}

void CString::Format(LPCTSTR fmt, ...)
{
   qDebug("WARNING: IF THE PRORGRAM JUST CRASHED A QSTRING MAY HAVE BEEN PASSED TO CString::Format");
   va_list argptr;
   va_start(argptr,fmt);
   FormatV(fmt,argptr);
   va_end(argptr);
}

void CString::FormatV(LPCTSTR fmt, va_list ap)
{
   // CPTODO: UN-HACK!!!
   char local[2048];
   _qstr.clear();
   vsprintf(local,(const char*)fmt,ap);
   _qstr.append(local);
//   _qstr.vsprintf((const char*)fmt,ap);
}

CString& CString::operator=(const char* str)
{
   _qstr.clear();
   _qstr.append(str);
   return *this;
}

CString& CString::operator=(TCHAR* str)
{
   _qstr.clear();
   _qstr.append((char*)str);
   return *this;
}

CString& CString::operator+=(TCHAR* str)
{
   _qstr.append((char*)str);
   return *this;
}

CString& CString::operator=(QString str)
{
   _qstr.clear();
   _qstr.append(str);
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
   _qstr.append(str._qstr);
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

CString::operator const char*()
{
   return _qstr.toAscii().constData();
}

CString::operator QString()
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

LPTSTR CString::GetBuffer()
{
   return (TCHAR*)_qstr.toAscii().data();
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
   return _qstr.compare(QString((char*)lpsz),Qt::CaseInsensitive);
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

/*
 *  Class CPen
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
}

CBrush::CBrush(
   COLORREF crColor 
)
{
   QColor color(GetRValue(crColor),GetGValue(crColor),GetBValue(crColor));
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

/*
 *  Class CDC
 */

CDC::CDC(QWidget *parent)
{
   _qpainter = new QPainter(parent);
}

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
}
int CDC::DrawText(
   const CString& str,
   LPRECT lpRect,
   UINT nFormat 
)
{
}
void CDC::FillSolidRect(
   LPCRECT lpRect,
   COLORREF clr 
)
{
}
void CDC::FillSolidRect(
   int x,
   int y,
   int cx,
   int cy,
   COLORREF clr 
)
{
}
BOOL CDC::GradientFill( 
   TRIVERTEX* pVertices, 
   ULONG nVertices, 
   void* pMesh, 
   ULONG nMeshElements, 
   DWORD dwMode  
)
{
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
}
BOOL CDC::TextOut(
   int x,
      int y,
      const CString& str 
)
{
}

/*
 *  Class CCursorPos
 */

CCursorPos::CCursorPos() : m_iRow(0), m_iChannel(0), m_iColumn(0) 
{
}

CCursorPos::CCursorPos(int Row, int Channel, int Column) : m_iRow(Row), m_iChannel(Channel), m_iColumn(Column) 
{
}

const CCursorPos& CCursorPos::operator=(const CCursorPos &pos) 
{
	// Copy position
	m_iRow = pos.m_iRow;
	m_iColumn = pos.m_iColumn;
	m_iChannel = pos.m_iChannel;
	return *this;
}

bool CCursorPos::Invalid() const 
{
	return (m_iRow == -1) || (m_iColumn == -1) || (m_iChannel == -1);
}

/*
 *  Class CSelection
 */

int CSelection::GetRowStart() const 
{
	return (m_cpEnd.m_iRow > m_cpStart.m_iRow ?  m_cpStart.m_iRow : m_cpEnd.m_iRow);
}

int CSelection::GetRowEnd() const 
{
	return (m_cpEnd.m_iRow > m_cpStart.m_iRow ? m_cpEnd.m_iRow : m_cpStart.m_iRow);
}

int CSelection::GetColStart() const 
{
	int Col;
	if (m_cpStart.m_iChannel == m_cpEnd.m_iChannel)
		Col = (m_cpEnd.m_iColumn > m_cpStart.m_iColumn ? m_cpStart.m_iColumn : m_cpEnd.m_iColumn); 
	else if (m_cpEnd.m_iChannel > m_cpStart.m_iChannel)
		Col = m_cpStart.m_iColumn;
	else 
		Col = m_cpEnd.m_iColumn;
	switch (Col) {
		case 2: Col = 1; break;
		case 5: case 6: Col = 4; break;
		case 8: case 9: Col = 7; break;
		case 11: case 12: Col = 10; break;
		case 14: case 15: Col = 13; break;
	}
	return Col;
}

int CSelection::GetColEnd() const 
{
	int Col;
	if (m_cpStart.m_iChannel == m_cpEnd.m_iChannel)
		Col = (m_cpEnd.m_iColumn > m_cpStart.m_iColumn ? m_cpEnd.m_iColumn : m_cpStart.m_iColumn); 
	else if (m_cpEnd.m_iChannel > m_cpStart.m_iChannel)
		Col = m_cpEnd.m_iColumn;
	else
		Col = m_cpStart.m_iColumn;
	switch (Col) {
		case 1: Col = 2; break;					// Instrument
		case 4: case 5: Col = 6; break;			// Eff 1
		case 7: case 8: Col = 9; break;			// Eff 2
		case 10: case 11: Col = 12; break;		// Eff 3
		case 13: case 14: Col = 15; break;		// Eff 4
	}
	return Col;	
}

int CSelection::GetChanStart() const 
{
	return (m_cpEnd.m_iChannel > m_cpStart.m_iChannel ? m_cpStart.m_iChannel : m_cpEnd.m_iChannel); 
}

int CSelection::GetChanEnd() const 
{
	return (m_cpEnd.m_iChannel > m_cpStart.m_iChannel ? m_cpEnd.m_iChannel : m_cpStart.m_iChannel); 
}

bool CSelection::IsWithin(CCursorPos pos) const 
{
	if (pos.m_iRow >= GetRowStart() && pos.m_iRow <= GetRowEnd()) {
		if (pos.m_iChannel == GetChanStart() && pos.m_iChannel == GetChanEnd()) {
			if (pos.m_iColumn >= GetColStart() && pos.m_iColumn <= GetColEnd()) {
				return true;
			}
		}
		else if (pos.m_iChannel == GetChanStart() && pos.m_iChannel != GetChanEnd()) {
			if (pos.m_iColumn >= GetColStart()) {
				return true;
			}
		}
		else if (pos.m_iChannel == GetChanEnd() && pos.m_iChannel != GetChanStart()) {
			if (pos.m_iColumn <= GetColEnd()) {
				return true;
			}
		}
		else if (pos.m_iChannel >= GetChanStart() && pos.m_iChannel < GetChanEnd()) {
			return true;
		}
	}
	return false;
}

bool CSelection::IsSingleChannel() const 
{
	return (m_cpStart.m_iChannel == m_cpEnd.m_iChannel);
}

void CSelection::SetStart(CCursorPos pos) 
{
	m_cpStart = pos;
}

void CSelection::SetEnd(CCursorPos pos) 
{
	m_cpEnd = pos;
}

