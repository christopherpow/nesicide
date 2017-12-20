#include <QCoreApplication>
#include <QApplication>
#include <QLinearGradient>
#include <QHeaderView>
#include <QMessageBox>
#include <QPixmap>
#include <QMainWindow>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenuBar>
#include <QUuid>
#include <QDateTime>
#include <QPaintEngine>
#include <QProcess>
#include <QWaitCondition>

#include "cqtmfc.h"
#include "resource.h"

#include <stdarg.h>

CWinApp* ptrToTheApp;

CBrush nullBrush;

QString gApplicationName = "FamiTracker";

using namespace qtmfc_workaround;

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
extern ACCEL* qtMfcAcceleratorResource(UINT id);

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

QHash<int,QString> qtIconNames;
QHash<int,QIcon*> qtIconResources;

QIcon* qtIconResource(int id)
{
   QIcon* ret = NULL;
   if ( qtIconResources.contains(id) )
   {
      ret = qtIconResources.value(id);
   }
   else
   {
      ret = new QIcon(qtIconNames.value(id));
      qtIconResources.insert(id,ret);
   }
   return ret;
}

QString qtIconName(int id)
{
   return qtIconNames.value(id);
}

CWinApp* AfxGetApp()
{
   return ptrToTheApp;
}

CFrameWnd* AfxGetMainWnd()
{
   return ptrToTheApp->m_pMainWnd;
}

CWinThread* AfxGetThread()
{
   // Just return main CWinThread for now...
   return ptrToTheApp;
}

void AfxDebugBreak()
{
   qFatal("AfxDebugBreak");
}

AFX_STATIC void AFXAPI _AfxAppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	CDocTemplate* pTemplate, CString* pstrDefaultExt)
{
	ENSURE_VALID(pTemplate);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	CString strFilterExt, strFilterName;
	if (pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt) &&
		!strFilterExt.IsEmpty() &&
		pTemplate->GetDocString(strFilterName, CDocTemplate::filterName) &&
		!strFilterName.IsEmpty())
	{
		if (pstrDefaultExt != NULL)
			pstrDefaultExt->Empty();

		// add to filter
		filter += strFilterName;
		ASSERT(!filter.IsEmpty());  // must have a file type name
		filter += (TCHAR)'\0';  // next string please

		int iStart = 0;
		do
		{
			CString strExtension = strFilterExt.Tokenize( _T( ";" ), iStart );

			if (iStart != -1)
			{
				// a file based document template - add to filter list

				// If you hit the following ASSERT, your document template
				// string is formatted incorrectly.  The section of your
				// document template string that specifies the allowable file
				// extensions should be formatted as follows:
				//    .<ext1>;.<ext2>;.<ext3>
				// Extensions may contain wildcards (e.g. '?', '*'), but must
				// begin with a '.' and be separated from one another by a ';'.
				// Example:
				//    .jpg;.jpeg
				ASSERT(strExtension[0] == '.');
				if ((pstrDefaultExt != NULL) && pstrDefaultExt->IsEmpty())
				{
					// set the default extension
					*pstrDefaultExt = strExtension.Mid( 1 );  // skip the '.'
					ofn.lpstrDefExt = const_cast< LPTSTR >((LPCTSTR)(*pstrDefaultExt));
					ofn.nFilterIndex = ofn.nMaxCustFilter + 1;  // 1 based number
				}

				filter += (TCHAR)'*';
				filter += strExtension;
				filter += (TCHAR)';';  // Always append a ';'.  The last ';' will get replaced with a '\0' later.
			}
		} while (iStart != -1);

		filter.SetAt( filter.GetLength()-1, '\0' );;  // Replace the last ';' with a '\0'
		ofn.nMaxCustFilter++;
	}
}

int WINAPI MessageBox(
   HWND    hWnd,
   LPCTSTR lpText,
   LPCTSTR lpCaption,
   UINT    uType
)
{
   QString button1 = "";
   QString button2 = "";
   QString button3 = "";
   int buttons = uType&0xF;
   int icon = uType&0xF0;
   int ret;
#if UNICODE
   QString text = QString::fromWCharArray(lpText);
   QString caption = QString::fromWCharArray(lpCaption);
#else
   QString text = QString::fromLatin1(lpText);
   QString caption = QString::fromLatin1(lpCaption);
#endif

   if ( buttons == MB_ABORTRETRYIGNORE )
   {
      button1 = "Abort";
      button2 = "Retry";
      button3 = "Ignore";
   }
   else if ( buttons == MB_CANCELTRYCONTINUE )
   {
      button1 = "Cancel";
      button2 = "Try Again";
      button3 = "Continue";
   }
   else if ( buttons == MB_OKCANCEL )
   {
      button1 = "OK";
      button2 = "Cancel";
   }
   else if ( buttons == MB_RETRYCANCEL )
   {
      button1 = "Retry";
      button2 = "Cancel";
   }
   else if ( buttons == MB_YESNO )
   {
      button1 = "Yes";
      button2 = "No";
   }
   else if ( buttons == MB_YESNOCANCEL )
   {
      button1 = "Yes";
      button2 = "No";
      button3 = "Cancel";
   }
   else
   {
      button1 = "OK";
   }

   switch ( icon )
   {
   case MB_ICONERROR:
      ret = QMessageBox::critical(0,caption,text,button1,button2,button3);
      break;
   case MB_ICONEXCLAMATION:
      ret = QMessageBox::warning(0,caption,text,button1,button2,button3);
      break;
   case MB_ICONQUESTION:
      ret = QMessageBox::question(0,caption,text,button1,button2,button3);
      break;
   default:
      ret = QMessageBox::information(0,caption,text,button1,button2,button3);
      break;
   }
   switch ( ret )
   {
   case 0:
      // First button.
      if ( button1 == "Abort" )
      {
         ret = IDABORT;
      }
      else if ( button1 == "Cancel" )
      {
         ret = IDCANCEL;
      }
      else if ( button1 == "OK" )
      {
         ret = IDOK;
      }
      else if ( button1 == "Retry" )
      {
         ret = IDRETRY;
      }
      else if ( button1 == "Yes" )
      {
         ret = IDYES;
      }
      break;
   case 1:
      // Second button.
      if ( button2 == "Retry" )
      {
         ret = IDRETRY;
      }
      else if ( button2 == "Try Again" )
      {
         ret = IDRETRY;
      }
      else if ( button2 == "Cancel" )
      {
         ret = IDCANCEL;
      }
      else if ( button2 == "No" )
      {
         ret = IDNO;
      }
      break;
   case 2:
      // Third button.
      if ( button2 == "Continue" )
      {
         ret = IDCONTINUE;
      }
      else if ( button2 == "Ignore" )
      {
         ret = IDIGNORE;
      }
      else if ( button2 == "Cancel" )
      {
         ret = IDCANCEL;
      }
      break;
   }
   return ret;
}

int AfxMessageBox(
   LPCTSTR lpszText,
   UINT nType,
   UINT nIDHelp
)
{
   QString button1 = "";
   QString button2 = "";
   QString button3 = "";
   int buttons = nType&0xF;
   int icon = nType&0xF0;
   int ret;
#if UNICODE
   QString text = QString::fromWCharArray(lpszText);
#else
   QString text = QString::fromLatin1(lpszText);
#endif

   if ( buttons == MB_ABORTRETRYIGNORE )
   {
      button1 = "Abort";
      button2 = "Retry";
      button3 = "Ignore";
   }
   else if ( buttons == MB_CANCELTRYCONTINUE )
   {
      button1 = "Cancel";
      button2 = "Try Again";
      button3 = "Continue";
   }
   else if ( buttons == MB_OKCANCEL )
   {
      button1 = "OK";
      button2 = "Cancel";
   }
   else if ( buttons == MB_RETRYCANCEL )
   {
      button1 = "Retry";
      button2 = "Cancel";
   }
   else if ( buttons == MB_YESNO )
   {
      button1 = "Yes";
      button2 = "No";
   }
   else if ( buttons == MB_YESNOCANCEL )
   {
      button1 = "Yes";
      button2 = "No";
      button3 = "Cancel";
   }
   else
   {
      button1 = "OK";
   }

   switch ( icon )
   {
   case MB_ICONERROR:
      ret = QMessageBox::critical(0,"FamiTracker",text,button1,button2,button3);
      break;
   case MB_ICONEXCLAMATION:
      ret = QMessageBox::warning(0,"FamiTracker",text,button1,button2,button3);
      break;
   case MB_ICONQUESTION:
      ret = QMessageBox::question(0,"FamiTracker",text,button1,button2,button3);
      break;
   default:
      ret = QMessageBox::information(0,"FamiTracker",text,button1,button2,button3);
      break;
   }
   switch ( ret )
   {
   case 0:
      // First button.
      if ( button1 == "Abort" )
      {
         ret = IDABORT;
      }
      else if ( button1 == "Cancel" )
      {
         ret = IDCANCEL;
      }
      else if ( button1 == "OK" )
      {
         ret = IDOK;
      }
      else if ( button1 == "Retry" )
      {
         ret = IDRETRY;
      }
      else if ( button1 == "Yes" )
      {
         ret = IDYES;
      }
      break;
   case 1:
      // Second button.
      if ( button2 == "Retry" )
      {
         ret = IDRETRY;
      }
      else if ( button2 == "Try Again" )
      {
         ret = IDRETRY;
      }
      else if ( button2 == "Cancel" )
      {
         ret = IDCANCEL;
      }
      else if ( button2 == "No" )
      {
         ret = IDNO;
      }
      break;
   case 2:
      // Third button.
      if ( button2 == "Continue" )
      {
         ret = IDCONTINUE;
      }
      else if ( button2 == "Ignore" )
      {
         ret = IDIGNORE;
      }
      else if ( button2 == "Cancel" )
      {
         ret = IDCANCEL;
      }
      break;
   }
   return ret;
}

int AFXAPI AfxMessageBox(
   UINT nIDPrompt,
   UINT nType,
   UINT nIDHelp
)
{
   CString prompt = qtMfcStringResource(nIDPrompt);
   return AfxMessageBox(prompt,nType,nIDHelp);
}

void AfxFormatString1(
   CString& rString,
   UINT nIDS,
   LPCTSTR lpsz1
)
{
   rString.Format(nIDS,lpsz1);
}

void AfxFormatString2(
   CString& rString,
   UINT nIDS,
   LPCTSTR lpsz1,
   LPCTSTR lpsz2
)
{
   rString.Format(nIDS,lpsz1,lpsz2);
}

void AfxFormatStrings(
   CString& rString,
   UINT nIDS,
   LPCTSTR* lpszArray,
   UINT nCount
)
{
   switch ( nCount )
   {
      case 1:
         AfxFormatString1(rString,nIDS,lpszArray[0]);
         break;
      case 2:
         AfxFormatString2(rString,nIDS,lpszArray[0],lpszArray[1]);
         break;
      case 3:
         rString.Format(nIDS,lpszArray[0],lpszArray[1],lpszArray[2]);
         break;
      default:
         qFatal("AfxFormatStrings doesn't handle %d parameters",nCount);
         break;
   }
}

void AfxGetFileTitle(
   LPCTSTR path,
   LPTSTR file,
   UINT max
)
{
   QString str;
#if UNICODE
   str = QString::fromWCharArray(path);
#else
   str = QString::fromLatin1(path);
#endif
   str = str.right(str.length()-str.lastIndexOf(QRegExp("[/\\]")));
#if UNICODE
   wcsncpy(file,str.unicode(),max);
#else
   strncpy(file,str.toLatin1().constData(),max);
#endif
}

HINSTANCE AFXAPI AfxGetInstanceHandle( )
{
   return (HINSTANCE)ptrToTheApp;
}

HCURSOR WINAPI SetCursor(
   HCURSOR hCursor
)
{
   return (HCURSOR)0;
}

int WINAPI GetClassName(
   HWND   hWnd,
   LPTSTR lpClassName,
   int    nMaxCount
)
{
   CWnd* pWnd = CWnd::FromHandle(hWnd);
   if ( pWnd )
   {
      qDebug("className: %s",pWnd->GetRuntimeClass()->m_lpszClassName);
#if UNICODE
   wcsncpy(lpClassName,pWnd->GetRuntimeClass()->m_lpszClassName,nMaxCount);
   return wcslen(lpClassName);
#else
   strncpy(lpClassName,pWnd->GetRuntimeClass()->m_lpszClassName,nMaxCount);
   return strlen(lpClassName);
#endif

   }
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

DWORD WINAPI GetFileAttributes(
   LPCTSTR lpFileName
)
{
   QFileInfo fileInfo;
   DWORD attribs = 0;
#if UNICODE
   fileInfo.setFile(QString::fromWCharArray(lpFileName));
#else
   fileInfo.setFile(QString::fromLatin1(lpFileName));
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
//   QString keyString;
   LONG modParam = 0;
   switch ( (lParam>>16)&0xFF )
   {
      // Mappable
//   case VK_ABNT_C1:		// 0xC1	Abnt C1
//      break;
//   case VK_ABNT_C2:		// 0xC2	Abnt C2
//      break;
   case VK_SHIFT:
      lParam = Qt::ShiftModifier;
      break;
   case VK_CONTROL:
      lParam = Qt::ControlModifier;
      break;
   case VK_ADD:		// 0x6B	Numpad +
      modParam = Qt::KeypadModifier;
      lParam = Qt::Key_Plus;
      break;
   case VK_ATTN:		// 0xF6	Attn
      break;
   case VK_BACK:		// 0x08	Backspace
      lParam = Qt::Key_Backspace;
      break;
   case VK_CANCEL:		// 0x03	Break
      lParam = Qt::Key_Cancel;
      break;
   case VK_CLEAR:		// 0x0C	Clear
      lParam = Qt::Key_Clear;
      break;
   case VK_CRSEL:		// 0xF7	Cr Sel
      break;
   case VK_DECIMAL:		// 0x6E	Numpad .
      modParam = Qt::KeypadModifier;
      lParam = Qt::Key_Period;
      break;
   case VK_DIVIDE:		// 0x6F	Numpad /
      modParam = Qt::KeypadModifier;
      lParam = Qt::Key_division;
      break;
   case VK_EREOF:		// 0xF9	Er Eof
      break;
   case VK_ESCAPE:		// 0x1B	Esc
      lParam = Qt::Key_Escape;
      break;
   case VK_EXECUTE:		// 0x2B	Execute
      lParam = Qt::Key_Execute;
      break;
   case VK_EXSEL:		// 0xF8	Ex Sel
      break;
//   case VK_ICO_CLEAR:		// 0xE6	IcoClr
//      break;
//   case VK_ICO_HELP:		// 0xE3	IcoHlp
//      break;
   case '0':    // 0x30 ('0')  0
      lParam = Qt::Key_0;
      break;
   case '1':		// 0x31 ('1')	1
      lParam = Qt::Key_1;
      break;
   case '2':		// 0x32 ('2')	2
      lParam = Qt::Key_2;
      break;
   case '3':		// 0x33 ('3')	3
      lParam = Qt::Key_3;
      break;
   case '4':		// 0x34 ('4')	4
      lParam = Qt::Key_4;
      break;
   case '5':		// 0x35 ('5')	5
      lParam = Qt::Key_5;
      break;
   case '6':		// 0x36 ('6')	6
      lParam = Qt::Key_6;
      break;
   case '7':		// 0x37 ('7')	7
      lParam = Qt::Key_7;
      break;
   case '8':		// 0x38 ('8')	8
      lParam = Qt::Key_8;
      break;
   case '9':		// 0x39 ('9')	9
      lParam = Qt::Key_9;
      break;
   case 'A':		// 0x41 ('A')	A
      lParam = Qt::Key_A;
      break;
   case 'B':		// 0x42 ('B')	B
      lParam = Qt::Key_B;
      break;
   case 'C':		// 0x43 ('C')	C
      lParam = Qt::Key_C;
      break;
   case 'D':		// 0x44 ('D')	D
      lParam = Qt::Key_D;
      break;
   case 'E':		// 0x45 ('E')	E
      lParam = Qt::Key_E;
      break;
   case 'F':		// 0x46 ('F')	F
      lParam = Qt::Key_F;
      break;
   case 'G':		// 0x47 ('G')	G
      lParam = Qt::Key_G;
      break;
   case 'H':		// 0x48 ('H')	H
      lParam = Qt::Key_H;
      break;
   case 'I':		// 0x49 ('I')	I
      lParam = Qt::Key_I;
      break;
   case 'J':		// 0x4A ('J')	J
      lParam = Qt::Key_J;
      break;
   case 'K':		// 0x4B ('K')	K
      lParam = Qt::Key_K;
      break;
   case 'L':		// 0x4C ('L')	L
      lParam = Qt::Key_L;
      break;
   case 'M':		// 0x4D ('M')	M
      lParam = Qt::Key_M;
      break;
   case 'N':		// 0x4E ('N')	N
      lParam = Qt::Key_N;
      break;
   case 'O':		// 0x4F ('O')	O
      lParam = Qt::Key_O;
      break;
   case 'P':		// 0x50 ('P')	P
      lParam = Qt::Key_P;
      break;
   case 'Q':		// 0x51 ('Q')	Q
      lParam = Qt::Key_Q;
      break;
   case 'R':		// 0x52 ('R')	R
      lParam = Qt::Key_R;
      break;
   case 'S':		// 0x53 ('S')	S
      lParam = Qt::Key_S;
      break;
   case 'T':		// 0x54 ('T')	T
      lParam = Qt::Key_T;
      break;
   case 'U':		// 0x55 ('U')	U
      lParam = Qt::Key_U;
      break;
   case 'V':		// 0x56 ('V')	V
      lParam = Qt::Key_V;
      break;
   case 'W':		// 0x57 ('W')	W
      lParam = Qt::Key_W;
      break;
   case 'X':		// 0x58 ('X')	X
      lParam = Qt::Key_X;
      break;
   case 'Y':		// 0x59 ('Y')	Y
      lParam = Qt::Key_Y;
      break;
   case 'Z':		// 0x5A ('Z')	Z
      lParam = Qt::Key_Z;
      break;
   case VK_MULTIPLY:		// 0x6A	Numpad *
      modParam = Qt::KeypadModifier;
      lParam = Qt::Key_multiply;
      break;
   case VK_NONAME:		// 0xFC	NoName
      break;
   case VK_NUMPAD0:     // 0x60	Numpad 0
      lParam = Qt::KeypadModifier|Qt::Key_0;
      break;
   case VK_NUMPAD1:		// 0x61	Numpad 1
      lParam = Qt::KeypadModifier|Qt::Key_1;
      break;
   case VK_NUMPAD2:		// 0x62	Numpad 2
      lParam = Qt::KeypadModifier|Qt::Key_2;
      break;
   case VK_NUMPAD3:		// 0x63	Numpad 3
      lParam = Qt::KeypadModifier|Qt::Key_3;
      break;
   case VK_NUMPAD4:		// 0x64	Numpad 4
      lParam = Qt::KeypadModifier|Qt::Key_4;
      break;
   case VK_NUMPAD5:		// 0x65	Numpad 5
      lParam = Qt::KeypadModifier|Qt::Key_5;
      break;
   case VK_NUMPAD6:		// 0x66	Numpad 6
      lParam = Qt::KeypadModifier|Qt::Key_6;
      break;
   case VK_NUMPAD7:		// 0x67	Numpad 7
      lParam = Qt::KeypadModifier|Qt::Key_7;
      break;
   case VK_NUMPAD8:		// 0x68	Numpad 8
      lParam = Qt::KeypadModifier|Qt::Key_8;
      break;
   case VK_NUMPAD9:		// 0x69	Numpad 9
      lParam = Qt::KeypadModifier|Qt::Key_9;
      break;
   case VK_OEM_1:		// 0xBA	OEM_1 (: ;)
      lParam = Qt::Key_Colon;
      break;
//   case VK_OEM_102:		// 0xE2	OEM_102 (> <)
//      lParam = Qt::Key_Greater;
//      break;
   case VK_OEM_2:		// 0xBF	OEM_2 (? /)
      lParam = Qt::Key_Question;
      break;
   case VK_OEM_3:		// 0xC0	OEM_3 (~ `)
      qDebug("Tilde???");
      break;
   case VK_OEM_4:		// 0xDB	OEM_4 ({ [)
      lParam = Qt::Key_BraceLeft;
      break;
   case VK_OEM_5:		// 0xDC	OEM_5 (| \)
      lParam = Qt::Key_Backslash;
      break;
   case VK_OEM_6:		// 0xDD	OEM_6 (} ])
      lParam = Qt::Key_BraceRight;
      break;
   case VK_OEM_7:		// 0xDE	OEM_7 (" ')
      lParam = Qt::Key_QuoteDbl;
      break;
   case VK_OEM_8:		// 0xDF	OEM_8 (� !)
      lParam = Qt::Key_Exclam;
      break;
//   case VK_OEM_ATTN:		// 0xF0	Oem Attn
//      break;
//   case VK_OEM_AUTO:		// 0xF3	Auto
//      break;
//   case VK_OEM_AX:		// 0xE1	Ax
//      break;
//   case VK_OEM_BACKTAB:		// 0xF5	Back Tab
//      break;
   case VK_OEM_CLEAR:		// 0xFE	OemClr
      break;
//   case VK_OEM_COMMA:		// 0xBC	OEM_COMMA (< ,)
//      lParam = Qt::Key_Comma;
//      break;
//   case VK_OEM_COPY:		// 0xF2	Copy
//      lParam = Qt::Key_Copy;
//      break;
//   case VK_OEM_CUSEL:		// 0xEF	Cu Sel
//      lParam = Qt::Key_Cut;
//      break;
//   case VK_OEM_ENLW:		// 0xF4	Enlw
//      break;
//   case VK_OEM_FINISH:		// 0xF1	Finish
//      break;
//   case VK_OEM_FJ_LOYA:		// 0x95	Loya
//      break;
//   case VK_OEM_FJ_MASSHOU:		// 0x93	Mashu
//      break;
//   case VK_OEM_FJ_ROYA:		// 0x96	Roya
//      break;
//   case VK_OEM_FJ_TOUROKU:		// 0x94	Touroku
//      break;
//   case VK_OEM_JUMP:		// 0xEA	Jump
//      break;
//   case VK_OEM_MINUS:		// 0xBD	OEM_MINUS (_ -)
//      lParam = Qt::Key_Minus;
//      break;
//   case VK_OEM_PA1:		// 0xEB	OemPa1
//      break;
//   case VK_OEM_PA2:		// 0xEC	OemPa2
//      break;
//   case VK_OEM_PA3:		// 0xED	OemPa3
//      break;
//   case VK_OEM_PERIOD:		// 0xBE	OEM_PERIOD (> .)
//      lParam = Qt::Key_Period;
//      break;
//   case VK_OEM_PLUS:		// 0xBB	OEM_PLUS (+ =)
//      lParam = Qt::Key_Plus;
//      break;
//   case VK_OEM_RESET:		// 0xE9	Reset
//      break;
//   case VK_OEM_WSCTRL:		// 0xEE	WsCtrl
//      break;
   case VK_PA1:		// 0xFD	Pa1
      break;
//   case VK_PACKET:		// 0xE7	Packet
//      break;
   case VK_PLAY:		// 0xFA	Play
      lParam = Qt::Key_Play;
      break;
   case VK_PROCESSKEY:		// 0xE5	Process
      break;
   case VK_RETURN:		// 0x0D	Enter
      lParam = Qt::Key_Return;
      break;
   case VK_SELECT:		// 0x29	Select
      lParam = Qt::Key_Select;
      break;
   case VK_SEPARATOR:		// 0x6C	Separator
      break;
   case VK_SPACE:		// 0x20	Space
      lParam = Qt::Key_Space;
      break;
   case VK_SUBTRACT:		// 0x6D	Num -
      modParam = Qt::KeypadModifier;
      lParam = Qt::Key_Minus;
      break;
   case VK_TAB:		// 0x09	Tab
      lParam = Qt::Key_Tab;
      break;
   case VK_ZOOM:		// 0xFB	Zoom
      lParam = Qt::Key_Zoom;
      break;
      // Non-mappable
//   case VK__none_:		// 0xFF	no VK mapping
//      break;
   case VK_ACCEPT:		// 0x1E	Accept
      break;
   case VK_APPS:		// 0x5D	Context Menu
      break;
//   case VK_BROWSER_BACK:		// 0xA6	Browser Back
//      break;
//   case VK_BROWSER_FAVORITES:		// 0xAB	Browser Favorites
//      break;
//   case VK_BROWSER_FORWARD:		// 0xA7	Browser Forward
//      break;
//   case VK_BROWSER_HOME:		// 0xAC	Browser Home
//      break;
//   case VK_BROWSER_REFRESH:		// 0xA8	Browser Refresh
//      break;
//   case VK_BROWSER_SEARCH:		// 0xAA	Browser Search
//      break;
//   case VK_BROWSER_STOP:		// 0xA9	Browser Stop
//      break;
   case VK_CAPITAL:		// 0x14	Caps Lock
      lParam = Qt::Key_CapsLock;
      break;
   case VK_CONVERT:		// 0x1C	Convert
      break;
   case VK_DELETE:		// 0x2E	Delete
      lParam = Qt::Key_Delete;
      break;
   case VK_DOWN:		// 0x28	Arrow Down
      lParam = Qt::Key_Down;
      break;
   case VK_END:		// 0x23	End
      lParam = Qt::Key_End;
      break;
   case VK_F1:		// 0x70	F1
      lParam = Qt::Key_F1;
      break;
   case VK_F10:		// 0x79	F10
      lParam = Qt::Key_F10;
      break;
   case VK_F11:		// 0x7A	F11
      lParam = Qt::Key_F11;
      break;
   case VK_F12:		// 0x7B	F12
      lParam = Qt::Key_F12;
      break;
   case VK_F13:		// 0x7C	F13
      lParam = Qt::Key_F13;
      break;
   case VK_F14:		// 0x7D	F14
      lParam = Qt::Key_F14;
      break;
   case VK_F15:		// 0x7E	F15
      lParam = Qt::Key_F15;
      break;
   case VK_F16:		// 0x7F	F16
      lParam = Qt::Key_F16;
      break;
   case VK_F17:		// 0x80	F17
      lParam = Qt::Key_F17;
      break;
   case VK_F18:		// 0x81	F18
      lParam = Qt::Key_F18;
      break;
   case VK_F19:		// 0x82	F19
      lParam = Qt::Key_F19;
      break;
   case VK_F2:		// 0x71	F2
      lParam = Qt::Key_F2;
      break;
   case VK_F20:		// 0x83	F20
      lParam = Qt::Key_F20;
      break;
   case VK_F21:		// 0x84	F21
      lParam = Qt::Key_F21;
      break;
   case VK_F22:		// 0x85	F22
      lParam = Qt::Key_F22;
      break;
   case VK_F23:		// 0x86	F23
      lParam = Qt::Key_F23;
      break;
   case VK_F24:		// 0x87	F24
      lParam = Qt::Key_F24;
      break;
   case VK_F3:		// 0x72	F3
      lParam = Qt::Key_F3;
      break;
   case VK_F4:		// 0x73	F4
      lParam = Qt::Key_F4;
      break;
   case VK_F5:		// 0x74	F5
      lParam = Qt::Key_F5;
      break;
   case VK_F6:		// 0x75	F6
      lParam = Qt::Key_F6;
      break;
   case VK_F7:		// 0x76	F7
      lParam = Qt::Key_F7;
      break;
   case VK_F8:		// 0x77	F8
      lParam = Qt::Key_F8;
      break;
   case VK_F9:		// 0x78	F9
      lParam = Qt::Key_F9;
      break;
   case VK_FINAL:		// 0x18	Final
      break;
   case VK_HELP:		// 0x2F	Help
      lParam = Qt::Key_Help;
      break;
   case VK_HOME:		// 0x24	Home
      lParam = Qt::Key_Home;
      break;
//   case VK_ICO_00:		// 0xE4	Ico00 *
//      break;
   case VK_INSERT:		// 0x2D	Insert
      lParam = Qt::Key_Insert;
      break;
   case VK_JUNJA:		// 0x17	Junja
      break;
   case VK_KANA:		// 0x15	Kana
      break;
   case VK_KANJI:		// 0x19	Kanji
      break;
//   case VK_LAUNCH_APP1:		// 0xB6	App1
//      break;
//   case VK_LAUNCH_APP2:		// 0xB7	App2
//      break;
//   case VK_LAUNCH_MAIL:		// 0xB4	Mail
//      break;
//   case VK_LAUNCH_MEDIA_SELECT:		// 0xB5	Media
//      break;
   case VK_LBUTTON:		// 0x01	Left Button **
      break;
   case VK_LCONTROL:		// 0xA2	Left Ctrl
      break;
   case VK_LEFT:		// 0x25	Arrow Left
      break;
   case VK_LMENU:		// 0xA4	Left Alt
      break;
   case VK_LSHIFT:		// 0xA0	Left Shift
      break;
   case VK_LWIN:		// 0x5B	Left Win
      break;
   case VK_MBUTTON:		// 0x04	Middle Button **
      break;
//   case VK_MEDIA_NEXT_TRACK:		// 0xB0	Next Track
//      break;
//   case VK_MEDIA_PLAY_PAUSE:		// 0xB3	Play / Pause
//      break;
//   case VK_MEDIA_PREV_TRACK:		// 0xB1	Previous Track
//      break;
//   case VK_MEDIA_STOP:		// 0xB2	Stop
//      break;
   case VK_MODECHANGE:		// 0x1F	Mode Change
      break;
   case VK_NEXT:		// 0x22	Page Down
      lParam = Qt::Key_PageDown;
      break;
   case VK_NONCONVERT:		// 0x1D	Non Convert
      break;
   case VK_NUMLOCK:		// 0x90	Num Lock
      lParam = Qt::Key_NumLock;
      break;
//   case VK_OEM_FJ_JISHO:		// 0x92	Jisho
//      break;
   case VK_PAUSE:		// 0x13	Pause
      lParam = Qt::Key_Pause;
      break;
   case VK_PRINT:		// 0x2A	Print
      lParam = Qt::Key_Print;
      break;
   case VK_PRIOR:		// 0x21	Page Up
      lParam = Qt::Key_PageUp;
      break;
   case VK_RBUTTON:		// 0x02	Right Button **
      break;
   case VK_RCONTROL:		// 0xA3	Right Ctrl
      lParam = Qt::Key_Control;
      break;
   case VK_RIGHT:		// 0x27	Arrow Right
      lParam = Qt::Key_Right;
      break;
   case VK_RMENU:		// 0xA5	Right Alt
      lParam = Qt::Key_Alt;
      break;
   case VK_RSHIFT:		// 0xA1	Right Shift
      lParam = Qt::Key_Shift;
      break;
   case VK_RWIN:		// 0x5C	Right Win
      break;
   case VK_SCROLL:		// 0x91	Scrol Lock
      lParam = Qt::Key_ScrollLock;
      break;
   case VK_SLEEP:		// 0x5F	Sleep
      lParam = Qt::Key_Sleep;
      break;
   case VK_SNAPSHOT:		// 0x2C	Print Screen
      lParam = Qt::Key_Print;
      break;
   case VK_UP:		// 0x26	Arrow Up
      lParam = Qt::Key_Up;
      break;
//   case VK_VOLUME_DOWN:		// 0xAE	Volume Down
//      lParam = Qt::Key_VolumeDown;
//      break;
//   case VK_VOLUME_MUTE:		// 0xAD	Volume Mute
//      lParam = Qt::Key_VolumeMute;
//      break;
//   case VK_VOLUME_UP:		// 0xAF	Volume Up
//      lParam = Qt::Key_VolumeUp;
//      break;
//   case VK_XBUTTON1:		// 0x05	X Button 1 **
//      break;
//   case VK_XBUTTON2:		// 0x06	X Button 2 **
//      break;
   }

   QKeySequence key(modParam+lParam);
   QString keyString = key.toString();
#if UNICODE
   wcsncpy(lpString,keyString.unicode(),cchSize);
   return wcslen(lpString);
#else
   strncpy(lpString,keyString.toLatin1().constData(),cchSize);
   return strlen(lpString);
#endif
}

#if !defined(Q_OS_WIN32)
HMODULE WINAPI LoadLibrary(
   LPCTSTR lpFileName
)
{
   QLibrary* pLib = new QLibrary;
#if UNICODE
   pLib->setFileName(QString::fromWCharArray(lpFileName));
#else
   pLib->setFileName(QString::fromLatin1(lpFileName));
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
   return (FARPROC)pLib->resolve(QString::fromWCharArray(lpProcName).toLatin1().constData());
#else
   return (FARPROC)pLib->resolve(QString::fromLatin1(lpProcName).toLatin1().constData());
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

BOOL WINAPI CopyFile(
   LPCTSTR lpExistingFileName,
   LPCTSTR lpNewFileName,
   BOOL bFailIfExists
)
{
#if UNICODE
   QFile src(QString::fromWCharArray(lpExistingFileName));
   QFile dst(QString::fromWCharArray(lpNewFileName));
#else
   QFile src(QString::fromLatin1(lpExistingFileName));
   QFile dst(QString::fromLatin1(lpNewFileName));
#endif

   if ( bFailIfExists && dst.exists() )
   {
      return FALSE;
   }
   if ( src.open(QIODevice::ReadOnly) )
   {
      if ( dst.open(QIODevice::WriteOnly|QIODevice::Truncate))
      {
         dst.write(src.readAll());
         dst.close();
      }
      src.close();
   }
   else
   {
      return FALSE;
   }
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
   strncpy(lpFilename,QCoreApplication::applicationFilePath().toLatin1().constData(),nSize);
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

#if !defined(Q_OS_WIN32)
static QElapsedTimer tickTimer;
DWORD WINAPI GetTickCount(void)
{
   return tickTimer.elapsed();
}
#endif

DWORD WINAPI GetSysColor(
  int nIndex
)
{
   switch ( nIndex )
   {
   case COLOR_3DFACE:
//   case COLOR_BTNFACE: // CP: Same as COLOR_3DFACE
      return 0xf0f0f0;
      break;
   case COLOR_3DHILIGHT:
//   case COLOR_BTNHIGHLIGHT: // CP: Same as COLOR_3DHILIGHT
      return 0xffffff;
      break;
   case COLOR_BTNSHADOW:
      return 0xeeeeee;
      break;
   case COLOR_APPWORKSPACE:
      return 0xababab;
      break;
   default:
      qDebug("unsupported sys color: %d",nIndex);
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
   {
      sb = new QScrollBar(Qt::Vertical);
   }
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
   case SM_CXDRAG:
      return 4;
      break;
   case SM_CYDRAG:
      return 4;
      break;
   case SM_CXEDGE:
      return 2;
      break;
   case SM_CYEDGE:
      return 2;
      break;
   case SM_CXDLGFRAME:
      return 3;
      break;
   case SM_CYDLGFRAME:
      return 3;
      break;
   case SM_CYCAPTION:
      return 22;
      break;
   default:
      qDebug("unsupported system metric: %d",nIndex);
      break;
   }
   return 0;
}

SHORT WINAPI GetKeyState(
  int nVirtKey
)
{
   Qt::KeyboardModifiers mod = QApplication::keyboardModifiers();
   SHORT ret = 0;
   
   switch ( nVirtKey )
   {
   case VK_CONTROL:
      ret = (mod&Qt::ControlModifier)?0x80:0;
      break;
   case VK_SHIFT:
      ret = (mod&Qt::ShiftModifier)?0x80:0;
      break;
   default:
      qDebug("GetKeyState(%d): unsupported key",nVirtKey);
   }
   return ret;
}

QMimeData* gpClipboardMimeData = NULL;

QHash<UINT,QString> _clipboardFormats;
#define CLIPBOARD_FORMAT_BASE 0xC000
UINT _clipboardReg = CLIPBOARD_FORMAT_BASE;

UINT WINAPI RegisterClipboardFormat(
   LPCTSTR lpszFormat
)
{
   UINT reg = _clipboardReg;
#if UNICODE
   QString clipboardFormat = QString::fromWCharArray(lpszFormat);
#else
   QString clipboardFormat = QString::fromLatin1(lpszFormat);
#endif
   if ( !_clipboardFormats.values().contains(clipboardFormat) )
   {
      _clipboardFormats.insert(reg,clipboardFormat);
   }
   _clipboardReg++;
   return reg;
}

BOOL CWnd::OpenClipboard(
//  HWND hWndNewOwner
)
{
   gpClipboardMimeData = new QMimeData;
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
   pMem->lock();
   gpClipboardMimeData->setData(_clipboardFormats.value(uFormat),
                                QByteArray::fromRawData((const char*)pMem->data(),pMem->size()));
   pMem->unlock();
   QApplication::clipboard()->setMimeData(gpClipboardMimeData);
   return hMem;
}

BOOL WINAPI IsClipboardFormatAvailable(
  UINT format
)
{
   QStringList formats = QApplication::clipboard()->mimeData()->formats();

   if ( formats.count() && formats.contains(_clipboardFormats.value(format)) )
      return TRUE;
   return FALSE;
}

HANDLE WINAPI GetClipboardData(
  UINT uFormat
)
{
   QByteArray value = QApplication::clipboard()->mimeData()->data(_clipboardFormats.value(uFormat));
   QUuid uuid = QUuid::createUuid();
   QSharedMemory* pMem = new QSharedMemory(uuid.toString());
   pMem->create(value.size());
   pMem->lock();
   memcpy(pMem->data(),value.constData(),value.size());
   pMem->unlock();
   return pMem;
}

HGLOBAL WINAPI GlobalAlloc(
  UINT uFlags,
  SIZE_T dwBytes
)
{
   QUuid uuid = QUuid::createUuid();
   QSharedMemory* pMem = new QSharedMemory(uuid.toString());
   pMem->create(dwBytes);
   return pMem;
}

LPVOID WINAPI GlobalLock(
  HGLOBAL hMem
)
{
   QSharedMemory* pMem = (QSharedMemory*)hMem;
   void* pData;

   pData = pMem->data();
   pMem->lock();
   return pData;
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

HGLOBAL WINAPI GlobalFree(
   HGLOBAL hMem
)
{
   QSharedMemory* pMem = (QSharedMemory*)hMem;
   delete pMem;
}


QList<ACCEL*> _acceleratorTables;

HACCEL WINAPI CreateAcceleratorTable(
  LPACCEL lpaccl,
  int cEntries
)
{
   ACCEL* pTable = new ACCEL[cEntries+1];
   memset(pTable,0,(cEntries+1)*sizeof(ACCEL));
   memcpy(pTable,lpaccl,cEntries*sizeof(ACCEL));
   _acceleratorTables.append(pTable);
   return (HACCEL)pTable;
}

HACCEL WINAPI LoadAccelerators(
   HINSTANCE hInstance,
   LPCTSTR lpTableName
)
{
   QString table;
#if UNICODE
   table = QString::fromWCharArray(lpTableName);
#else
   table = QString::fromLatin1(lpTableName);
#endif
   UINT id = (UINT)table.toInt();
   ACCEL* pTable = qtMfcAcceleratorResource(id);
   return (HACCEL)pTable;
}

int WINAPI TranslateAccelerator(
  HWND hWnd,
  HACCEL hAccTable,
  LPMSG lpMsg
)
{
   ACCEL* pAccel = (ACCEL*)hAccTable;
   Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
   if ( (lpMsg->message == WM_KEYDOWN) ||
        (lpMsg->message == WM_SYSKEYDOWN) )
   {
      while ( pAccel->cmd )
      {
         if ( lpMsg->wParam == pAccel->key )
         {
            if ( pAccel->fVirt&FCONTROL )
            {
               if ( !(modifiers & Qt::ControlModifier) )
               {
                  pAccel++;
                  continue;
               }
            }
            if ( pAccel->fVirt&FSHIFT )
            {
               if ( !(modifiers & Qt::ShiftModifier) )
               {
                  pAccel++;
                  continue;
               }
            }
            if ( pAccel->fVirt&FALT )
            {
               if ( !(modifiers & Qt::AltModifier) )
               {
                  pAccel++;
                  continue;
               }
            }
            CWnd* pWnd = (CWnd*)hWnd;

            // CP: Include window handle in message to skip enabled check.
            qDebug("Translating %d key and sending %d message...",pAccel->key,pAccel->cmd);
            pWnd->SendMessage(WM_COMMAND,pAccel->cmd,(LPARAM)pWnd);
            return 1;
         }
         pAccel++;
      }
   }
   return 0;
}

BOOL WINAPI DestroyAcceleratorTable(
  HACCEL hAccel
)
{
   _acceleratorTables.removeAll((ACCEL*)hAccel);
   return TRUE;
}

UINT WINAPI MapVirtualKey(
  UINT uCode,
  UINT uMapType
)
{
   // CP: Nothing to do here?
   if ( uMapType != MAPVK_VK_TO_VSC )
   {
      qDebug("MapVirtualKey: unsupported uMapType %d",uMapType);
   }
   return uCode;
}

//IMPLEMENT_DYNAMIC(CObject,NULL) <- CObject is base...treat it special.
CRuntimeClass CObject::classCObject = 
{
"CObject",
sizeof(CObject),
(UINT)-1,
NULL,
NULL,
NULL
};

CRuntimeClass* CObject::GetRuntimeClass() const
{
   return &classCObject;
}

CObject* CRuntimeClass::CreateObject()
{
   return m_pfnCreateObject();
}

BOOL CObject::IsKindOf( 
   const CRuntimeClass* pClass  
) const
{
   CRuntimeClass* pMyClass = GetRuntimeClass();
   while ( pMyClass )
   {
      if ( pClass == pMyClass )
      {
         return TRUE;
      }
      pMyClass = pMyClass->m_pBaseClass;
   }
   return FALSE;
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
   UpdateScratch();
}

CString::CString(LPCTSTR str)
{
   _qstr.clear();
#if UNICODE
   _qstr = QString::fromWCharArray(str);
#else
   _qstr = QString::fromLatin1(str);
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
   _qstr.append(QString::fromLatin1(qtMfcStringResource(nID).GetString()));
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
   // First replace placement-specifiers in format
   va_list ap_copy;
   va_copy(ap_copy,ap);
   int m = 1;
   QString str = fmt;
   int count = 0;
   do
   {
      QString pat = "%"+QString::number(m);
      if ( str.contains(pat) )
      {
         str.replace(pat,va_arg(ap_copy,LPCTSTR));
         count++;
      }
      else
      {
         break;
      }
   } while ( 1 );
   // If we did any placement-specific replacements, use the updated format str
   if ( count )
      vsprintf(local,str.toLatin1().constData(),ap);
   // Otherwise, use the original fmt format string
   else
      vsprintf(local,fmt,ap);
   _qstr.clear();
   _qstr = QString::fromLatin1(local);
#endif
   UpdateScratch();
}

void CString::AppendChar(TCHAR c)
{
   _qstr += c;
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
   // First replace placement-specifiers in format
   va_list ap_copy;
   va_copy(ap_copy,ap);
   int m = 1;
   QString str = fmt;
   int count = 0;
   do
   {
      QString pat = "%"+QString::number(m);
      if ( str.contains(pat) )
      {
         str.replace(pat,va_arg(ap_copy,LPCTSTR));
         count++;
      }
      else
      {
         break;
      }
   } while ( 1 );
   // If we did any placement-specific replacements, use the updated format str
   if ( count )
      vsprintf(local,str.toLatin1().constData(),ap);
   // Otherwise, use the original fmt format string
   else
      vsprintf(local,fmt,ap);
   _qstr += QString::fromLatin1(local);
#endif
   UpdateScratch();
}

void CString::Truncate(int length)
{
   _qstr.truncate(length);
   UpdateScratch();
}

int CString::FindOneOf( LPCTSTR lpszCharSet ) const
{
#if UNICODE
   return _qstr.indexOf(QString::fromWCharArray(lpszCharSet));
#else
   return _qstr.indexOf(QString::fromLatin1(lpszCharSet));
#endif
}

CString CString::Tokenize(
   LPCTSTR pszTokens,
   int& iStart
) const
{
   CString token;
   QString delim;
   QString temp = _qstr.right(_qstr.length()-iStart);

#if UNICODE
   delim = QString::fromWCharArray(pszTokens);
#else
   delim = QString::fromLatin1(pszTokens);
#endif

   QStringList tokens = temp.split(delim,QString::SkipEmptyParts);

   if ( tokens.count() )
   {
      token = tokens.at(0);
      iStart += tokens.at(0).length();
   }
   else
   {
      iStart = -1;
   }
   return token;
}

int CString::ReverseFind( TCHAR ch ) const
{
#if UNICODE
   return _qstr.lastIndexOf(QString::fromWCharArray(&ch));
#else
   return _qstr.lastIndexOf(ch);
#endif
}

int CString::Compare( LPCTSTR lpsz ) const
{
#if UNICODE
   return _qstr.compare(QString::fromWCharArray(lpsz));
#else
   return _qstr.compare(QString::fromLatin1(lpsz));
#endif
}

BOOL CString::IsEmpty() const
{
   return _qstr.isEmpty();
}

const CString& CString::operator=(const CString& str)
{
   _qstr.clear();
   _qstr = str._qstr;
   UpdateScratch();
   return *this;
}

const CString& CString::operator=(LPCTSTR str)
{
   _qstr.clear();
#if UNICODE
   _qstr = QString::fromWCharArray(str);
#else
   _qstr = QString::fromLatin1(str);
#endif
   UpdateScratch();
   return *this;
}

const CString& CString::operator=(TCHAR c)
{
   _qstr.clear();
#if UNICODE
   TCHAR qc[2] = { c, 0 };
   _qstr = QString::fromWCharArray(qc);
#else
   TCHAR qc[2] = { c, 0 };
   _qstr = QString::fromLatin1(qc);
#endif
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
   _qstr.append(QString::fromLatin1(str));
#endif
   UpdateScratch();
   return *this;
}

CString& CString::operator+=(TCHAR c)
{
#if UNICODE
   TCHAR qc[2] = { c, 0 };
   _qstr.append(QString::fromWCharArray(qc));
#else
   TCHAR qc[2] = { c, 0 };
   _qstr.append(QString::fromLatin1(qc));
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

CString& CString::operator+(LPCTSTR str)
{
#if UNICODE
   _qstr += QString::fromWCharArray(str);
#else
   _qstr += QString::fromLatin1(str);
#endif
   UpdateScratch();
   return *this;
}

CString& CString::operator+(TCHAR c)
{
#if UNICODE
   TCHAR qc[2] = { c, 0 };
   _qstr += QString::fromWCharArray(qc);
#else
   TCHAR qc[2] = { c, 0 };
   _qstr += QString::fromLatin1(qc);
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
   if ( nMinBufLength > _qstr.length() )
      _qstr.resize(nMinBufLength+1); // Space for null-terminator.
   UpdateScratch();
#if UNICODE
   return (LPTSTR)_qstrn.unicode();
#else
   return (LPTSTR)_qstrn.data();
#endif
}

void CString::ReleaseBuffer( int nNewLength )
{
   if ( nNewLength >= 0 )
   {
      // Append null.
      _qstrn[nNewLength] = 0;
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
   return _qstr.indexOf(QString::fromLatin1(lpszSub));
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
   return _qstr.indexOf(QString::fromLatin1(pstr),nStart);
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
   return _qstr.compare(QString::fromLatin1(lpsz),Qt::CaseInsensitive);
#endif
}

TCHAR CString::GetAt( int nIndex ) const
{
   return _qstr.at(nIndex).toLatin1();
}

void CString::SetAt( int nIndex, TCHAR ch )
{
   _qstr[nIndex] = ch;
}

CStringA::CStringA(CString str)
{
   _qstr = str.GetString();
   UpdateScratch();
}

CStringA::CStringA(LPCTSTR str)
{
   _qstr.clear();
#if UNICODE
   _qstr = QString::fromWCharArray(str);
#else
   _qstr = QString::fromLatin1(str);
#endif
   UpdateScratch();
}

CStringA::operator char*() const
{
   return _qstr.toLatin1().data();
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

HANDLE CFile::hFileNull = 0;

IMPLEMENT_DYNAMIC(CFile,CCmdTarget)

BEGIN_MESSAGE_MAP(CFile,CCmdTarget)
END_MESSAGE_MAP()

CFile::CFile()
   : m_hFile(hFileNull)
{
}

CFile::CFile(
   LPCTSTR lpszFileName,
   UINT nOpenFlags
)
   : m_hFile(hFileNull)
{
   QFile::OpenMode flags;
#if UNICODE
   _qfile.setFileName(QString::fromWCharArray(lpszFileName));
#else
   _qfile.setFileName(QString::fromLatin1(lpszFileName));
#endif
   if ( nOpenFlags&modeRead ) flags = QIODevice::ReadOnly;
   if ( nOpenFlags&modeWrite ) flags = QIODevice::WriteOnly;
   if ( nOpenFlags&modeCreate ) flags |= QIODevice::Truncate;
   _qfile.open(flags);
   if ( _qfile.isOpen() )
   {
      m_hFile = (HANDLE)&_qfile;
   }
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
   _qfile.setFileName(QString::fromLatin1(lpszFileName));
#endif
   if ( nOpenFlags&modeRead ) flags = QIODevice::ReadOnly;
   if ( nOpenFlags&modeWrite ) flags = QIODevice::WriteOnly;
   if ( nOpenFlags&modeCreate ) flags |= QIODevice::Truncate;
   BOOL ok = _qfile.open(flags);
   if ( ok )
   {
      m_hFile = (HANDLE)&_qfile;
   }
   return ok;
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
         _qfile.seek(lOff);
         break;
      case current:
         _qfile.seek(_qfile.pos()+lOff);
         break;
      case end:
         _qfile.seek(_qfile.size()+lOff);
         break;
      }
      return _qfile.pos();
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

void PASCAL CFile::Remove(
   LPCTSTR lpszFileName
)
{
#if UNICODE
   QFile::remove(QString::fromWCharArray(lpszFileName));
#else
   QFile::remove(QString::fromLatin1(lpszFileName));
#endif
}

void CFile::Close()
{
   if ( _qfile.isOpen() )
      _qfile.close();
   m_hFile = hFileNull;
}

IMPLEMENT_DYNAMIC(CStdioFile,CFile)

BEGIN_MESSAGE_MAP(CStdioFile,CFile)
END_MESSAGE_MAP()

void CStdioFile::WriteString( 
   LPCTSTR lpsz  
)
{
#if UNICODE
   _qfile.write(QString::fromWCharArray(lpsz).toLatin1());
#else
   _qfile.write(QString::fromLatin1(lpsz).toLatin1());
#endif
}

LPTSTR CStdioFile::ReadString( 
   LPTSTR lpsz, 
   UINT nMax  
)
{
   qint64 bytes = _qfile.readLine(lpsz,nMax);
   if ( bytes < (nMax-1) )
   {
      lpsz[bytes] = '\n';
   }
   if ( bytes )
   {
      return lpsz;
   }
   else
   {
      return NULL;
   }
}

BOOL CStdioFile::ReadString( 
   CString& rString 
)
{
   rString = _qfile.readLine().constData();
   if ( rString.GetLength() )
   {
      rString += '\n';
   }
   return rString.GetLength()?TRUE:FALSE;
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
   right = x+right;
   left = x;
}

void CRect::MoveToY(
   int y
)
{
   bottom = y+bottom;
   top = y;
}

void CRect::MoveToXY(
   int x,
   int y
)
{
   right = x+right;
   left = x;
   bottom = y+bottom;
   top = y;
}

void CRect::MoveToXY(
   POINT point
)
{
   right = point.x+right;
   left = point.x;
   bottom = point.y+bottom;
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

void CRect::OffsetRect( 
   int x, 
   int y  
)
{
   left += x;
   right += x;
   top += y;
   bottom += y;
}

void CRect::OffsetRect( 
   POINT point  
)
{
   left += point.x;
   right += point.x;
   top += point.y;
   bottom += point.y;
}

void CRect::OffsetRect( 
   SIZE size  
)
{
   left += size.cx;
   right += size.cx;
   top += size.cy;
   bottom += size.cy;
}

BOOL CRect::PtInRect(
   POINT point
) const
{
   if ( point.x >= left &&
        point.y >= top &&
        point.x < right &&
        point.y < bottom )
   {
      return TRUE;
   }
   return FALSE;
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

int CFont::GetLogFont(
   LOGFONT * pLogFont
)
{
   strncpy(pLogFont->lfFaceName,_qfont.family().toLatin1().data(),32);
   pLogFont->lfHeight = _qfont.pointSize()/.75;
   pLogFont->lfItalic = _qfont.italic();
   pLogFont->lfWeight = (_qfont.bold()?FW_BOLD:FW_NORMAL);
   return 1;
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
   _qfont.setFamily(QString::fromLatin1(lpszFacename));
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
   _qfont.setFamily(QString::fromLatin1(lpLogFont->lfFaceName));
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
   LOGFONT lf;
   m_hDC = (HDC)NULL;
   m_pWnd = NULL;
   _qwidget = NULL;
   _pen = NULL;
   _brush = NULL;
   _font = new CFont();
   _defaultFont = _font;
   memset(&lf,0,sizeof(LOGFONT));
   strcpy(lf.lfFaceName,"MS Shell Dlg");
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   lf.lfHeight = 8;
#else
   lf.lfHeight = 11;
#endif
   _font->CreateFontIndirect(&lf);
   _bitmap = NULL;
   _bitmapSize = QSize(-1,-1);
   _rgn = NULL;
   _gdiobject = NULL;
   _object = NULL;
   _lineOrg.x = 0;
   _lineOrg.y = 0;
   _bkColor = QColor(0,0,0);
   _bkMode = 0;
   _textColor = QColor(255,255,255);
   _windowOrg.x = 0;
   _windowOrg.y = 0;
   attached = false;
}

CDC::CDC(CWnd* parent)
{
   LOGFONT lf;
   m_hDC = (HDC)NULL;
   m_pWnd = parent;
   _qwidget = parent->toQWidget();
   _pen = NULL;
   _brush = NULL;
   _font = new CFont();
   _defaultFont = _font;
   memset(&lf,0,sizeof(LOGFONT));
   strcpy(lf.lfFaceName,"MS Shell Dlg");
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   lf.lfHeight = 8;
#else
   lf.lfHeight = 11;
#endif
   _font->CreateFontIndirect(&lf);
   _bitmap = NULL;
   _bitmapSize = QSize(-1,-1);
   _rgn = NULL;
   _gdiobject = NULL;
   _object = NULL;
   _lineOrg.x = 0;
   _lineOrg.y = 0;
   _bkColor = QColor(0,0,0);
   _bkMode = 0;
   _textColor = QColor(255,255,255);
   _windowOrg.x = 0;
   _windowOrg.y = 0;
   attached = false;

   attach(parent->toQWidget(),parent);
}

CDC::~CDC()
{
   detach();
   delete _defaultFont;
}

CDC* PASCAL CDC::FromHandle(
   HDC hDC  
)
{
   return (CDC*)hDC;
}

void CDC::attach()
{
   _qpixmap = QPixmap(1,1);
   _qpainter.begin(&_qpixmap);
   m_hDC = (HDC)this;
   attached = true;
}

void CDC::attach(QWidget* qtParent, CWnd* mfcParent, bool transparent)
{
   _qwidget = qtParent;
   if ( _qpainter.isActive() )
      _qpainter.end();
   _qpixmap = QPixmap(_qwidget->size());
   if ( transparent )
      _qpixmap.fill(QColor(0,0,0,0));
   else
      _qpixmap.fill(_qwidget->palette().color(QPalette::Window)); // CP: paint over an existing widget
   _qpainter.begin(&_qpixmap);
   m_hDC = (HDC)this;
   m_pWnd = mfcParent;
   attached = true;
}

void CDC::detach(bool silent)
{
   QObject sig;
   if ( attached )
   {      
      attached = false;
      if ( _qpainter.isActive() )
         _qpainter.end();
      if ( m_pWnd && !silent )
         QObject::connect(&sig,SIGNAL(destroyed(QObject*)),this,SLOT(flush()));
   }
}

void CDC::flush()
{
   int offset = 0;
   if ( _qwidget )
   {
      QPainter p;
      if ( p.begin(_qwidget) )
      {
         if ( m_pWnd )
         {
            offset = m_pWnd->getFrameWidth();
         }
         p.drawPixmap(0,0,_qpixmap);
         p.end();
      }
   }
}

BOOL CDC::CreateCompatibleDC(
   CDC* pDC
)
{
   if ( pDC->widget() )
      attach(pDC->widget(),pDC->window());
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
      _qpainter.setPen((QPen)(*_pen));
   return temp;
}

CBrush* CDC::SelectObject(
   CBrush* pBrush
)
{
   CBrush* temp = _brush;
   _brush = pBrush;
   if ( _brush )
      _qpainter.setBrush((QBrush)(*_brush));
   return temp;
}

CFont* CDC::SelectObject(
   CFont* pFont
)
{
   CFont* temp = _font;
   _font = pFont;
   if ( _font )
      _qpainter.setFont((QFont)(*_font));
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
      _qpainter.drawPixmap(0,0,*_bitmap->toQPixmap());
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
   QImage image = _qpixmap.toImage();
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
   QPixmap tempPixmap = pixmap->copy();
   QSize pixmapSize = pSrcDC->pixmapSize();
   pixmapSize = pixmapSize.boundedTo(QSize(nWidth,nHeight));
   if ( pixmap && (pixmapSize.width() >= 0) )
      _qpainter.drawPixmap(x,y,pixmapSize.width(),pixmapSize.height(),tempPixmap,xSrc,ySrc,pixmapSize.width(),pixmapSize.height());
   else
      _qpainter.drawPixmap(x,y,nWidth,nHeight,tempPixmap,xSrc,ySrc,nWidth,nHeight);
   return TRUE;
}

CWaitCursor::CWaitCursor()
{
   QApplication::setOverrideCursor(Qt::WaitCursor);
}

CWaitCursor::~CWaitCursor()
{
   QApplication::restoreOverrideCursor();
}

HICON WINAPI LoadIcon(
   HINSTANCE hInstance,
   LPCTSTR lpIconName
)
{
   CWinApp* pApp = (CWinApp*)hInstance;
#if UNICODE
   QString str = QString::fromWCharArray(lpIconName);
#else
   QString str = QString::fromLatin1(lpIconName);
#endif
   bool ok;
   UINT nIDResource = str.toInt(&ok);
   if ( ok )
   {
      return (HICON)qtIconResource(nIDResource);
   }
   else
   {
      qFatal("cannot find icon resource");
   }
}

BOOL WINAPI GetFileVersionInfo(
   LPCTSTR lptstrFilename,
   DWORD dwHandle,
   DWORD dwLen,
   LPVOID lpData
)
{
}

DWORD WINAPI GetFileVersionInfoSize(
   LPCTSTR lptstrFilename,
   LPDWORD lpdwHandle
)
{
}

BOOL WINAPI VerQueryValue(
   char*& pBlock,
   LPCTSTR lpSubBlock,
   LPVOID *lplpBuffer,
   PUINT puLen
)
{
}

int StretchDIBits(
  HDC hDC,
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
   CDC* pDC = (CDC*)hDC;
   QImage image((const uchar*)lpBits,nSrcWidth,nSrcHeight,QImage::Format_RGB32);
   image = image.scaled(nDestWidth,nDestHeight);
   pDC->painter()->drawImage(XDest,YDest,image);
   return 0;
}

BOOL CDC::DrawEdge(
   LPRECT lpRect,
   UINT nEdge,
   UINT nFlags
)
{
   QRect rect1(lpRect->left,lpRect->top,lpRect->right-lpRect->left-1,lpRect->bottom-lpRect->top-1);
   QRect rect2(lpRect->left+1,lpRect->top+1,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top);
   QPen pen1(::GetSysColor(COLOR_BTNSHADOW));
   QPen pen2(::GetSysColor(COLOR_3DFACE));
   QPen pen = _qpainter.pen();
   _qpainter.setPen(pen1);
   _qpainter.drawRect(rect1);
   _qpainter.setPen(pen2);
   _qpainter.drawRect(rect2);
   _qpainter.setPen(pen);
   return TRUE;
}

BOOL CDC::RectVisible(
   LPCRECT lpRect
) const
{
   // CP: Could be smart here...but do we need to be?
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
   _qpainter.drawRect(rect);
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
   QPen origPen = _qpainter.pen();
   x -= _windowOrg.x;
   y -= _windowOrg.y;
   _qpainter.setPen(tlc);
   _qpainter.drawLine(x,y,x+cx-1,y);
   _qpainter.drawLine(x,y,x,y+cy-1);
   _qpainter.setPen(brc);
   _qpainter.drawLine(x+cx-1,y+cy-1,x,y+cy-1);
   _qpainter.drawLine(x+cx-1,y+cy-1,x+cx-1,y);
   _qpainter.setPen(origPen);
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
   QString qstr = QString::fromLatin1((LPCTSTR)str);
#endif
   QTextOption to;
   QPen origPen = _qpainter.pen();
   _qpainter.setPen(QPen(_textColor));
//   _qpainter.setFont((QFont)*_font);
   if ( nFormat&DT_CENTER )
   {
      to.setAlignment(Qt::AlignCenter);
   }
   _qpainter.drawText(rect,qstr.toLatin1().constData(),to);
   _qpainter.setPen(origPen);
   return 0; // CP: should be text height
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
   QString qstr = QString::fromLatin1(lpszString);
#endif
   QPen origPen = _qpainter.pen();
   QTextOption to;
   _qpainter.setPen(QPen(_textColor));
//   _qpainter.setFont((QFont)*_font);
   if ( nFormat&DT_CENTER )
   {
      to.setAlignment(Qt::AlignCenter);
   }
   _qpainter.drawText(rect,qstr.left(nCount).toLatin1().constData(),to);
   _qpainter.setPen(origPen);
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
   _qpainter.fillRect(rect,color);
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
   _qpainter.fillRect(rect,color);
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

      _qpainter.fillRect(rect,brush);
   }
   return TRUE;
}

BOOL CDC::LineTo(
   int x,
   int y
)
{
   _qpainter.drawLine(_lineOrg.x,_lineOrg.y,x,y);
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
   _qpainter.fillPath(path,(QBrush)*_brush);
   _qpainter.drawPath(path);
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
   QString qstr = QString::fromLatin1(lpszString);
#endif
   QFontMetrics fontMetrics((QFont)*_font);
   QPen origPen = _qpainter.pen();
   _qpainter.setPen(QPen(_textColor));
//   _qpainter.setFont((QFont)*_font);
   x += -_windowOrg.x;
   y += -_windowOrg.y;
   y += fontMetrics.ascent();
   _qpainter.drawText(x,y,qstr.left(nCount));
   _qpainter.setPen(origPen);
   return TRUE;
}

BOOL CDC::TextOut(
   int x,
      int y,
      const CString& str
)
{
   QFontMetrics fontMetrics((QFont)*_font);
   QPen origPen = _qpainter.pen();
   _qpainter.setPen(QPen(_textColor));
//   _qpainter.setFont((QFont)*_font);
   x += -_windowOrg.x;
   y += -_windowOrg.y;
   y += fontMetrics.ascent();
   _qpainter.drawText(x,y,(const QString&)str);
   _qpainter.setPen(origPen);
   return TRUE;
}

IMPLEMENT_DYNAMIC(CComboBox,CWnd)

BEGIN_MESSAGE_MAP(CComboBox,CWnd)
END_MESSAGE_MAP()

CComboBox::CComboBox(CWnd *parent)
   : CWnd(parent),
     _qtd(NULL)
{
}

CComboBox::~CComboBox()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CComboBox::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CComboBox*>(this));
   widget->setParent(NULL);
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

   if ( _qt )
      delete _qt;

   _grid = NULL;

   if ( pParentWnd )
      _qt = new QComboBox(pParentWnd->toQWidget());
   else
      _qt = new QComboBox();

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QComboBox*>(_qt);

   _qtd->setMouseTracking(true);

   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChanged(int)));

   _qtd->setGeometry(rect.left,rect.top,(rect.right-rect.left),_qtd->sizeHint().height());
//   _qtd->setVisible(dwStyle&WS_VISIBLE);

   QFontMetrics fm(_qtd->font());

   _qtd->setMaxVisibleItems((rect.bottom-rect.top)/fm.height());
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif
   
   SetParent(pParentWnd);
   
   qtToMfcWindow.insert(_qtd,this);

   return TRUE;
}

void CComboBox::currentIndexChanged(int index)
{
   GetOwner()->PostMessage(WM_COMMAND,(CBN_SELCHANGE<<16)|(_id),(LPARAM)m_hWnd);
}

void CComboBox::SetWindowText(
   LPCTSTR lpszString
)
{
   QLineEdit* edit = _qtd->lineEdit();
   if ( edit )
   {
#if UNICODE
      edit->setText(QString::fromWCharArray(lpszString));
#else
      edit->setText(QString::fromLatin1(lpszString));
#endif
   }
   else
   {
#if UNICODE
      _qtd->setCurrentIndex(_qtd->findText(QString::fromWCharArray(lpszString)));
#else
      _qtd->setCurrentIndex(_qtd->findText(QString::fromLatin1(lpszString)));
#endif
   }
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
   _qtd->addItem(QString::fromLatin1(lpszString));
#endif
   _qtd->blockSignals(false); // Don't cause CbnSelchange yet...

   return _qtd->count()-1;
}

void CComboBox::SetCurSel(int sel)
{
   _qtd->blockSignals(true);
   _qtd->setCurrentIndex(sel);
   _qtd->blockSignals(false);
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
   strcpy(lpszText,lbText.toLatin1().constData());
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
   strcpy(lpszText,lbText.toLatin1().constData());
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
   QString string = QString::fromLatin1(lpszString);
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
   _qtd->setEditable(true);
   _qtd->lineEdit()->setReadOnly(true);
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
   _qtd->setEditable(true);
   _qtd->lineEdit()->setReadOnly(true);
#if UNICODE
   _qtd->setEditText(QString::fromWCharArray(lpszString));
#else
   _qtd->setEditText(QString::fromLatin1(lpszString));
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
   strncpy(lpStr,_qtd->currentText().toLatin1().constData(),nMaxCount);
#endif
   return _qtd->currentText().length();
}

IMPLEMENT_DYNAMIC(CListBox,CWnd)

BEGIN_MESSAGE_MAP(CListBox,CWnd)
END_MESSAGE_MAP()

CListBox::CListBox(CWnd* parent)
   : CWnd(parent),
     _qtd(NULL)
{
}

CListBox::~CListBox()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CListBox::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CListBox*>(this));
   widget->setParent(NULL);
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

   if ( _qt )
      delete _qt;

   _grid = NULL;

   if ( pParentWnd )
      _qt = new QListWidget(pParentWnd->toQWidget());
   else      
      _qt = new QListWidget();

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QListWidget*>(_qt);

#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif
   _qtd->setEditTriggers(QAbstractItemView::NoEditTriggers);
   _qtd->setMouseTracking(true);

   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged()));
   QObject::connect(_qtd,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClicked(QListWidgetItem*)));
   QObject::connect(_qtd,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(itemDoubleClicked(QListWidgetItem*)));

   _qtd->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
   
   SetParent(pParentWnd);
   
   qtToMfcWindow.insert(_qtd,this);

   return TRUE;
}

void CListBox::itemSelectionChanged()
{
   qDebug("CListBox::itemSelectionChanged not implemented");
}

void CListBox::itemClicked(QListWidgetItem* lwi)
{
   qDebug("CListBox::itemClicked not implemented");
}

void CListBox::itemDoubleClicked(QListWidgetItem* lwi)
{
   qDebug("CListBox::itemDoubleClicked not implemented");
}

int CListBox::GetCount( ) const
{
   return _qtd->count();
}

void CListBox::ResetContent( )
{
   _qtd->clear();
}

int CListBox::AddString( 
   LPCTSTR lpszItem  
)
{
#if UNICODE
   _qtd->addItem(QString::fromWCharArray(lpszItem));
#else
   _qtd->addItem(QString::fromLatin1(lpszItem));
#endif
   return _qtd->count()-1;
}

IMPLEMENT_DYNAMIC(CCheckListBox,CListBox)

BEGIN_MESSAGE_MAP(CCheckListBox,CListBox)
END_MESSAGE_MAP()

CCheckListBox::CCheckListBox(CWnd* parent)
   : CListBox(parent)
{
}

CCheckListBox::~CCheckListBox()
{
}

void CCheckListBox::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CCheckListBox*>(this));
   widget->setParent(NULL);
}

int CCheckListBox::GetCheck(
   int nIndex 
)
{
   QListWidgetItem* lwi;

   lwi = _qtd->item(nIndex);

   if ( lwi )
   {
      return lwi->checkState()==Qt::Checked?BST_CHECKED:BST_UNCHECKED;
   }
   return BST_INDETERMINATE;
}

void CCheckListBox::SetCheck(
   int nIndex,
   int nCheck 
)
{
   QListWidgetItem* lwi;

   lwi = _qtd->item(nIndex);

   if ( lwi )
   {
      lwi->setCheckState(nCheck==BST_CHECKED?Qt::Checked:Qt::Unchecked);
   }
}

void CCheckListBox::SetCheckStyle( 
   UINT nStyle  
)
{
   switch ( nStyle )
   {
   case BS_CHECKBOX:
      break;
   case BS_AUTOCHECKBOX:
      break;
   case BS_AUTO3STATE:
      break;
   case BS_3STATE:
      break;
   }
}

IMPLEMENT_DYNAMIC(CListCtrl,CWnd)

BEGIN_MESSAGE_MAP(CListCtrl,CWnd)
END_MESSAGE_MAP()

CListCtrl::CListCtrl(CWnd* parent)
   : CWnd(parent),
     _qtd_table(NULL),
     _qtd_list(NULL),
     m_pImageList(NULL)
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

void CListCtrl::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CListCtrl*>(this));
   widget->setParent(NULL);
}

bool CListCtrl::event(QEvent *event)
{
   MFCMessageEvent* msgEvent = dynamic_cast<MFCMessageEvent*>(event);
   if ( msgEvent )
   {
      if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
      {
         switch ( msgEvent->msg.message )
         {
         case LVM_SETEXTENDEDLISTVIEWSTYLE:
            if ( !msgEvent->msg.wParam )
            {
               switch ( msgEvent->msg.lParam )
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
         return true;
      }
      else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
      {
         switch ( msgEvent->msg.message )
         {
         case LVM_SETEXTENDEDLISTVIEWSTYLE:
            if ( !msgEvent->msg.wParam )
            {
               switch ( msgEvent->msg.lParam )
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
         return true;
      }
   }
   return CWnd::event(event);
}

bool CListCtrl::eventFilter(QObject *object, QEvent *event)
{
   if ( object == _qt )
   {
      if ( event->type() == QEvent::Close )
      {
         closeEvent(dynamic_cast<QCloseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Show )
      {
         showEvent(dynamic_cast<QShowEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Hide )
      {
         hideEvent(dynamic_cast<QHideEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Move )
      {
         moveEvent(dynamic_cast<QMoveEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Paint )
      {
         paintEvent(dynamic_cast<QPaintEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::FocusIn )
      {
         focusInEvent(dynamic_cast<QFocusEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::FocusOut )
      {
         focusOutEvent(dynamic_cast<QFocusEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Leave )
      {
         leaveEvent(event);
         return false;
      }
      if ( event->type() == QEvent::MouseButtonPress )
      {
         mousePressEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::MouseButtonRelease )
      {
         mouseReleaseEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::MouseButtonDblClick )
      {
         mouseDoubleClickEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::MouseMove )
      {
         mouseMoveEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Wheel )
      {
         wheelEvent(dynamic_cast<QWheelEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Resize )
      {
         resizeEvent(dynamic_cast<QResizeEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::KeyPress )
      {
         keyPressEvent(dynamic_cast<QKeyEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::KeyRelease )
      {
         keyReleaseEvent(dynamic_cast<QKeyEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::ContextMenu )
      {
         contextMenuEvent(dynamic_cast<QContextMenuEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::DragEnter )
      {
         dragEnterEvent(dynamic_cast<QDragEnterEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::DragMove )
      {
         dragMoveEvent(dynamic_cast<QDragMoveEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::Drop )
      {
         dropEvent(dynamic_cast<QDropEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::DragLeave )
      {
         dragLeaveEvent(dynamic_cast<QDragLeaveEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::Timer )
      {
         timerEvent(dynamic_cast<QTimerEvent*>(event));
         return true;
      }
   }
//   qDebug("eventFilter: unhandled %d object %s", event->type(), object->objectName().toLatin1().constData());
   return false;
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
   return QModelIndex();
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

#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
      _qtd_table->setFont(QFont("MS Shell Dlg",8));
#else
      _qtd_table->setFont(QFont("MS Shell Dlg",9));
#endif
      _qtd_table->horizontalHeader()->setStretchLastSection(true);
      _qtd_table->verticalHeader()->hide();
      _qtd_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
      _qtd_table->setSelectionBehavior(QAbstractItemView::SelectRows);
      _qtd_table->installEventFilter(this);
      _qtd_table->setMouseTracking(true);
      _qtd_table->setShowGrid(false);

      // Pass-through signals
      QObject::connect(_qtd_table,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged()));
      QObject::connect(_qtd_table,SIGNAL(cellClicked(int,int)),this,SLOT(cellClicked(int,int)));
      QObject::connect(_qtd_table,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(cellDoubleClicked(int,int)));

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

      _qtd_table->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
//      _qtd_table->setContextMenuPolicy(Qt::PreventContextMenu);
      _qtd_table->setVisible(dwStyle&WS_VISIBLE);
      
      qtToMfcWindow.insert(_qtd_table,this);
   }
   else if ( (dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      if ( pParentWnd )
         _qt = new QListWidget(pParentWnd->toQWidget());
      else
         _qt = new QListWidget;

      // Downcast to save having to do it all over the place...
      _qtd_list = dynamic_cast<QListWidget*>(_qt);

#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
      _qtd_list->setFont(QFont("MS Shell Dlg",8));
#else
      _qtd_list->setFont(QFont("MS Shell Dlg",9));
#endif
      _qtd_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
      _qtd_list->setSelectionBehavior(QAbstractItemView::SelectRows);
      _qtd_list->setMouseTracking(true);
      _qtd_list->installEventFilter(this);

      // Pass-through signals
      QObject::connect(_qtd_list,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged()));
      QObject::connect(_qtd_list,SIGNAL(clicked(QModelIndex)),this,SLOT(clicked(QModelIndex)));
      QObject::connect(_qtd_list,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClicked(QModelIndex)));
// CP: These cause problems for CInstrumentList...
//      QObject::connect(_qtd_list,SIGNAL(pressed(QModelIndex)),this,SLOT(clicked(QModelIndex)));
//      QObject::connect(_qtd_list,SIGNAL(entered(QModelIndex)),this,SLOT(clicked(QModelIndex)));

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

      _qtd_list->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
//      _qtd_list->setContextMenuPolicy(Qt::PreventContextMenu);
      _qtd_list->setVisible(dwStyle&WS_VISIBLE);
      
      qtToMfcWindow.insert(_qtd_list,this);
   }
   
   SetParent(pParentWnd);

   return TRUE;
}

void CListCtrl::itemSelectionChanged()
{
   NMLISTVIEW nmlv;

   nmlv.hdr.hwndFrom = m_hWnd;
   nmlv.hdr.idFrom = _id;
   nmlv.hdr.code = LVN_ITEMCHANGED;
   nmlv.uChanged = LVIF_STATE;
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {   
      nmlv.iItem = _qtd_table->currentIndex().row();
      nmlv.iSubItem = _qtd_table->currentIndex().column();
      if ( _qtd_table->currentItem() )
      {
         nmlv.uNewState = (_qtd_table->currentItem()->checkState()==Qt::Checked?0x2000:0);
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      nmlv.iItem = _qtd_list->currentIndex().row();
      nmlv.iSubItem = _qtd_list->currentIndex().column();
      if ( _qtd_list->currentItem() )
      {
         nmlv.uNewState = (_qtd_list->currentItem()->checkState()==Qt::Checked?0x2000:0);
      }
   }
   nmlv.uNewState |= LVNI_SELECTED;
   nmlv.uOldState = 0;
   
   GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmlv);
}

void CListCtrl::cellClicked(int row, int column)
{
   NMITEMACTIVATE nmia;

   nmia.hdr.hwndFrom = m_hWnd;
   nmia.hdr.idFrom = _id;
   nmia.hdr.code = NM_CLICK;
   nmia.iItem = row;
   nmia.iSubItem = column;
   nmia.ptAction.x = QCursor::pos().x();
   nmia.ptAction.y = QCursor::pos().y();
   
   GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmia);
}

void CListCtrl::cellDoubleClicked(int row, int column)
{
   NMITEMACTIVATE nmia;
   
   nmia.hdr.hwndFrom = m_hWnd;
   nmia.hdr.idFrom = _id;
   nmia.hdr.code = NM_DBLCLK;
   nmia.iItem = row;
   nmia.iSubItem = column;
   nmia.ptAction.x = QCursor::pos().x();
   nmia.ptAction.y = QCursor::pos().y();
   
   GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmia);
}

void CListCtrl::clicked(QModelIndex index)
{
   NMITEMACTIVATE nmia;
   
   nmia.hdr.hwndFrom = m_hWnd;
   nmia.hdr.idFrom = _id;
   nmia.hdr.code = NM_CLICK;
   nmia.iItem = index.row();
   nmia.iSubItem = index.column();
   nmia.ptAction.x = QCursor::pos().x();
   nmia.ptAction.y = QCursor::pos().y();
   
   GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmia);
}

void CListCtrl::doubleClicked(QModelIndex index)
{
   NMITEMACTIVATE nmia;
   
   nmia.hdr.hwndFrom = m_hWnd;
   nmia.hdr.idFrom = _id;
   nmia.hdr.code = NM_DBLCLK;
   nmia.iItem = index.row();
   nmia.iSubItem = index.column();
   nmia.ptAction.x = QCursor::pos().x();
   nmia.ptAction.y = QCursor::pos().y();
   
   GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmia);
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
   return 0;
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
            if ( (nItem == -1) || (twis.at(item)->row() == nItem) )
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
            if ( (nItem == -1) || (_qtd_list->row(lwis.at(item)) == nItem) )
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
         strcpy(lpszText,twi->text().toLatin1().constData());
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
         strcpy(lpszText,lwi->text().toLatin1().constData());
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
         strcpy(lpszText,twi->text().toLatin1().constData());
         length = strlen(lpszText);
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem,nSubItem);

      if ( lwi )
      {
         strcpy(lpszText,lwi->text().toLatin1().constData());
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

   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      QTableWidgetItem* twi = new QTableWidgetItem;

#if UNICODE
      twi->setText(QString::fromWCharArray(lpszColumnHeading));
#else
      twi->setText(QString::fromLatin1(lpszColumnHeading));
#endif

      _qtd_table->setColumnWidth(nCol,nWidth);
      _qtd_table->setHorizontalHeaderItem(nCol,twi);
      return _qtd_table->columnCount()-1;
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
      twi->setText(QString::fromLatin1(lpszItem));
#endif

      _qtd_table->blockSignals(true);
      _qtd_table->insertRow(nItem);
      _qtd_table->setItem(nItem,0,twi);
      _qtd_table->blockSignals(false);
      _qtd_table->update();
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
      lwi->setText(QString::fromLatin1(lpszItem));
#endif

      _qtd_list->blockSignals(true);
      _qtd_list->insertItem(nItem,lwi);
      _qtd_list->blockSignals(false);
      _qtd_list->update();
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
      twi->setText(QString::fromLatin1(lpszItem));
#endif
      if ( m_pImageList )
      {
         twi->setIcon(*(QIcon*)m_pImageList->ExtractIcon(nImage));
      }

      _qtd_table->blockSignals(true);
      _qtd_table->insertRow(nItem);
      _qtd_table->setItem(nItem,0,twi);
      _qtd_table->blockSignals(false);
      _qtd_table->update();
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
      lwi->setText(QString::fromLatin1(lpszItem));
#endif
      if ( m_pImageList )
      {
         lwi->setIcon(*(QIcon*)m_pImageList->ExtractIcon(nImage));
      }
      _qtd_list->blockSignals(true);
      _qtd_list->insertItem(nItem,lwi);
      _qtd_list->blockSignals(false);
      _qtd_list->update();
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

      if ( !lwi )
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

      if ( twi )
      {
         return twi->checkState()==Qt::Checked;
      }
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      lwi = _qtd_list->item(nItem);

      if ( lwi )
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
      twi->setText(QString::fromLatin1(lpszText));
#endif

      if ( add )
         _qtd_table->setItem(nItem,nSubItem,twi);
      _qtd_table->update();
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
      lwi->setText(QString::fromLatin1(lpszText));
#endif

      if ( add )
         _qtd_list->insertItem(nItem,lwi);
      _qtd_list->update();
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
      twis = _qtd_table->findItems(QString::fromLatin1(pFindInfo->psz),flags);
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
      lwis = _qtd_list->findItems(QString::fromLatin1(pFindInfo->psz),flags);
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
   QString styleSheet;
   bkColor = QColor(GetRValue(cr),GetGValue(cr),GetBValue(cr));
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      styleSheet = "QTableWidget {";
      styleSheet += "color: rgba("+QString::number(txtColor.red())+","+QString::number(txtColor.green())+","+QString::number(txtColor.blue())+",255);";
      styleSheet += "background: rgba("+QString::number(bkColor.red())+","+QString::number(bkColor.green())+","+QString::number(bkColor.blue())+",255);";
      styleSheet += "}";
      styleSheet += "QTableWidget::item:selected {";
      styleSheet += "color: rgba(0,0,0,255);";
      styleSheet += "background: rgba(240,240,240,255);";
      styleSheet += "}";
      _qtd_table->setStyleSheet(styleSheet);
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      styleSheet = "QListWidget {";
      styleSheet += "color: rgba("+QString::number(txtColor.red())+","+QString::number(txtColor.green())+","+QString::number(txtColor.blue())+",255);";
      styleSheet += "background: rgba("+QString::number(bkColor.red())+","+QString::number(bkColor.green())+","+QString::number(bkColor.blue())+",255);";
      styleSheet += "}";
      styleSheet += "QListWidget::item:selected {";
      styleSheet += "color: rgba(0,0,0,255);";
      styleSheet += "background: rgba(240,240,240,255);";
      styleSheet += "}";
      _qtd_list->setStyleSheet(styleSheet);
   }
   return TRUE;
}

BOOL CListCtrl::SetTextBkColor(
   COLORREF cr
)
{
   QString styleSheet;
   txtBkColor = QColor(GetRValue(cr),GetGValue(cr),GetBValue(cr));
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      styleSheet = "QTableWidget {";
      styleSheet += "color: rgba("+QString::number(txtColor.red())+","+QString::number(txtColor.green())+","+QString::number(txtColor.blue())+",255);";
      styleSheet += "background: rgba("+QString::number(bkColor.red())+","+QString::number(bkColor.green())+","+QString::number(bkColor.blue())+",255);";
      styleSheet += "}";
      styleSheet += "QTableWidget::item:selected {";
      styleSheet += "color: rgba(0,0,0,255);";
      styleSheet += "background: rgba(240,240,240,255);";
      styleSheet += "}";
      _qtd_table->setStyleSheet(styleSheet);
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      styleSheet = "QListWidget {";
      styleSheet += "color: rgba("+QString::number(txtColor.red())+","+QString::number(txtColor.green())+","+QString::number(txtColor.blue())+",255);";
      styleSheet += "background: rgba("+QString::number(bkColor.red())+","+QString::number(bkColor.green())+","+QString::number(bkColor.blue())+",255);";
      styleSheet += "}";
      styleSheet += "QListWidget::item:selected {";
      styleSheet += "color: rgba(0,0,0,255);";
      styleSheet += "background: rgba(240,240,240,255);";
      styleSheet += "}";
      _qtd_list->setStyleSheet(styleSheet);
   }
   return TRUE;
}

BOOL CListCtrl::SetTextColor(
   COLORREF cr
)
{
   QString styleSheet;
   txtColor = QColor(GetRValue(cr),GetGValue(cr),GetBValue(cr));
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      styleSheet = "QTableWidget {";
      styleSheet += "color: rgba("+QString::number(txtColor.red())+","+QString::number(txtColor.green())+","+QString::number(txtColor.blue())+",255);";
      styleSheet += "background: rgba("+QString::number(bkColor.red())+","+QString::number(bkColor.green())+","+QString::number(bkColor.blue())+",255);";
      styleSheet += "}";
      styleSheet += "QTableWidget::item:selected {";
      styleSheet += "color: rgba(0,0,0,255);";
      styleSheet += "background: rgba(240,240,240,255);";
      styleSheet += "}";
      _qtd_table->setStyleSheet(styleSheet);
   }
   else if ( (_dwStyle&LVS_TYPEMASK) == LVS_LIST )
   {
      styleSheet = "QListWidget {";
      styleSheet += "color: rgba("+QString::number(txtColor.red())+","+QString::number(txtColor.green())+","+QString::number(txtColor.blue())+",255);";
      styleSheet += "background: rgba("+QString::number(bkColor.red())+","+QString::number(bkColor.green())+","+QString::number(bkColor.blue())+",255);";
      styleSheet += "}";
      styleSheet += "QListWidget::item:selected {";
      styleSheet += "color: rgba(0,0,0,255);";
      styleSheet += "background: rgba(240,240,240,255);";
      styleSheet += "}";
      _qtd_list->setStyleSheet(styleSheet);
   }
   return TRUE;
}

BOOL CListCtrl::DeleteAllItems()
{
   int row;
   if ( (_dwStyle&LVS_TYPEMASK) == LVS_REPORT )
   {
      _qtd_table->blockSignals(true);
      _qtd_table->clearContents();
      for ( row = _qtd_table->rowCount(); row > 0; row-- )
         _qtd_table->removeRow(row-1);
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
      if ( nItem < _qtd_table->rowCount() )
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
         _qtd_list->update();
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
   return 0;
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
   return NULL;
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

IMPLEMENT_DYNAMIC(CTreeCtrl,CWnd)

BEGIN_MESSAGE_MAP(CTreeCtrl,CWnd)
END_MESSAGE_MAP()

CTreeCtrl::CTreeCtrl(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;

   _grid = NULL;

   _qt = new QTreeWidget(parent->toQWidget());

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QTreeWidget*>(_qt);

#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif
   _qtd->setEditTriggers(QAbstractItemView::NoEditTriggers);
   _qtd->setHeaderHidden(true);
   _qtd->setMouseTracking(true);

   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged()));
   QObject::connect(_qtd,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(itemClicked(QTreeWidgetItem*,int)));
   QObject::connect(_qtd,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));
}

CTreeCtrl::~CTreeCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CTreeCtrl::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CTreeCtrl*>(this));
   widget->setParent(NULL);
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

   _qtd->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
   _qtd->setContextMenuPolicy(Qt::DefaultContextMenu);
   _qtd->setVisible(dwStyle&WS_VISIBLE);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif

   SetParent(pParentWnd);
   
   qtToMfcWindow.insert(_qtd,this);

   return TRUE;
}

void CTreeCtrl::itemSelectionChanged()
{
}

void CTreeCtrl::itemClicked(QTreeWidgetItem* item, int column)
{
   NMITEMACTIVATE nmia;

   nmia.hdr.hwndFrom = m_hWnd;
   nmia.hdr.idFrom = _id;
   nmia.hdr.code = NM_CLICK;
   nmia.iItem = _qtd->indexAt(QCursor::pos()).row();
   nmia.iSubItem = column;
   nmia.ptAction.x = QCursor::pos().x();
   nmia.ptAction.y = QCursor::pos().y();
   
   GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmia);
}

void CTreeCtrl::itemDoubleClicked(QTreeWidgetItem* item, int column)
{
   NMITEMACTIVATE nmia;

   nmia.hdr.hwndFrom = m_hWnd;
   nmia.hdr.idFrom = _id;
   nmia.hdr.code = NM_DBLCLK;
   nmia.iItem = _qtd->indexAt(QCursor::pos()).row();
   nmia.iSubItem = column;
   nmia.ptAction.x = QCursor::pos().x();
   nmia.ptAction.y = QCursor::pos().y();
   
   GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmia);
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
   twi->setText(0,QString::fromLatin1(lpszItem));
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

IMPLEMENT_DYNAMIC(CScrollBar,CWnd)

BEGIN_MESSAGE_MAP(CScrollBar,CWnd)
END_MESSAGE_MAP()

CScrollBar::CScrollBar(CWnd *parent)
   : CWnd(parent)
{
}

CScrollBar::~CScrollBar()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CScrollBar::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CScrollBar*>(this));
   widget->setParent(NULL);
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
   
   _dwStyle = dwStyle;

   if ( _qt )
      delete _qt;

   _grid = NULL;

   if ( pParentWnd )
      _qt = new QScrollBar(pParentWnd->toQWidget());
   else
      _qt = new QScrollBar;

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QScrollBar*>(_qt);

   _qtd->setMouseTracking(true);

   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(actionTriggered(int)),this,SLOT(actionTriggered(int)));

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
   
   SetParent(pParentWnd);
   
   qtToMfcWindow.insert(_qtd,this);

   return TRUE;
}

void CScrollBar::actionTriggered(int action)
{
   // CP: these values don't match Qt apparently...
   switch ( action )
   {
   case QAbstractSlider::SliderSingleStepAdd: 
      action = SB_LINEDOWN;
      break;
   case QAbstractSlider::SliderSingleStepSub: 
      action = SB_LINEUP;
      break;
   case QAbstractSlider::SliderPageStepAdd: 
      action = SB_PAGEDOWN;
      break;
   case QAbstractSlider::SliderPageStepSub: 
      action = SB_PAGEUP;
      break;
   case QAbstractSlider::SliderToMinimum:
      action = SB_TOP;
      break;
   case QAbstractSlider::SliderToMaximum:
      action = SB_BOTTOM;
      break;
   case QAbstractSlider::SliderMove:
      action = SB_THUMBTRACK;
      break;
   }
   
   if ( _dwStyle&SBS_VERT )
   {
      GetOwner()->SendMessage(WM_VSCROLL,(sliderPosition()<<16)|(action),(LPARAM)m_hWnd);
   }
   else
   {
      GetOwner()->SendMessage(WM_HSCROLL,(sliderPosition()<<16)|(action),(LPARAM)m_hWnd);
   }
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

int CScrollBar::GetScrollPos() const
{
   return _qtd->value();
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

IMPLEMENT_DYNCREATE(CCmdTarget,CObject)

// End-of-the-line entry for message maps.
// Instead of auto-creating using the BEGIN_MESSAGE_MAP/END_MESSAGE_MAP,
// we need to create a NULL-terminator.
const AFX_MSGMAP* CCmdTarget::GetMessageMap() const 
   { return GetThisMessageMap(); } 
const AFX_MSGMAP* PASCAL CCmdTarget::GetThisMessageMap() 
{ 
   static const AFX_MSGMAP_ENTRY _messageEntries[] =  
   {
      {0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 } 
	}; 
   static const AFX_MSGMAP messageMap = 
   { NULL, &_messageEntries[0] }; 
   return &messageMap; 
}

// From afximpl.h
union MessageMapFunctions
{
	AFX_PMSG pfn;   // generic member function pointer

	BOOL (AFX_MSG_CALL CCmdTarget::*pfn_b_D)(CDC*);
	BOOL (AFX_MSG_CALL CCmdTarget::*pfn_b_b)(BOOL);
	BOOL (AFX_MSG_CALL CCmdTarget::*pfn_b_u)(UINT);
	BOOL (AFX_MSG_CALL CCmdTarget::*pfn_b_h)(HANDLE);
	BOOL (AFX_MSG_CALL CCmdTarget::*pfn_b_W_u_u)(CWnd*, UINT, UINT);
	BOOL (AFX_MSG_CALL CCmdTarget::*pfn_b_W_COPYDATASTRUCT)(CWnd*, COPYDATASTRUCT*);
	BOOL (AFX_MSG_CALL CCmdTarget::*pfn_b_HELPINFO)(LPHELPINFO);
	HBRUSH (AFX_MSG_CALL CCmdTarget::*pfn_B_D_W_u)(CDC*, CWnd*, UINT);
	HBRUSH (AFX_MSG_CALL CCmdTarget::*pfn_B_D_u)(CDC*, UINT);
	int (AFX_MSG_CALL CCmdTarget::*pfn_i_u_W_u)(UINT, CWnd*, UINT);
	int (AFX_MSG_CALL CCmdTarget::*pfn_i_u_u)(UINT, UINT);
	int (AFX_MSG_CALL CCmdTarget::*pfn_i_W_u_u)(CWnd*, UINT, UINT);
	int (AFX_MSG_CALL CCmdTarget::*pfn_i_s)(LPTSTR); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	LRESULT (AFX_MSG_CALL CCmdTarget::*pfn_l_w_l)(WPARAM, LPARAM); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	LRESULT (AFX_MSG_CALL CCmdTarget::*pfn_l_u_u_M)(UINT, UINT, CMenu*); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_b_h)(BOOL, HANDLE); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_h)(HANDLE); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_h_h)(HANDLE,HANDLE); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_v)(); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	int (AFX_MSG_CALL CCmdTarget::*pfn_i_u)(UINT); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	HCURSOR (AFX_MSG_CALL CCmdTarget::*pfn_C_v)(); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	UINT (AFX_MSG_CALL CCmdTarget::*pfn_u_u)(UINT); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	BOOL (AFX_MSG_CALL CCmdTarget::*pfn_b_v)(); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_u)(UINT); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_u_u)(UINT, UINT); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_i_i)(int, int); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_u_u_u)(UINT, UINT, UINT); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_u_i_i)(UINT, int, int); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_w_l)(WPARAM, LPARAM); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_b_W_W)(BOOL, CWnd*, CWnd*); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_D)(CDC*); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_M)(CMenu*); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_M_u_b)(CMenu*, UINT, BOOL); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_W)(CWnd*); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_W_u_u)(CWnd*, UINT, UINT); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_W_p)(CWnd*, CPoint); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_W_h)(CWnd*, HANDLE); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_u_W)(UINT, CWnd*); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_u_W_b)(UINT, CWnd*, BOOL); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_u_u_W)(UINT, UINT, CWnd*); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_s)(LPTSTR); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_u_cs)(UINT, LPCTSTR); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_i_s)(int, LPTSTR); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	int (AFX_MSG_CALL CCmdTarget::*pfn_i_i_s)(int, LPTSTR); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	UINT (AFX_MSG_CALL CCmdTarget::*pfn_u_p)(CPoint); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	LRESULT (AFX_MSG_CALL CCmdTarget::*pfn_l_p)(CPoint); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	UINT (AFX_MSG_CALL CCmdTarget::*pfn_u_v)(); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_b_NCCALCSIZEPARAMS)(BOOL, NCCALCSIZE_PARAMS*); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_v_WINDOWPOS)(WINDOWPOS*); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_u_u_M)(UINT, UINT, HMENU); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_u_p)(UINT, CPoint); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void (AFX_MSG_CALL CCmdTarget::*pfn_v_u_pr)(UINT, LPRECT); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	BOOL (AFX_MSG_CALL CCmdTarget::*pfn_b_u_s_p)(UINT, short, CPoint); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	LRESULT (AFX_MSG_CALL CCmdTarget::*pfn_l_v)(); // CP: Originally was CWnd:: but this breaks mingw's member function pointer

	// type safe variant for thread messages
   void (AFX_MSG_CALL CCmdTarget::*pfn_THREAD)(WPARAM, LPARAM); // CP: Originally was CWnd:: but this breaks mingw's member function pointer

	// specific type safe variants for WM_COMMAND and WM_NOTIFY messages
	void (AFX_MSG_CALL CCmdTarget::*pfnCmd_v_v)();
	BOOL (AFX_MSG_CALL CCmdTarget::*pfnCmd_b_v)();
	void (AFX_MSG_CALL CCmdTarget::*pfnCmd_v_u)(UINT);
	BOOL (AFX_MSG_CALL CCmdTarget::*pfnCmd_b_u)(UINT);

	void (AFX_MSG_CALL CCmdTarget::*pfnNotify_v_NMHDR_pl)(NMHDR*, LRESULT*);
	BOOL (AFX_MSG_CALL CCmdTarget::*pfnNotify_b_NMHDR_pl)(NMHDR*, LRESULT*);
	void (AFX_MSG_CALL CCmdTarget::*pfnNotify_v_u_NMHDR_pl)(UINT, NMHDR*, LRESULT*);
	BOOL (AFX_MSG_CALL CCmdTarget::*pfnNotify_b_u_NMHDR_pl)(UINT, NMHDR*, LRESULT*);
	void (AFX_MSG_CALL CCmdTarget::*pfnCmdUI_v_C)(CCmdUI*);
	void (AFX_MSG_CALL CCmdTarget::*pfnCmdUI_v_C_u)(CCmdUI*, UINT);

	void (AFX_MSG_CALL CCmdTarget::*pfnCmd_v_pv)(void*);
	BOOL (AFX_MSG_CALL CCmdTarget::*pfnCmd_b_pv)(void*);

//OLD
	// specific type safe variants for WM-style messages
//	BOOL    (AFX_MSG_CALL CWnd::*pfn_bD)(CDC*);
//	BOOL    (AFX_MSG_CALL CWnd::*pfn_bb)(BOOL);
//	BOOL    (AFX_MSG_CALL CWnd::*pfn_bWww)(CWnd*, UINT, UINT);
//	BOOL    (AFX_MSG_CALL CWnd::*pfn_bHELPINFO)(HELPINFO*);
//	BOOL    (AFX_MSG_CALL CWnd::*pfn_bWCDS)(CWnd*, COPYDATASTRUCT*);
//	HBRUSH  (AFX_MSG_CALL CWnd::*pfn_hDWw)(CDC*, CWnd*, UINT);
//	HBRUSH  (AFX_MSG_CALL CWnd::*pfn_hDw)(CDC*, UINT);
//	int     (AFX_MSG_CALL CWnd::*pfn_iwWw)(UINT, CWnd*, UINT);
//	int     (AFX_MSG_CALL CWnd::*pfn_iww)(UINT, UINT);
//	int     (AFX_MSG_CALL CWnd::*pfn_iWww)(CWnd*, UINT, UINT);
//	int     (AFX_MSG_CALL CWnd::*pfn_is)(LPTSTR);
//	LRESULT (AFX_MSG_CALL CWnd::*pfn_lwl)(WPARAM, LPARAM);
//	LRESULT (AFX_MSG_CALL CWnd::*pfn_lwwM)(UINT, UINT, CMenu*);
//	void    (AFX_MSG_CALL CWnd::*pfn_vv)(void);

//	void    (AFX_MSG_CALL CWnd::*pfn_vw)(UINT);
//	void    (AFX_MSG_CALL CWnd::*pfn_vww)(UINT, UINT);
//	void    (AFX_MSG_CALL CWnd::*pfn_vvii)(int, int);
//	void    (AFX_MSG_CALL CWnd::*pfn_vwww)(UINT, UINT, UINT);
//	void    (AFX_MSG_CALL CWnd::*pfn_vwii)(UINT, int, int);
//	void    (AFX_MSG_CALL CWnd::*pfn_vwl)(WPARAM, LPARAM);
//	void    (AFX_MSG_CALL CWnd::*pfn_vbWW)(BOOL, CWnd*, CWnd*);
//	void    (AFX_MSG_CALL CWnd::*pfn_vD)(CDC*);
//	void    (AFX_MSG_CALL CWnd::*pfn_vM)(CMenu*);
//	void    (AFX_MSG_CALL CWnd::*pfn_vMwb)(CMenu*, UINT, BOOL);

//	void    (AFX_MSG_CALL CWnd::*pfn_vW)(CWnd*);
//	void    (AFX_MSG_CALL CWnd::*pfn_vWww)(CWnd*, UINT, UINT);
//	void    (AFX_MSG_CALL CWnd::*pfn_vWp)(CWnd*, CPoint);
//	void    (AFX_MSG_CALL CWnd::*pfn_vWh)(CWnd*, HANDLE);
//	void    (AFX_MSG_CALL CWnd::*pfn_vwW)(UINT, CWnd*);
//	void    (AFX_MSG_CALL CWnd::*pfn_vwWb)(UINT, CWnd*, BOOL);
//	void    (AFX_MSG_CALL CWnd::*pfn_vwwW)(UINT, UINT, CWnd*);
//	void    (AFX_MSG_CALL CWnd::*pfn_vwwx)(UINT, UINT);
//	void    (AFX_MSG_CALL CWnd::*pfn_vs)(LPTSTR);
   void    (AFX_MSG_CALL CWnd::*pfn_vOWNER)(int, LPTSTR);   // force return TRUE
//	int     (AFX_MSG_CALL CWnd::*pfn_iis)(int, LPTSTR);
//	UINT    (AFX_MSG_CALL CWnd::*pfn_wp)(CPoint);
//	UINT    (AFX_MSG_CALL CWnd::*pfn_wv)(void);
	void    (AFX_MSG_CALL CCmdTarget::*pfn_vPOS)(WINDOWPOS*); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void    (AFX_MSG_CALL CCmdTarget::*pfn_vCALC)(BOOL, NCCALCSIZE_PARAMS*); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void    (AFX_MSG_CALL CCmdTarget::*pfn_vwp)(UINT, CPoint); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	void    (AFX_MSG_CALL CCmdTarget::*pfn_vwwh)(UINT, UINT, HANDLE); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
	BOOL    (AFX_MSG_CALL CCmdTarget::*pfn_bwsp)(UINT, short, CPoint); // CP: Originally was CWnd:: but this breaks mingw's member function pointer
//	void    (AFX_MSG_CALL CWnd::*pfn_vws)(UINT, LPCTSTR);
};

INT_PTR _AfxGetDlgCtrlID(HWND hWnd)
{
   CWnd* pWnd = (CWnd*)hWnd;
   return pWnd->GetDlgCtrlID();
}

const AFX_MSGMAP_ENTRY* AFXAPI
AfxFindMessageEntry(const AFX_MSGMAP_ENTRY* lpEntry,
	UINT nMsg, UINT nCode, UINT nID)
{
	// C version of search routine
	while (lpEntry->nSig != AfxSig_end)
	{
      if (lpEntry->nMessage == nMsg && lpEntry->nCode == nCode &&
			nID >= lpEntry->nID && nID <= lpEntry->nLastID)
		{
			return lpEntry;
		}
		lpEntry++;
	}
	return NULL;    // not found
}

AFX_STATIC BOOL AFXAPI _AfxDispatchCmdMsg(CCmdTarget* pTarget, UINT nID, int nCode,
	AFX_PMSG pfn, void* pExtra, UINT_PTR nSig, AFX_CMDHANDLERINFO* pHandlerInfo)
		// return TRUE to stop routing
{
	ENSURE_VALID(pTarget);
	UNUSED(nCode);   // unused in release builds

	union MessageMapFunctions mmf;
	mmf.pfn = pfn;
	BOOL bResult = TRUE; // default is ok

	if (pHandlerInfo != NULL)
	{
		// just fill in the information, don't do it
		pHandlerInfo->pTarget = pTarget;
//		pHandlerInfo->pmf = mmf.pfn;
		return TRUE;
	}

	switch (nSig)
	{
	default:    // illegal
		ASSERT(FALSE);
		return 0;
		break;

	case AfxSigCmd_v:
		// normal command or control notification
		ASSERT(CN_COMMAND == 0);        // CN_COMMAND same as BN_CLICKED
		ASSERT(pExtra == NULL);
		(pTarget->*mmf.pfnCmd_v_v)();
		break;

	case AfxSigCmd_b:
		// normal command or control notification
		ASSERT(CN_COMMAND == 0);        // CN_COMMAND same as BN_CLICKED
		ASSERT(pExtra == NULL);
		bResult = (pTarget->*mmf.pfnCmd_b_v)();
		break;

	case AfxSigCmd_RANGE:
		// normal command or control notification in a range
		ASSERT(CN_COMMAND == 0);        // CN_COMMAND same as BN_CLICKED
		ASSERT(pExtra == NULL);
		(pTarget->*mmf.pfnCmd_v_u)(nID);
		break;

	case AfxSigCmd_EX:
		// extended command (passed ID, returns bContinue)
		ASSERT(pExtra == NULL);
		bResult = (pTarget->*mmf.pfnCmd_b_u)(nID);
		break;

	case AfxSigNotify_v:
		{
			AFX_NOTIFY* pNotify = (AFX_NOTIFY*)pExtra;
			ENSURE(pNotify != NULL);
			ASSERT(pNotify->pResult != NULL);
			ASSERT(pNotify->pNMHDR != NULL);
			(pTarget->*mmf.pfnNotify_v_NMHDR_pl)(pNotify->pNMHDR, pNotify->pResult);
		}
		break;

	case AfxSigNotify_b:
		{
			AFX_NOTIFY* pNotify = (AFX_NOTIFY*)pExtra;
			ENSURE(pNotify != NULL);
			ASSERT(pNotify->pResult != NULL);
			ASSERT(pNotify->pNMHDR != NULL);
			bResult = (pTarget->*mmf.pfnNotify_b_NMHDR_pl)(pNotify->pNMHDR, pNotify->pResult);
		}
		break;

	case AfxSigNotify_RANGE:
		{
			AFX_NOTIFY* pNotify = (AFX_NOTIFY*)pExtra;
			ENSURE(pNotify != NULL);
			ASSERT(pNotify->pResult != NULL);
			ASSERT(pNotify->pNMHDR != NULL);
			(pTarget->*mmf.pfnNotify_v_u_NMHDR_pl)(nID, pNotify->pNMHDR,
				pNotify->pResult);
		}
		break;

	case AfxSigNotify_EX:
		{
			AFX_NOTIFY* pNotify = (AFX_NOTIFY*)pExtra;
			ENSURE(pNotify != NULL);
			ASSERT(pNotify->pResult != NULL);
			ASSERT(pNotify->pNMHDR != NULL);
			bResult = (pTarget->*mmf.pfnNotify_b_u_NMHDR_pl)(nID, pNotify->pNMHDR,
				pNotify->pResult);
		}
		break;

	case AfxSigCmdUI:
		{
			// ON_UPDATE_COMMAND_UI or ON_UPDATE_COMMAND_UI_REFLECT case
			ASSERT(CN_UPDATE_COMMAND_UI == (UINT)-1);
			ASSERT(nCode == CN_UPDATE_COMMAND_UI || nCode == 0xFFFF);
			ENSURE_ARG(pExtra != NULL);
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			ASSERT(!pCmdUI->m_bContinueRouting);    // idle - not set
			(pTarget->*mmf.pfnCmdUI_v_C)(pCmdUI);
			bResult = !pCmdUI->m_bContinueRouting;
			pCmdUI->m_bContinueRouting = FALSE;     // go back to idle
		}
		break;

	case AfxSigCmdUI_RANGE:
		{
			// ON_UPDATE_COMMAND_UI case
			ASSERT(nCode == CN_UPDATE_COMMAND_UI);
			ENSURE_ARG(pExtra != NULL);
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			ASSERT(pCmdUI->m_nID == nID);           // sanity assert
			ASSERT(!pCmdUI->m_bContinueRouting);    // idle - not set
			(pTarget->*mmf.pfnCmdUI_v_C_u)(pCmdUI, nID);
			bResult = !pCmdUI->m_bContinueRouting;
			pCmdUI->m_bContinueRouting = FALSE;     // go back to idle
		}
		break;

	// general extensibility hooks
	case AfxSigCmd_v_pv:
		(pTarget->*mmf.pfnCmd_v_pv)(pExtra);
		break;
	case AfxSigCmd_b_pv:
		bResult = (pTarget->*mmf.pfnCmd_b_pv)(pExtra);
		break;
	/*
	case AfxSig_vv:
		// normal command or control notification
		ASSERT(CN_COMMAND == 0);        // CN_COMMAND same as BN_CLICKED
		ASSERT(pExtra == NULL);
		(pTarget->*mmf.pfn_COMMAND)();
		break;

	case AfxSig_bv:
		// normal command or control notification
		ASSERT(CN_COMMAND == 0);        // CN_COMMAND same as BN_CLICKED
		ASSERT(pExtra == NULL);
		bResult = (pTarget->*mmf.pfn_bCOMMAND)();
		break;

	case AfxSig_vw:
		// normal command or control notification in a range
		ASSERT(CN_COMMAND == 0);        // CN_COMMAND same as BN_CLICKED
		ASSERT(pExtra == NULL);
		(pTarget->*mmf.pfn_COMMAND_RANGE)(nID);
		break;

	case AfxSig_bw:
		// extended command (passed ID, returns bContinue)
		ASSERT(pExtra == NULL);
		bResult = (pTarget->*mmf.pfn_COMMAND_EX)(nID);
		break;

	case AfxSig_vNMHDRpl:
		{
			AFX_NOTIFY* pNotify = (AFX_NOTIFY*)pExtra;
			ENSURE(pNotify != NULL);
			ASSERT(pNotify->pResult != NULL);
			ASSERT(pNotify->pNMHDR != NULL);
			(pTarget->*mmf.pfn_NOTIFY)(pNotify->pNMHDR, pNotify->pResult);
		}
		break;
	case AfxSig_bNMHDRpl:
		{
			AFX_NOTIFY* pNotify = (AFX_NOTIFY*)pExtra;
			ENSURE(pNotify != NULL);
			ASSERT(pNotify->pResult != NULL);
			ASSERT(pNotify->pNMHDR != NULL);
			bResult = (pTarget->*mmf.pfn_bNOTIFY)(pNotify->pNMHDR, pNotify->pResult);
		}
		break;
	case AfxSig_vwNMHDRpl:
		{
			AFX_NOTIFY* pNotify = (AFX_NOTIFY*)pExtra;
			ENSURE(pNotify != NULL);
			ASSERT(pNotify->pResult != NULL);
			ASSERT(pNotify->pNMHDR != NULL);
			(pTarget->*mmf.pfn_NOTIFY_RANGE)(nID, pNotify->pNMHDR,
				pNotify->pResult);
		}
		break;
	case AfxSig_bwNMHDRpl:
		{
			AFX_NOTIFY* pNotify = (AFX_NOTIFY*)pExtra;
			ENSURE(pNotify != NULL);
			ASSERT(pNotify->pResult != NULL);
			ASSERT(pNotify->pNMHDR != NULL);
			bResult = (pTarget->*mmf.pfn_NOTIFY_EX)(nID, pNotify->pNMHDR,
				pNotify->pResult);
		}
		break;
	case AfxSig_cmdui:
		{
			// ON_UPDATE_COMMAND_UI or ON_UPDATE_COMMAND_UI_REFLECT case
			ASSERT(CN_UPDATE_COMMAND_UI == (UINT)-1);
			ASSERT(nCode == CN_UPDATE_COMMAND_UI || nCode == 0xFFFF);
			ENSURE_ARG(pExtra != NULL);
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			ASSERT(!pCmdUI->m_bContinueRouting);    // idle - not set
			(pTarget->*mmf.pfn_UPDATE_COMMAND_UI)(pCmdUI);
			bResult = !pCmdUI->m_bContinueRouting;
			pCmdUI->m_bContinueRouting = FALSE;     // go back to idle
		}
		break;

	case AfxSig_cmduiw:
		{
			// ON_UPDATE_COMMAND_UI case
			ASSERT(nCode == CN_UPDATE_COMMAND_UI);
			ENSURE_ARG(pExtra != NULL);
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			ASSERT(pCmdUI->m_nID == nID);           // sanity assert
			ASSERT(!pCmdUI->m_bContinueRouting);    // idle - not set
			(pTarget->*mmf.pfn_UPDATE_COMMAND_UI_RANGE)(pCmdUI, nID);
			bResult = !pCmdUI->m_bContinueRouting;
			pCmdUI->m_bContinueRouting = FALSE;     // go back to idle
		}
		break;

	// general extensibility hooks
	case AfxSig_vpv:
		(pTarget->*mmf.pfn_OTHER)(pExtra);
		break;
	case AfxSig_bpv:
		bResult = (pTarget->*mmf.pfn_OTHER_EX)(pExtra);
		break;
	*/

	}
	return bResult;
}

BOOL CCmdTarget::OnCmdMsg(
   UINT nID,
   int nCode,
   void* pExtra,
   AFX_CMDHANDLERINFO* pHandlerInfo
)
{
	// determine the message number and code (packed into nCode)
	const AFX_MSGMAP* pMessageMap;
	const AFX_MSGMAP_ENTRY* lpEntry;
	UINT nMsg = 0;

	if (nCode != CN_UPDATE_COMMAND_UI)
	{
		nMsg = HIWORD(nCode);
		nCode = LOWORD(nCode);
	}

	// for backward compatibility HIWORD(nCode)==0 is WM_COMMAND
	if (nMsg == 0)
		nMsg = WM_COMMAND;

	// look through message map to see if it applies to us

	for (pMessageMap = GetMessageMap(); pMessageMap->pfnGetBaseMap != NULL;
	  pMessageMap = (*pMessageMap->pfnGetBaseMap)())
	{
		// Note: catches BEGIN_MESSAGE_MAP(CMyClass, CMyClass)!
		ASSERT(pMessageMap != (*pMessageMap->pfnGetBaseMap)());
		lpEntry = AfxFindMessageEntry(pMessageMap->lpEntries, nMsg, nCode, nID);
		if (lpEntry != NULL)
		{
			// found it
			return _AfxDispatchCmdMsg(this, nID, nCode,
				lpEntry->pfn, pExtra, lpEntry->nSig, pHandlerInfo);
		}
	}
	return FALSE;   // not handled
}

MFCWidget::MFCWidget(QWidget *parent)
   : QWidget(parent)
{
   setContextMenuPolicy(Qt::PreventContextMenu);
}

MFCWidget::~MFCWidget()
{
}

CWnd* CWnd::focusWnd = NULL;
QHash<QWidget*,CWnd*> CWnd::qtToMfcWindow;
CFrameWnd* CWnd::m_pFrameWnd = NULL;

IMPLEMENT_DYNAMIC(CWnd,CCmdTarget)

BEGIN_MESSAGE_MAP(CWnd,CCmdTarget)
   ON_WM_SETFOCUS()
   ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

_AFX_THREAD_STATE CWnd::_afxThreadState;

CWnd::CWnd(CWnd *parent)
   : firstPaintEvent(true),
     m_pParentWnd(parent),
     m_pOwnerWnd(parent),
     m_pFont(NULL),
     mfcVerticalScrollBar(NULL),
     mfcHorizontalScrollBar(NULL),
     m_hWnd((HWND)NULL),
     _grid(NULL),
     _myDC(NULL),
     _mfcBuddy(NULL),
     _dwStyle(0),
     _frameWidth(0)
{
   if ( parent )
   {
      _qt = new QFrame(parent->toQWidget());
   }
   else
   {
      _qt = new QFrame;
   }
   _qt->setGeometry(0,0,1,1); // CP: Without this the pattern view is overshadowed by the translucent frame.
   _grid = new QGridLayout;
   _grid->setContentsMargins(0,0,0,0);
   _grid->setSpacing(0);
   _qt->setLayout(_grid);
   _myDC = new CDC(this);

   _qt->setMouseTracking(true);
   _qt->installEventFilter(this);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qt->setFont(QFont("MS Shell Dlg",8));
#else
   _qt->setFont(QFont("MS Shell Dlg",9));
#endif

   _qtd = dynamic_cast<QFrame*>(_qt);
   this->moveToThread(QApplication::instance()->thread());
   QObject::connect(this,SIGNAL(update()),_qtd,SLOT(update()));
}

CWnd::~CWnd()
{
   if ( mfcVerticalScrollBar )
      delete mfcVerticalScrollBar;
   if ( mfcHorizontalScrollBar )
      delete mfcHorizontalScrollBar;
   mfcVerticalScrollBar = NULL;
   mfcHorizontalScrollBar = NULL;

   _myDC->detach(true);
   delete _myDC;

   if ( _qt )
      delete _qt;
   _qt = NULL;
   _qtd = NULL;
}

CWnd* PASCAL CWnd::FromHandle( 
   HWND hWnd  
)
{
   return (CWnd*)hWnd;
}

CWnd* PASCAL CWnd::FromHandlePermanent(
   HWND hWnd 
)
{
   return (CWnd*)hWnd;
}

CWnd* CWnd::SetFocus()
{
   CWnd* pWnd = focusWnd;
   _qt->setFocus();
   focusWnd = this;
   return pWnd;
}

BOOL CWnd::PreTranslateMessage(
   MSG* pMsg
)
{
   return FALSE;
}

CWnd* CWnd::GetFocus()
{
   return focusWnd;
}

void CWnd::SetOwner(
   CWnd* pOwnerWnd
)
{
   m_pOwnerWnd = pOwnerWnd; // Messages will go here.  
}

CWnd* CWnd::GetDescendantWindow( 
   int nID, 
   BOOL bOnlyPerm 
) const
{
   foreach ( CWnd* pWnd, mfcToQtWidget )
   {
      if ( pWnd->GetDlgCtrlID() == nID )
      {
         return pWnd;
      }
   }
   return NULL;
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
   _myDC->attach(toQWidget(),this,true);
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
   if ( this->thread() != QThread::currentThread() )
   {
      MFCMessageEvent* post = new MFCMessageEvent(QEvent::User);
      post->msg.message = message;
      post->msg.wParam = wParam;
      post->msg.lParam = lParam;
      post->msg.hwnd = m_hWnd;

      if ( backgroundedFamiTracker )
      {
         // CP: Don't want comment boxes popping up!
         if ( wParam == ID_MODULE_COMMENTS )
         {
            return false;
         }
      }

      memcpy(&_afxThreadState.m_lastSentMsg,&post->msg,sizeof(MSG));

      QApplication::postEvent(this,post);

      return true;
   }
   else
   {
      MFCMessageEvent post(QEvent::User);
      BOOL handled;
      post.msg.message = message;
      post.msg.wParam = wParam;
      post.msg.lParam = lParam;
      post.msg.hwnd = m_hWnd;

      _afxThreadState.m_lastSentMsg = post.msg;

      handled = QApplication::sendEvent(this,&post);

      return handled;
   }
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
      if ( bDeep && (mfcToQtWidget.count()) )
      {
         pWnd->SendMessageToDescendants(message,wParam,lParam,bDeep,bOnlyPerm);
      }
   }
}

LRESULT CWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// OnWndMsg does most of the work, except for DefWindowProc call
	LRESULT lResult = 0;
//	if (!OnWndMsg(message, wParam, lParam, &lResult))
//		lResult = DefWindowProc(message, wParam, lParam);
   // CP: We just need OnWndMsg...
   OnWndMsg(message, wParam, lParam, &lResult);
   
	return lResult;
}

struct AFX_MSG_CACHE
{
	UINT nMsg;
	const AFX_MSGMAP_ENTRY* lpEntry;
	const AFX_MSGMAP* pMessageMap;
};

AFX_MSG_CACHE _afxMsgCache;

BOOL CWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LRESULT lResult = 0;
	union MessageMapFunctions mmf;
	mmf.pfn = 0;
//	CInternalGlobalLock winMsgLock;
	// special case for commands
	if (message == WM_COMMAND)
	{
		if (OnCommand(wParam, lParam))
		{
			lResult = 1;
			goto LReturnTrue;
		}
		return FALSE;
	}

	// special case for notifies
   if (message == WM_NOTIFY)
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (pNMHDR->hwndFrom != NULL && OnNotify(wParam, lParam, &lResult))
			goto LReturnTrue;
		return FALSE;
	}
   
//   qDebug("WM_ACTIVATE or WM_SETCURSOR or ActiveX not handled");
//	// special case for activation
//	if (message == WM_ACTIVATE)
//		_AfxHandleActivate(this, wParam, CWnd::FromHandle((HWND)lParam));

//	// special case for set cursor HTERROR
//	if (message == WM_SETCURSOR &&
//		_AfxHandleSetCursor(this, (short)LOWORD(lParam), HIWORD(lParam)))
//	{
//		lResult = 1;
//		goto LReturnTrue;
//	}

//   // special case for windows that contain windowless ActiveX controls
//   BOOL bHandled;

//   bHandled = FALSE;
//   if ((m_pCtrlCont != NULL) && (m_pCtrlCont->m_nWindowlessControls > 0))
//   {
//	  if (((message >= WM_MOUSEFIRST) && (message <= AFX_WM_MOUSELAST)) ||
//		 ((message >= WM_KEYFIRST) && (message <= WM_IME_KEYLAST)) ||
//		 ((message >= WM_IME_SETCONTEXT) && (message <= WM_IME_KEYUP)))
//	  {
//		 bHandled = m_pCtrlCont->HandleWindowlessMessage(message, wParam, lParam, &lResult);
//	  }
//   }
//   if (bHandled)
//   {
//	  goto LReturnTrue;
//   }

//	switch (message)
//	{
//	case WM_SIZE:
//		{
//			CHwndRenderTarget* pRenderTarget = GetRenderTarget();
//			if (pRenderTarget != NULL && pRenderTarget->IsValid())
//			{
//				pRenderTarget->Resize(CD2DSizeU(UINT32(LOWORD(lParam)), UINT32(HIWORD(lParam))));
//				RedrawWindow();
//			}
//		}
//		break;

//	case WM_PAINT:
//		if (DoD2DPaint())
//		{
//			lResult = 1;
//			goto LReturnTrue;
//		}
//		break;

//	case WM_ERASEBKGND:
//		{
//			CHwndRenderTarget* pRenderTarget = GetRenderTarget();
//			if (pRenderTarget != NULL && pRenderTarget->IsValid())
//			{
//				lResult = 1;
//				goto LReturnTrue;
//			}
//		}
//		break;
//	}
   
	const AFX_MSGMAP* pMessageMap; pMessageMap = GetMessageMap();
//   AFX_MSG_CACHE* pMsgCache; pMsgCache = &_afxMsgCache;
   const AFX_MSGMAP_ENTRY* lpEntry;
//   if (message == pMsgCache->nMsg && pMessageMap == pMsgCache->pMessageMap)
//   {
//      // cache hit
//      lpEntry = pMsgCache->lpEntry;
//      qDebug("CACHED MESSAGE %x", message);
//      if (lpEntry == NULL)
//         return FALSE;

//      // cache hit, and it needs to be handled
//      if (message < 0xC000)
//         goto LDispatch;
//      else
//         goto LDispatchRegistered;
//   }
//   else
	{
		// not in cache, look for it
//		pMsgCache->nMsg = message;
//		pMsgCache->pMessageMap = pMessageMap;

      for (/* pMessageMap already init'ed */; pMessageMap->pfnGetBaseMap != NULL;
         pMessageMap = (*pMessageMap->pfnGetBaseMap)())
      {
         // Note: catch not so common but fatal mistake!!
         //      BEGIN_MESSAGE_MAP(CMyWnd, CMyWnd)
         ASSERT(pMessageMap != (*pMessageMap->pfnGetBaseMap)());
         if (message < 0xC000)
         {
            // constant window message
            if ((lpEntry = AfxFindMessageEntry(pMessageMap->lpEntries,
               message, 0, 0)) != NULL)
            {
//               pMsgCache->lpEntry = lpEntry;
               goto LDispatch;
            }
         }
         else
         {
            // registered windows message
            lpEntry = pMessageMap->lpEntries;
            while ((lpEntry = AfxFindMessageEntry(lpEntry, 0xC000, 0, 0)) != NULL)
            {
               UINT* pnID = (UINT*)(lpEntry->nSig);
               ASSERT(*pnID >= 0xC000 || *pnID == 0);
                  // must be successfully registered
               if (*pnID == message)
               {
//                  pMsgCache->lpEntry = lpEntry;
                  goto LDispatchRegistered;
               }
               lpEntry++;      // keep looking past this one
            }
         }
      }
      
//      pMsgCache->lpEntry = NULL;
      return FALSE;
   }

LDispatch:
	ASSERT(message < 0xC000);

	mmf.pfn = lpEntry->pfn;

	switch (lpEntry->nSig)
	{
	default:
		ASSERT(FALSE);
		break;
	case AfxSig_l_p:
		{
			CPoint point(lParam);		
			lResult = (this->*mmf.pfn_l_p)(point);
			break;
		}		
	case AfxSig_b_D_v:
		lResult = (this->*mmf.pfn_b_D)(CDC::FromHandle(reinterpret_cast<HDC>(wParam)));
		break;

	case AfxSig_b_b_v:
		lResult = (this->*mmf.pfn_b_b)(static_cast<BOOL>(wParam));
		break;

	case AfxSig_b_u_v:
		lResult = (this->*mmf.pfn_b_u)(static_cast<UINT>(wParam));
		break;

	case AfxSig_b_h_v:
		lResult = (this->*mmf.pfn_b_h)(reinterpret_cast<HANDLE>(wParam));
		break;

	case AfxSig_i_u_v:
		lResult = (this->*mmf.pfn_i_u)(static_cast<UINT>(wParam));
		break;

	case AfxSig_C_v_v:
		lResult = reinterpret_cast<LRESULT>((this->*mmf.pfn_C_v)());
		break;

	case AfxSig_v_u_W:
		(this->*mmf.pfn_v_u_W)(static_cast<UINT>(wParam), 
			CWnd::FromHandle(reinterpret_cast<HWND>(lParam)));
		break;

	case AfxSig_u_u_v:
		lResult = (this->*mmf.pfn_u_u)(static_cast<UINT>(wParam));
		break;

	case AfxSig_b_v_v:
		lResult = (this->*mmf.pfn_b_v)();
		break;

	case AfxSig_b_W_uu:
		lResult = (this->*mmf.pfn_b_W_u_u)(CWnd::FromHandle(reinterpret_cast<HWND>(wParam)),
			LOWORD(lParam), HIWORD(lParam));
		break;

	case AfxSig_b_W_COPYDATASTRUCT:
		lResult = (this->*mmf.pfn_b_W_COPYDATASTRUCT)(
			CWnd::FromHandle(reinterpret_cast<HWND>(wParam)),
			reinterpret_cast<COPYDATASTRUCT*>(lParam));
		break;

	case AfxSig_b_v_HELPINFO:
		lResult = (this->*mmf.pfn_b_HELPINFO)(reinterpret_cast<LPHELPINFO>(lParam));
		break;

	case AfxSig_CTLCOLOR:
		{
			// special case for OnCtlColor to avoid too many temporary objects
			ASSERT(message == WM_CTLCOLOR);
			AFX_CTLCOLOR* pCtl = reinterpret_cast<AFX_CTLCOLOR*>(lParam);
			CDC dcTemp; 
			dcTemp.m_hDC = pCtl->hDC;
			CWnd wndTemp; 
			wndTemp.m_hWnd = pCtl->hWnd;
			UINT nCtlType = pCtl->nCtlType;
			// if not coming from a permanent window, use stack temporary
			CWnd* pWnd = CWnd::FromHandlePermanent(wndTemp.m_hWnd);
			if (pWnd == NULL)
			{
				pWnd = &wndTemp;
			}
			HBRUSH hbr = (this->*mmf.pfn_B_D_W_u)(&dcTemp, pWnd, nCtlType);
			// fast detach of temporary objects
			dcTemp.m_hDC = NULL;
			wndTemp.m_hWnd = NULL;
			lResult = reinterpret_cast<LRESULT>(hbr);
		}
		break;

	case AfxSig_CTLCOLOR_REFLECT:
		{
			// special case for CtlColor to avoid too many temporary objects
			ASSERT(message == WM_REFLECT_BASE+WM_CTLCOLOR);
			AFX_CTLCOLOR* pCtl = reinterpret_cast<AFX_CTLCOLOR*>(lParam);
			CDC dcTemp; 
			dcTemp.m_hDC = pCtl->hDC;
			UINT nCtlType = pCtl->nCtlType;
			HBRUSH hbr = (this->*mmf.pfn_B_D_u)(&dcTemp, nCtlType);
			// fast detach of temporary objects
			dcTemp.m_hDC = NULL;
			lResult = reinterpret_cast<LRESULT>(hbr);
		}
		break;

	case AfxSig_i_u_W_u:
		lResult = (this->*mmf.pfn_i_u_W_u)(LOWORD(wParam),
			CWnd::FromHandle(reinterpret_cast<HWND>(lParam)), HIWORD(wParam));
		break;

	case AfxSig_i_uu_v:
		lResult = (this->*mmf.pfn_i_u_u)(LOWORD(wParam), HIWORD(wParam));
		break;

	case AfxSig_i_W_uu:
		lResult = (this->*mmf.pfn_i_W_u_u)(CWnd::FromHandle(reinterpret_cast<HWND>(wParam)),
			LOWORD(lParam), HIWORD(lParam));
		break;

	case AfxSig_i_v_s:
		lResult = (this->*mmf.pfn_i_s)(reinterpret_cast<LPTSTR>(lParam));
		break;

	case AfxSig_l_w_l:
		lResult = (this->*mmf.pfn_l_w_l)(wParam, lParam);
		break;

	case AfxSig_l_uu_M:
		lResult = (this->*mmf.pfn_l_u_u_M)(LOWORD(wParam), HIWORD(wParam), 
			CMenu::FromHandle(reinterpret_cast<HMENU>(lParam)));
		break;
		
	case AfxSig_v_b_h:
	    (this->*mmf.pfn_v_b_h)(static_cast<BOOL>(wParam), 
			reinterpret_cast<HANDLE>(lParam));
		break;

	case AfxSig_v_h_v:
	    (this->*mmf.pfn_v_h)(reinterpret_cast<HANDLE>(wParam));
		break;

	case AfxSig_v_h_h:
	    (this->*mmf.pfn_v_h_h)(reinterpret_cast<HANDLE>(wParam), 
			reinterpret_cast<HANDLE>(lParam));
		break;

	case AfxSig_v_v_v:
		(this->*mmf.pfn_v_v)();
		break;

	case AfxSig_v_u_v:
		(this->*mmf.pfn_v_u)(static_cast<UINT>(wParam));
		break;

	case AfxSig_v_u_u:
		(this->*mmf.pfn_v_u_u)(static_cast<UINT>(wParam), static_cast<UINT>(lParam));
		break;

	case AfxSig_v_uu_v:
		(this->*mmf.pfn_v_u_u)(LOWORD(wParam), HIWORD(wParam));
		break;

	case AfxSig_v_v_ii:
		(this->*mmf.pfn_v_i_i)(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case AfxSig_v_u_uu:
		(this->*mmf.pfn_v_u_u_u)(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
		break;

	case AfxSig_v_u_ii:
		(this->*mmf.pfn_v_u_i_i)(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
		break;

   case AfxSig_v_w_l:
		(this->*mmf.pfn_v_w_l)(wParam, lParam);
		break;

	case AfxSig_MDIACTIVATE:
		(this->*mmf.pfn_v_b_W_W)(m_hWnd == reinterpret_cast<HWND>(lParam),
			CWnd::FromHandle(reinterpret_cast<HWND>(lParam)),
			CWnd::FromHandle(reinterpret_cast<HWND>(wParam)));
		break;

	case AfxSig_v_D_v:
		(this->*mmf.pfn_v_D)(CDC::FromHandle(reinterpret_cast<HDC>(wParam)));
		break;

	case AfxSig_v_M_v:
		(this->*mmf.pfn_v_M)(CMenu::FromHandle(reinterpret_cast<HMENU>(wParam)));
		break;

	case AfxSig_v_M_ub:
		(this->*mmf.pfn_v_M_u_b)(CMenu::FromHandle(reinterpret_cast<HMENU>(wParam)),
			GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case AfxSig_v_W_v:
		(this->*mmf.pfn_v_W)(CWnd::FromHandle(reinterpret_cast<HWND>(wParam)));
		break;

	case AfxSig_v_v_W:
		(this->*mmf.pfn_v_W)(CWnd::FromHandle(reinterpret_cast<HWND>(lParam)));
		break;

	case AfxSig_v_W_uu:
		(this->*mmf.pfn_v_W_u_u)(CWnd::FromHandle(reinterpret_cast<HWND>(wParam)), LOWORD(lParam),
			HIWORD(lParam));
		break;

	case AfxSig_v_W_p:
		{
			CPoint point(lParam);
			(this->*mmf.pfn_v_W_p)(CWnd::FromHandle(reinterpret_cast<HWND>(wParam)), point);
		}
		break;

	case AfxSig_v_W_h:
		(this->*mmf.pfn_v_W_h)(CWnd::FromHandle(reinterpret_cast<HWND>(wParam)),
				reinterpret_cast<HANDLE>(lParam));
		break;

	case AfxSig_ACTIVATE:
		(this->*mmf.pfn_v_u_W_b)(LOWORD(wParam),
			CWnd::FromHandle(reinterpret_cast<HWND>(lParam)), HIWORD(wParam));
		break;

	case AfxSig_SCROLL:
	case AfxSig_SCROLL_REFLECT:
		{
			// special case for WM_VSCROLL and WM_HSCROLL
			ASSERT(message == WM_VSCROLL || message == WM_HSCROLL ||
				message == WM_VSCROLL+WM_REFLECT_BASE || message == WM_HSCROLL+WM_REFLECT_BASE);
			int nScrollCode = (short)LOWORD(wParam);
			int nPos = (short)HIWORD(wParam);
			if (lpEntry->nSig == AfxSig_SCROLL)
				(this->*mmf.pfn_v_u_u_W)(nScrollCode, nPos,
					CWnd::FromHandle(reinterpret_cast<HWND>(lParam)));
			else
				(this->*mmf.pfn_v_u_u)(nScrollCode, nPos);
		}
		break;

	case AfxSig_v_v_s:
		(this->*mmf.pfn_v_s)(reinterpret_cast<LPTSTR>(lParam));
		break;

	case AfxSig_v_u_cs:
		(this->*mmf.pfn_v_u_cs)(static_cast<UINT>(wParam), reinterpret_cast<LPCTSTR>(lParam));
		break;

	case AfxSig_OWNERDRAW:
		(this->*mmf.pfn_v_i_s)(static_cast<int>(wParam), reinterpret_cast<LPTSTR>(lParam));
		lResult = TRUE;
		break;

	case AfxSig_i_i_s:
		lResult = (this->*mmf.pfn_i_i_s)(static_cast<int>(wParam), reinterpret_cast<LPTSTR>(lParam));
		break;

	case AfxSig_u_v_p:
		{
			CPoint point(lParam);
			lResult = (this->*mmf.pfn_u_p)(point);
		}
		break;

	case AfxSig_u_v_v:
		lResult = (this->*mmf.pfn_u_v)();
		break;

	case AfxSig_v_b_NCCALCSIZEPARAMS:
		(this->*mmf.pfn_v_b_NCCALCSIZEPARAMS)(static_cast<BOOL>(wParam), 
			reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam));
		break;

	case AfxSig_v_v_WINDOWPOS:
		(this->*mmf.pfn_v_v_WINDOWPOS)(reinterpret_cast<WINDOWPOS*>(lParam));
		break;

	case AfxSig_v_uu_M:
		(this->*mmf.pfn_v_u_u_M)(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HMENU>(lParam));
		break;

	case AfxSig_v_u_p:
		{
			CPoint point(lParam);
         (this->*mmf.pfn_v_u_p)(static_cast<UINT>(wParam), point);
		}
		break;

	case AfxSig_SIZING:
		(this->*mmf.pfn_v_u_pr)(static_cast<UINT>(wParam), reinterpret_cast<LPRECT>(lParam));
		lResult = TRUE;
		break;

	case AfxSig_MOUSEWHEEL:
		lResult = (this->*mmf.pfn_b_u_s_p)(LOWORD(wParam), (short)HIWORD(wParam),
			CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		if (!lResult)
			return FALSE;
		break;
	case AfxSig_MOUSEHWHEEL:
      qDebug("AfxSig_MOUSEWHEEL");
//		(this->*mmf.pfn_MOUSEHWHEEL)(LOWORD(wParam), (short)HIWORD(wParam),
//			CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;
	case AfxSig_l:
		lResult = (this->*mmf.pfn_l_v)();
		if (lResult != 0)
			return FALSE;
		break;
	case AfxSig_u_W_u:
      qDebug("AfxSig_u_W_u");
//		lResult = (this->*mmf.pfn_u_W_u)(CWnd::FromHandle(reinterpret_cast<HWND>(wParam)), static_cast<UINT>(lParam));
		break;
	case AfxSig_v_u_M:
      qDebug("AfxSig_v_u_M");
//		(this->*mmf.pfn_v_u_M)(static_cast<UINT>(wParam), CMenu::FromHandle(reinterpret_cast<HMENU>(lParam)));
		break;
	case AfxSig_u_u_M:
      qDebug("AfxSig_u_u_M");
//		lResult = (this->*mmf.pfn_u_u_M)(static_cast<UINT>(wParam), CMenu::FromHandle(reinterpret_cast<HMENU>(lParam)));
		break;
	case AfxSig_u_v_MENUGETOBJECTINFO:
      qDebug("AfxSig_u_v_MENUGETOBJECTINFO");
//		lResult = (this->*mmf.pfn_u_v_MENUGETOBJECTINFO)(reinterpret_cast<MENUGETOBJECTINFO*>(lParam));
		break;
	case AfxSig_v_M_u:
      qDebug("AfxSig_v_M_u");
//		(this->*mmf.pfn_v_M_u)(CMenu::FromHandle(reinterpret_cast<HMENU>(wParam)), static_cast<UINT>(lParam));
		break;
	case AfxSig_v_u_LPMDINEXTMENU:
      qDebug("AfxSig_v_u_LPMDINEXTMENU");
//		(this->*mmf.pfn_v_u_LPMDINEXTMENU)(static_cast<UINT>(wParam), reinterpret_cast<LPMDINEXTMENU>(lParam));
		break;
	case AfxSig_APPCOMMAND:
      qDebug("AfxSig_APPCOMMAND");
//		(this->*mmf.pfn_APPCOMMAND)(CWnd::FromHandle(reinterpret_cast<HWND>(wParam)), static_cast<UINT>(GET_APPCOMMAND_LPARAM(lParam)), static_cast<UINT>(GET_DEVICE_LPARAM(lParam)), static_cast<UINT>(GET_KEYSTATE_LPARAM(lParam)));
		lResult = TRUE;
		break;
	case AfxSig_RAWINPUT:
      qDebug("AfxSig_RAWINPUT");
//		(this->*mmf.pfn_RAWINPUT)(static_cast<UINT>(GET_RAWINPUT_CODE_WPARAM(wParam)), reinterpret_cast<HRAWINPUT>(lParam));
		break;
	case AfxSig_u_u_u:
      qDebug("AfxSig_u_u_u");
//		lResult = (this->*mmf.pfn_u_u_u)(static_cast<UINT>(wParam), static_cast<UINT>(lParam));
		break;
	case AfxSig_MOUSE_XBUTTON:
      qDebug("AfxSig_MOUSE_XBUTTON");
//		(this->*mmf.pfn_MOUSE_XBUTTON)(static_cast<UINT>(GET_KEYSTATE_WPARAM(wParam)), static_cast<UINT>(GET_XBUTTON_WPARAM(wParam)), CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		lResult = TRUE;
		break;
	case AfxSig_MOUSE_NCXBUTTON:
      qDebug("AfxSig_MOUSE_NCXBUTTON");
//		(this->*mmf.pfn_MOUSE_NCXBUTTON)(static_cast<short>(GET_NCHITTEST_WPARAM(wParam)), static_cast<UINT>(GET_XBUTTON_WPARAM(wParam)), CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		lResult = TRUE;
		break;
	case AfxSig_INPUTLANGCHANGE:
      qDebug("AfxSig_INPUTLANGCHANGE");
//		(this->*mmf.pfn_INPUTLANGCHANGE)(static_cast<UINT>(wParam), static_cast<UINT>(lParam));
		lResult = TRUE;
		break;
	case AfxSig_INPUTDEVICECHANGE:
      qDebug("AfxSig_INPUTDEVICECHANGE");
//		(this->*mmf.pfn_INPUTDEVICECHANGE)(GET_DEVICE_CHANGE_LPARAM(wParam), reinterpret_cast<HANDLE>(lParam));
		break;
	case AfxSig_v_u_hkl:
      qDebug("AfxSig_v_u_h");
//		(this->*mmf.pfn_v_u_h)(static_cast<UINT>(wParam), reinterpret_cast<HKL>(lParam));
		break;
	}
	goto LReturnTrue;

LDispatchRegistered:    // for registered windows messages
	ASSERT(message >= 0xC000);
	ASSERT(sizeof(mmf) == sizeof(mmf.pfn));
	mmf.pfn = lpEntry->pfn;
	lResult = (this->*mmf.pfn_l_w_l)(wParam, lParam);

LReturnTrue:
	if (pResult != NULL)
		*pResult = lResult;
	return TRUE;
}

BOOL CWnd::OnCommand( 
   WPARAM wParam, 
   LPARAM lParam  
)
// return TRUE if command invocation was attempted
{
   UINT nID = LOWORD(wParam);
   HWND hWndCtrl = (HWND)lParam;
   int nCode = HIWORD(wParam);
   
   // default routing for command messages (through closure table)
   
   if (hWndCtrl == NULL)
   {
      // zero IDs for normal commands are not allowed
      if (nID == 0)
         return FALSE;
   
      // make sure command has not become disabled before routing
      CTestCmdUI state;
      state.m_nID = nID;
      OnCmdMsg(nID, CN_UPDATE_COMMAND_UI, &state, NULL);
      if (!state.m_bEnabled)
      {
//         TRACE(traceAppMsg, 0, "Warning: not executing disabled command %d\n", nID);
         return TRUE;
      }
   
      // menu or accelerator
      nCode = CN_COMMAND;
   }
   else
   {
      // control notification
//      ASSERT(nID == 0 || ::IsWindow(hWndCtrl));
   
//      if (_afxThreadState->m_hLockoutNotifyWindow == m_hWnd)
//         return TRUE;        // locked out - ignore control notification
   
      // reflect notification to child window control
      if (ReflectLastMsg(hWndCtrl))
         return TRUE;    // eaten by child
   
      // zero IDs for normal commands are not allowed
      if (nID == 0)
         return FALSE;
   }
   
#ifdef _DEBUG
//if (nCode < 0 && nCode != (int)0x8000)
//   TRACE(traceAppMsg, 0, "Implementation Warning: control notification = $%X.\n",
//      nCode);
#endif

   return OnCmdMsg(nID, nCode, NULL, NULL);
}

BOOL PASCAL CWnd::ReflectLastMsg(HWND hWndChild, LRESULT* pResult)
{
   CWnd* pWnd = (CWnd*)hWndChild;
   ASSERT(pWnd == NULL || pWnd->m_hWnd == hWndChild);
   if (pWnd == NULL)
   {
      return FALSE;
   }

   // only OLE controls and permanent windows will get reflected msgs
   ASSERT(pWnd != NULL);
   return pWnd->SendChildNotifyLastMsg(pResult);
}

BOOL CWnd::SendChildNotifyLastMsg(LRESULT* pResult)
{
   _AFX_THREAD_STATE* pThreadState = _afxThreadState.GetData();
   return OnChildNotify(pThreadState->m_lastSentMsg.message,
      pThreadState->m_lastSentMsg.wParam, pThreadState->m_lastSentMsg.lParam, pResult);
}


BOOL CWnd::OnChildNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
   return ReflectChildNotify(uMsg, wParam, lParam, pResult);
}

BOOL CWnd::ReflectChildNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
   // Note: reflected messages are send directly to CWnd::OnWndMsg
   //  and CWnd::OnCmdMsg for speed and because these messages are not
   //  routed by normal OnCmdMsg routing (they are only dispatched)

   switch (uMsg)
   {
   // normal messages (just wParam, lParam through OnWndMsg)
   case WM_HSCROLL:
   case WM_VSCROLL:
   case WM_PARENTNOTIFY:
   case WM_DRAWITEM:
   case WM_MEASUREITEM:
   case WM_DELETEITEM:
   case WM_VKEYTOITEM:
   case WM_CHARTOITEM:
   case WM_COMPAREITEM:
      // reflect the message through the message map as WM_REFLECT_BASE+uMsg
      return CWnd::OnWndMsg(WM_REFLECT_BASE+uMsg, wParam, lParam, pResult);

   // special case for WM_COMMAND
   case WM_COMMAND:
      {
         // reflect the message through the message map as OCM_COMMAND
         int nCode = HIWORD(wParam);
         if (CWnd::OnCmdMsg(0, MAKELONG(nCode, WM_REFLECT_BASE+WM_COMMAND), NULL, NULL))
         {
            if (pResult != NULL)
               *pResult = 1;
            return TRUE;
         }
      }
      break;

   // special case for WM_NOTIFY
   case WM_NOTIFY:
      {
         // reflect the message through the message map as OCM_NOTIFY
         NMHDR* pNMHDR = (NMHDR*)lParam;
         int nCode = pNMHDR->code;
         AFX_NOTIFY notify;
         notify.pResult = pResult;
         notify.pNMHDR = pNMHDR;
         return CWnd::OnCmdMsg(0, MAKELONG(nCode, WM_REFLECT_BASE+WM_NOTIFY), &notify, NULL);
      }

   // other special cases (WM_CTLCOLOR family)
   default:
      if (uMsg >= WM_CTLCOLORMSGBOX && uMsg <= WM_CTLCOLORSTATIC)
      {
         // fill in special struct for compatiblity with 16-bit WM_CTLCOLOR
         AFX_CTLCOLOR ctl;
         ctl.hDC = (HDC)wParam;
         ctl.nCtlType = uMsg - WM_CTLCOLORMSGBOX;
         //ASSERT(ctl.nCtlType >= CTLCOLOR_MSGBOX);
         ASSERT(ctl.nCtlType <= CTLCOLOR_STATIC);

         // reflect the message through the message map as OCM_CTLCOLOR
         BOOL bResult = CWnd::OnWndMsg(WM_REFLECT_BASE+WM_CTLCOLOR, 0, (LPARAM)&ctl, pResult);
         if ((HBRUSH)*pResult == NULL)
            bResult = FALSE;
         return bResult;
      }
      break;
   }

   return FALSE;   // let the parent handle it
}

BOOL CWnd::OnNotify(WPARAM, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(pResult != NULL);
	NMHDR* pNMHDR = (NMHDR*)lParam;
	HWND hWndCtrl = pNMHDR->hwndFrom;

	// get the child ID from the window itself
	UINT_PTR nID = _AfxGetDlgCtrlID(hWndCtrl);
	int nCode = pNMHDR->code;

//	ASSERT(hWndCtrl != NULL);
//	ASSERT(::IsWindow(hWndCtrl));

//	if (_afxThreadState->m_hLockoutNotifyWindow == m_hWnd)
//		return TRUE;        // locked out - ignore control notification

   // reflect notification to child window control
   if (ReflectLastMsg(hWndCtrl, pResult))
      return TRUE;        // eaten by child

	AFX_NOTIFY notify;
	notify.pResult = pResult;
	notify.pNMHDR = pNMHDR;
	return OnCmdMsg((UINT)nID, MAKELONG(nCode, WM_NOTIFY), &notify, NULL);
}

bool CWnd::event(QEvent *event)
{
   MFCMessageEvent* msgEvent = dynamic_cast<MFCMessageEvent*>(event);
   bool proc = false;
   if ( msgEvent )
   {
      proc = ptrToTheApp->PreTranslateMessage(&msgEvent->msg);
      if ( !proc )
      {
         proc = WindowProc(msgEvent->msg.message,msgEvent->msg.wParam,msgEvent->msg.lParam);
         if ( proc )
         {
            event->accept();
            emit update();
         }
         else
         {
            event->ignore();
         }
      }
      else
      {
         event->accept();
         emit update();
      }
   }
   return proc;
}

void CWnd::mousePressEvent(QMouseEvent *event)
{
   CPoint point(event->pos());
   unsigned int flags = 0;
   if ( event->modifiers()&Qt::ControlModifier )
   {
      flags |= MK_CONTROL;
   }
   if ( event->modifiers()&Qt::ShiftModifier )
   {
      flags |= MK_SHIFT;
   }
   if ( event->buttons()&Qt::LeftButton )
   {
      flags |= MK_LBUTTON;
   }
   if ( event->buttons()&Qt::MiddleButton )
   {
      flags |= MK_MBUTTON;
   }
   if ( event->buttons()&Qt::RightButton )
   {
      flags |= MK_RBUTTON;            
   }
   if ( event->button() == Qt::LeftButton )
   {
      PostMessage(WM_LBUTTONDOWN,flags,point.y<<16|point.x);
   }
   else if ( event->button() == Qt::RightButton )
   {
      PostMessage(WM_RBUTTONDOWN,flags,point.y<<16|point.x);
      
      // Also handle context menu...
      NMITEMACTIVATE nmia;
   
      nmia.hdr.hwndFrom = m_hWnd;
      nmia.hdr.idFrom = _id;
      nmia.hdr.code = NM_RCLICK;
      nmia.ptAction.x = QCursor::pos().x();
      nmia.ptAction.y = QCursor::pos().y();
      
      if ( GetOwner() )
         GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmia);
      PostMessage(WM_CONTEXTMENU,(WPARAM)m_hWnd,(LPARAM)((QCursor::pos().x()<<16)|(QCursor::pos().y())));
   }
}

void CWnd::mouseMoveEvent(QMouseEvent *event)
{
   CPoint point(event->pos());
   unsigned int flags = 0;
   if ( event->modifiers()&Qt::ControlModifier )
   {
      flags |= MK_CONTROL;
   }
   if ( event->modifiers()&Qt::ShiftModifier )
   {
      flags |= MK_SHIFT;
   }
   if ( event->buttons()&Qt::LeftButton )
   {
      flags |= MK_LBUTTON;
   }
   if ( event->buttons()&Qt::MiddleButton )
   {
      flags |= MK_MBUTTON;
   }
   if ( event->buttons()&Qt::RightButton )
   {
      flags |= MK_RBUTTON;            
   }
   PostMessage(WM_MOUSEMOVE,flags,point.y<<16|point.x);
}

void CWnd::mouseReleaseEvent(QMouseEvent *event)
{
   CPoint point(event->pos());
   unsigned int flags = 0;
   if ( event->modifiers()&Qt::ControlModifier )
   {
      flags |= MK_CONTROL;
   }
   if ( event->modifiers()&Qt::ShiftModifier )
   {
      flags |= MK_SHIFT;
   }
   if ( event->buttons()&Qt::LeftButton )
   {
      flags |= MK_LBUTTON;
   }
   if ( event->buttons()&Qt::MiddleButton )
   {
      flags |= MK_MBUTTON;
   }
   if ( event->buttons()&Qt::RightButton )
   {
      flags |= MK_RBUTTON;            
   }
   if ( event->button() == Qt::LeftButton )
   {
      PostMessage(WM_LBUTTONUP,flags,point.y<<16|point.x);
   }
   else if ( event->button() == Qt::RightButton )
   {
      PostMessage(WM_RBUTTONUP,flags,point.y<<16|point.x);
   }
}

void CWnd::mouseDoubleClickEvent(QMouseEvent *event)
{
   CPoint point(event->pos());
   unsigned int flags = 0;
   if ( event->modifiers()&Qt::ControlModifier )
   {
      flags |= MK_CONTROL;
   }
   if ( event->modifiers()&Qt::ShiftModifier )
   {
      flags |= MK_SHIFT;
   }
   if ( event->buttons()&Qt::LeftButton )
   {
      flags |= MK_LBUTTON;
   }
   if ( event->buttons()&Qt::MiddleButton )
   {
      flags |= MK_MBUTTON;
   }
   if ( event->buttons()&Qt::RightButton )
   {
      flags |= MK_RBUTTON;            
   }
   if ( event->button() == Qt::LeftButton )
   {
      PostMessage(WM_LBUTTONDBLCLK,flags,point.y<<16|point.x);
   }
   else if ( event->button() == Qt::RightButton )
   {
      PostMessage(WM_RBUTTONDBLCLK,flags,point.y<<16|point.x);
   }
}

void CWnd::keyPressEvent(QKeyEvent *event)
{
   qDebug("keyPress: key=%x, scan=%x %d %d",event->key(),event->nativeScanCode(),dynamic_cast<CDialog*>(this),dynamic_cast<CFrameWnd*>(this));
#ifdef __APPLE__
   SendMessage(WM_KEYDOWN,qtToMfcKeycode(event->key()),event->key()<<16);
#else
   SendMessage(WM_KEYDOWN,qtToMfcKeycode(event->key()),event->nativeScanCode()<<16);
#endif
}

void CWnd::keyReleaseEvent(QKeyEvent *event)
{
   SendMessage(WM_KEYUP,qtToMfcKeycode(event->key()),0);
}

void CWnd::timerEvent(QTimerEvent *event)
{
   int mfcId = mfcTimerId(event->timerId());
   PostMessage(WM_TIMER,mfcId);
}

void CWnd::paintEvent(QPaintEvent *event)
{
   static QSize currentSize = _qt->size();
   CDC* pDC = _myDC;
   AFX_CTLCOLOR ctlColor;
   ctlColor.hWnd = m_hWnd;
   ctlColor.hDC = (HDC)pDC;
   ctlColor.nCtlType = 0;
   DRAWITEMSTRUCT di;
   di.hDC = (HDC)pDC;

   if ( firstPaintEvent ||
        _qt->size() != currentSize )
   {
      SendMessage(WM_CTLCOLOR+WM_REFLECT_BASE,0,(LPARAM)&ctlColor);
      // MUST erase the background on the first call!
      firstPaintEvent = false;
      SendMessage(WM_ERASEBKGND,(WPARAM)(HDC)pDC);
      currentSize = _qt->size();
   }
   SendMessage(WM_PAINT);
   SendMessage(WM_DRAWITEM,_id,(LPARAM)&di);
}

void CWnd::contextMenuEvent(QContextMenuEvent *event)
{
   NMITEMACTIVATE nmia;

   nmia.hdr.hwndFrom = m_hWnd;
   nmia.hdr.idFrom = _id;
   nmia.hdr.code = NM_RCLICK;
   nmia.ptAction.x = QCursor::pos().x();
   nmia.ptAction.y = QCursor::pos().y();

   if ( GetOwner() )   
      GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmia);
   PostMessage(WM_CONTEXTMENU,(WPARAM)m_hWnd,(LPARAM)((QCursor::pos().x()<<16)|(QCursor::pos().y())));
}

void CWnd::leaveEvent(QEvent *event)
{
   PostMessage(WM_NCMOUSEMOVE); // CP: Have to emulate this since Qt doesn't really tell us NC.
   PostMessage(WM_MOUSELEAVE);
}

void CWnd::resizeEvent(QResizeEvent *event)
{
   QSize size = event->size();

   if ( _dwStyle&WS_VSCROLL )
   {
      size.setWidth(size.width()-1-(GetSystemMetrics(SM_CXVSCROLL)+1));
   }
   if ( _dwStyle&WS_HSCROLL )
   {
      size.setHeight(size.height()-1-(GetSystemMetrics(SM_CYHSCROLL)+1));
   }
//   qDebug("size %dx%d",size.width(),size.height());
   SendMessage(WM_SIZE,SIZE_RESTORED,(size.height()<<16)|(size.width()));
}

void CWnd::moveEvent(QMoveEvent *event)
{
   PostMessage(WM_MOVE,0,(event->pos().y()<<16)|(event->pos().x()));
}

void CWnd::subclassWidget(int nID,CWnd* widget)
{
   mfcToQtWidget.remove(nID);
   mfcToQtWidget.insert(nID,widget);
}

void CWnd::focusInEvent(QFocusEvent *event)
{
   PostMessage(WM_SETFOCUS,(WPARAM)(HWND)focusWnd);
}

void CWnd::focusOutEvent(QFocusEvent *event)
{
   PostMessage(WM_KILLFOCUS,(WPARAM)(HWND)NULL);
}

void CWnd::closeEvent(QCloseEvent *)
{
   DestroyWindow();
}

bool CWnd::eventFilter(QObject *object, QEvent *event)
{
   if ( object == _qt )
   {
      if ( event->type() == QEvent::Close )
      {
         closeEvent(dynamic_cast<QCloseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Show )
      {
         showEvent(dynamic_cast<QShowEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Hide )
      {
         hideEvent(dynamic_cast<QHideEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Move )
      {
         moveEvent(dynamic_cast<QMoveEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Paint )
      {
         paintEvent(dynamic_cast<QPaintEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::FocusIn )
      {
         focusInEvent(dynamic_cast<QFocusEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::FocusOut )
      {
         focusOutEvent(dynamic_cast<QFocusEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Leave )
      {
         leaveEvent(event);
         return false;
      }
      if ( event->type() == QEvent::MouseButtonPress )
      {
         mousePressEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::MouseButtonRelease )
      {
         mouseReleaseEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::MouseButtonDblClick )
      {
         mouseDoubleClickEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::MouseMove )
      {
         mouseMoveEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Wheel )
      {
         wheelEvent(dynamic_cast<QWheelEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Resize )
      {
         resizeEvent(dynamic_cast<QResizeEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::KeyPress )
      {
         keyPressEvent(dynamic_cast<QKeyEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::KeyRelease )
      {
         keyReleaseEvent(dynamic_cast<QKeyEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::ContextMenu )
      {
         contextMenuEvent(dynamic_cast<QContextMenuEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::DragEnter )
      {
         dragEnterEvent(dynamic_cast<QDragEnterEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::DragMove )
      {
         dragMoveEvent(dynamic_cast<QDragMoveEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::Drop )
      {
         dropEvent(dynamic_cast<QDropEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::DragLeave )
      {
         dragLeaveEvent(dynamic_cast<QDragLeaveEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::Timer )
      {
         timerEvent(dynamic_cast<QTimerEvent*>(event));
         return true;
      }
   }
//   qDebug("eventFilter: unhandled %d object %s", event->type(), object->objectName().toLatin1().constData());
   return false;
}

BOOL CWnd::IsWindowVisible( ) const
{
   return _qt->isVisible();
}

BOOL CWnd::DestroyWindow()
{
   SendMessage(WM_DESTROY);
   if ( _qt )
   {
      QList<QWidget *> widgets = _qt->findChildren<QWidget *>();
      foreach ( QWidget* widget, widgets ) widget->deleteLater();
      _qt->close();
   }
   foreach ( int timerID, mfcToQtTimer )
   {
      _qt->killTimer(timerID);
   }
   mfcToQtTimer.clear();
   qtToMfcTimer.clear();

   if ( focusWnd == this )
   {
      focusWnd = NULL;
      m_pFrameWnd->SetFocus();
   }
   return TRUE;
}

BOOL CWnd::EnableWindow(
   BOOL bEnable
)
{
   BOOL state = _qt->isEnabled();
   _qt->setEnabled(bEnable);
   return state;
}

BOOL CWnd::Create( 
   LPCTSTR lpszClassName, 
   LPCTSTR lpszWindowName, 
   DWORD dwStyle, 
   const RECT& rect, 
   CWnd* pParentWnd, 
   LPCTSTR lpszMenuName, 
   DWORD dwExStyle, 
   CCreateContext* pContext
)
{
   return CreateEx(dwExStyle,lpszClassName,lpszWindowName,dwStyle,rect,pParentWnd,0,(LPVOID)pContext);
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
   _dwStyle = dwStyle;

   createStruct.dwExStyle = dwExStyle;
   createStruct.style = dwStyle;
   createStruct.x = rect.left;
   createStruct.y = rect.top;
   createStruct.cx = rect.right-rect.left;
   createStruct.cy = rect.bottom-rect.top;
   createStruct.lpCreateParams = lpParam;
   createStruct.lpszClass = lpszClassName;
   createStruct.lpszName = lpszWindowName;
   
   // For widgets that aren't added to a layout...
   m_pParentWnd = pParentWnd;
   if ( pParentWnd )
      _qt->setParent(pParentWnd->toQWidget());
   else
      _qt->setParent(NULL);
   PreCreateWindow(createStruct);
   _qtd->setLineWidth(0);
   _qtd->setMidLineWidth(0);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif
   if ( createStruct.dwExStyle&WS_EX_STATICEDGE )
   {
      _qtd->setFrameShape(QFrame::Panel);
      _qtd->setFrameShadow(QFrame::Sunken);
      _qtd->setLineWidth(1);
      _frameWidth = 1;
   }
   else if ( createStruct.dwExStyle&WS_EX_CLIENTEDGE )
   {
      _qtd->setFrameShape(QFrame::Panel);
      _qtd->setFrameShadow(QFrame::Sunken);
      _qtd->setLineWidth(2);
      _frameWidth = 2;
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
   OnCreate(&createStruct);

   if ( pParentWnd )
      pParentWnd->mfcToQtWidgetMap()->insertMulti(nID,this);
   
   return TRUE;
}

int CWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   return 0;
}

void CWnd::OnSetFocus(CWnd *)
{
   focusWnd = this;
}

void CWnd::OnDestroy( )
{
}

void CWnd::SetParent(CWnd *parent)
{
   _dwStyle |= WS_CHILD; 
   m_pParentWnd = parent; 
   m_pOwnerWnd = parent; 
   _qt->setParent(parent->toQWidget());
}

void CWnd::UpdateDialogControls(
   CCmdTarget* pTarget,
   BOOL bDisableIfNoHndler
)
{
   CCmdUI state;
   CWnd* pWnd;

   foreach ( pWnd, mfcToQtWidget )
   {
      state.m_nID = pWnd->GetDlgCtrlID();
      state.m_pOther = pWnd;      
      
      // call it directly to disable any routing
      if (pWnd->CWnd::OnCmdMsg(0, MAKELONG(0xffff,
         WM_COMMAND+WM_REFLECT_BASE), &state, NULL))
         continue;

		// check for handlers in the parent window
		if (CWnd::OnCmdMsg((UINT)state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
			continue;
      
      		// determine whether to disable when no handler exists
      		BOOL bDisableTemp = bDisableIfNoHndler;
      		if (bDisableTemp)
      		{
               if ( !dynamic_cast<CButton*>(pWnd) )               
               {
                  // non-button controls don't get automagically disabled
      				bDisableTemp = FALSE;
               }
               else
               {
                  // only certain button controls get automagically disabled
                  if ( (pWnd->GetStyle()&0x0F) == (UINT)BS_AUTOCHECKBOX ||
                     (pWnd->GetStyle()&0x0F) == (UINT)BS_AUTO3STATE ||
                     (pWnd->GetStyle()&0x0F) == (UINT)BS_GROUPBOX ||
                     (pWnd->GetStyle()&0x0F) == (UINT)BS_AUTORADIOBUTTON )
                  {
                     bDisableTemp = FALSE;
                  }
               }
      		}
		// check for handlers in the target (owner)
		state.DoUpdate(pTarget, bDisableTemp);
   }
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

   // Resize all windows...
   foreach ( CWnd* pWnd, mfcToQtWidget )
   {
      if ( pWnd == pWndExtra )
      {
//         qDebug("layout.rect %d+%d,%dx%d",layout.rect.left,layout.rect.top,layout.rect.right-layout.rect.left,layout.rect.bottom-layout.rect.top);
         pWndExtra->MoveWindow(&layout.rect); //this causes wobble of the pattern view...
      }
      else
      {
         // CP: Not sure how to resize dialogbars yet.
      }
   }
}

CFont* CWnd::GetFont( )
{
   if ( !m_pFont )
   {
      m_pFont = new CFont(_qt->font());
   }
   return m_pFont;
}

void CWnd::SetFont(
   CFont* pFont,
   BOOL bRedraw
)
{
   m_pFont = pFont;
   _qt->setFont((QFont)*pFont);
}

void CWnd::MoveWindow(int x, int y, int cx, int cy, BOOL bRepaint)
{
   MoveWindow(CRect(CPoint(x,y),CSize(cx,cy)),bRepaint);
}

void CWnd::MoveWindow(LPCRECT lpRect, BOOL bRepaint)
{
   CRect rectOrig = *lpRect;
   if ( _dwStyle&WS_CHILD )
   {
      CRect rectParent;
      GetParent()->GetClientRect(&rectParent);
      rectOrig.OffsetRect(rectParent.left,rectParent.top);
      if ( rectOrig.Width() < 0 ) rectOrig.right = rectOrig.left;
      if ( rectOrig.Height() < 0 ) rectOrig.bottom = rectOrig.top;
      setGeometry(rectOrig.left,rectOrig.top,rectOrig.Width(),rectOrig.Height());
      setFixedSize(rectOrig.Width(),rectOrig.Height());
   }
   else
   {
      if ( rectOrig.Width() < 0 ) rectOrig.right = rectOrig.left;
      if ( rectOrig.Height() < 0 ) rectOrig.bottom = rectOrig.top;
      setGeometry(rectOrig.left,rectOrig.top,rectOrig.Width(),rectOrig.Height());
      setBaseSize(rectOrig.Width(),rectOrig.Height());
   }
   if ( bRepaint )
      emit update();
}

void CWnd::DragAcceptFiles(
   BOOL bAccept
)
{
   _qtd->setAcceptDrops(bAccept);
}

MFCMessageEvent::MFCMessageEvent(Type type)
   : QEvent(type)
{
   memset(&msg,0,sizeof(msg));
}

BOOL CWnd::PostMessage(
   UINT message,
   WPARAM wParam,
   LPARAM lParam
)
{
   MFCMessageEvent* post = new MFCMessageEvent(QEvent::User);
   post->msg.message = message;
   post->msg.wParam = wParam;
   post->msg.lParam = lParam;
   post->msg.hwnd = m_hWnd;
   
   if ( backgroundedFamiTracker )
   {
      // CP: Don't want comment boxes popping up!
      if ( wParam == ID_MODULE_COMMENTS )
      {
         return false;
      }
   }

   _afxThreadState.m_lastSentMsg = post->msg;

   QApplication::postEvent(this,post);
   emit update();

   return true;
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
   {
      pUIE->SetDlgItemInt(nID,nValue,bSigned);
   }
}

UINT CWnd::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nID));
   if ( pUIE )
   {
      return pUIE->GetDlgItemInt(nID,lpTrans,bSigned);
   }
   else
   {
      return 0;
   }
}

void CWnd::SetDlgItemText(
   int nID,
   LPCTSTR lpszString
)
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nID));
   if ( pUIE )
   {
      pUIE->SetDlgItemText(nID,lpszString);
   }
}

int CWnd::GetDlgItemText(
   int nID,
   CString& rString
) const
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nID));
   if ( pUIE )
   {
      return pUIE->GetDlgItemText(nID,rString);
   }
   else
   {
      return 0;
   }
}

int CWnd::GetDlgItemText(
   int nID,
   LPTSTR lpStr,
   int nMaxCount
) const
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nID));
   if ( pUIE )
   {
      return pUIE->GetDlgItemText(nID,lpStr,nMaxCount);
   }
   else
   {
      return 0;
   }
}

void CWnd::CheckDlgButton(
   int nIDButton,
   UINT nCheck
)
{
   QtUIElement* pUIE = dynamic_cast<QtUIElement*>(GetDlgItem(nIDButton));
   if ( pUIE )
   {
      pUIE->CheckDlgButton(nIDButton,nCheck);
   }
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
   CWnd* pParentWnd
)
{
   CWnd* pWndSrc = pParentWnd->GetDlgItem(nID);

   if ( pWndSrc )
   {
      SetParent(pParentWnd);
      setParent(pParentWnd->toQWidget());
      setGeometry(pWndSrc->geometry());
      pParentWnd->subclassWidget(nID,this);
      subclassWidget(nID,pWndSrc);
      PreSubclassWindow();
//      delete pWndSrc;
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

void CWnd::CalcWindowRect(
   LPRECT lpClientRect,
   UINT nAdjustType 
)
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
      _qt->killTimer(mfcToQtTimer.value((int)id));
      qtToMfcTimer.remove(mfcToQtTimer.value((int)id));
      mfcToQtTimer.remove((int)id);
   }
   int qtId = _qt->startTimer(interval);
   mfcToQtTimer.insert((int)id,qtId);
   qtToMfcTimer.insert(qtId,(int)id);
   return (UINT)id;
}

void CWnd::KillTimer(UINT id)
{
   if ( mfcToQtTimer.contains((int)id) )
   {
      _qt->killTimer(mfcToQtTimer.value((int)id));
      qtToMfcTimer.remove(mfcToQtTimer.value((int)id));
      mfcToQtTimer.remove((int)id);
   }
}

BOOL CWnd::UpdateData(
   BOOL bSaveAndValidate
)
{
   CDataExchange dx(this,bSaveAndValidate);
   DoDataExchange(&dx);
   return TRUE;
}

CWnd* CWnd::GetWindow(
      UINT nCmd
) const
{
   CWnd* pWnd = NULL;
   UINT myID = _id;
   int idx;
   switch ( nCmd )
   {
   case GW_CHILD:
      pWnd = mfcToQtWidget.begin().value();
      break;
   case GW_HWNDNEXT:
      pWnd = GetParent();
      idx = pWnd->mfcToQtWidgetMap()->keys().indexOf(myID)+1;
      if ( idx < pWnd->mfcToQtWidgetMap()->count() )
      {
         pWnd = pWnd->mfcToQtWidgetMap()->value(pWnd->mfcToQtWidgetMap()->keys().at(idx));
      }
      else
      {
         pWnd = NULL;
      }
      break;
   default:
      qDebug("CWnd::GetWindow unsupported nCmd %d",nCmd);
      break;
   }
   return pWnd;
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
   strncpy(lpszStringBuf,(LPTSTR)_qt->windowTitle().toLatin1().constData(),nMaxCount);
   return strlen(lpszStringBuf);
#endif
}

void CWnd::SetWindowText(
   LPCTSTR lpszString
)
{
#if UNICODE
   QString str = QString::fromWCharArray(lpszString);
#else
   QString str = QString::fromLatin1(lpszString);
#endif
   _qt->setWindowTitle(str);
}

void CWnd::GetWindowRect(
   LPRECT lpRect
) const
{
   lpRect->left = _qt->frameGeometry().topLeft().x();
   lpRect->right = _qt->frameGeometry().topRight().x();
   lpRect->top = _qt->frameGeometry().topLeft().y();
   lpRect->bottom = _qt->frameGeometry().bottomRight().y();
}

void CWnd::GetClientRect(
   LPRECT lpRect
) const
{
   lpRect->left = 0;
   lpRect->right = rect().right();
   lpRect->top = 0;
   lpRect->bottom = rect().bottom();
   if ( _dwStyle&WS_VSCROLL )/*mfcVerticalScrollBar &&
        mfcVerticalScrollBar->toQWidget()->isVisible() )*/
   {
      lpRect->right -= (GetSystemMetrics(SM_CXVSCROLL)+1);
   }
   else
   {
      lpRect->right -= (2*_frameWidth);
      lpRect->right++;
   }
   if ( _dwStyle&WS_HSCROLL )/*mfcHorizontalScrollBar &&
        mfcHorizontalScrollBar->toQWidget()->isVisible() )*/
   {
      lpRect->bottom -= (GetSystemMetrics(SM_CYHSCROLL)+1);
   }
   else
   {
      lpRect->bottom -= (2*_frameWidth);
      lpRect->bottom++;
   }
   if ( (lpRect->right < 0) ||
        (lpRect->bottom < 0) )
   {
      lpRect->right = 0;
      lpRect->bottom = 0;
   }
}

void CWnd::ShowWindow(int code)
{
   switch ( code )
   {
   case SW_SHOW:
      _dwStyle |= WS_VISIBLE;
      _qtd->setVisible(true);
      break;
   case SW_HIDE:
      _dwStyle &= (~WS_VISIBLE);
      _qtd->setVisible(false);
      break;
   }
}

IMPLEMENT_DYNCREATE(CFrameWnd,CWnd)

BEGIN_MESSAGE_MAP(CFrameWnd,CWnd)
   ON_WM_SETFOCUS()
   ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()

CFrameWnd::CFrameWnd(CWnd *parent)
   : CWnd(parent),
     m_pViewActive(NULL),
     m_pDocument(NULL),
     m_bInRecalcLayout(FALSE),
     m_bAutoMenuEnable(TRUE)
{
   int idx;

   m_pFrameWnd = this;
   ptrToTheApp->m_pMainWnd = this;

   _grid->setSpacing(0);
   _grid->setContentsMargins(0, 0, 0, 0);
   _grid->setObjectName(QString::fromUtf8("gridLayout"));

   cbrsBottom = new QVBoxLayout();
   cbrsBottom->setSpacing(0);
   cbrsBottom->setContentsMargins(0,0,0,0);
   cbrsBottom->setObjectName(QString::fromUtf8("cbrsBottom"));

   _grid->addLayout(cbrsBottom, 2, 0, 1, -1);
   _grid->setRowMinimumHeight(2,0);
   _grid->setRowStretch(2,0);

   cbrsTop = new QVBoxLayout();
   cbrsTop->setSpacing(0);
   cbrsTop->setContentsMargins(0,0,0,0);
   cbrsTop->setObjectName(QString::fromUtf8("cbrsTop"));

   _grid->addLayout(cbrsTop, 0, 0, 1, -1);
   _grid->setRowMinimumHeight(0,0);
   _grid->setRowStretch(0,0);

   cbrsLeft = new QHBoxLayout();
   cbrsLeft->setSpacing(0);
   cbrsLeft->setContentsMargins(0,0,0,0);
   cbrsLeft->setObjectName(QString::fromUtf8("cbrsLeft"));

   _grid->addLayout(cbrsLeft, 1, 0, 1, 1);
   _grid->setColumnMinimumWidth(0,0);
   _grid->setColumnStretch(0,0);

   cbrsRight = new QHBoxLayout();
   cbrsRight->setSpacing(0);
   cbrsRight->setContentsMargins(0,0,0,0);
   cbrsRight->setObjectName(QString::fromUtf8("cbrsRight"));

   _grid->addLayout(cbrsRight, 1, 2, 1, 1);
   _grid->setColumnMinimumWidth(2,0);
   _grid->setColumnStretch(2,0);

   _qt->setLayout(_grid);

   if ( !backgroundedFamiTracker )
   {
      ptrToTheApp->qtMainWindow->menuBar()->setNativeMenuBar(false);
   }
}

CFrameWnd::~CFrameWnd()
{
   delete m_pMenu;
}

void CFrameWnd::setModified(bool modified)
{
}

void CFrameWnd::focusChanged(QWidget *old, QWidget *now)
{
   CWnd* pOldWnd = qtToMfcWindow.value(old);
   CWnd* pNowWnd = qtToMfcWindow.value(now);
   if ( pOldWnd )
      pOldWnd->SendMessage(WM_KILLFOCUS,(WPARAM)pNowWnd);
   if ( pNowWnd )
      pNowWnd->SendMessage(WM_SETFOCUS,(WPARAM)pOldWnd);
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

void CFrameWnd::addView(QWidget *view)
{
   realCentralWidget = view;
   realCentralWidget->setObjectName(QString::fromUtf8("realCentralWidget"));

   _grid->addWidget(realCentralWidget, 1, 1, 1, 1);
   _grid->setRowStretch(1,1);
   _grid->setColumnStretch(1,1);

   RecalcLayout();
}

BOOL CFrameWnd::Create( 
   LPCTSTR lpszClassName, 
   LPCTSTR lpszWindowName, 
   DWORD dwStyle, 
   const RECT& rect, 
   CWnd* pParentWnd, 
   LPCTSTR lpszMenuName, 
   DWORD dwExStyle, 
   CCreateContext* pContext
)
{
   CView* pView;
   int idx;

   m_pMenu = new CMenu;
   m_pMenu->LoadMenu(128);

   for ( idx = 0; idx < m_pMenu->GetMenuItemCount(); idx++ )
   {
      if ( !backgroundedFamiTracker )
      {
         QMenu* pMenu = m_pMenu->GetSubMenu(idx)->toQMenu();
         pMenu->menuAction()->setMenuRole(QAction::NoRole);
         ptrToTheApp->qtMainWindow->menuBar()->addMenu(pMenu);
      }
   }

   // Get focus changes...
   QObject::connect(QApplication::instance(),SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(focusChanged(QWidget*,QWidget*)));

   if ( !CWnd::Create(lpszClassName,lpszWindowName,dwStyle,rect,pParentWnd,lpszMenuName,dwExStyle,pContext) )
      return FALSE;

   // Set this frame's document.
   m_pDocument = pContext->m_pCurrentDoc;
   
   // The view is actually created in CFrameWnd::CreateView but I'm being lazy...
   pView = (CView*)pContext->m_pNewViewClass->CreateObject();   
   
   mfcToQtWidgetMap()->insert(AFX_IDW_PANE_FIRST,pView);
   
   // Set frame in hijacked context.
   pContext->m_pCurrentFrame = this;

   // Create the view!
   pView->Create(lpszClassName,lpszWindowName,dwStyle|WS_VSCROLL|WS_HSCROLL,rect,pParentWnd,lpszMenuName,dwExStyle,pContext);

   pView->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));

   pView->setFocusPolicy(Qt::StrongFocus);

   realCentralWidget->setLayout(pView->toQWidget()->layout());

   QObject::connect(m_pDocument,SIGNAL(documentSaved()),this,SIGNAL(documentSaved()));
   QObject::connect(m_pDocument,SIGNAL(documentClosed()),this,SIGNAL(documentClosed()));

   // Connect buried signals.
   QObject::connect(m_pDocument,SIGNAL(setModified(bool)),this,SIGNAL(editor_modificationChanged(bool)));
   QObject::connect(m_pDocument,SIGNAL(setModified(bool)),this,SLOT(setModified(bool)));
   
   return TRUE;
}

BOOL CFrameWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
	CWnd* pParentWnd, CCreateContext* pContext)
{
//	// only do this once
//	ASSERT_VALID_IDR(nIDResource);
//	ASSERT(m_nIDHelp == 0 || m_nIDHelp == nIDResource);

//	m_nIDHelp = nIDResource;    // ID for help context (+HID_BASE_RESOURCE)

//	CString strFullString;
//	if (strFullString.LoadString(nIDResource))
//		AfxExtractSubString(m_strTitle, strFullString, 0);    // first sub-string
   
//	VERIFY(AfxDeferRegisterClass(AFX_WNDFRAMEORVIEW_REG));

	// attempt to create the window
	LPCTSTR lpszClass;// = GetIconWndClass(dwDefaultStyle, nIDResource);
	CString strTitle = m_strTitle;
   if (!Create(lpszClass, strTitle, dwDefaultStyle, rectDefault,
	  pParentWnd, ATL_MAKEINTRESOURCE(nIDResource), 0L, pContext))
	{
		return FALSE;   // will self destruct on failure normally
	}

//	// save the default menu handle
//	ASSERT(m_hWnd != NULL);
//	m_hMenuDefault = m_dwMenuBarState == AFX_MBS_VISIBLE ? ::GetMenu(m_hWnd) : m_hMenu;

//	// load accelerator resource
//	LoadAccelTable(ATL_MAKEINTRESOURCE(nIDResource));

	if (pContext == NULL)   // send initial update
		SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

	return TRUE;
}

void CFrameWnd::GetMessageString(
   UINT nID,
   CString& rMessage
) const
{
   rMessage = qtMfcStringResource(nID);
}

void CFrameWnd::UpdateFrameTitleForDocument(LPCTSTR title)
{
   QString windowTitle;
   CString appName;
   m_strTitle = title;

#if UNICODE
   windowTitle = QString::fromWCharArray((LPCTSTR)title);
#else
   windowTitle = QString::fromLatin1((LPCTSTR)title);
#endif
   m_pDocument->GetDocTemplate()->GetDocString(appName,CDocTemplate::windowTitle);
#if UNICODE
   windowTitle += QString::fromWCharArray((LPCTSTR)appName);
#else
   windowTitle += QString::fromLatin1((LPCTSTR)appName);
#endif
   ptrToTheApp->qtMainWindow->setWindowTitle(windowTitle);
}

void CFrameWnd::InitialUpdateFrame(
   CDocument* pDoc,
   BOOL bMakeVisible
)
{
   // if the frame does not have an active view, set to first pane
	CView* pView = NULL;
	if (GetActiveView() == NULL)
	{
		CWnd* pWnd = GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
		if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
		{
			pView = (CView*)pWnd;
			SetActiveView(pView, FALSE);
		}
	}

    if (bMakeVisible)
	{
        // send initial update to all views (and other controls) in the frame
		SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

//		// give view a chance to save the focus (CFormView needs this)
//		if (pView != NULL)
//			pView->OnActivateFrame(WA_INACTIVE, this);

//		// finally, activate the frame
//		// (send the default show command unless the main desktop window)
//		int nCmdShow = -1;      // default
//		CWinApp* pApp = AfxGetApp();
//		if (pApp != NULL && pApp->m_pMainWnd == this)
//		{
//			nCmdShow = pApp->m_nCmdShow; // use the parameter from WinMain
//			pApp->m_nCmdShow = -1; // set to default after first time
//		}
//		ActivateFrame(nCmdShow);
      if ( pView )
      {
         pView->toQWidget()->setVisible(true);
      }
//		if (pView != NULL)
//			pView->OnActivateView(TRUE, pView, pView);
	}

//	// update frame counts and frame title (may already have been visible)
//	if (pDoc != NULL)
//		pDoc->UpdateFrameCounts();
//	OnUpdateFrameTitle(TRUE);
}

void CFrameWnd::SetMessageText(LPCTSTR fmt,...)
{
   CString message;
   va_list args;
   va_start(args,fmt);
   message.FormatV(fmt,args);
   va_end(args);
   if ( !backgroundedFamiTracker )
   {
#if UNICODE
      ptrToTheApp->qtMainWindow->statusBar()->showMessage(QString::fromWCharArray(message));
#else
      ptrToTheApp->qtMainWindow->statusBar()->showMessage(QString::fromLatin1(message));
#endif
   }
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

void CFrameWnd::OnSetFocus(CWnd *pOldWnd)
{
   if (m_pViewActive != NULL)
		m_pViewActive->SetFocus();
	else
		CWnd::OnSetFocus(pOldWnd);
}

void CFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);    // important for MDI Children
	if (nType != SIZE_MINIMIZED)
		RecalcLayout();
}

void CFrameWnd::OnDestroy( )
{
   // main window is being destroyed...make sure we can't refer to it
   ptrToTheApp->m_pMainWnd = NULL;
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

void CFrameWnd::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
//	AfxCancelModes(m_hWnd);

	if (bSysMenu)
		return;     // don't support system menu

	ENSURE_VALID(pMenu);
	
	// check the enabled state of various menu items

	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
//	ASSERT(state.m_pParentMenu == NULL);

//	// determine if menu is popup in top-level menu and set m_pOther to
//	//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
//	HMENU hParentMenu;
//	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
//		state.m_pParentMenu = pMenu;    // parent == child for tracking popup
//	else if ((hParentMenu = (m_dwMenuBarState == AFX_MBS_VISIBLE) ? ::GetMenu(m_hWnd) : m_hMenu) != NULL)
//	{
//		CWnd* pParent = GetTopLevelParent();
//			// child windows don't have menus -- need to go to the top!
//		if (pParent != NULL &&
//			(hParentMenu = pParent->GetMenu()->GetSafeHmenu()) != NULL)
//		{
//			int nIndexMax = ::GetMenuItemCount(hParentMenu);
//			for (int nItemIndex = 0; nItemIndex < nIndexMax; nItemIndex++)
//			{
//				if (::GetSubMenu(hParentMenu, nItemIndex) == pMenu->m_hMenu)
//				{
//					// when popup is found, m_pParentMenu is containing menu
//					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
//					break;
//				}
//			}
//		}
//	}

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(this, FALSE);    // popups are never auto disabled
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, m_bAutoMenuEnable && state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void CFrameWnd::OnClose()
{
   // Note: only queries the active document
	CDocument* pDocument = GetActiveDocument();
	if (pDocument != NULL && !pDocument->CanCloseFrame(this))
	{
		// document can't close right now -- don't close it
		return;
	}
	CWinApp* pApp = AfxGetApp();
	if (pApp != NULL && pApp->m_pMainWnd == this)
	{
      pDocument->OnCloseDocument();
   }
}

IMPLEMENT_DYNAMIC(CView,CWnd)

BEGIN_MESSAGE_MAP(CView,CWnd)
   ON_MESSAGE(WM_INITIALUPDATE,OnInitialUpdate)
   ON_WM_SETFOCUS()
   ON_WM_KILLFOCUS()
   ON_WM_PAINT()
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CView::CView()
   : m_pDocument(NULL)
{
}

CView::~CView()
{
}

bool CView::eventFilter(QObject *object, QEvent *event)
{
   if ( object == _qt )
   {
      if ( event->type() == QEvent::Close )
      {
         closeEvent(dynamic_cast<QCloseEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::Show )
      {
         showEvent(dynamic_cast<QShowEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::Hide )
      {
         hideEvent(dynamic_cast<QHideEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::Move )
      {
         moveEvent(dynamic_cast<QMoveEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::Paint )
      {
         paintEvent(dynamic_cast<QPaintEvent*>(event));
         return false; // always propagate
      }
      if ( event->type() == QEvent::FocusIn )
      {
         focusInEvent(dynamic_cast<QFocusEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::FocusOut )
      {
         focusOutEvent(dynamic_cast<QFocusEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::Leave )
      {
         leaveEvent(event);
         return event->isAccepted();
      }
      if ( event->type() == QEvent::MouseButtonPress )
      {
         mousePressEvent(dynamic_cast<QMouseEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::MouseButtonRelease )
      {
         mouseReleaseEvent(dynamic_cast<QMouseEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::MouseButtonDblClick )
      {
         mouseDoubleClickEvent(dynamic_cast<QMouseEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::MouseMove )
      {
         mouseMoveEvent(dynamic_cast<QMouseEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::Wheel )
      {
         wheelEvent(dynamic_cast<QWheelEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::Resize )
      {
         resizeEvent(dynamic_cast<QResizeEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::KeyPress )
      {
         keyPressEvent(dynamic_cast<QKeyEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::KeyRelease )
      {
         keyReleaseEvent(dynamic_cast<QKeyEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::ContextMenu )
      {
         contextMenuEvent(dynamic_cast<QContextMenuEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::DragEnter )
      {
         dragEnterEvent(dynamic_cast<QDragEnterEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::DragMove )
      {
         dragMoveEvent(dynamic_cast<QDragMoveEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::Drop )
      {
         dropEvent(dynamic_cast<QDropEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::DragLeave )
      {
         dragLeaveEvent(dynamic_cast<QDragLeaveEvent*>(event));
         return event->isAccepted();
      }
      if ( event->type() == QEvent::Timer )
      {
         timerEvent(dynamic_cast<QTimerEvent*>(event));
         return event->isAccepted();
      }
   }
//   qDebug("eventFilter: unhandled %d object %s", event->type(), object->objectName().toLatin1().constData());
   return false;
}

void CView::resizeEvent(QResizeEvent *event)
{
   CRect rect(0,0,event->size().width(),event->size().height());
   rect.InflateRect(0,0,2*GetSystemMetrics(SM_CXEDGE),2*GetSystemMetrics(SM_CYEDGE));
//   qDebug("cv size %dx%d",event->size().width(),event->size().height());
   CalcWindowRect(&rect);
}

void CView::focusInEvent(QFocusEvent *event)
{
   PostMessage(WM_SETFOCUS,(WPARAM)(HWND)focusWnd);
}

void CView::showEvent(QShowEvent *event)
{
   SetFocus();
}

CWnd* CView::SetFocus()
{
   CWnd* pWnd = focusWnd;
   _qt->setFocus();
   return pWnd;
}

void CView::OnSetFocus(CWnd *)
{
   _qt->setFocus(); 
   focusWnd = this;
}

void CView::OnPaint()
{
   CPaintDC dc(this);
   dc.attach(_qt,this);
   OnDraw(&dc);
}

BOOL CView::Create( 
   LPCTSTR lpszClassName, 
   LPCTSTR lpszWindowName, 
   DWORD dwStyle, 
   const RECT& rect, 
   CWnd* pParentWnd, 
   LPCTSTR lpszMenuName, 
   DWORD dwExStyle, 
   CCreateContext* pContext
)
{    
   if ( !CWnd::Create(lpszClassName,lpszWindowName,dwStyle,rect,pParentWnd,lpszMenuName,dwExStyle,pContext) )
      return FALSE;

   _id = AFX_IDW_PANE_FIRST;
   
   // Add document to view.
   m_pDocument = pContext->m_pCurrentDoc;
   
   // Add view to document.
   m_pDocument->AddView(this);
   
   // Set parent frame.
   m_pFrameWnd = pContext->m_pCurrentFrame;
   
   _qtd->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
   
   qtToMfcWindow.insert(_qtd,this);

   m_pFrameWnd->addView(toQWidget());

   _qtd->setAttribute(Qt::WA_OpaquePaintEvent);
   
   return TRUE;
}

BOOL CView::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// first pump through pane
	if (CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// then pump through document
	if (m_pDocument != NULL)
	{
		// special state for saving view before routing to document
// CP: probably don't need this...
//		CPushRoutingView push(this);
		return m_pDocument->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	}

	return FALSE;
}

IMPLEMENT_DYNAMIC(CControlBar,CWnd)

BEGIN_MESSAGE_MAP(CControlBar,CWnd)
   ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

LRESULT CControlBar::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   ASSERT_VALID(this);

	LRESULT lResult;
	switch (nMsg)
	{
	case WM_NOTIFY:
	case WM_COMMAND:
	case WM_DRAWITEM:
	case WM_MEASUREITEM:
	case WM_DELETEITEM:
	case WM_COMPAREITEM:
	case WM_VKEYTOITEM:
	case WM_CHARTOITEM:
		// send these messages to the owner if not handled
		if (OnWndMsg(nMsg, wParam, lParam, &lResult))
			return lResult;
		else
		{

//			if (m_pInPlaceOwner && nMsg == WM_COMMAND)
//				lResult = m_pInPlaceOwner->SendMessage(nMsg, wParam, lParam);
//			else
				lResult = GetOwner()->SendMessage(nMsg, wParam, lParam);

//			// special case for TTN_NEEDTEXTA and TTN_NEEDTEXTW
//			if(nMsg == WM_NOTIFY)
//			{
//				NMHDR* pNMHDR = (NMHDR*)lParam;
//				if (pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW)
//				{
//					TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
//					TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

//					if (pNMHDR->code == TTN_NEEDTEXTA)
//					{
//						if (pTTTA->hinst == 0 && (!pTTTA->lpszText || !*pTTTA->lpszText))
//						{
//							// not handled by owner, so let bar itself handle it
//							lResult = CWnd::WindowProc(nMsg, wParam, lParam);
//						}
//					} else if (pNMHDR->code == TTN_NEEDTEXTW)
//					{
//						if (pTTTW->hinst == 0 && (!pTTTW->lpszText || !*pTTTW->lpszText))
//						{
//							// not handled by owner, so let bar itself handle it
//							lResult = CWnd::WindowProc(nMsg, wParam, lParam);
//						}
//					}
//				}
//			}
			return lResult;
		}
	}

	// otherwise, just handle in default way
	lResult = CWnd::WindowProc(nMsg, wParam, lParam);
	return lResult;
}

LRESULT CControlBar::OnIdleUpdateCmdUI(WPARAM wParam,LPARAM)
{
   // the style must be visible and if it is docked
	// the dockbar style must also be visible
	if ((GetStyle() & WS_VISIBLE) /*&&
		(m_pDockBar == NULL || (m_pDockBar->GetStyle() & WS_VISIBLE))*/)
	{
		CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
		if (pTarget == NULL || !pTarget->IsFrameWnd())
			pTarget = GetParentFrame();
      if (pTarget != NULL)
         OnUpdateCmdUI(pTarget, (BOOL)wParam);
	}
	return 0L;
}

LRESULT CControlBar::OnSizeParent(WPARAM, LPARAM lParam)
{
   AFX_SIZEPARENTPARAMS* pLayout = (AFX_SIZEPARENTPARAMS*)lParam;

   if ( _qt->isVisible() )
   {
      if ( _dwStyle&CBRS_TOP )
      {
         pLayout->rect.top += m_sizeDefault.cy;
         pLayout->sizeTotal.cx = (pLayout->rect.right-pLayout->rect.left)+1;
         pLayout->sizeTotal.cy += m_sizeDefault.cy;
      }
      else if ( _dwStyle&CBRS_LEFT )
      {
         pLayout->rect.left += m_sizeDefault.cx;
         pLayout->sizeTotal.cx += m_sizeDefault.cx;
         pLayout->sizeTotal.cy = (pLayout->rect.bottom-pLayout->rect.top)+1;
      }
      else if ( _dwStyle&CBRS_BOTTOM )
      {
         pLayout->rect.bottom -= m_sizeDefault.cy;
         pLayout->sizeTotal.cx = (pLayout->rect.right-pLayout->rect.left)+1;
         pLayout->sizeTotal.cy += m_sizeDefault.cy;
      }
      else if ( _dwStyle&CBRS_RIGHT )
      {
         pLayout->rect.right -= m_sizeDefault.cx;
         pLayout->sizeTotal.cx += m_sizeDefault.cx;
         pLayout->sizeTotal.cy = (pLayout->rect.bottom-pLayout->rect.top)+1;
      }
   }
   return 0;
}

CSize CControlBar::CalcFixedLayout(
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

IMPLEMENT_DYNAMIC(CReBarCtrl,CWnd)

BEGIN_MESSAGE_MAP(CReBarCtrl,CWnd)
END_MESSAGE_MAP()

void CReBarCtrl::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CReBarCtrl*>(this));
   widget->setParent(NULL);
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

   _qtd->setStyleSheet("QToolBar {"
                          "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                          "stop: 0 #f0f0f0, stop: .4 #ffffff, stop: 1 #ababab);"
                          "border-color: #3f3f3f;"
                       "}");
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif

   _qtd->setMouseTracking(true);
   _qtd->setMovable(false);
   CRect clientRect;
   pParentWnd->GetClientRect(&clientRect);
   _qtd->setGeometry(clientRect.left,clientRect.top,(clientRect.right-clientRect.left),(clientRect.bottom-clientRect.top));

   pParentWnd->mfcToQtWidgetMap()->insert(nID,this);
   
   qtToMfcWindow.insert(_qtd,this);
   
   return TRUE;
}

BOOL CReBarCtrl::InsertBand(
   UINT uIndex,
   REBARBANDINFO* prbbi
)
{
   CWnd* pWnd = (CWnd*)prbbi->hwndChild;
   CToolBar* pToolBar = dynamic_cast<CToolBar*>(pWnd);
   if ( _qtd->actions().count() )
   {
      _qtd->addSeparator();
   }
   if ( pToolBar )
   {
      QToolBar* toolBar = dynamic_cast<QToolBar*>(pWnd->toQWidget());
      _qtd->addActions(toolBar->actions());
      _qtd->setIconSize(toolBar->iconSize());
      pWnd->toQWidget()->setVisible(false);
      
      mfcToQtWidgetMap()->insert(pWnd->GetDlgCtrlID(),pWnd);   
   }
   else
   {
      _qtd->addWidget(pWnd->toQWidget());
      pWnd->toQWidget()->setMinimumWidth(1);
      pWnd->toQWidget()->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
   }
   return TRUE;
}

void CReBarCtrl::MinimizeBand(
   UINT uBand
)
{
   // CP: This API doesn't need to do much, I think.
}

void CReBarCtrl::toolBarAction_triggered()
{
   SendMessage(WM_COMMAND,qobject_cast<QAction*>(sender())->data().toInt());
}

IMPLEMENT_DYNAMIC(CReBar,CControlBar)

BEGIN_MESSAGE_MAP(CReBar,CControlBar)
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

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
   m_pReBarCtrl->Create(dwStyle,rect,this,nID);
   SetParent(pParentWnd);   

   if ( !backgroundedFamiTracker )
   {
      ptrToTheApp->qtMainWindow->addToolBar(dynamic_cast<QToolBar*>(m_pReBarCtrl->toQWidget()));
   }

   return TRUE;
}

LRESULT CReBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// special handling for certain messages (forwarding to owner/parent)
	switch (message)
	{
	case WM_POPMESSAGESTRING:
	case WM_SETMESSAGESTRING:
		{
			CWnd* pOwner=GetOwner();
			ENSURE(pOwner);
			return pOwner->SendMessage(message, wParam, lParam);
		}
	}
	return CControlBar::WindowProc(message, wParam, lParam);
}

IMPLEMENT_DYNAMIC(CToolBar,CControlBar)

BEGIN_MESSAGE_MAP(CToolBar,CControlBar)
END_MESSAGE_MAP()

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

   _qtd->setMouseTracking(true);
   _qtd->setMovable(false);
}

CToolBar::~CToolBar()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CToolBar::subclassWidget(int nID,CWnd* widget)
{
   Create(widget->GetParent(),widget->GetStyle(),nID);
   _qt->installEventFilter(dynamic_cast<CToolBar*>(this));
   widget->setParent(NULL);
}

BOOL CToolBar::Create( 
   CWnd* pParentWnd, 
   DWORD dwStyle, 
   UINT nID
)
{
   return CreateEx(pParentWnd,TBSTYLE_FLAT,dwStyle,CRect(0,0,0,0),nID);
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
   
   SetParent(pParentWnd);

   return TRUE;
}

BOOL CToolBar::LoadToolBar(
   UINT nIDResource
)
{
   qtMfcInitToolBarResource(nIDResource,this);
   return TRUE;
}

UINT CToolBar::GetButtonStyle(
   int nIndex
)
{
//   qDebug("implement CToolBar::GetButtonStyle.");
   return 0;
}

void CToolBar::SetButtonStyle(
   int nIndex,
   UINT nStyle
)
{
   QAction* cur;
   QMenu* menu;
   QList<QAction*> actions;
   UINT type = nStyle&0xff;
   
   if ( type == TBBS_DROPDOWN )
   {
      actions = _qtd->actions();
      cur = actions.at(nIndex);
      menu = new QMenu;
      cur->setMenu(menu);
      QObject::connect(menu,SIGNAL(aboutToShow()),this,SLOT(menu_aboutToShow()));
   }
   if ( nStyle&TBBS_DISABLED )
   {
      actions = _qtd->actions();
      cur = actions.at(nIndex);
      cur->setEnabled(false);
   }
   else
   {
      actions = _qtd->actions();
      cur = actions.at(nIndex);
      cur->setEnabled(true);
   }
}

void CToolBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CToolCmdUI state;
	state.m_pOther = this;

//	state.m_nIndexMax = (UINT)DefWindowProc(TB_BUTTONCOUNT, 0, 0);
   state.m_nIndexMax = _qtd->actions().count();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
	{
//		// get buttons state
//		TBBUTTON button;
//		_GetButton(state.m_nIndex, &button);
//		state.m_nID = button.idCommand;
      state.m_nID = _qtd->actions().at(state.m_nIndex)->data().toInt();

		// ignore separators
//		if (!(button.fsStyle & TBSTYLE_SEP))
      if ( !_qtd->actions().at(state.m_nIndex)->isSeparator() )
		{
			// allow reflections
			if (CWnd::OnCmdMsg(0, 
				MAKELONG(CN_UPDATE_COMMAND_UI&0xffff, WM_COMMAND+WM_REFLECT_BASE), 
				&state, NULL))
				continue;

			// allow the toolbar itself to have update handlers
			if (CWnd::OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
				continue;

			// allow the owner to process the update
			state.DoUpdate(pTarget, bDisableIfNoHndler);
		}
	}

	// update the dialog controls added to the toolbar
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

void CToolBar::toolBarAction_triggered()
{
   GetOwner()->SendMessage(WM_COMMAND,qobject_cast<QAction*>(sender())->data().toInt());
}

void CToolBar::menu_aboutToShow()
{
   NMTOOLBAR nmtb;
   LRESULT result;

   QList<QAction*> actions = _qtd->actions();
   QAction* origin = NULL;
   foreach ( QAction* action, actions )
   {
      if ( action->menu() == qobject_cast<QMenu*>(sender()) )
      {
         origin = action;
      }
   }

   if ( origin )
   {
      nmtb.hdr.hwndFrom = m_hWnd;
      nmtb.hdr.idFrom = _id;
      nmtb.hdr.code = TBN_DROPDOWN;
      nmtb.iItem = origin->data().toInt();
      
      GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmtb);
   }
}

IMPLEMENT_DYNAMIC(CStatusBar,CControlBar)

BEGIN_MESSAGE_MAP(CStatusBar,CControlBar)
END_MESSAGE_MAP()

CStatusBar::CStatusBar(CWnd* parent)
{
   _dwStyle = 0;
}

CStatusBar::~CStatusBar()
{
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

   // Pass-through signals

   return TRUE;
}

void CStatusBar::SetWindowText(
   LPCTSTR lpszString
)
{
   if ( !backgroundedFamiTracker )
   {
#if UNICODE
   ptrToTheApp->qtMainWindow->statusBar()->showMessage(QString::fromWCharArray(lpszString));
#else
   ptrToTheApp->qtMainWindow->statusBar()->showMessage(QString::fromLatin1(lpszString));
#endif
   }
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
      newPane->Create(qtMfcStringResource(lpIDArray[pane]),WS_VISIBLE,CRect(0,0,0,0),NULL,lpIDArray[pane]);
      _panes.insert(pane,newPane);

      if ( lpIDArray[pane] != ID_SEPARATOR )
      {
         mfcToQtWidget.insert(lpIDArray[pane],newPane);
      }
      if ( !backgroundedFamiTracker )
      {
         ptrToTheApp->qtMainWindow->statusBar()->addPermanentWidget(newPane->toQWidget());
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
      ((QLabel*)pane->toQWidget())->setText(QString::fromLatin1(lpszNewText));
#endif
      return TRUE;
   }
   return FALSE;
}

void CStatusBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
   CStatusCmdUI state;
	state.m_pOther = this;
   state.m_nIndexMax = (UINT)_panes.count();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = _panes.value(state.m_nIndex)->GetDlgCtrlID();

		// allow the statusbar itself to have update handlers
		if (CWnd::OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
			continue;

		// allow target (owner) to handle the remaining updates
		state.DoUpdate(pTarget, FALSE);
	}

	// update the dialog controls added to the status bar
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

IMPLEMENT_DYNAMIC(CDialogBar,CControlBar)

BEGIN_MESSAGE_MAP(CDialogBar,CControlBar)
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

CDialogBar::CDialogBar()
{
   _mfcd = new CDialog;
   // CP: If I don't do this the widget isn't visible...
   _mfcd->setGeometry(0,0,10,10);
}

CDialogBar::~CDialogBar()
{
   if ( _mfcd )
      delete _mfcd;
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

   _dwStyle = nStyle;

   _mfcd->Create(nIDTemplate,this);

   SetParent(pParentWnd);

   // This is a container.
   foreach ( UINT key, _mfcd->mfcToQtWidgetMap()->keys() )
   {
      mfcToQtWidget.insert(key,_mfcd->mfcToQtWidgetMap()->value(key));
   }
   foreach ( CWnd* pWnd, mfcToQtWidget )
   {
      pWnd->SetOwner(this);
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
   
   CRect rect;
	GetWindowRect(&rect);
   m_sizeDefault = rect.Size();//CSize(_mfcd->rect().size());    // set fixed size
   
   ShowWindow(SW_SHOW);

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

void CDialogBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

void QDialog_MFC::keyPressEvent(QKeyEvent *event)
{
   QDialog::keyPressEvent(event);
}

void QDialog_MFC::keyReleaseEvent(QKeyEvent *event)
{
   QDialog::keyReleaseEvent(event);
}

IMPLEMENT_DYNAMIC(CDialog,CWnd)

BEGIN_MESSAGE_MAP(CDialog,CWnd)
   ON_BN_CLICKED(IDOK,OnOK)
   ON_BN_CLICKED(IDCANCEL,OnCancel)
END_MESSAGE_MAP()

CDialog::CDialog()
{
   m_hWnd = (HWND)this;

   if ( _qt )
      delete _qt;

   _grid = NULL;

   _qt = new QDialog_MFC;

   _qtd = dynamic_cast<QDialog_MFC*>(_qt);
   _inited = false;

   _qtd->setMouseTracking(true);
   _qtd->installEventFilter(this);

   // Pass-through signals
   QObject::connect(this,SIGNAL(update()),_qtd,SLOT(update()));
}

CDialog::CDialog(int dlgID, CWnd *parent)
   : CWnd(parent)
{
   m_hWnd = (HWND)this;
   _id = dlgID;

   if ( _qt )
      delete _qt;

   _grid = NULL;

   if ( parent )
      _qt = new QDialog_MFC(parent);
   else
      _qt = new QDialog_MFC;

   _qtd = dynamic_cast<QDialog_MFC*>(_qt);
   _inited = false;
   _id = dlgID;

   _qtd->setMouseTracking(true);
   _qtd->installEventFilter(this);

   // Pass-through signals
   QObject::connect(this,SIGNAL(update()),_qtd,SLOT(update()));
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

void CDialog::closeEvent(QCloseEvent *event)
{
   SendMessage(WM_COMMAND,IDCANCEL);
   CWnd::closeEvent(event);
}

BOOL CDialog::Create(
   UINT nIDTemplate,
   CWnd* pParentWnd
)
{
   m_hWnd = (HWND)this;
   _id = nIDTemplate;

   qtMfcInitDialogResource(nIDTemplate,this);

   if ( !pParentWnd )
      pParentWnd = m_pFrameWnd;
   if ( pParentWnd )
      _qt->setParent(pParentWnd->toQWidget());
   else
      _qt->setParent(NULL);
   SetParent(pParentWnd);
   if ( pParentWnd == m_pFrameWnd )
   {
      _qtd->setWindowFlags(_qtd->windowFlags()|Qt::Dialog|Qt::WindowStaysOnTopHint);
   }
   else
   {
      pParentWnd->mfcToQtWidgetMap()->insertMulti(nIDTemplate,this);
   }
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif

   BOOL result = OnInitDialog();
   _inited = true;
   
   qtToMfcWindow.insert(_qtd,this);
   
   return result;
}

BOOL CDialog::PreTranslateMessage(MSG* pMsg)
{
   return CWnd::PreTranslateMessage(pMsg);
	// for modeless processing (or modal)
	ASSERT(m_hWnd != NULL);

	// allow tooltip messages to be filtered
//	if (CWnd::PreTranslateMessage(pMsg))
//		return TRUE;

	// don't translate dialog messages when in Shift+F1 help mode
//	CFrameWnd* pFrameWnd = GetTopLevelFrame();
//	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
//		return FALSE;

//	 fix around for VK_ESCAPE in a multiline Edit that is on a Dialog
//	 that doesn't have a cancel or the cancel is disabled.
//	if (pMsg->message == WM_KEYDOWN &&
//		(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL) &&
//		(::GetWindowLong(pMsg->hwnd, GWL_STYLE) & ES_MULTILINE) &&
//		_AfxCompareClassName(pMsg->hwnd, _T("Edit")))
//	{
//		HWND hItem = ::GetDlgItem(m_hWnd, IDCANCEL);
//		if (hItem == NULL || ::IsWindowEnabled(hItem))
//		{
//			SendMessage(WM_COMMAND, IDCANCEL, 0);
//			return TRUE;
//		}
//	}
//	// filter both messages to dialog and from children
//	return PreTranslateInput(pMsg);
}

BOOL CDialog::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

//   if ((nCode != CN_COMMAND && nCode != CN_UPDATE_COMMAND_UI) ||
//         !IS_COMMAND_ID(nID) || nID >= 0xf000)
//   {
//      // control notification or non-command button or system command
//      return FALSE;       // not routed any further
//   }

	// if we have an owner window, give it second crack
	CWnd* pOwner = GetParent();
	if (pOwner != NULL)
	{
//		TRACE(traceCmdRouting, 1, "Routing command id 0x%04X to owner window.\n", nID);

		ASSERT(pOwner != this);
		if (pOwner->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	}

	// last crack goes to the current CWinThread object
	CWinThread* pThread = AfxGetThread();
	if (pThread != NULL)
	{
//		TRACE(traceCmdRouting, 1, "Routing command id 0x%04X to app.\n", nID);

		if (pThread->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	}

//	TRACE(traceCmdRouting, 1, "IGNORING command id 0x%04X sent to %hs dialog.\n", nID,
//			GetRuntimeClass()->m_lpszClassName);

	return FALSE;
}

INT_PTR CDialog::DoModal()
{
   if ( !_inited )
   {
      qtMfcInitDialogResource(_id,this);
      
      if ( !GetParent() )
      {
         // CP: Don't call SetParent...it causes problems!
         // But an 'unparented' CDialog's parent is the CFrameWnd.
         m_pParentWnd = m_pFrameWnd;
      }

      OnInitDialog();
   }
   _inited = true;

   SetFocus();

   INT_PTR result = _qtd->exec();
   DestroyWindow();
   if ( result == QDialog::Accepted )
      return IDOK;
   else
      return IDCANCEL;
}

void CDialog::SetWindowText(
   LPCTSTR lpszString
)
{
#if UNICODE
   _qt->setWindowTitle(QString::fromWCharArray(lpszString));
#else
   _qt->setWindowTitle(QString::fromLatin1(lpszString));
#endif
}

void CDialog::ShowWindow(int code)
{
   switch ( code )
   {
   case SW_SHOW:
      _dwStyle |= WS_VISIBLE;
      _qtd->setVisible(true);
      _qtd->setFocus();
      break;
   case SW_HIDE:
      _dwStyle &= (~WS_VISIBLE);
      _qtd->setVisible(false);
      break;
   }
}

void CDialog::MapDialogRect(
   LPRECT lpRect
) const
{
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   QFontMetrics sysFontMetrics(QFont("MS Shell Dlg",8));
#else
   QFontMetrics sysFontMetrics(QFont("MS Shell Dlg",11));
#endif

//   int baseunitX = sysFontMetrics.averageCharWidth()+1;
   int baseunitX = 5+1;
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

IMPLEMENT_DYNAMIC(CCommonDialog,CDialog)

BEGIN_MESSAGE_MAP(CCommonDialog,CDialog)
END_MESSAGE_MAP()

CCommonDialog::CCommonDialog(CWnd *pParentWnd)
 : CDialog(0,pParentWnd)
{
}

CCommonDialog::~CCommonDialog()
{
}

IMPLEMENT_DYNCREATE(CWinThread,CCmdTarget)

BEGIN_MESSAGE_MAP(CWinThread,CCmdTarget)
END_MESSAGE_MAP()

CWinThread::CWinThread() : 
   _priority(QThread::NormalPriority),
   pTimer(NULL),
   pThread(NULL),
   _initialized(false)
{
   m_hThread = (HANDLE)this;
   m_pfnThreadProc = NULL;
   m_pParam = NULL;

   pThread = new QThread();

   m_pMainWnd = NULL;
}

CWinThread::~CWinThread()
{
   if ( pTimer )
   {
      QObject::disconnect(pTimer,SIGNAL(timeout()),this,SLOT(runSlot()));

      pTimer->stop();

      delete pTimer;
   }
}

bool CWinThread::event(QEvent *event)
{
   MFCMessageEvent* msgEvent = dynamic_cast<MFCMessageEvent*>(event);
   bool proc = false;
   if ( msgEvent )
   {
      // Terminator.
      if ( msgEvent->msg.message == WM_QUIT )
      {
         pThread->terminate();
         pThread->wait();
         return true;
      }

      MSG* pMsg = &msgEvent->msg;

      const AFX_MSGMAP* pMessageMap; pMessageMap = GetMessageMap();
      const AFX_MSGMAP_ENTRY* lpEntry;

      for (/* pMessageMap already init'ed */; pMessageMap->pfnGetBaseMap != NULL;
         pMessageMap = (*pMessageMap->pfnGetBaseMap)())
      {
         // Note: catch not so common but fatal mistake!!
         //      BEGIN_MESSAGE_MAP(CMyThread, CMyThread)

         ASSERT(pMessageMap != (*pMessageMap->pfnGetBaseMap)());
         if (pMsg->message < 0xC000)
         {
            // constant window message
            if ((lpEntry = AfxFindMessageEntry(pMessageMap->lpEntries,
               pMsg->message, 0, 0)) != NULL)
               goto LDispatch;
         }
         else
         {
            // registered windows message
            lpEntry = pMessageMap->lpEntries;
            while ((lpEntry = AfxFindMessageEntry(lpEntry, 0xC000, 0, 0)) != NULL)
            {
               UINT* pnID = (UINT*)(lpEntry->nSig);
               ASSERT(*pnID >= 0xC000);
                  // must be successfully registered
               if (*pnID == pMsg->message)
                  goto LDispatch;
               lpEntry++;      // keep looking past this one
            }
         }
      }
      return FALSE;

      LDispatch:
         union MessageMapFunctions mmf;
         mmf.pfn = lpEntry->pfn;

         // always posted, so return value is meaningless

         (this->*mmf.pfn_THREAD)(pMsg->wParam, pMsg->lParam);
         return TRUE;
   }
   return proc;
}

HANDLE WINAPI CreateThread(
   LPSECURITY_ATTRIBUTES  lpThreadAttributes,
   SIZE_T                 dwStackSize,
   LPTHREAD_START_ROUTINE lpStartAddress,
   LPVOID                 lpParameter,
   DWORD                  dwCreationFlags,
   LPDWORD                lpThreadId
)
{
    CWinThread* pThread = new CWinThread();

    pThread->m_pfnThreadProc = (AFX_THREADPROC)lpStartAddress;
    pThread->m_pParam = lpParameter;

    pThread->CreateThread(dwCreationFlags,dwStackSize,lpThreadAttributes);

//    (*lpThreadId) = pThread->m_nThreadID;

    return (HANDLE)pThread;
}

BOOL CWinThread::CreateThread(
   DWORD dwCreateFlags,
   UINT nStackSize,
   LPSECURITY_ATTRIBUTES lpSecurityAttrs
)
{
   m_pMainWnd = AfxGetMainWnd();

   pThread = new QThread();
   moveToThread(pThread);

   if ( !(dwCreateFlags&CREATE_SUSPENDED) )
   {      
      ResumeThread();
   }

   return TRUE;
}

DWORD CWinThread::ResumeThread( )
{
   if ( !_initialized )
   {
      InitInstance();

      pTimer = new QTimer;

      pTimer->start();
      pTimer->moveToThread(pThread);

      QObject::connect(pTimer,SIGNAL(timeout()),this,SLOT(runSlot()));

      _initialized = true;
   }
   if ( !pThread->isRunning() )
   {
      pThread->start();
      return 0;
   }
   else
   {
      return 1;
   }
}

BOOL CWinThread::SetThreadPriority(
   int nPriority
)
{
   switch ( nPriority )
   {
   case THREAD_PRIORITY_TIME_CRITICAL:
      _priority = QThread::TimeCriticalPriority;
      break;
   case THREAD_PRIORITY_HIGHEST:
      _priority = QThread::HighestPriority;
      break;
   case THREAD_PRIORITY_ABOVE_NORMAL:
      _priority = QThread::HighPriority;
      break;
   case THREAD_PRIORITY_NORMAL:
      _priority = QThread::NormalPriority;
      break;
   case THREAD_PRIORITY_BELOW_NORMAL:
      _priority = QThread::LowPriority;
      break;
   case THREAD_PRIORITY_LOWEST:
      _priority = QThread::LowestPriority;
      break;
   case THREAD_PRIORITY_IDLE:
      _priority = QThread::IdlePriority;
      break;
   }
   return TRUE;
}

BOOL CWinThread::PostThreadMessage(
   UINT message ,
   WPARAM wParam,
   LPARAM lParam
      )
{
   MFCMessageEvent* post = new MFCMessageEvent(QEvent::User);
   post->msg.message = message;
   post->msg.wParam = wParam;
   post->msg.lParam = lParam;

   QApplication::postEvent(this,post);

   return true;
}

void CWinThread::runSlot()
{
#ifdef Q_OS_MAC
      m_nThreadID = QThread::currentThreadId();
#else
//      m_nThreadID = QThread::currentThreadId();
#endif

   pThread->setPriority(_priority);
   Run();
}

int CWinThread::Run( )
{
   if ( m_pfnThreadProc )
   {
      m_pfnThreadProc(m_pParam);
      pTimer->stop();
      QObject::disconnect(pTimer,SIGNAL(timeout()),this,SLOT(runSlot()));
      m_pfnThreadProc = NULL;
      ExitInstance();
      return 0;
   }

   OnIdle(0);
   if ( pThread->thread() == QApplication::instance()->thread() )
   {
      if ( ptrToTheApp && ptrToTheApp->m_pMainWnd )
      {
         ptrToTheApp->m_pMainWnd->SendMessageToDescendants(WM_IDLEUPDATECMDUI,
                                                           (WPARAM)TRUE, 0, TRUE, TRUE);
      }
   }

   return 0;
}

CWinThread* AfxBeginThread( 
   AFX_THREADPROC pfnThreadProc, 
   LPVOID pParam, 
   int nPriority, 
   UINT nStackSize, 
   DWORD dwCreateFlags, 
   LPSECURITY_ATTRIBUTES lpSecurityAttrs
)
{
   CWinThread* pThread = new CWinThread();
   pThread->CreateThread(dwCreateFlags,nStackSize,lpSecurityAttrs);
   
   pThread->m_pfnThreadProc = pfnThreadProc;
   pThread->m_pParam = pParam;
}

CWinThread* AfxBeginThread( 
   CRuntimeClass* pThreadClass, 
   int nPriority, 
   UINT nStackSize, 
   DWORD dwCreateFlags, 
   LPSECURITY_ATTRIBUTES lpSecurityAttrs  
)
{
   CWinThread* pThread = (CWinThread*)pThreadClass->CreateObject();
   pThread->CreateThread(dwCreateFlags,nStackSize,lpSecurityAttrs);
}

IMPLEMENT_DYNCREATE(CDocument,CCmdTarget)

BEGIN_MESSAGE_MAP(CDocument,CCmdTarget)
END_MESSAGE_MAP()

CDocument::CDocument()
   : m_pDocTemplate(NULL), m_bModified(FALSE), m_bAutoDelete(TRUE)
{
   QObject::connect(this,SIGNAL(documentSaved()),ptrToTheApp->qtMainWindow,SLOT(documentSaved()));
   QObject::connect(this,SIGNAL(documentClosed()),ptrToTheApp->qtMainWindow,SLOT(documentClosed()));
}

void CDocument::OnCloseDocument()
{ 
   emit documentClosed(); 
   ptrToTheApp->GetMainWnd()->DestroyWindow();
}

void CDocument::UpdateAllViews(CView* pSender, LPARAM lHint, CObject* pHint)
{
   ASSERT(pSender == NULL || !_views.count());
		// must have views if sent by one of them

	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		ASSERT_VALID(pView);
		if (pView != pSender)
      {
			pView->OnUpdate(pSender, lHint, pHint);
      }
	}
}

void CDocument::AddView( 
   CView* pView  
)
{
   _views.append(pView);
}

void CDocument::SetTitle(LPCTSTR title )
{
   m_strTitle = title;
}

void CDocument::OnFileSave()
{
   DoFileSave();
}

void CDocument::OnFileSaveAs()
{
   DoSave(NULL);
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

CView* CDocument::GetNextView( 
   POSITION& rPosition  
) const
{
   if ( !rPosition )
   {
      return NULL; // Choker for end-of-list
   }
   CView* pView = _views.at((*rPosition)++);
   if ( (*rPosition) >= _views.count() )
   {
      delete rPosition;
      rPosition = NULL;
   }
   return pView;
}

void CDocument::SetPathName(
   LPCTSTR lpszPathName,
   BOOL bAddToMRU
)
{
   m_strPathName = lpszPathName;
   
   QFileInfo fileInfo(lpszPathName);
   CString str = fileInfo.fileName();
   SetTitle(str);
   
   if ( bAddToMRU )
   {
      ptrToTheApp->AddToRecentFileList(lpszPathName);
   }
}

BOOL CDocument::CanCloseFrame(
   CFrameWnd* pFrame
)
{
	return SaveModified();
}

BOOL CDocument::DoFileSave()
{
	DWORD dwAttrib = GetFileAttributes(m_strPathName);
	if (dwAttrib & FILE_ATTRIBUTE_READONLY)
	{
		// we do not have read-write access or the file does not (now) exist
		if (!DoSave(NULL))
		{
			return FALSE;
		}
	}
	else
	{
		if (!DoSave(m_strPathName))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CDocument::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{
	CString newName = lpszPathName;

	if (newName.IsEmpty())
	{
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

		newName = m_strPathName;
		if (bReplace && newName.IsEmpty())
		{
			newName = m_strTitle;
			// check for dubious filename
			int iBad = newName.FindOneOf(_T(":/\\"));
			if (iBad != -1)
				newName.ReleaseBuffer(iBad);

//			if (AfxGetApp() && AfxGetApp()->GetDataRecoveryHandler())
//			{
//				// remove "[recovered]" from the title if it exists
//				CString strNormalTitle = AfxGetApp()->GetDataRecoveryHandler()->GetNormalDocumentTitle(this);
//				if (!strNormalTitle.IsEmpty())
//					newName = strNormalTitle;
//			}

			// append the default suffix if there is one
			CString strExt;
			if (pTemplate->GetDocString(strExt, CDocTemplate::filterExt) && !strExt.IsEmpty())
			{
				ASSERT(strExt[0] == '.');
				int iStart = 0;
				newName += strExt.Tokenize(_T(";"), iStart);
			}
		}

		if (!AfxGetApp()->DoPromptFileName(newName,
		  bReplace ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY,
		  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, pTemplate))
			return FALSE;       // don't even attempt to save
	}

//	CWaitCursor wait;

	if (!OnSaveDocument(newName))
	{
		if (lpszPathName == NULL)
		{
//			// be sure to delete the file
//			TRY
//			{
				CFile::Remove(newName);
//			}
//			CATCH_ALL(e)
//			{
//				DELETE_EXCEPTION(e);
//			}
//			END_CATCH_ALL
		}
		return FALSE;
	}

	// reset the title and change the document name
	if (bReplace)
	{
		SetPathName(newName);
//		OnDocumentEvent(onAfterSaveDocument);
	}

	return TRUE;        // success
}

BOOL CDocument::SaveModified()
{
	if (!IsModified())
		return TRUE;        // ok to continue

//	CDataRecoveryHandler *pHandler = NULL;
//	if (AfxGetApp())
//	{
//		// if close is triggered by the restart manager, the file
//		// will be auto-saved and a prompt for save is not permitted.
//		pHandler = AfxGetApp()->GetDataRecoveryHandler();
//		if (pHandler != NULL)
//		{
//			if (pHandler->GetShutdownByRestartManager())
//				return TRUE;
//		}
//	}

	// get name/title of document
	CString name;
	if (m_strPathName.IsEmpty())
	{
		// get name based on caption
		name = m_strTitle;

//		if (pHandler != NULL)
//		{
//			// remove "[recovered]" from the title if it exists
//			CString strNormalTitle = pHandler->GetNormalDocumentTitle(this);
//			if (!strNormalTitle.IsEmpty())
//				name = strNormalTitle;
//		}

		if (name.IsEmpty())
			ENSURE(name.LoadString(AFX_IDS_UNTITLED));
	}
	else
	{
		// get name based on file title of path name
		name = m_strPathName;
		AfxGetFileTitle(m_strPathName, name.GetBuffer(_MAX_PATH), _MAX_PATH);
		name.ReleaseBuffer();
	}

	CString prompt;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
   
   // CP: HACK to get rid of unsupportable 'cancel' option in IDEified FamiTracker
   UINT nType = MB_YESNOCANCEL;
   if ( ideifiedFamiTracker )
   {
      nType = MB_YESNO;
   }
   
	switch (AfxMessageBox(prompt, nType, AFX_IDP_ASK_TO_SAVE))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoFileSave())
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}

IMPLEMENT_DYNAMIC(CDocTemplate,CCmdTarget)

BEGIN_MESSAGE_MAP(CDocTemplate,CCmdTarget)
END_MESSAGE_MAP()

CDocTemplate::CDocTemplate(UINT nIDResource,CRuntimeClass* pDocClass,CRuntimeClass* pFrameClass,CRuntimeClass* pViewClass)
{
   m_nIDResource = nIDResource;
   m_pDocClass = pDocClass;
   m_pFrameClass = pFrameClass;
   m_pViewClass = pViewClass;
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
   BOOL ret = TRUE;
   switch ( index )
   {
   case windowTitle:
      if ( backgroundedFamiTracker )
      {
         rString = " - FamiPlayer";
      }
      else
      {
         rString = " - FamiTracker";
      }
      break;

   case docName:
      rString = "Untitled";
      break;

   case fileNewName:
      rString = "Untitled";
      break;

   case filterName:
      rString = "FamiTracker files|";
      break;

   case filterExt:
      rString = ".ftm|";
      break;

   case regFileTypeId:
      break;

   case regFileTypeName:
      break;

   default:
      ret = FALSE;
      break;
   }
   return ret;
}

void CDocTemplate::AddDocument(CDocument* pDoc)
{
	ASSERT_VALID(pDoc);
	ASSERT(pDoc->m_pDocTemplate == NULL);   // no template attached yet
	pDoc->m_pDocTemplate = this;
}

void CDocTemplate::RemoveDocument(CDocument* pDoc)
{
	ASSERT_VALID(pDoc);
	ASSERT(pDoc->m_pDocTemplate == this);   // must be attached to us
	pDoc->m_pDocTemplate = NULL;
}

CDocument* CDocTemplate::CreateNewDocument()
{
	// default implementation constructs one from CRuntimeClass
	if (m_pDocClass == NULL)
	{
//		TRACE(traceAppMsg, 0, "Error: you must override CDocTemplate::CreateNewDocument.\n");
		ASSERT(FALSE);
		return NULL;
	}
	CDocument* pDocument = (CDocument*)m_pDocClass->CreateObject();
	if (pDocument == NULL)
	{
//		TRACE(traceAppMsg, 0, "Warning: Dynamic create of document type %hs failed.\n",
//			m_pDocClass->m_lpszClassName);
		return NULL;
	}
	ASSERT_KINDOF(CDocument, pDocument);
	AddDocument(pDocument);
	return pDocument;
}

CFrameWnd* CDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther)
{
	if (pDoc != NULL)
		ASSERT_VALID(pDoc);
	// create a frame wired to the specified document

	ASSERT(m_nIDResource != 0); // must have a resource ID to load from
	CCreateContext context;
	context.m_pCurrentFrame = pOther;
	context.m_pCurrentDoc = pDoc;
	context.m_pNewViewClass = m_pViewClass;
	context.m_pNewDocTemplate = this;

	if (m_pFrameClass == NULL)
	{
//		TRACE(traceAppMsg, 0, "Error: you must override CDocTemplate::CreateNewFrame.\n");
		ASSERT(FALSE);
		return NULL;
	}
	CFrameWnd* pFrame = (CFrameWnd*)m_pFrameClass->CreateObject();
	if (pFrame == NULL)
	{
//		TRACE(traceAppMsg, 0, "Warning: Dynamic create of frame %hs failed.\n",
//			m_pFrameClass->m_lpszClassName);
		return NULL;
	}
	ASSERT_KINDOF(CFrameWnd, pFrame);

//	if (context.m_pNewViewClass == NULL)
//		TRACE(traceAppMsg, 0, "Warning: creating frame with no default view.\n");

	// create new from resource
	if (!pFrame->LoadFrame(m_nIDResource,
			WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,   // default frame styles
			NULL, &context))
	{
//		TRACE(traceAppMsg, 0, "Warning: CDocTemplate couldn't create a frame.\n");
		// frame will be deleted in PostNcDestroy cleanup
		return NULL;
	}

	// it worked !
	return pFrame;
}

IMPLEMENT_DYNAMIC(CSingleDocTemplate,CDocTemplate)

BEGIN_MESSAGE_MAP(CSingleDocTemplate,CDocTemplate)
END_MESSAGE_MAP()

CSingleDocTemplate::CSingleDocTemplate(UINT f,CRuntimeClass* pDocClass,CRuntimeClass* pFrameClass,CRuntimeClass* pViewClass)
   : CDocTemplate(f,pDocClass,pFrameClass,pViewClass)
{
   m_pOnlyDoc = NULL;
}

void CSingleDocTemplate::SetDefaultTitle( 
   CDocument* pDocument  
)
{
   CString title;
   GetDocString(title,docName);
   pDocument->SetTitle(title);
}

void CSingleDocTemplate::AddDocument(CDocument* pDoc)
{
	ASSERT(m_pOnlyDoc == NULL);     // one at a time please
	ASSERT_VALID(pDoc);

	CDocTemplate::AddDocument(pDoc);
	m_pOnlyDoc = pDoc;
}

void CSingleDocTemplate::RemoveDocument(CDocument* pDoc)
{
	ASSERT(m_pOnlyDoc == pDoc);     // must be this one
	ASSERT_VALID(pDoc);

	CDocTemplate::RemoveDocument(pDoc);
	m_pOnlyDoc = NULL;
}

POSITION CSingleDocTemplate::GetFirstDocPosition( ) const
{
   POSITION pos = NULL;

   if ( m_pOnlyDoc )
   {
      pos = new int;
      (*pos) = 0;
   }
   return pos;
}

CDocument* CSingleDocTemplate::GetNextDoc(
   POSITION& rPos
) const
{
   if ( !rPos )
   {
      return NULL; // Choker for end-of-list
   }
   CDocument* pDoc = m_pOnlyDoc;
   delete rPos;
   rPos = NULL;

   return pDoc;
}

CDocument* CSingleDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible)
{
	return OpenDocumentFile(lpszPathName, TRUE, bMakeVisible);
}

CDocument* CSingleDocTemplate::OpenDocumentFile(
   LPCTSTR lpszPathName,
   BOOL bAddToMRU,
   BOOL bMakeVisible
)
{
   CDocument* pDocument = NULL;
	CFrameWnd* pFrame = NULL;
	BOOL bCreated = FALSE;      // => doc and frame created
	BOOL bWasModified = FALSE;

	if (m_pOnlyDoc != NULL)
	{
		// already have a document - reinit it
        pDocument = m_pOnlyDoc;
		if (!pDocument->SaveModified())
		{
            // set a flag to indicate that the document being opened should not
			// be removed from the MRU list, if it was being opened from there
//			g_bRemoveFromMRU = FALSE;
			return NULL;        // leave the original one
		}

        pFrame = (CFrameWnd*)AfxGetMainWnd();
		ASSERT(pFrame != NULL);
		ASSERT_KINDOF(CFrameWnd, pFrame);
		ASSERT_VALID(pFrame);
    }
	else
	{
		// create a new document
        pDocument = CreateNewDocument();
		ASSERT(pFrame == NULL);     // will be created below
        bCreated = TRUE;
	}

    if (pDocument == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return NULL;
	}
	ASSERT(pDocument == m_pOnlyDoc);
    if (pFrame == NULL)
	{
		ASSERT(bCreated);

		// create frame - set as main document frame
		BOOL bAutoDelete = pDocument->m_bAutoDelete;
		pDocument->m_bAutoDelete = FALSE;
					// don't destroy if something goes wrong
		pFrame = CreateNewFrame(pDocument, NULL);
		pDocument->m_bAutoDelete = bAutoDelete;
		if (pFrame == NULL)
		{
			AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
			delete pDocument;       // explicit delete on error
			return NULL;
		}
	}

	if (lpszPathName == NULL)
	{
		// create a new document
		SetDefaultTitle(pDocument);

		// avoid creating temporary compound file when starting up invisible
		if (!bMakeVisible)
			pDocument->m_bEmbedded = TRUE;

		if (!pDocument->OnNewDocument())
		{
			// user has been alerted to what failed in OnNewDocument
//			TRACE(traceAppMsg, 0, "CDocument::OnNewDocument returned FALSE.\n");
			if (bCreated)
				pFrame->DestroyWindow();    // will destroy document
			return NULL;
		}
	}
	else
	{
//		CWaitCursor wait;

		// open an existing document
		bWasModified = pDocument->IsModified();
		pDocument->SetModifiedFlag(FALSE);  // not dirty for open

		if (!pDocument->OnOpenDocument(lpszPathName))
		{
			// user has been alerted to what failed in OnOpenDocument
//			TRACE(traceAppMsg, 0, "CDocument::OnOpenDocument returned FALSE.\n");
			if (bCreated)
			{
				pFrame->DestroyWindow();    // will destroy document
			}
			else if (!pDocument->IsModified())
			{
				// original document is untouched
				pDocument->SetModifiedFlag(bWasModified);
			}
			else
			{
				// we corrupted the original document
				SetDefaultTitle(pDocument);

				if (!pDocument->OnNewDocument())
				{
//					TRACE(traceAppMsg, 0, "Error: OnNewDocument failed after trying "
//						"to open a document - trying to continue.\n");
					// assume we can continue
				}
			}
			return NULL;        // open failed
		}
		pDocument->SetPathName(lpszPathName, bAddToMRU);
//		pDocument->OnDocumentEvent(CDocument::onAfterOpenDocument);
	}

	CWinThread* pThread = AfxGetThread();
	ASSERT(pThread);

    if (bCreated && pThread->m_pMainWnd == NULL)
	{
		// set as main frame (InitialUpdateFrame will show the window)
        pThread->m_pMainWnd = pFrame;
	}

    InitialUpdateFrame(pFrame, pDocument, bMakeVisible);

	return pDocument;
}

IMPLEMENT_DYNAMIC(CCommandLineInfo,CObject)

CCommandLineInfo::CCommandLineInfo( )
{
   _args = QCoreApplication::arguments();
   m_nShellCommand = FileNew;
}

void CCommandLineInfo::ParseParam(
   const TCHAR* pszParam,
   BOOL bFlag,
   BOOL bLast
)
{
   if ( !bFlag )
   {
      m_strFileName = pszParam;
      m_nShellCommand = FileOpen;
   }
   else
   {
      if ( stricmp(pszParam,"Unregister") == 0 )
      {
         m_nShellCommand = AppUnregister;
      }
      else if ( stricmp(pszParam,"Unregserver") == 0 )
      {
         m_nShellCommand = AppUnregister;
      }
   }
}

IMPLEMENT_DYNCREATE(CWinApp,CWinThread)

BEGIN_MESSAGE_MAP(CWinApp,CWinThread)
   ON_COMMAND_RANGE(ID_FILE_MRU_FILE1,ID_FILE_MRU_FILE16,OnOpenRecentFile)
   ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1,OnUpdateRecentFileList)
   ON_COMMAND(ID_APP_EXIT, OnAppExit)
END_MESSAGE_MAP()

CWinApp::CWinApp() 
   : m_pRecentFileList(NULL) 
{
   ptrToTheApp = this;
}

CWinApp::~CWinApp()
{
   delete m_pRecentFileList;
}

void CWinApp::OnAppExit()
{
   m_pMainWnd->OnClose();   
}

void CWinApp::OnUpdateRecentFileList(CCmdUI *pCmdUI)
{
   CRecentFileList* pRecentFileList = AfxGetApp()->m_pRecentFileList;
   CMenu* pMRU = pCmdUI->m_pMenu;
   CString mruPath;
   CString mruString;
   int menu = 0;
   int mru;
   int mruPosition = -1;
   int idx;

   // Enabled if content.
   pCmdUI->Enable(pRecentFileList->GetSize());
   
   // Might be called just to test enabled state...if so, just return.
   if ( !pMRU )
   {
      return;
   }
   
   // Find MRU index in menu...
   for ( idx = 0; idx < pMRU->GetMenuItemCount(); idx++ )
   {
      if ( pMRU->GetMenuItemID(idx) == ID_FILE_MRU_FILE1 )
      {
         mruPosition = idx;
         break;
      }
   }

   if ( mruPosition >= 0 )
   {
      for ( mru = ID_FILE_MRU_FILE1; mru <= ID_FILE_MRU_FILE16; mru++ )
      {
         pMRU->RemoveMenu(mru,MF_BYCOMMAND);
      }
      
      if ( pRecentFileList->GetSize() )
      {
         for ( mru = 0; mru < pRecentFileList->GetSize(); mru++ )
         {
            pRecentFileList->GetDisplayName(mruPath,mru,QDir::currentPath().toLatin1().constData(),QDir::currentPath().length());
            mruString.Format("%d %s",mru+1,(LPCTSTR)mruPath);
            pMRU->InsertMenu(mruPosition+mru,MF_STRING|MF_BYPOSITION,ID_FILE_MRU_FILE1+mru,mruString);
         }
      }
      else
      {
         pMRU->InsertMenu(mruPosition,MF_STRING|MF_BYPOSITION,ID_FILE_MRU_FILE1,"Recent File");
         pMRU->EnableMenuItem(ID_FILE_MRU_FILE1,false);
      }
   }
}

BOOL CWinApp::DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags,
	BOOL bOpenFileDialog, CDocTemplate* pTemplate)
		// if pTemplate==NULL => all document templates
{
   CFileDialog dlgFile(bOpenFileDialog, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0);

	CString title;
	ENSURE(title.LoadString(nIDSTitle));

	dlgFile.m_ofn.Flags |= lFlags;

	CString strFilter;
	CString strDefault;
	if (pTemplate != NULL)
	{
		ASSERT_VALID(pTemplate);
		_AfxAppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate, &strDefault);
	}
	else
	{
		// do for all doc template
//		POSITION pos = m_templateList.GetHeadPosition();
      POSITION pos = GetFirstDocTemplatePosition();
		BOOL bFirst = TRUE;
		while (pos != NULL)
		{
//			pTemplate = (CDocTemplate*)m_templateList.GetNext(pos);
         pTemplate = GetNextDocTemplate(pos);
			_AfxAppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate,
				bFirst ? &strDefault : NULL);
			bFirst = FALSE;
		}
	}

	// append the "*.*" all files filter
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);

	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();
	return nResult == IDOK;
}

void CWinApp::ParseCommandLine(
   CCommandLineInfo& rCmdInfo
)
{
   int arg;
   QString localArg;
   BOOL bLast = FALSE;
   BOOL bFlag;

   for ( arg = 1; arg < rCmdInfo._args.count(); arg++ )
   {
      if ( arg == rCmdInfo._args.count()-1 ) bLast = TRUE;
      localArg = rCmdInfo._args.at(arg);
      bFlag = FALSE;
      if ( localArg.startsWith("/") ||
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
   if ( rCmdInfo.m_nShellCommand == CCommandLineInfo::FileNew )
   {
      // New file...
      OpenDocumentFile(NULL);
      return TRUE;
   }
   else if ( rCmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen )
   {
      // Open file...
      OpenDocumentFile(rCmdInfo.m_strFileName);
      return TRUE;
   }
   return FALSE;
}

HKEY CWinApp::GetAppRegistryKey(
   CAtlTransactionManager* pTM
)
{
   static QSettings settingsStatic(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
   return (HKEY)settingsStatic.fileName().toLatin1().constData();
}

LONG CWinApp::DelRegTree(
   HKEY hParentKey,
   const CString& strKeyName
)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
   QString key;
#ifdef UNICODE
   key = QString::fromWCharArray((LPCTSTR)strKeyName);
#else
   key = QString::fromLatin1((LPCTSTR)strKeyName);
#endif
   qDebug("Unregistering app...");
   settings.remove(key);
}

BOOL CWinApp::WriteProfileInt(
   LPCTSTR lpszSection,
   LPCTSTR lpszEntry,
   int nValue
)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
   QString key;
#ifdef UNICODE
   key = QString::fromWCharArray(lpszSection);
   key += "/";
   key += QString::fromWCharArray(lpszEntry);
#else
   key = QString::fromLatin1(lpszSection);
   key += "/";
   key += QString::fromLatin1(lpszEntry);
#endif

   settings.setValue(key,nValue);
}

BOOL CWinApp::WriteProfileString(
   LPCTSTR lpszSection,
   LPCTSTR lpszEntry,
   LPCTSTR lpszValue
)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
   QString key;
   QString value;
#ifdef UNICODE
   key = QString::fromWCharArray(lpszSection);
   key += "/";
   key += QString::fromWCharArray(lpszEntry);
   value = QString::fromWCharArray(lpszValue);
#else
   key = QString::fromLatin1(lpszSection);
   key += "/";
   key += QString::fromLatin1(lpszEntry);
   value = QString::fromLatin1(lpszValue);
#endif

   settings.setValue(key,value);
}

UINT CWinApp::GetProfileInt(
   LPCTSTR lpszSection,
   LPCTSTR lpszEntry,
   int nDefault
)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
   QString key;
   UINT value = nDefault;
#ifdef UNICODE
   key = QString::fromWCharArray(lpszSection);
   key += "/";
   key += QString::fromWCharArray(lpszEntry);
#else
   key = QString::fromLatin1(lpszSection);
   key += "/";
   key += QString::fromLatin1(lpszEntry);
#endif
//   qDebug("StoreSetting");
//   qDebug(key.toLatin1().constData());

   if ( settings.contains(key) )
   {
      value = settings.value(key).toInt();
   }

   return value;
}

CString CWinApp::GetProfileString(
   LPCTSTR lpszSection,
   LPCTSTR lpszEntry,
   LPCTSTR lpszDefault
)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
   QString key;
   QString defaultStr;
   QString value;

#ifdef UNICODE
   key = QString::fromWCharArray(lpszSection);
   key += "/";
   key += QString::fromWCharArray(lpszEntry);
   defaultStr = QString::fromWCharArray(lpszDefault);
#else
   key = QString::fromLatin1(lpszSection);
   key += "/";
   key += QString::fromLatin1(lpszEntry);
   defaultStr = QString::fromLatin1(lpszDefault);
#endif

   if ( settings.contains(key) )
   {
      value = settings.value(key,defaultStr).toString();
   }

   return CString(value);
}

BOOL CWinApp::PreTranslateMessage(
   MSG* pMsg
)
{
   CWnd* pWnd = CWnd::FromHandle(pMsg->hwnd);
   for ( ; pWnd ; pWnd = pWnd->GetParent() )
   {
      if ( pWnd->PreTranslateMessage(pMsg) )
         return TRUE;
   }
   return FALSE;
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
   if ( !pos )
   {
      return NULL; // Choker for end-of-list
   }
   CDocTemplate* pDocTemplate = _docTemplates.at((*pos)++);
   if ( (*pos) >= _docTemplates.count() )
   {
      delete pos;
      pos = NULL;
   }
   return pDocTemplate;
}

void CWinApp::AddDocTemplate(CDocTemplate* pDocTemplate)
{
   _docTemplates.append(pDocTemplate);
}

void CWinApp::AddToRecentFileList( 
   LPCTSTR lpszPathName  
)
{
}

void CWinApp::LoadStdProfileSettings( 
   UINT nMaxMRU 
)
{
   if ( m_pRecentFileList )
   {
      delete m_pRecentFileList;
   }
   m_pRecentFileList = new CRecentFileList(0,"Recent File List","File%d",nMaxMRU);
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
      if ( lpszFileName )
      {
         m_pRecentFileList->Add(lpszFileName);
      }
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

void CWinApp::OnOpenRecentFile(UINT nID)
{
   CString fileName;
   m_pRecentFileList->GetDisplayName(fileName,nID-ID_FILE_MRU_FILE1,"",0);
   OpenDocumentFile(fileName);
}

HCURSOR CWinApp::LoadStandardCursor(
   LPCTSTR lpszCursorName
) const
{
#if UNICODE
   QString str = QString::fromWCharArray(lpszCursorName);
#else
   QString str = QString::fromLatin1(lpszCursorName);
#endif
   return (HCURSOR)NULL;
}

IMPLEMENT_DYNAMIC(CMenu,CCmdTarget)

BEGIN_MESSAGE_MAP(CMenu,CCmdTarget)
END_MESSAGE_MAP()

CMenu::CMenu()
   : m_hMenu(NULL),
     m_pOwnerWnd(NULL)
{
   _qtd = new QMenu;
   _cmenu = new QHash<int,CMenu*>;
   m_hMenu = (HMENU)this;
   _qtd->setSeparatorsCollapsible(false);
}

CMenu::~CMenu()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   foreach ( CMenu* menu, *_cmenu )
   {
      delete menu;
   }
   _cmenu->clear();
   delete _cmenu;
}

CMenu* PASCAL CMenu::FromHandle( 
   HMENU hMenu  
)
{
   return (CMenu*)hMenu;
}

void CMenu::menuAction_triggered()
{
   // Check if command is enabled by calling its UI update...
   CTestCmdUI state;
   QAction* pAction = dynamic_cast<QAction*>(sender());
   state.m_nID = pAction->data().toInt();
   ptrToTheApp->GetMainWnd()->OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL);
   if ( !state.m_bEnabled )
   {
      // Command is disabled, and we just got triggered, which means the
      // shortcut associated with this menu triggered us.  Re-route the
      // shortcut as a keyPressEvent.
      QKeySequence keys = pAction->shortcut();
      QWidget* focusWidget = QApplication::focusWidget();
      int keyc;
      if ( !focusWidget )
      {
         focusWidget = ptrToTheApp->GetMainWnd()->toQWidget();
      }
      for ( keyc = 0; keyc < keys.count(); keyc++ )
      {
         QKeyEvent keyEvent(QEvent::KeyPress,keys[keyc],0);
         pAction->setEnabled(false);
         QApplication::sendEvent(focusWidget,&keyEvent);
         pAction->setEnabled(true);
      }
      return;
   }
   if ( m_pOwnerWnd )
   {
      m_pOwnerWnd->SendMessage(WM_COMMAND,qtToMfcMenu.value((QAction*)sender()));
   }
   else
   {
      ptrToTheApp->GetMainWnd()->SendMessage(WM_COMMAND,qtToMfcMenu.value((QAction*)sender()));
   }
}

void CMenu::menu_aboutToShow()
{
   if ( m_pOwnerWnd )
   {
      m_pOwnerWnd->toQWidget()->setFocus();
      m_pOwnerWnd->SendMessage(WM_INITMENUPOPUP,(WPARAM)m_hMenu);
   }
   else
   {
      AfxGetMainWnd()->toQWidget()->setFocus();
      AfxGetMainWnd()->SendMessage(WM_INITMENUPOPUP,(WPARAM)m_hMenu);
   }
}

QAction* CMenu::findMenuItemByID(UINT id) const
{
   foreach ( CMenu* menu, *_cmenu )
   {
      if ( menu->mfcToQtMenuMap()->contains(id) )
      {
         return menu->mfcToQtMenuMap()->value(id);
      }
   }
   if ( mfcToQtMenu.contains(id) )
   {
      return mfcToQtMenu.value(id);
   }
   return NULL;
}

QAction* CMenu::findMenuItemByPosition(UINT pos) const
{
   if ( pos < _qtd->actions().count() )
   {
      return _qtd->actions().at(pos);
   }
   return NULL;
}

UINT CMenu::findMenuID(QAction* action) const
{
   foreach ( CMenu* menu, *_cmenu )
   {
      if ( menu->qtToMfcMenuMap()->contains(action) )
      {
         return menu->qtToMfcMenuMap()->value(action);
      }
   }
   if ( qtToMfcMenu.contains(action) )
   {
      return qtToMfcMenu.value(action);
   }
   return 0;
}

BOOL CMenu::LoadMenu(
   UINT nIDResource
)
{
   qtMfcInitMenuResource(nIDResource,this);
   return TRUE;
}

BOOL CMenu::RemoveMenu(
   UINT nPosition,
   UINT nFlags
)
{
   if ( nFlags&MF_BYPOSITION )
   {
      qtToMfcMenu.remove(findMenuItemByPosition(nPosition));
      _qtd->removeAction(findMenuItemByPosition(nPosition));
   }
   else
   {
      qtToMfcMenu.remove(findMenuItemByID(nPosition));
      _qtd->removeAction(findMenuItemByID(nPosition));
   }
   mfcToQtMenu.remove(nPosition);
   return TRUE;
}

CMenu* CMenu::GetSubMenu(
   int nPos
) const
{
   return _cmenu->value(nPos);
}

BOOL CMenu::CreatePopupMenu()
{
   _cmenu->clear();
   _qtd->clear();
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif
   mfcToQtMenu.clear();
   qtToMfcMenu.clear();
   return TRUE;
}

BOOL CMenu::AppendMenu(
   UINT nFlags,
   UINT_PTR nIDNewItem,
   LPCTSTR lpszNewItem
)
{
   CMenu* pMenu = (CMenu*)nIDNewItem; // For MF_POPUP
   QAction* action;
   if ( nFlags&MF_POPUP )
   {
      _cmenu->insert(_qtd->actions().count(),pMenu);
      action = _qtd->addMenu(pMenu->toQMenu());
      action->setData((uint_ptr)-1);

#if UNICODE
      pMenu->toQMenu()->setTitle(QString::fromWCharArray(lpszNewItem));
#else
      pMenu->toQMenu()->setTitle(QString::fromLatin1(lpszNewItem));
#endif
   }
   else if ( nFlags&MF_SEPARATOR )
   {
      _qtd->addSeparator();
   }
   else
   {
#if UNICODE
      action = _qtd->addAction(QString::fromWCharArray(lpszNewItem));
#else
      action = _qtd->addAction(QString::fromLatin1(lpszNewItem));
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
      action->setEnabled(true);
      if ( (nFlags&MF_DISABLED) ||
           (nFlags&MF_GRAYED) )
      {
         action->setEnabled(false);
      }
      QObject::connect(action,SIGNAL(triggered()),this,SLOT(menuAction_triggered()));
      action->setData((uint_ptr)nIDNewItem);
      mfcToQtMenu.insert(nIDNewItem,action);
      qtToMfcMenu.insert(action,nIDNewItem);
   }

   return TRUE;
}

BOOL CMenu::InsertMenu( 
   UINT nPosition, 
   UINT nFlags, 
   UINT_PTR nIDNewItem, 
   LPCTSTR lpszNewItem
)
{
   CMenu* pMenu = (CMenu*)nIDNewItem; // For MF_POPUP
   QAction* action;
   switch ( nFlags&MF_BYPOSITION )
   {
   case MF_BYPOSITION:
      action = findMenuItemByPosition(nPosition);
      break;
   default:
      action = findMenuItemByID(nPosition);
      break;
   }
   if ( action )
   {
      if ( nFlags&MF_POPUP )
      {
         _cmenu->insert(_qtd->actions().indexOf(action),pMenu);
         action = _qtd->insertMenu(action,pMenu->toQMenu());
         action->setData((uint_ptr)-1);
   
#if UNICODE
         pMenu->toQMenu()->setTitle(QString::fromWCharArray(lpszNewItem));
#else
         pMenu->toQMenu()->setTitle(QString::fromLatin1(lpszNewItem));
#endif
      }
      else if ( nFlags&MF_SEPARATOR )
      {
         _qtd->insertSeparator(action);
      }
      else
      {
#if UNICODE
         QAction* newAction = new QAction(QString::fromWCharArray(lpszNewItem),NULL);
#else
         QAction* newAction = new QAction(QString::fromLatin1(lpszNewItem),NULL);
#endif
         _qtd->insertAction(action,newAction);
         action = newAction;
         if ( action->text().contains("\t") )
         {
            action->setShortcut(QKeySequence(action->text().split("\t").at(1)));
         }
         if ( nFlags&MF_CHECKED )
         {
            action->setCheckable(true);
            action->setChecked(true);
         }
         action->setEnabled(true);
         if ( (nFlags&MF_DISABLED) ||
              (nFlags&MF_GRAYED) )
         {
            action->setEnabled(false);
         }
         QObject::connect(action,SIGNAL(triggered()),this,SLOT(menuAction_triggered()));
         action->setData((uint_ptr)nIDNewItem);
         mfcToQtMenu.insert(nIDNewItem,action);
         qtToMfcMenu.insert(action,nIDNewItem);
      }
   }
   return TRUE;
}

UINT CMenu::GetMenuItemCount( ) const
{
   return _qtd->actions().count();
}

UINT CMenu::GetMenuItemID(
   int nPos
) const
{
   return findMenuID(findMenuItemByPosition(nPos));
}

UINT CMenu::GetMenuState(
   UINT nID,
   UINT nFlags
) const
{
   QAction* action;
   UINT state = 0;

   switch ( nFlags&MF_BYPOSITION )
   {
   case MF_BYPOSITION:
      if ( _cmenu->value(nID) )
      {
         state |= MF_POPUP;
         return state;
      }
      else
      {
         action = findMenuItemByPosition(nID);
      }
      break;
   default:
      action = findMenuItemByID(nID);
      break;
   }
   if ( action )
   {
      if ( action->menu() )
      {
         state |= MF_POPUP;
      }
      if ( action->isSeparator() )
      {
         state |= MF_SEPARATOR;
      }
      if ( action->isChecked() )
      {
         state |= MF_CHECKED;
      }
      if ( !action->isEnabled() )
      {
         state |= MF_DISABLED;
      }
      if ( action == _qtd->defaultAction() )
      {
         state |= MF_DEFAULT;
      }
   }
   return state;
}

int CMenu::GetMenuString(
   UINT nIDItem,
   LPTSTR lpString,
   int nMaxCount,
   UINT nFlags
) const
{
   QAction* action;
   int len = 0;

   switch ( nFlags&MF_BYPOSITION )
   {
   case MF_BYPOSITION:
      action = findMenuItemByPosition(nIDItem);
      break;
   default:
      action = findMenuItemByID(nIDItem);
      break;
   }
   if ( action )
   {
#if UNICODE
      wcsncpy(lpString,(LPTSTR)action->text().unicode(),nMaxCount);
      len = wcslen(lpString);
#else
      strncpy(lpString,(LPTSTR)action->text().toLatin1().constData(),nMaxCount);
      len = strlen(lpString);
#endif
   }
   return len;
}

int CMenu::GetMenuString(
   UINT nIDItem,
   CString& rString,
   UINT nFlags
) const
{
   QAction* action;
   int len = 0;

   switch ( nFlags&MF_BYPOSITION )
   {
   case MF_BYPOSITION:
      action = findMenuItemByPosition(nIDItem);
      break;
   default:
      action = findMenuItemByID(nIDItem);
      break;
   }
   if ( action )
   {
      rString = action->text();
      len = action->text().length();
   }
   return len;
}

BOOL CMenu::ModifyMenu(
   UINT nPosition,
   UINT nFlags,
   UINT_PTR nIDNewItem,
   LPCTSTR lpszNewItem
)
{
   QAction* action;
   int len = 0;

   switch ( nFlags&MF_BYPOSITION )
   {
   case MF_BYPOSITION:
      action = findMenuItemByPosition(nPosition);
      break;
   default:
      action = findMenuItemByID(nPosition);
      break;
   }
   if ( action )
   {
      if ( lpszNewItem )
      {
#if UNICODE
         action->setText(QString::fromWCharArray(lpszNewItem));
#else
         action->setText(QString::fromLatin1(lpszNewItem));
#endif
         if ( action->text().contains("\t") )
         {
            action->setShortcut(QKeySequence(action->text().split("\t").at(1)));
         }
      }
      return TRUE;
   }
   return FALSE;
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
      action = findMenuItemByPosition(uItem);
   }
   else
   {
      action = findMenuItemByID(uItem);
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
   QAction* action;
   UINT prevState = (UINT)-1;
   if ( nCheck&MF_BYPOSITION )
   {
      action = findMenuItemByPosition(nIDCheckItem);
   }
   else
   {
      action = findMenuItemByID(nIDCheckItem);
   }
   if ( action )
   {
      prevState = action->isChecked();
      action->setCheckable(true);
      action->setChecked(nCheck&(~MF_BYPOSITION));
   }
   return prevState;
}

BOOL CMenu::CheckMenuRadioItem(
   UINT nIDFirst,
   UINT nIDLast,
   UINT nIDItem,
   UINT nFlags
)
{
   QAction* action = NULL;
   UINT prevState = (UINT)-1;
   UINT id;
   QActionGroup* group = _groups.value(nIDFirst);
   int idx;

   if ( !group )
   {
      group = new QActionGroup(NULL);
   }

   if ( nFlags&MF_BYPOSITION )
   {
      for ( idx = nIDFirst; idx <= nIDLast; idx++ )
      {
         QAction* groupMember = findMenuItemByPosition(idx);
         group->addAction(groupMember);
         groupMember->setActionGroup(group);
         if ( idx == nIDItem )
         {
            action = groupMember;
         }
      }
   }
   else
   {
      for ( id = nIDFirst; id <= nIDLast; id++ )
      {
         QAction* groupMember = findMenuItemByID(id);
         group->addAction(groupMember);
         groupMember->setActionGroup(group);
         if ( id == nIDItem )
         {
            action = groupMember;
         }
      }
   }
   if ( action )
   {
      prevState = action->isChecked();
      action->setCheckable(true);
      action->setChecked(true);
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
   // Set owner window so menu updates go to right place.
   m_pOwnerWnd = pWnd;
   
   QAction* action = _qtd->exec(QCursor::pos());
   int result = 0;
   if ( action && (nFlags&TPM_RETURNCMD) )
   {
      result = findMenuID(action);
   }
   else if ( action )
   {
//      m_pOwnerWnd->SendMessage(WM_COMMAND,findMenuID(action));
      result = 1;
   }
   return result;
}

UINT CMenu::EnableMenuItem(
   UINT nIDEnableItem,
   UINT nEnable
)
{
   QAction* action;
   if ( nEnable&MF_BYPOSITION )
   {
      action = findMenuItemByPosition(nIDEnableItem);
   }
   else
   {
      action = findMenuItemByID(nIDEnableItem);
   }
   if ( action )
   {
      bool enabled = action->isEnabled();
      action->setEnabled(nEnable&(~MF_BYPOSITION));
      return enabled;
   }
   return -1;
}

BOOL CMenu::DestroyMenu( )
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   return TRUE;
}

IMPLEMENT_DYNAMIC(CTabCtrl,CWnd)

BEGIN_MESSAGE_MAP(CTabCtrl,CWnd)
END_MESSAGE_MAP()

CTabCtrl::CTabCtrl(CWnd* parent)
   : CWnd(parent)
{
}

CTabCtrl::~CTabCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CTabCtrl::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CTabCtrl*>(this));
   widget->setParent(NULL);
}

void CTabCtrl::currentChanged(int value)
{
   NMHDR nmhdr;
   
   nmhdr.hwndFrom = m_hWnd;
   nmhdr.idFrom = _id;
   nmhdr.code = TCN_SELCHANGE;
   
   GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmhdr);
}

BOOL CTabCtrl::Create( 
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

   if ( pParentWnd )
      _qt = new QTabWidget(pParentWnd->toQWidget());
   else
      _qt = new QTabWidget();

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QTabWidget*>(_qt);

   _qtd->setMouseTracking(true);
   _qtd->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif

   SetParent(pParentWnd);

   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));
   
   qtToMfcWindow.insert(_qtd,this);
   
   return TRUE;
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
   _qtd->insertTab(nItem,new QWidget(),QString::fromLatin1(lpszItem));
#endif
   _qtd->blockSignals(false);
   return nItem;
}

int CTabCtrl::SetCurSel(
  int nItem
)
{
   int oldSel = _qtd->currentIndex();
   _qtd->blockSignals(true);
   _qtd->setCurrentIndex(nItem);
   _qtd->blockSignals(false);
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

QPlainTextEdit_MFC::~QPlainTextEdit_MFC()
{
   _mfc = NULL;
}

void QPlainTextEdit_MFC::paintEvent(QPaintEvent *event)
{
   CDC* pDC = _mfc?_mfc->GetDC():NULL;
   if ( _mfc )
   {
      QString style;
      AFX_CTLCOLOR ctlColor;
      ctlColor.hWnd = (HWND)_mfc;
      ctlColor.hDC = (HDC)pDC;
      ctlColor.nCtlType = 0;
      _mfc->SendMessage(WM_ERASEBKGND,(WPARAM)(HDC)pDC);
      _mfc->SendMessage(WM_CTLCOLOR,0,(LPARAM)&ctlColor);
      style.sprintf("QLabel { color: #%02x%02x%02x; }",GetRValue(pDC->GetTextColor()),GetGValue(pDC->GetTextColor()),GetBValue(pDC->GetTextColor()));
      setStyleSheet(style);
   }
   QPlainTextEdit::paintEvent(event);      
   if ( _mfc )
   {
      _mfc->ReleaseDC(pDC);
      _mfc->SendMessage(WM_PAINT);
   }
}

QLineEdit_MFC::~QLineEdit_MFC()
{
   _mfc = NULL;
}

void QLineEdit_MFC::paintEvent(QPaintEvent *event)
{
   CDC* pDC = _mfc?_mfc->GetDC():NULL;
   if ( _mfc )
   {
      QString style;
      AFX_CTLCOLOR ctlColor;
      ctlColor.hWnd = (HWND)_mfc;
      ctlColor.hDC = (HDC)pDC;
      ctlColor.nCtlType = 0;
      _mfc->SendMessage(WM_ERASEBKGND,(WPARAM)(HDC)pDC);
      _mfc->SendMessage(WM_CTLCOLOR,0,(LPARAM)&ctlColor);
   }
   QLineEdit::paintEvent(event);      
   if ( _mfc )
   {
      _mfc->ReleaseDC(pDC);
      _mfc->SendMessage(WM_PAINT);
   }
}

void QLineEdit_MFC::keyPressEvent(QKeyEvent *event)
{
   if ( (event->key() != Qt::Key_Enter) &&
        (event->key() != Qt::Key_Return) )
   {
      QLineEdit::keyPressEvent(event);
   }
   else
   {
      event->accept();
   }
}

void QLineEdit_MFC::keyReleaseEvent(QKeyEvent *event)
{
   if ( (event->key() != Qt::Key_Enter) &&
        (event->key() != Qt::Key_Return) )
   {
      QLineEdit::keyReleaseEvent(event);
   }
   else
   {
      event->accept();
   }
}

IMPLEMENT_DYNAMIC(CEdit,CWnd)

BEGIN_MESSAGE_MAP(CEdit,CWnd)
END_MESSAGE_MAP()

CEdit::CEdit(CWnd* parent)
   : CWnd(parent),
     _qtd_ptedit(NULL),
     _qtd_ledit(NULL)
{
}

CEdit::~CEdit()
{
   if ( _qtd_ledit )
      delete _qtd_ledit;
   if ( _qtd_ptedit )
      delete _qtd_ptedit;
   _qtd_ledit = NULL;
   _qtd_ptedit = NULL;
   _qtd = NULL;
   _qt = NULL;
}

void CEdit::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   setMfcBuddy(widget->mfcBuddy());
   widget->setMfcBuddy(mfcBuddy());
   if ( _qtd_ledit )
   {
      _qtd_ledit->setMfc(this);
   }
   if ( _qtd_ptedit )
   {
      _qtd_ptedit->setMfc(this);
   }
   _qt->installEventFilter(dynamic_cast<CEdit*>(this));
}

void CEdit::updateFromBuddy()
{
   CString str;
   mfcBuddy()->GetDlgItemText(mfcBuddy()->GetDlgCtrlID(),str);
   _qtd_ledit->blockSignals(true);
#if UNICODE
   _qtd_ledit->setText(QString::fromWCharArray((LPCTSTR)str));
#else
   _qtd_ledit->setText(QString::fromLatin1((LPCTSTR)str));
#endif
   _qtd_ledit->blockSignals(false);
   GetOwner()->SendMessage(WM_COMMAND,(EN_CHANGE<<16)|(_id),(LPARAM)m_hWnd);
}

bool CEdit::event(QEvent *event)
{
   MFCMessageEvent* msgEvent = dynamic_cast<MFCMessageEvent*>(event);
   if ( msgEvent )
   {
      switch ( msgEvent->msg.message )
      {
      case EM_SETREADONLY:
         if ( _dwStyle&ES_MULTILINE )
         {
            if ( msgEvent->msg.wParam )
            {
               _dwStyle |= ES_READONLY;
            }
            else
            {
               _dwStyle &= (~ES_READONLY);
            }
            _qtd_ptedit->setReadOnly(msgEvent->msg.wParam);
         }
         else
         {
            if ( msgEvent->msg.wParam )
            {
               _dwStyle |= ES_READONLY;
            }
            else
            {
               _dwStyle &= (~ES_READONLY);
            }
            _qtd_ledit->setReadOnly(msgEvent->msg.wParam);
         }
         return true;
         break;
      }
   }
   return CWnd::event(event);
}

void CEdit::keyPressEvent(QKeyEvent *event)
{
   if ( mfcBuddy() )
   {
      if ( event->key() == Qt::Key_Up ||
           event->key() == Qt::Key_Down ||
           event->key() == Qt::Key_PageUp ||
           event->key() == Qt::Key_PageDown )
      {
         QApplication::sendEvent(mfcBuddy()->toQWidget(),event);
         //CWnd::keyPressEvent(event);
      }
      else if ( event->key() == Qt::Key_Enter ||
                event->key() == Qt::Key_Return )
      {
         toQWidget()->clearFocus();
      }
   }
   else
   {
      CWnd::keyPressEvent(event);
   }
}

void CEdit::keyReleaseEvent(QKeyEvent *event)
{
   if ( mfcBuddy() )
   {
      if ( event->key() == Qt::Key_Up ||
           event->key() == Qt::Key_Down ||
           event->key() == Qt::Key_PageUp ||
           event->key() == Qt::Key_PageDown )
      {
         QApplication::sendEvent(mfcBuddy()->toQWidget(),event);
         //CWnd::keyReleaseEvent(event);
      }
   }
   else
   {
      CWnd::keyReleaseEvent(event);
   }
}

void CEdit::focusInEvent(QFocusEvent *event)
{
   GetOwner()->SendMessage(WM_COMMAND,(EN_SETFOCUS<<16)|(_id),(LPARAM)m_hWnd);
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
         _qt = new QPlainTextEdit_MFC(pParentWnd->toQWidget());
      else
         _qt = new QPlainTextEdit_MFC;

      // Downcast to save having to do it all over the place...
      _qtd_ptedit = dynamic_cast<QPlainTextEdit_MFC*>(_qt);

      // Pass-through signals
      QObject::connect(_qtd_ptedit,SIGNAL(textChanged()),this,SLOT(textChanged()));
      
      _qtd_ptedit->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
      _qtd_ptedit->setReadOnly(dwStyle&ES_READONLY);
      _qtd_ptedit->setVisible(dwStyle&WS_VISIBLE);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
      _qtd_ptedit->setFont(QFont("MS Shell Dlg",8));
#else
      _qtd_ptedit->setFont(QFont("MS Shell Dlg",9));
#endif

      if ( dwStyle&ES_NUMBER )
      {
         _qtd_ptedit->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
      }
      
      qtToMfcWindow.insert(_qtd_ptedit,this);
   }
   else
   {
      if ( pParentWnd )
         _qt = new QLineEdit_MFC(pParentWnd->toQWidget());
      else
         _qt = new QLineEdit_MFC;

      // Downcast to save having to do it all over the place...
      _qtd_ledit = dynamic_cast<QLineEdit_MFC*>(_qt);

      // Pass-through signals
      QObject::connect(_qtd_ledit,SIGNAL(textChanged(QString)),this,SLOT(textChanged()));
      QObject::connect(_qtd_ledit,SIGNAL(returnPressed()),this,SLOT(textChanged()));
            
      _qtd_ledit->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
      _qtd_ledit->setReadOnly(dwStyle&ES_READONLY);
      _qtd_ledit->setVisible(dwStyle&WS_VISIBLE);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
      _qtd_ledit->setFont(QFont("MS Shell Dlg",8));
#else
      _qtd_ledit->setFont(QFont("MS Shell Dlg",9));
#endif

      if ( dwStyle&ES_NUMBER )
      {
         _qtd_ledit->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
      }
      
      qtToMfcWindow.insert(_qtd_ledit,this);
   }
   
   SetParent(pParentWnd);
   
   return TRUE;
}

void CEdit::textChanged()
{
   GetOwner()->SendMessage(WM_COMMAND,(EN_CHANGE<<16)|(_id),(LPARAM)m_hWnd);
   
   if ( mfcBuddy() )
   {
      mfcBuddy()->updateFromBuddy();
   }
}

void CEdit::Clear()
{
   if ( _dwStyle&ES_MULTILINE )
   {
      _qtd_ptedit->clear();
   }
   else
   {
      _qtd_ledit->clear();
   }
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
      strncpy(lpszStringBuf,(LPTSTR)_qtd_ptedit->toPlainText().toLatin1().constData(),nMaxCount);
      return strlen(lpszStringBuf);
#endif
   }
   else
   {
#if UNICODE
      wcsncpy(lpszStringBuf,(LPTSTR)_qtd_ledit->text().unicode(),nMaxCount);
      return wcslen(lpszStringBuf);
#else
      strncpy(lpszStringBuf,(LPTSTR)_qtd_ledit->text().toLatin1().constData(),nMaxCount);
      return strlen(lpszStringBuf);
#endif
   }
}

void CEdit::SetLimitText(
   UINT nMax
)
{
   if ( _dwStyle&ES_MULTILINE )
   {
   }
   else
   {
      _qtd_ledit->setMaxLength(nMax);
   }
}

void CEdit::SetWindowText(
   LPCTSTR lpszString
)
{
   if ( _dwStyle&ES_MULTILINE )
   {
      _qtd_ptedit->blockSignals(true);
      _qtd_ptedit->clear();
      _qtd_ptedit->blockSignals(true);
#if UNICODE
      _qtd_ptedit->setPlainText(QString::fromWCharArray(lpszString));
#else
      _qtd_ptedit->setPlainText(QString::fromLatin1(lpszString));
#endif
   }
   else
   {
      _qtd_ledit->blockSignals(true);
      _qtd_ledit->clear();
      _qtd_ledit->blockSignals(false);
#if UNICODE
      _qtd_ledit->setText(QString::fromWCharArray(lpszString));
#else
      _qtd_ledit->setText(QString::fromLatin1(lpszString));
#endif
   }
   
   // Tell our buddied CSpinButtonCtrl if necessary...
   if ( mfcBuddy() )
   {
      mfcBuddy()->SetWindowText(lpszString);
   }
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
      if ( nStartChar < 0 )
      {
         _qtd_ledit->deselect();
      }
      else if ( (nStartChar == 0) &&
                (nEndChar == -1) )
      {
         _qtd_ledit->selectAll();
      }
      else
      {
         _qtd_ledit->setSelection(nStartChar,nEndChar);
      }
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
      textCursor.insertText(QString::fromLatin1(lpszNewText));
#endif
      _qtd_ptedit->setTextCursor(textCursor);
   }
   else
   {
      _qtd_ledit->blockSignals(true);
#if UNICODE
      _qtd_ledit->insert(QString::fromWCharArray(lpszNewText));
#else
      _qtd_ledit->insert(QString::fromLatin1(lpszNewText));
#endif
      _qtd_ledit->blockSignals(false);
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
   
   // Tell our buddied CSpinButtonCtrl if necessary...
   if ( mfcBuddy() )
   {
      mfcBuddy()->updateFromBuddy();
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
      if ( lpTrans )
      {
         // Check for numeric...
         (*lpTrans) = TRUE;
         
         QString text = _qtd_ptedit->toPlainText();
         if ( !text.remove(QRegExp("[0-9]*")).isEmpty() )
         {
            (*lpTrans) = FALSE;
         }
      }
      return _qtd_ptedit->toPlainText().toInt();
   }
   else
   {
      if ( lpTrans )
      {
         // Check for numeric...
         (*lpTrans) = TRUE;
         
         QString text = _qtd_ledit->text();
         if ( !text.remove(QRegExp("[0-9]*")).isEmpty() )
         {
            (*lpTrans) = FALSE;
         }
      }
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
      _qtd_ptedit->setPlainText(QString::fromLatin1(lpszString));
#endif
   }
   else
   {
#if UNICODE
      _qtd_ledit->setText(QString::fromWCharArray(lpszString));
#else
      _qtd_ledit->setText(QString::fromLatin1(lpszString));
#endif
   }
   
   // Tell our buddied CSpinButtonCtrl if necessary...
   if ( mfcBuddy() )
   {
      mfcBuddy()->updateFromBuddy();
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
      strncpy(lpStr,_qtd_ptedit->toPlainText().toLatin1().constData(),nMaxCount);
#endif
      return _qtd_ptedit->toPlainText().length();
   }
   else
   {
#if UNICODE
      wcsncpy(lpStr,(LPWSTR)_qtd_ledit->text().unicode(),nMaxCount);
#else
      strncpy(lpStr,_qtd_ledit->text().toLatin1().constData(),nMaxCount);
#endif
      return _qtd_ledit->text().length();
   }
}

IMPLEMENT_DYNAMIC(CButton,CWnd)

BEGIN_MESSAGE_MAP(CButton,CWnd)
   ON_WM_DRAWITEM()
END_MESSAGE_MAP()

CButton::CButton(CWnd* parent)
   : CWnd(parent),
     _qtd_push(NULL),
     _qtd_radio(NULL),
     _qtd_check(NULL),
     _qtd_groupbox(NULL)
{
}

CButton::~CButton()
{
}

void CButton::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   CString text;
   widget->GetWindowText(text);
   Create(text,widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CButton*>(this));
   widget->setParent(NULL);
}

void CButton::clicked()
{
   GetOwner()->SendMessage(WM_COMMAND,(BN_CLICKED<<16)|(_id),(LPARAM)m_hWnd);
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
   _dwStyle = dwStyle;

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

#if UNICODE
      _qtd_check->setText(QString::fromWCharArray(lpszCaption));
#else
      _qtd_check->setText(QString::fromLatin1(lpszCaption));
#endif

      _qtd_check->setCheckable(true);
      
      _qtd_check->setMouseTracking(true);
      _qtd_check->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
      _qtd_check->setVisible(dwStyle&WS_VISIBLE);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
      _qtd_check->setFont(QFont("MS Shell Dlg",8));
#else
      _qtd_check->setFont(QFont("MS Shell Dlg",9));
#endif

      // Pass-through signals
      QObject::connect(_qtd_check,SIGNAL(clicked()),this,SLOT(clicked()));
      
      qtToMfcWindow.insert(_qtd_check,this);
   }
   else if ( buttonType == BS_AUTO3STATE )
   {
      _qt = new QCheckBox(pParentWnd->toQWidget());

      // Downcast to save having to do it all over the place...
      _qtd_check = dynamic_cast<QCheckBox*>(_qt);

#if UNICODE
      _qtd_check->setText(QString::fromWCharArray(lpszCaption));
#else
      _qtd_check->setText(QString::fromLatin1(lpszCaption));
#endif

      _qtd_check->setCheckable(true);
      _qtd_check->setTristate(true);
      
      _qtd_check->setMouseTracking(true);
      _qtd_check->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
      _qtd_check->setVisible(dwStyle&WS_VISIBLE);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
      _qtd_check->setFont(QFont("MS Shell Dlg",8));
#else
      _qtd_check->setFont(QFont("MS Shell Dlg",9));
#endif

      // Pass-through signals
      QObject::connect(_qtd_check,SIGNAL(clicked()),this,SLOT(clicked()));
      
      qtToMfcWindow.insert(_qtd_check,this);
   }
   else if ( buttonType == BS_AUTORADIOBUTTON )
   {
      _qt = new QRadioButton(pParentWnd->toQWidget());

      // Downcast to save having to do it all over the place...
      _qtd_radio = dynamic_cast<QRadioButton*>(_qt);

#if UNICODE
      _qtd_radio->setText(QString::fromWCharArray(lpszCaption));
#else
      _qtd_radio->setText(QString::fromLatin1(lpszCaption));
#endif

      _qtd_radio->setCheckable(true);
      
      _qtd_radio->setMouseTracking(true);
      _qtd_radio->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
      _qtd_radio->setVisible(dwStyle&WS_VISIBLE);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
      _qtd_radio->setFont(QFont("MS Shell Dlg",8));
#else
      _qtd_radio->setFont(QFont("MS Shell Dlg",9));
#endif

      // Pass-through signals
      QObject::connect(_qtd_radio,SIGNAL(clicked()),this,SLOT(clicked()));
      
      qtToMfcWindow.insert(_qtd_radio,this);
   }
   else if ( (buttonType == BS_PUSHBUTTON) ||
             (buttonType == BS_DEFPUSHBUTTON) )
   {
      _qt = new QPushButton(pParentWnd->toQWidget());

      // Downcast to save having to do it all over the place...
      _qtd_push = dynamic_cast<QPushButton*>(_qt);

#if UNICODE
      _qtd_push->setText(QString::fromWCharArray(lpszCaption));
#else
      _qtd_push->setText(QString::fromLatin1(lpszCaption));
#endif

      _qtd_push->setDefault(buttonType==BS_DEFPUSHBUTTON);
      
      _qtd_push->setMouseTracking(true);
      _qtd_push->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
      _qtd_push->setVisible(dwStyle&WS_VISIBLE);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
      _qtd_push->setFont(QFont("MS Shell Dlg",8));
#else
      _qtd_push->setFont(QFont("MS Shell Dlg",9));
#endif

      // Pass-through signals
      QObject::connect(_qtd_push,SIGNAL(clicked()),this,SLOT(clicked()));
      
      qtToMfcWindow.insert(_qtd_push,this);
   }
   else if ( buttonType == BS_GROUPBOX )
   {
      _qt = new QGroupBox(pParentWnd->toQWidget());
   
      // Downcast to save having to do it all over the place...
      _qtd_groupbox = dynamic_cast<QGroupBox*>(_qt);
   
#if UNICODE
      _qtd_groupbox->setTitle(QString::fromWCharArray(lpszCaption));
#else
      _qtd_groupbox->setTitle(QString::fromLatin1(lpszCaption));
#endif

      _qtd_groupbox->setContentsMargins(0,0,0,0);
      
      _qtd_groupbox->setMouseTracking(true);
      _qtd_groupbox->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
      _qtd_groupbox->setVisible(dwStyle&WS_VISIBLE);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
      _qtd_groupbox->setFont(QFont("MS Shell Dlg",8));
#else
      _qtd_groupbox->setFont(QFont("MS Shell Dlg",9));
#endif

      // Pass-through signals
      QObject::connect(_qtd_groupbox,SIGNAL(clicked()),this,SLOT(clicked()));
      
      qtToMfcWindow.insert(_qtd_groupbox,this);
   }
   SetParent(pParentWnd);

   return TRUE;
}

void CButton::DrawItem(
   LPDRAWITEMSTRUCT lpDrawItemStruct
)
{
}

void CButton::OnDrawItem(
   int nIDCtl,
   LPDRAWITEMSTRUCT lpDrawItemStruct
)
{
   DrawItem(lpDrawItemStruct);
}

HICON CButton::SetIcon(
   HICON hIcon
)
{
   DWORD buttonType = _dwStyle&0x000F;
   DWORD buttonStyle = _dwStyle&0xFFF0;

   if ( buttonType == BS_AUTOCHECKBOX )
   {
      _qtd_check->setIcon(*(QIcon*)hIcon);
   }
   else if ( buttonType == BS_AUTO3STATE )
   {
      _qtd_check->setIcon(*(QIcon*)hIcon);
   }
   else if ( buttonType == BS_AUTORADIOBUTTON )
   {
      _qtd_radio->setIcon(*(QIcon*)hIcon);
   }
   else if ( (buttonType == BS_PUSHBUTTON) ||
             (buttonType == BS_DEFPUSHBUTTON) )
   {
      _qtd_push->setIcon(*(QIcon*)hIcon);
   }

   return (HICON)0;
}

HBITMAP CButton::SetBitmap(
   HBITMAP hBitmap
)
{
   CBitmap* pBitmap = (CBitmap*)hBitmap;
   DWORD buttonType = _dwStyle&0x000F;
   DWORD buttonStyle = _dwStyle&0xFFF0;

   if ( buttonType == BS_AUTOCHECKBOX )
   {
      _qtd_check->setIcon(QIcon(*pBitmap->toQPixmap()));
   }
   else if ( buttonType == BS_AUTO3STATE )
   {
      _qtd_check->setIcon(QIcon(*pBitmap->toQPixmap()));
   }
   else if ( buttonType == BS_AUTORADIOBUTTON )
   {
      _qtd_radio->setIcon(QIcon(*pBitmap->toQPixmap()));
   }
   else if ( (buttonType == BS_PUSHBUTTON) ||
             (buttonType == BS_DEFPUSHBUTTON) )
   {
      _qtd_push->setIcon(QIcon(*pBitmap->toQPixmap()));
   }
   
   return (HBITMAP)0;
}

void CButton::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned
)
{
   DWORD buttonType = _dwStyle&0x000F;
   DWORD buttonStyle = _dwStyle&0xFFF0;

   if ( buttonType == BS_AUTOCHECKBOX )
   {      
      _qtd_check->setText(QString::number(nValue));
   }
   else if ( buttonType == BS_AUTO3STATE )
   {
      _qtd_check->setText(QString::number(nValue));
   }
   else if ( buttonType == BS_AUTORADIOBUTTON )
   {
      _qtd_radio->setText(QString::number(nValue));
   }
   else if ( (buttonType == BS_PUSHBUTTON) ||
             (buttonType == BS_DEFPUSHBUTTON) )
   {
      _qtd_push->setText(QString::number(nValue));
   }
   else if ( buttonType == BS_GROUPBOX )
   {
      _qtd_groupbox->setTitle(QString::number(nValue));
   }   
}

UINT CButton::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   DWORD buttonType = _dwStyle&0x000F;
   DWORD buttonStyle = _dwStyle&0xFFF0;

   if ( lpTrans )
   {
      (*lpTrans) = 1;
   }
   if ( buttonType == BS_AUTOCHECKBOX )
   {
      return _qtd_check->text().toInt();
   }
   else if ( buttonType == BS_AUTO3STATE )
   {
      return _qtd_check->text().toInt();
   }
   else if ( buttonType == BS_AUTORADIOBUTTON )
   {
      return _qtd_radio->text().toInt();
   }
   else if ( (buttonType == BS_PUSHBUTTON) ||
             (buttonType == BS_DEFPUSHBUTTON) )
   {
      return _qtd_push->text().toInt();
   }
   else if ( buttonType == BS_GROUPBOX )
   {
      return _qtd_groupbox->title().toInt();
   }
   if ( lpTrans )
   {
      (*lpTrans) = 0;
   }
   return 0;
}

void CButton::SetDlgItemText(
   int nID,
   LPCTSTR lpszString
)
{
   DWORD buttonType = _dwStyle&0x000F;
   DWORD buttonStyle = _dwStyle&0xFFF0;

   if ( buttonType == BS_AUTOCHECKBOX )
   {
#if UNICODE
      _qtd_check->setText(QString::fromWCharArray(lpszString));
#else
      _qtd_check->setText(QString::fromLatin1(lpszString));
#endif
   }
   else if ( buttonType == BS_AUTO3STATE )
   {
#if UNICODE
      _qtd_check->setText(QString::fromWCharArray(lpszString));
#else
      _qtd_check->setText(QString::fromLatin1(lpszString));
#endif
   }
   else if ( buttonType == BS_AUTORADIOBUTTON )
   {
#if UNICODE
      _qtd_radio->setText(QString::fromWCharArray(lpszString));
#else
      _qtd_radio->setText(QString::fromLatin1(lpszString));
#endif
   }
   else if ( (buttonType == BS_PUSHBUTTON) ||
             (buttonType == BS_DEFPUSHBUTTON) )
   {
#if UNICODE
      _qtd_push->setText(QString::fromWCharArray(lpszString));
#else
      _qtd_push->setText(QString::fromLatin1(lpszString));
#endif
   }
   else if ( buttonType == BS_GROUPBOX )
   {
#if UNICODE
      _qtd_groupbox->setTitle(QString::fromWCharArray(lpszString));
#else
      _qtd_groupbox->setTitle(QString::fromLatin1(lpszString));
#endif
   }   
}

int CButton::GetDlgItemText(
   int nID,
   CString& rString
) const
{
   DWORD buttonType = _dwStyle&0x000F;
   DWORD buttonStyle = _dwStyle&0xFFF0;

   if ( buttonType == BS_AUTOCHECKBOX )
   {
      rString = _qtd_check->text();
      return _qtd_check->text().length();
   }
   else if ( buttonType == BS_AUTO3STATE )
   {
      rString = _qtd_check->text();
      return _qtd_check->text().length();
   }
   else if ( buttonType == BS_AUTORADIOBUTTON )
   {
      rString = _qtd_radio->text();
      return _qtd_radio->text().length();
   }
   else if ( (buttonType == BS_PUSHBUTTON) ||
             (buttonType == BS_DEFPUSHBUTTON) )
   {
      rString = _qtd_push->text();
      return _qtd_push->text().length();
   }
   else if ( buttonType == BS_GROUPBOX )
   {
      rString = _qtd_groupbox->title();
      return _qtd_groupbox->title().length();
   }
   return 0;
}

int CButton::GetDlgItemText(
   int nID,
   LPTSTR lpStr,
   int nMaxCount
) const
{
   DWORD buttonType = _dwStyle&0x000F;
   DWORD buttonStyle = _dwStyle&0xFFF0;

   if ( buttonType == BS_AUTOCHECKBOX )
   {
#if UNICODE
      wcsncpy(lpStr,(LPWSTR)_qtd_check->text().unicode(),nMaxCount);
#else
      strncpy(lpStr,_qtd_check->text().toLatin1().constData(),nMaxCount);
#endif
      return _qtd_check->text().length();
   }
   else if ( buttonType == BS_AUTO3STATE )
   {
#if UNICODE
      wcsncpy(lpStr,(LPWSTR)_qtd_check->text().unicode(),nMaxCount);
#else
      strncpy(lpStr,_qtd_check->text().toLatin1().constData(),nMaxCount);
#endif
      return _qtd_check->text().length();
   }
   else if ( buttonType == BS_AUTORADIOBUTTON )
   {
#if UNICODE
      wcsncpy(lpStr,(LPWSTR)_qtd_radio->text().unicode(),nMaxCount);
#else
      strncpy(lpStr,_qtd_radio->text().toLatin1().constData(),nMaxCount);
#endif
      return _qtd_radio->text().length();
   }
   else if ( (buttonType == BS_PUSHBUTTON) ||
             (buttonType == BS_DEFPUSHBUTTON) )
   {
#if UNICODE
      wcsncpy(lpStr,(LPWSTR)_qtd_push->text().unicode(),nMaxCount);
#else
      strncpy(lpStr,_qtd_push->text().toLatin1().constData(),nMaxCount);
#endif
      return _qtd_push->text().length();
   }
   else if ( buttonType == BS_GROUPBOX )
   {
#if UNICODE
      wcsncpy(lpStr,(LPWSTR)_qtd_groupbox->title().unicode(),nMaxCount);
#else
      strncpy(lpStr,_qtd_groupbox->title().toLatin1().constData(),nMaxCount);
#endif
      return _qtd_groupbox->title().length();
   }
   return 0;
}

void CButton::CheckDlgButton(
   int nIDButton,
   UINT nCheck
)
{
   DWORD buttonType = _dwStyle&0x000F;
   DWORD buttonStyle = _dwStyle&0xFFF0;

   if ( buttonType == BS_AUTOCHECKBOX )
   {      
      _qtd_check->setChecked(nCheck);
   }
   else if ( buttonType == BS_AUTO3STATE )
   {
      _qtd_check->setChecked(nCheck);
   }
   else if ( buttonType == BS_AUTORADIOBUTTON )
   {
      _qtd_radio->setChecked(nCheck);
   }
   else if ( (buttonType == BS_PUSHBUTTON) ||
             (buttonType == BS_DEFPUSHBUTTON) )
   {
      _qtd_push->setChecked(nCheck);
   }
   else if ( buttonType == BS_GROUPBOX )
   {
      _qtd_groupbox->setChecked(nCheck);
   }   
}

UINT CButton::IsDlgButtonChecked(
   int nIDButton
) const
{
   DWORD buttonType = _dwStyle&0x000F;
   DWORD buttonStyle = _dwStyle&0xFFF0;

   if ( buttonType == BS_AUTOCHECKBOX )
   {
      return _qtd_check->isChecked();
   }
   else if ( buttonType == BS_AUTO3STATE )
   {
      return _qtd_check->isChecked();
   }
   else if ( buttonType == BS_AUTORADIOBUTTON )
   {
      return _qtd_radio->isChecked();
   }
   else if ( (buttonType == BS_PUSHBUTTON) ||
             (buttonType == BS_DEFPUSHBUTTON) )
   {
      return _qtd_push->isChecked();
   }
   else if ( buttonType == BS_GROUPBOX )
   {
      return _qtd_groupbox->isChecked();
   }
   return 0;
}

IMPLEMENT_DYNAMIC(CSpinButtonCtrl,CWnd)

BEGIN_MESSAGE_MAP(CSpinButtonCtrl,CWnd)
END_MESSAGE_MAP()

CSpinButtonCtrl::CSpinButtonCtrl(CWnd* parent)
   : CWnd(parent),
     _oldValue(0),
     _curPos(0)
{
}

CSpinButtonCtrl::~CSpinButtonCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CSpinButtonCtrl::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CSpinButtonCtrl*>(this));
   widget->setParent(NULL);
}

bool CSpinButtonCtrl::eventFilter(QObject *object, QEvent *event)
{
   if ( object == _qt )
   {
      if ( event->type() == QEvent::Close )
      {
         closeEvent(dynamic_cast<QCloseEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::Show )
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
         return false;
      }
      if ( event->type() == QEvent::FocusIn )
      {
         focusInEvent(dynamic_cast<QFocusEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::FocusOut )
      {
         focusOutEvent(dynamic_cast<QFocusEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Leave )
      {
         leaveEvent(event);
         return true;
      }
      if ( event->type() == QEvent::MouseButtonPress )
      {
         mousePressEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::MouseButtonRelease )
      {
         mouseReleaseEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::MouseButtonDblClick )
      {
         mouseDoubleClickEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::MouseMove )
      {
         mouseMoveEvent(dynamic_cast<QMouseEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Wheel )
      {
         wheelEvent(dynamic_cast<QWheelEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::Resize )
      {
         resizeEvent(dynamic_cast<QResizeEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::KeyPress )
      {
         keyPressEvent(dynamic_cast<QKeyEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::KeyRelease )
      {
         keyReleaseEvent(dynamic_cast<QKeyEvent*>(event));
         return false;
      }
      if ( event->type() == QEvent::ContextMenu )
      {
         contextMenuEvent(dynamic_cast<QContextMenuEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::DragEnter )
      {
         dragEnterEvent(dynamic_cast<QDragEnterEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::DragMove )
      {
         dragMoveEvent(dynamic_cast<QDragMoveEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::Drop )
      {
         dropEvent(dynamic_cast<QDropEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::DragLeave )
      {
         dragLeaveEvent(dynamic_cast<QDragLeaveEvent*>(event));
         return true;
      }
      if ( event->type() == QEvent::Timer )
      {
         timerEvent(dynamic_cast<QTimerEvent*>(event));
         return true;
      }
   }
//   qDebug("eventFilter: unhandled %d object %s", event->type(), object->objectName().toLatin1().constData());
   return false;
}

void CSpinButtonCtrl::updateFromBuddy()
{
   CString lpszString;
   mfcBuddy()->GetDlgItemText(mfcBuddy()->GetDlgCtrlID(),lpszString);
//   _qtd->blockSignals(true);
//#if UNICODE
//   _qtd->lineEdit()->setText(QString::fromWCharArray((LPCTSTR)lpszString));
//#else
//   _qtd->lineEdit()->setText(QString::fromLatin1((LPCTSTR)lpszString));
//#endif
//   _qtd->blockSignals(true);
   QString val;
#if UNICODE
   val = QString::fromWCharArray(lpszString);
#else
   val = QString::fromLatin1(lpszString);
#endif
   _qtd->blockSignals(true);
   bool ok;
   val.toInt(&ok);
   if ( ok )
   {
      _qtd->setValue(val.toInt());
#if UNICODE
      _qtd->lineEdit()->setText(QString::fromWCharArray(lpszString));
      _oldValue = QString::fromWCharArray(lpszString).toInt();
#else
      _qtd->lineEdit()->setText(QString::fromLatin1(lpszString));
      _oldValue = QString::fromLatin1(lpszString).toInt();
#endif
   }
   _qtd->blockSignals(false);
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
   
   _dwStyle = dwStyle;

   if ( _qt )
      delete _qt;

   _grid = NULL;
   
   _qt = new QSpinBox_MFC(pParentWnd->toQWidget());

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QSpinBox_MFC*>(_qt);

   _qtd->setMouseTracking(true);
   _qtd->setKeyboardTracking(false);
   _qtd->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
   _qtd->setRange(-65535,65536);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif
   _qtd->setVisible(dwStyle&WS_VISIBLE);

   // Figure out if we need to buddy-up.
   if ( dwStyle&UDS_AUTOBUDDY )
   {
      // Check each widget that might be near me.
      foreach ( CWnd* pWnd, *(GetParent()->mfcToQtWidgetMap()) )
      {
         // Do I overlap?  If so I'm auto-buddying.
         // Cheat a bit on the overlap by inflating the spin button a pixel.
         if ( geometry().adjusted(-1,-1,1,1).intersects(pWnd->toQWidget()->geometry()) )
         {
            // But only to CEdit...
            if ( dynamic_cast<CEdit*>(pWnd) )
            {
               pWnd->setMfcBuddy(this);
               setMfcBuddy(pWnd);
               
               QRect wndRect = pWnd->toQWidget()->geometry();
               _qtd->lineEdit()->setValidator(NULL);
               _qtd->setGeometry(wndRect.adjusted(wndRect.width()-((rect.right-rect.left)+1),0,0,0));
               pWnd->setGeometry(pWnd->geometry().adjusted(0,0,-(rect.right-rect.left),0));
            }
         }
      }
   }
   
   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int)));
   
   SetParent(pParentWnd);
   
   qtToMfcWindow.insert(_qtd,this);

   return TRUE;
}

void CSpinButtonCtrl::valueChanged(int value)
{
   NMUPDOWN nmud;
   
   // We need to pretend we're seeing this before the value's actually changed in the control.
   _qtd->blockSignals(true);
   _qtd->setValue(_oldValue);
   _qtd->blockSignals(false);
   
   nmud.hdr.hwndFrom = m_hWnd;
   nmud.hdr.idFrom = _id;
   nmud.hdr.code = UDN_DELTAPOS;
   nmud.iPos = value;
   nmud.iDelta = ((_oldValue-value)>0)?1:-1;
   
   GetOwner()->SendMessage(WM_NOTIFY,_id,(LPARAM)&nmud);
   
   // We need to pretend we're seeing this before the value's actually changed in the control.
   _qtd->blockSignals(true);
   _qtd->setValue(value);
   _qtd->blockSignals(false);
   _oldValue = value;
   
   if ( _dwStyle&UDS_SETBUDDYINT )
   {
      if ( mfcBuddy() )
      {
         mfcBuddy()->updateFromBuddy();
      }
   }
}

int CSpinButtonCtrl::SetPos(
   int nPos
)
{
   int pos = _qtd->value();
   _qtd->blockSignals(true);
   _oldValue = pos;
   _curPos = pos;
   _qtd->setValue(nPos);
   _qtd->blockSignals(false);
   
   if ( _dwStyle&UDS_SETBUDDYINT )
   {
      if ( mfcBuddy() )
      {
         mfcBuddy()->updateFromBuddy();
      }
   }
   
   return pos;
}

int CSpinButtonCtrl::GetPos( ) const
{
   return _curPos;
}

void CSpinButtonCtrl::SetRange(
   short nLower,
   short nUpper
)
{
   int val = _qtd->value();
   _qtd->blockSignals(true);
   _qtd->setRange(nLower,nUpper);
   _qtd->blockSignals(false);
   if ( val < nLower )
   {
      SetPos(nLower);
   }
   if ( val > nUpper )
   {
      SetPos(nUpper);
   }
}

void CSpinButtonCtrl::SetWindowText(
   LPCTSTR lpszString
)
{
   QString val;
#if UNICODE
   val = QString::fromWCharArray(lpszString);
#else
   val = QString::fromLatin1(lpszString);
#endif
   _qtd->blockSignals(true);
   bool ok;
   val.toInt(&ok);
   if ( ok )
   {
      _qtd->setValue(val.toInt());
#if UNICODE
      _qtd->lineEdit()->setText(QString::fromWCharArray(lpszString));
      _oldValue = QString::fromWCharArray(lpszString).toInt();
#else
      _qtd->lineEdit()->setText(QString::fromLatin1(lpszString));
      _oldValue = QString::fromLatin1(lpszString).toInt();
#endif
   }
   _qtd->blockSignals(false);
   
   if ( _dwStyle&UDS_SETBUDDYINT )
   {
      if ( mfcBuddy() )
      {
         mfcBuddy()->updateFromBuddy();
      }
   }
}

void CSpinButtonCtrl::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned
)
{
   _qtd->blockSignals(true);
   _qtd->setValue(nValue);
   _oldValue = nValue;
   _qtd->blockSignals(false);
   
   if ( _dwStyle&UDS_SETBUDDYINT )
   {
      if ( mfcBuddy() )
      {
         mfcBuddy()->updateFromBuddy();
      }
   }
}

UINT CSpinButtonCtrl::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   if ( lpTrans )
   {
      // Check for numeric...
      (*lpTrans) = TRUE;
      
      QString text = _qtd->lineEdit()->text();
      if ( !text.remove(QRegExp("[0-9]*")).isEmpty() )
      {
         (*lpTrans) = FALSE;
      }
   }
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
   val = QString::fromLatin1(lpszString);
#endif
   _qtd->blockSignals(true);
   bool ok;
   val.toInt(&ok);
   if ( ok )
   {
      _qtd->setValue(val.toInt());
      _qtd->lineEdit()->setText(val);
      _oldValue = val.toInt();
   }
   _qtd->blockSignals(false);
   
   if ( _dwStyle&UDS_SETBUDDYINT )
   {
      if ( mfcBuddy() )
      {
         mfcBuddy()->updateFromBuddy();
      }
   }
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
   strncpy(lpStr,_qtd->text().toLatin1().constData(),nMaxCount);
#endif
   return _qtd->text().length();
}

IMPLEMENT_DYNAMIC(CSliderCtrl,CWnd)

BEGIN_MESSAGE_MAP(CSliderCtrl,CWnd)
END_MESSAGE_MAP()

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
   _qtd->setMouseTracking(true);

   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int)));
}

CSliderCtrl::~CSliderCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CSliderCtrl::valueChanged(int value)
{
   if ( _dwStyle&TBS_VERT )
   {
      GetOwner()->PostMessage(WM_VSCROLL,(value<<16)|(SB_VERT|SB_THUMBTRACK),(LPARAM)m_hWnd);
   }
   else
   {
      GetOwner()->PostMessage(WM_HSCROLL,(value<<16)|(SB_HORZ|SB_THUMBTRACK),(LPARAM)m_hWnd);
   }
}

void CSliderCtrl::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CSliderCtrl*>(this));
   widget->setParent(NULL);
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
   
   _dwStyle = dwStyle;
   
   SetParent(pParentWnd);
   
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

   _qtd->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
   _qtd->setVisible(dwStyle&WS_VISIBLE);
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif

   qtToMfcWindow.insert(_qtd,this);

   return TRUE;
}

void CSliderCtrl::SetRange(
   short nLower,
   short nUpper
)
{
   _qtd->blockSignals(true);
   _qtd->setRange(nLower,nUpper);
   _qtd->blockSignals(false);
}

void CSliderCtrl::SetRangeMax(
   int nMax,
   BOOL bRedraw
)
{
   _qtd->blockSignals(true);
   _qtd->setMaximum(nMax);
   _qtd->blockSignals(false);
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

int CSliderCtrl::SetPageSize(
   int nSize
)
{
   int step = _qtd->pageStep();
   _qtd->setPageStep(nSize);
   return step;
}

void CSliderCtrl::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned
)
{
   _qtd->setValue(nValue);
}

UINT CSliderCtrl::GetDlgItemInt(
   int nID,
   BOOL* lpTrans,
   BOOL bSigned
) const
{
   return _qtd->value();
}

void CSliderCtrl::SetDlgItemText(
   int nID,
   LPCTSTR lpszString
)
{
   QString val;
#if UNICODE
   val = QString::fromWCharArray(lpszString);
#else
   val = QString::fromLatin1(lpszString);
#endif
   _qtd->setValue(val.toInt());
}

int CSliderCtrl::GetDlgItemText(
   int nID,
   CString& rString
) const
{
   QString value = QString::number(_qtd->value());
   rString = value;
   return value.length();
}

int CSliderCtrl::GetDlgItemText(
   int nID,
   LPTSTR lpStr,
   int nMaxCount
) const
{
   QString value = QString::number(_qtd->value());
#if UNICODE
   wcsncpy(lpStr,value.unicode(),nMaxCount);
#else
   strncpy(lpStr,value.toLatin1().constData(),nMaxCount);
#endif
   return value.length();
}

IMPLEMENT_DYNAMIC(CProgressCtrl,CWnd)

BEGIN_MESSAGE_MAP(CProgressCtrl,CWnd)
END_MESSAGE_MAP()

CProgressCtrl::CProgressCtrl(CWnd* parent)
   : CWnd(parent)
{
}

CProgressCtrl::~CProgressCtrl()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CProgressCtrl::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   Create(widget->GetStyle(),rect,widget->GetParent(),nID);
   _qt->installEventFilter(dynamic_cast<CProgressCtrl*>(this));
   widget->setParent(NULL);
}

BOOL CProgressCtrl::Create( 
   DWORD dwStyle, 
   const RECT& rect, 
   CWnd* pParentWnd, 
   UINT nID  
)
{
   if ( _qt )
      delete _qt;

   _grid = NULL;

   if ( pParentWnd )
      _qt = new QProgressBar(pParentWnd->toQWidget());
   else
      _qt = new QProgressBar();

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QProgressBar*>(_qt);

   // Not sure if there's vertical sliders in MFC...
   _qtd->setOrientation(Qt::Horizontal);
   _qtd->setMouseTracking(true);
   _qtd->setGeometry(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top));
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif

   // Pass-through signals
   
   qtToMfcWindow.insert(_qtd,this);
   
   return TRUE;
}

void CProgressCtrl::SetRange(
   short nLower,
   short nUpper
)
{
   _qtd->blockSignals(true);
   _qtd->setRange(nLower,nUpper);
   _qtd->blockSignals(false);
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

QLabel_MFC::~QLabel_MFC()
{
   _mfc = NULL;
}

void QLabel_MFC::paintEvent(QPaintEvent *event)
{
   CDC* pDC = _mfc?_mfc->GetDC():NULL;
   if ( _mfc )
   {
      QString style;
      AFX_CTLCOLOR ctlColor;
      ctlColor.hWnd = (HWND)_mfc;
      ctlColor.hDC = (HDC)pDC;
      ctlColor.nCtlType = 0;
      _mfc->SendMessage(WM_ERASEBKGND,(WPARAM)(HDC)pDC);
      _mfc->SendMessage(WM_CTLCOLOR+WM_REFLECT_BASE,0,(LPARAM)&ctlColor);
      style.sprintf("QLabel { color: #%02x%02x%02x; }",GetRValue(pDC->GetTextColor()),GetGValue(pDC->GetTextColor()),GetBValue(pDC->GetTextColor()));
      setStyleSheet(style);
   }
   QLabel::paintEvent(event);
   if ( _mfc )
   {
      _mfc->ReleaseDC(pDC);
      _mfc->SendMessage(WM_PAINT);
   }
}

IMPLEMENT_DYNAMIC(CStatic,CWnd)

BEGIN_MESSAGE_MAP(CStatic,CWnd)
   ON_WM_DRAWITEM()
END_MESSAGE_MAP()

CStatic::CStatic(CWnd *parent)
   : CWnd(parent)
{
}

CStatic::~CStatic()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CStatic::subclassWidget(int nID,CWnd* widget)
{
   CRect rect;
   widget->GetWindowRect(&rect);
   CString text;
   widget->GetWindowText(text);
   Create(text,widget->GetStyle(),rect,widget->GetParent(),nID);
   _qtd->setMfc(this);
   _qt->installEventFilter(dynamic_cast<CStatic*>(this));
   widget->setParent(NULL);
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

   _dwStyle = dwStyle;
   
   if ( _qt )
      delete _qt;

   _grid = NULL;

   if ( pParentWnd )
      _qt = new QLabel_MFC(pParentWnd->toQWidget());
   else
      _qt = new QLabel_MFC;

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QLabel_MFC*>(_qt);
   _qtd->setMouseTracking(true);
   _qtd->setGeometry(QRect(rect.left,rect.top,(rect.right-rect.left),(rect.bottom-rect.top)));
#if (defined(Q_OS_WIN)||defined(Q_OS_WIN32))
   _qtd->setFont(QFont("MS Shell Dlg",8));
#else
   _qtd->setFont(QFont("MS Shell Dlg",9));
#endif

   if ( dwStyle&SS_SUNKEN )
   {
      _qtd->setFrameShape(QFrame::Panel);
      _qtd->setFrameShadow(QFrame::Sunken);
      _qtd->setLineWidth(1);
      _frameWidth = 1;
   }
//   if ( dwStyle&SS_OWNERDRAW )
//   {
//      _qtd->setAttribute(Qt::WA_OpaquePaintEvent, true);
//   }
   if ( (dwStyle&SS_LEFTNOWORDWRAP) != SS_LEFTNOWORDWRAP )
   {
      _qtd->setWordWrap(true);      
   }
   else if ( dwStyle&SS_RIGHT )
   {
      _qtd->setAlignment(Qt::AlignRight);
      if ( dwStyle&SS_RIGHTJUST )
      {
         _qtd->setAlignment(Qt::AlignJustify|Qt::AlignRight);
      }
   }
   else if ( dwStyle&SS_CENTER )
   {
      _qtd->setAlignment(Qt::AlignCenter);
   }
   else // SS_LEFT == 0
   {
      _qtd->setAlignment(Qt::AlignLeft);
   }
   
   // CP: Should really handle SS_BITMAP, SS_ICON, and SS_REALSIZEIMAGE here
   // but instead I just call CStatic::SetBitmap during resource construction.

#if UNICODE
   _qtd->setText(QString::fromWCharArray(lpszText));
#else
   _qtd->setText(QString::fromLatin1(lpszText));
#endif

   // Pass-through signals
   
   qtToMfcWindow.insert(_qtd,this);

   return TRUE;
}

void CStatic::DrawItem(
   LPDRAWITEMSTRUCT lpDrawItemStruct
)
{
}

void CStatic::OnDrawItem(
   int nIDCtl,
   LPDRAWITEMSTRUCT lpDrawItemStruct
)
{
   DrawItem(lpDrawItemStruct);
}

HBITMAP CStatic::SetBitmap(
   HBITMAP hBitmap
)
{
   CBitmap* pBitmap = (CBitmap*)hBitmap;
   _qtd->setPixmap(*pBitmap->toQPixmap());
   if ( _dwStyle&SS_REALSIZEIMAGE )
   {
      _qtd->setFixedSize(pBitmap->toQPixmap()->size());
   }      
   
   return (HBITMAP)0;
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
   _qtd->setText(QString::fromLatin1(lpszString));
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
   strncpy(lpStr,_qtd->text().toLatin1().constData(),nMaxCount);
#endif
   return _qtd->text().length();
}

int CStatic::GetWindowTextLength( ) const
{
   return _qtd->text().length();
}

void CStatic::GetWindowText(
   CString& rString
) const
{
   rString = _qtd->text();
}

int CStatic::GetWindowText(
   LPTSTR lpszStringBuf,
   int nMaxCount
) const
{
#if UNICODE
   wcsncpy(lpszStringBuf,(LPTSTR)_qtd->text().unicode(),nMaxCount);
   return wcslen(lpszStringBuf);
#else
   strncpy(lpszStringBuf,(LPTSTR)_qtd->text().toLatin1().constData(),nMaxCount);
   return strlen(lpszStringBuf);
#endif
}

void CStatic::SetWindowText(
   LPCTSTR lpszString
)
{
#if UNICODE
   QString str = QString::fromWCharArray(lpszString);
#else
   QString str = QString::fromLatin1(lpszString);
#endif
   _qt->setWindowTitle(str);
}

IMPLEMENT_DYNAMIC(CFileDialog,CCommonDialog)

BEGIN_MESSAGE_MAP(CFileDialog,CCommonDialog)
END_MESSAGE_MAP()

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
   m_hWnd = (HWND)this;

   m_pOFN = &m_ofn;
   memset(m_pOFN,0,sizeof(OPENFILENAME));

   if ( _qt )
      delete _qt;

   if ( pParentWnd )
      _qt = new QFileDialog(pParentWnd->toQWidget());
   else
      _qt = new QFileDialog;

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QFileDialog*>(_qt);

   // Pass-through signals

   _qtd->setMouseTracking(true);
   _qtd->setOption(QFileDialog::DontUseNativeDialog);

#if UNICODE
   _qtd->setDefaultSuffix(QString::fromWCharArray(lpszDefExt));
   _qtd->selectFile(QString::fromWCharArray(lpszFileName));
#else
   _qtd->setDefaultSuffix(QString::fromLatin1(lpszDefExt));
   _qtd->selectFile(QString::fromLatin1(lpszFileName));
#endif

   translateFilters(lpszFilter);

   qDebug("CFileDialog::CFileDialog...need dwFlags impl");
   if ( dwFlags&OFN_ALLOWMULTISELECT )
   {
      _qtd->setFileMode(QFileDialog::ExistingFiles);
   }
   if ( dwFlags&OFN_OVERWRITEPROMPT )
   {
      _qtd->setConfirmOverwrite(true);
   }
   if ( dwFlags&OFN_HIDEREADONLY )
   {
      _qtd->setOption(QFileDialog::ReadOnly,false);
   }
   if ( dwFlags&OFN_FILEMUSTEXIST )
   {
      _qtd->setFileMode(QFileDialog::ExistingFile);
   }
   if ( dwFlags&OFN_PATHMUSTEXIST )
   {
   }
   if ( dwFlags&OFN_EXPLORER )
   {
   }
   if ( bOpenFileDialog )
   {
      _qtd->setAcceptMode(QFileDialog::AcceptOpen);
      m_ofn.lpstrTitle = _T("Open");
   }
   else
   {
      _qtd->setAcceptMode(QFileDialog::AcceptSave);
      m_ofn.lpstrTitle = _T("Save");
   }
}

CFileDialog::~CFileDialog()
{
   if ( _qtd )
      delete _qtd;
   _qtd = NULL;
   _qt = NULL;
}

void CFileDialog::translateFilters(LPCTSTR lpszFilter)
{
   int seg;

   if ( lpszFilter )
   {
#if UNICODE
      QStringList filtersList = QString::fromWCharArray(lpszFilter).split(QRegExp("(\||\(|\))"),QString::SkipEmptyParts);
#else
      QStringList filtersList = QString::fromLatin1(lpszFilter).split(QRegExp("[|()]"),QString::SkipEmptyParts);
#endif

      // Take out extra filter patterns, 'empty' strings, and convert |'s to ;;'s.
      for ( seg = filtersList.count()-2; seg > 0; seg-- )
      {
         if ( filtersList.at(seg) == filtersList.at(seg+1) )
         {
            filtersList.removeAt(seg+1);
         }
         if ( filtersList.at(seg).startsWith(" ") )
         {
            filtersList.removeAt(seg);
         }
      }
      // Add all files filter pattern if it's not there
      for ( seg = 0; seg < filtersList.count(); seg++ )
      {
         if ( !filtersList.at(seg).compare("all files",Qt::CaseInsensitive) )
         {
            if ( seg == filtersList.count()-1 )
            {
               filtersList.insert(seg+1,"*.*");
            }
         }
      }
      // Add ()'s back to filter pattern parts
      for ( seg = 1; seg < filtersList.count(); seg += 2 )
      {
         filtersList[seg].prepend("(");
         filtersList[seg].append(")");
      }
      // Join each two-part filter string together
      for ( seg = filtersList.count()-2; seg >= 0; seg -= 2 )
      {
         QString b = filtersList.takeAt(seg+1);
         QString a = filtersList.takeAt(seg);
         filtersList.append(a+" "+b);
      }

      _qtd->setNameFilters(filtersList);
   }
}

INT_PTR CFileDialog::DoModal()
{
   INT_PTR result;

   SetWindowText(m_ofn.lpstrTitle);
#if UNICODE
   if ( m_ofn.lpstrInitialDir )
      _qtd->setDirectory(QString::fromWCharArray((LPCTSTR)m_ofn.lpstrInitialDir));
#else
   if ( m_ofn.lpstrInitialDir )
      _qtd->setDirectory(QString::fromLatin1((LPCTSTR)m_ofn.lpstrInitialDir));
#endif

   translateFilters(m_ofn.lpstrFilter);

   result = _qtd->exec();
   if ( result == QDialog::Accepted )
   {
      if ( m_ofn.lpstrFile )
      {
#if UNICODE
         wcscpy(m_ofn.lpstrFile,_qtd->selectedFiles().at(0).unicode());
#else
         strcpy(m_ofn.lpstrFile,_qtd->selectedFiles().at(0).toLatin1().constData());
#endif
      }
      return IDOK;
   }
   else
   {
      return IDCANCEL;
   }
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
   if ( !pos )
   {
      return CString(); // Choker for end-of-list
   }
   QStringList files = _qtd->selectedFiles();
   CString file = files.at((*pos)++);
   if ( (*pos) >= files.count() )
   {
      delete pos;
      pos = NULL;
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

IMPLEMENT_DYNAMIC(CColorDialog,CCommonDialog)

BEGIN_MESSAGE_MAP(CColorDialog,CCommonDialog)
END_MESSAGE_MAP()

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

   _qtd->setMouseTracking(true);
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
      return IDOK;
   else
      return IDCANCEL;
}

COLORREF CColorDialog::GetColor( ) const
{
   QColor col = _qtd->selectedColor();
   COLORREF ret = RGB(col.red(),col.green(),col.blue());
   return ret;
}

IMPLEMENT_DYNAMIC(COleDataSource,CCmdTarget)

BEGIN_MESSAGE_MAP(COleDataSource,CCmdTarget)
END_MESSAGE_MAP()

void COleDataSource::CacheGlobalData( 
   CLIPFORMAT cfFormat, 
   HGLOBAL hGlobal, 
   LPFORMATETC lpFormatEtc
)
{
   qDebug("COleDataSource::CacheGlobalData?");
}

DROPEFFECT COleDataSource::DoDragDrop( 
   DWORD dwEffects, 
   LPCRECT lpRectStartDrag,
   COleDropSource* pDropSource
)
{
   DROPEFFECT eff = DROPEFFECT_NONE;
   if ( QApplication::mouseButtons()&Qt::LeftButton )
   {
      eff = DROPEFFECT_COPY;      
   }
   return eff;
}

IMPLEMENT_DYNAMIC(COleDropSource,CCmdTarget)

BEGIN_MESSAGE_MAP(COleDropSource,CCmdTarget)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(COleDropTarget,CCmdTarget)

BEGIN_MESSAGE_MAP(COleDropTarget,CCmdTarget)
END_MESSAGE_MAP()

BOOL COleDropTarget::Register( 
   CWnd* pWnd  
)
{
   pWnd->toQWidget()->setAcceptDrops(true);
   return TRUE;
}

BOOL COleDataObject::IsDataAvailable( 
   CLIPFORMAT cfFormat, 
   LPFORMATETC lpFormatEtc
)
{
   return IsClipboardFormatAvailable(cfFormat);
}

HGLOBAL COleDataObject::GetGlobalData( 
   CLIPFORMAT cfFormat, 
   LPFORMATETC lpFormatEtc
)
{
   return (HGLOBAL)GetClipboardData(cfFormat);
}

DWORD WINAPI WaitForSingleObject(
   HANDLE hHandle,
   DWORD dwMilliseconds
)
{
   CObject* pObject = (CObject*)hHandle;
   CCmdTarget* pCmdTarget = dynamic_cast<CCmdTarget*>(pObject);
   CWinThread* pWinThread = dynamic_cast<CWinThread*>(pCmdTarget);
   CEvent* pEvent = dynamic_cast<CEvent*>(pObject);
   static QSemaphore* waitingSem = NULL;
   bool timedOut;

   if ( pWinThread && pWinThread->IsKindOf(RUNTIME_CLASS(CWinThread)) )
   {
      timedOut = pWinThread->wait(dwMilliseconds);
      if ( timedOut )
      {
         return WAIT_TIMEOUT;
      }
      else
      {
         return WAIT_OBJECT_0;
      }
   }
   else if ( pEvent && pEvent->IsKindOf(RUNTIME_CLASS(CEvent)) )
   {
      if ( !waitingSem )
      {
         waitingSem = new QSemaphore();
      }
      pEvent->addWaiter(waitingSem);
      timedOut = waitingSem->tryAcquire(1,dwMilliseconds);
      if ( !timedOut )
      {
         return WAIT_TIMEOUT;
      }
      else
      {
         return WAIT_OBJECT_0;
      }
   }
   else
   {
      qDebug("WaitForSingleObject not thread or event");
   }
   return WAIT_FAILED;
}

BOOL WINAPI CloseHandle(
   HANDLE hObject
)
{
   // CP: Nothing to do here?
   return TRUE;
}

IMPLEMENT_DYNAMIC(CSyncObject,CObject)

IMPLEMENT_DYNAMIC(CSemaphore,CSyncObject)

IMPLEMENT_DYNAMIC(CMutex,CSyncObject)

CMutex::CMutex(
   BOOL bInitiallyOwn,
   LPCTSTR lpszName,
   LPSECURITY_ATTRIBUTES lpsaAttribute
)
{
   _qtd = new QMutex(QMutex::Recursive);
   if ( bInitiallyOwn )
      Lock();
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

IMPLEMENT_DYNAMIC(CCriticalSection,CSyncObject)

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

IMPLEMENT_DYNAMIC(CEvent,CSyncObject)

CEvent::CEvent(
   BOOL bInitiallyOwn,
   BOOL bManualReset,
   LPCTSTR lpszName,
   LPSECURITY_ATTRIBUTES lpsaAttribute
)
{
   m_bSignalled = false;
   m_bManualReset = bManualReset;
}

CEvent::~CEvent()
{
}

BOOL CEvent::SetEvent()
{
   _access.lock();
   m_bSignalled = true;   
   foreach ( QSemaphore* waiter, _waiters )
   {
      waiter->release();
      _waiters.removeAll(waiter);
   }
   _access.unlock();
   return TRUE;
}

BOOL CEvent::ResetEvent()
{
   _access.lock();
   m_bSignalled = false;   
   _access.unlock();
   return TRUE;
}

BOOL CEvent::PulseEvent()
{
   _access.lock();
   m_bSignalled = true;   
   foreach ( QSemaphore* waiter, _waiters )
   {
      waiter->release();
      _waiters.removeAll(waiter);
   }
   _access.unlock();
   _access.lock();
   m_bSignalled = false;   
   _access.unlock();
   return TRUE;
}

void CEvent::addWaiter(QSemaphore *waiter)
{ 
   _access.lock();
   _waiters.append(waiter); 
   _access.unlock();
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
      full = QString::fromLatin1(pstrName);
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

IMPLEMENT_DYNAMIC(CImageList,CObject)

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
   int ret = _images.count();
   _images.append(pbmImage);
   return ret;
   // Not sure what to do with mask yet.
}

int CImageList::Add(
   CBitmap* pbmImage,
   COLORREF crMask
)
{
   int ret = _images.count();
   _images.append(pbmImage);
   return ret;
   // Not sure what to do with mask yet.
}

int CImageList::Add(
   HICON hIcon
)
{
   int ret = _images.count();
   CBitmap* pBitmap = (CBitmap*)hIcon;
   _images.append(pBitmap);
   return ret;
}

HICON CImageList::ExtractIcon(
   int nImage
)
{
   return (HICON)_images.at(nImage);
}

IMPLEMENT_DYNAMIC(CPropertySheet,CWnd)

BEGIN_MESSAGE_MAP(CPropertySheet,CWnd)
END_MESSAGE_MAP()

CPropertySheet::CPropertySheet(
   UINT nIDCaption,
   CWnd* pParentWnd,
   UINT iSelectPage
)
{
   CString title;
   title = qtMfcStringResource(nIDCaption);
   _commonConstruct(title,pParentWnd,iSelectPage);
}

CPropertySheet::CPropertySheet(
   LPCTSTR pszCaption,
   CWnd* pParentWnd,
   UINT iSelectPage
)
{
   _commonConstruct(pszCaption,pParentWnd,iSelectPage);
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
   CString title;
   title = qtMfcStringResource(nIDCaption);
   _commonConstruct(title,pParentWnd,iSelectPage);
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
   _commonConstruct(pszCaption,pParentWnd,iSelectPage);
}

void CPropertySheet::_commonConstruct(CString title,CWnd* parent,UINT selectedPage)
{
   if ( _qt )
      delete _qt;

   _qt = new QDialog;

   _qtd = dynamic_cast<QDialog*>(_qt);
   _qtd->setWindowTitle(title);

   _selectedPage = selectedPage;

   _qtd->setMouseTracking(true);
   
   _grid = new QGridLayout(_qtd);
   _qtabwidget = new QTabWidget;
   QObject::connect(_qtabwidget,SIGNAL(currentChanged(int)),this,SLOT(tabWidget_currentChanged(int)));
   _grid->addWidget(_qtabwidget,0,0,1,1);
   _qbuttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel|QDialogButtonBox::Apply,Qt::Horizontal);
   QObject::connect(_qbuttons->button(QDialogButtonBox::Ok),SIGNAL(clicked()),this,SLOT(ok_clicked()));
   QObject::connect(_qbuttons->button(QDialogButtonBox::Cancel),SIGNAL(clicked()),this,SLOT(cancel_clicked()));
   QObject::connect(_qbuttons->button(QDialogButtonBox::Apply),SIGNAL(clicked()),this,SLOT(apply_clicked()));
   _grid->addWidget(_qbuttons,1,0,1,1);
   
   _qbuttons->button(QDialogButtonBox::Apply)->setEnabled(false);

   // Pass-through signals
   QObject::connect(this,SIGNAL(update()),_qtd,SLOT(update()));
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

void CPropertySheet::pageModified()
{
   _qbuttons->button(QDialogButtonBox::Apply)->setEnabled(true);
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
   _qtabwidget->addTab(pPage->toQWidget(),QString::fromLatin1((LPCTSTR)windowText));
#endif
   _qtabwidget->blockSignals(false);
   _pages.append(pPage);

   pPage->OnInitDialog();
   
   QObject::connect(pPage,SIGNAL(setModified()),this,SLOT(pageModified()));
}

INT_PTR CPropertySheet::DoModal( )
{
   INT_PTR result;
   _qtabwidget->setCurrentIndex(_selectedPage);
   _pages.at(_selectedPage)->OnSetActive();
   SetFocus();
   result = _qtd->exec();
   if ( result == QDialog::Accepted )
      return IDOK;
   else
      return IDCANCEL;
}

IMPLEMENT_DYNAMIC(CPropertyPage,CDialog)

BEGIN_MESSAGE_MAP(CPropertyPage,CDialog)
END_MESSAGE_MAP()

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

   _qt = new QDialog_MFC;

   _qtd = dynamic_cast<QDialog_MFC*>(_qt);
   _inited = false;

   _qtd->setMouseTracking(true);
   _qtd->installEventFilter(this);

   // Pass-through signals
   QObject::connect(this,SIGNAL(update()),_qtd,SLOT(update()));
}

CPropertyPage::~CPropertyPage()
{
   if ( _qt )
   {
      if ( _qtd )
         delete _qtd;
      _qtd = NULL;
      _qt = NULL;
   }
}

void CPropertyPage::SetModified(
   BOOL bChanged
)
{
   emit setModified();
}

BOOL CPropertyPage::OnApply( )
{
   return TRUE;
}

BOOL CPropertyPage::OnSetActive( )
{
   return TRUE;
}

IMPLEMENT_DYNAMIC(CToolTipCtrl,CWnd)

BEGIN_MESSAGE_MAP(CToolTipCtrl,CWnd)
END_MESSAGE_MAP()

CToolTipCtrl::CToolTipCtrl( )
{
   // nothing to do here...
}

void CToolTipCtrl::subclassWidget(int nID,CWnd* widget)
{
   Create(widget->GetParent(),widget->GetStyle());
   _qt->installEventFilter(dynamic_cast<CToolTipCtrl*>(this));
   widget->setParent(NULL);
}

BOOL CToolTipCtrl::Create(
   CWnd* pParentWnd,
      DWORD dwStyle
)
{
   // nothing to do here...
   return TRUE;
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
   pWnd->toQWidget()->setToolTip(qtMfcStringResource(nIDText));
   return TRUE;
}

BOOL CToolTipCtrl::AddTool(
   CWnd* pWnd,
   LPCTSTR lpszText,
   LPCRECT lpRectTool,
   UINT_PTR nIDTool
)
{
#if UNICODE
   pWnd->toQWidget()->setToolTip(QString::fromWCharArray(lpszText));
#else
   pWnd->toQWidget()->setToolTip(QString::fromLatin1(lpszText));
#endif
   return TRUE;
}

void CToolTipCtrl::RelayEvent(
   LPMSG lpMsg
)
{
   // nothing to do here...
}

CCmdUI::CCmdUI()
   : m_pMenu(NULL),
     m_pSubMenu(NULL),
     m_pOther(NULL),
     m_nID(0),
     m_nIndex(0),
     m_nIndexMax(0),
     m_bEnableChanged(FALSE),
     m_bContinueRouting(FALSE)
{
}

void CCmdUI::ContinueRouting( )
{
}

BOOL CCmdUI::DoUpdate(CCmdTarget* pTarget, BOOL bDisableIfNoHndler)
{
	if (m_nID == 0 || LOWORD(m_nID) == 0xFFFF)
		return TRUE;     // ignore invalid IDs

	ENSURE_VALID(pTarget);

	m_bEnableChanged = FALSE;
	BOOL bResult = pTarget->OnCmdMsg(m_nID, CN_UPDATE_COMMAND_UI, this, NULL);
	if (!bResult)
		ASSERT(!m_bEnableChanged); // not routed

	if (bDisableIfNoHndler && !m_bEnableChanged)
	{
		AFX_CMDHANDLERINFO info;
		info.pTarget = NULL;
		BOOL bHandler = pTarget->OnCmdMsg(m_nID, CN_COMMAND, this, &info);

      // Enable or Disable based on whether there is a handler there
		Enable(bHandler);
	}
	return bResult;
}

void CCmdUI::Enable(
   BOOL bOn
)
{
   m_bEnableChanged = TRUE;
   if ( m_pOther )
   {
      m_pOther->EnableWindow(bOn);
   }
   else if ( m_pMenu )
   {
      m_pMenu->EnableMenuItem(m_nID,bOn);
   }
   else if ( m_pSubMenu )
   {
      m_pSubMenu->EnableMenuItem(m_nID,bOn);
   }
}

void CCmdUI::SetCheck(
   int nCheck
)
{
   if ( m_pOther )
   {
      m_pOther->CheckDlgButton(m_nID,nCheck);
   }
   else if ( m_pMenu )
   {
      m_pMenu->CheckMenuItem(m_nID,nCheck);
   }
   else if ( m_pSubMenu )
   {
      m_pSubMenu->CheckMenuItem(m_nID,nCheck);
   }
}

void CCmdUI::SetRadio(
   BOOL bOn
)
{
   if ( m_pOther )
   {
      m_pOther->CheckDlgButton(m_nID,bOn);
   }
   else if ( m_pMenu )
   {
      m_pMenu->CheckMenuItem(m_nID,bOn);
   }
   else if ( m_pSubMenu )
   {
      m_pSubMenu->CheckMenuItem(m_nID,bOn);
   }
}

void CCmdUI::SetText(
   LPCTSTR lpszText
)
{
   if ( m_pOther )
   {
      m_pOther->SetDlgItemText(m_nID,lpszText);
   }
   else if ( m_pMenu )
   {
   }
   else if ( m_pSubMenu )
   {
   }
}

void CToolCmdUI::Enable(BOOL bOn)
{
	m_bEnableChanged = TRUE;
	CToolBar* pToolBar = (CToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) & ~TBBS_DISABLED;
	if (!bOn)
	{
		nNewStyle |= TBBS_DISABLED;
		// If a button is currently pressed and then is disabled
		// COMCTL32.DLL does not unpress the button, even after the mouse
		// button goes up!  We work around this bug by forcing TBBS_PRESSED
		// off when a button is disabled.
		nNewStyle &= ~TBBS_PRESSED;
	}
	ASSERT(!(nNewStyle & TBBS_SEPARATOR));
	pToolBar->SetButtonStyle(m_nIndex, nNewStyle);
}

void CToolCmdUI::SetCheck(int nCheck)
{
//	ASSERT(nCheck >= 0 && nCheck <= 2); // 0=>off, 1=>on, 2=>indeterminate
//	CToolBar* pToolBar = (CToolBar*)m_pOther;
//	ASSERT(pToolBar != NULL);
//	ASSERT_KINDOF(CToolBar, pToolBar);
//	ASSERT(m_nIndex < m_nIndexMax);

//	UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) &
//				~(TBBS_CHECKED | TBBS_INDETERMINATE);
//	if (nCheck == 1)
//		nNewStyle |= TBBS_CHECKED;
//	else if (nCheck == 2)
//		nNewStyle |= TBBS_INDETERMINATE;
//	ASSERT(!(nNewStyle & TBBS_SEPARATOR));
//	pToolBar->SetButtonStyle(m_nIndex, nNewStyle | TBBS_CHECKBOX);
}

void CToolCmdUI::SetText(LPCTSTR)
{
	// ignore it
}

void CTestCmdUI::Enable(
   BOOL bOn
)
{
   m_bEnabled = bOn;
}

BOOL CArchive::IsStoring( ) const
{
   return FALSE;
}

CRecentFileList::CRecentFileList( 
   UINT nStart, 
   LPCTSTR lpszSection, 
   LPCTSTR lpszEntryFormat, 
   int nSize, 
   int nMaxDispLen  
)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", gApplicationName);
   QString key;
   int n;
   
#if UNICODE
   _regSection = QString::fromWCharArray(lpszSection);
   _regEntryFormat = QString::fromWCharArray(lpszEntryFormat);
#else
   _regSection = QString::fromLatin1(lpszSection);
   _regEntryFormat = QString::fromLatin1(lpszEntryFormat);
#endif

   _nSize = nSize;

   for ( n = 0; n < nSize; n++ )
   {
      key.sprintf(_regEntryFormat.toLatin1().constData(),n);
      key.prepend(_regSection+"/");
      
      if ( !settings.value(key).toString().isEmpty() )
      {
         if ( !_recentFiles.contains(settings.value(key).toString()) )
         {
            _recentFiles.append(settings.value(key).toString());
         }
      }
   }
}

int CRecentFileList::GetSize() const
{
   return _recentFiles.count();
}

void CRecentFileList::Add( 
   LPCTSTR lpszPathName  
)
{
#if UNICODE
   QString str = QString::fromWCharArray(lpszPathName);
#else
   QString str = QString::fromLatin1(lpszPathName);
#endif
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", gApplicationName);
   QString key;
   QString def;
   int n;
   
   _recentFiles.removeAll(str);
   _recentFiles.prepend(str);
   if ( _recentFiles.count() > _nSize )
   {
      _recentFiles.removeLast();
   }
   
   for ( n = 0; n < _recentFiles.count(); n++ )
   {
      key.sprintf(_regEntryFormat.toLatin1().constData(),n+1);
      key.prepend(_regSection+"/");
         
      def.sprintf(_recentFiles.at(n).toLatin1().constData());
      
      settings.setValue(key,def);   
   }
}

BOOL CRecentFileList::GetDisplayName( 
   CString& strName, 
   int nIndex, 
   LPCTSTR lpszCurDir, 
   int nCurDir, 
   BOOL bAtLeastName 
) const
{
   if ( nIndex < _recentFiles.count() )
   {
      strName = _recentFiles.at(nIndex);
      return TRUE;
   }
   return FALSE;
}

CDataExchange::CDataExchange(
   CWnd* pDlgWnd,
   BOOL bSaveAndValidate
) : m_pDlgWnd(pDlgWnd),
    m_bSaveAndValidate(bSaveAndValidate)
{}

void AFXAPI DDX_Slider(
   CDataExchange* pDX,
   int nIDC,
   int& value
)
{
   if ( pDX->m_bSaveAndValidate )
   {
      value = pDX->m_pDlgWnd->GetDlgItemInt(nIDC);
   }
   else
   {
      pDX->m_pDlgWnd->SetDlgItemInt(nIDC,value);
   }
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
   int ret = 0;

   foreach ( QString family, database.families() )
   {
      memset(&ntme,0,sizeof(ntme));
      memset(&elfe,0,sizeof(ntme));
      strcpy((char*)elfe.elfFullName,family.toLatin1().constData());
      ret = lpEnumFontFamExProc((LOGFONT*)&elfe,NULL,0,lParam);
      if ( !ret ) break;
   }
   return ret;
}

QHash<QFile*,LPMMIOINFO> mmioInfos;

HMMIO mmioOpen(
  LPTSTR szFilename,
  LPMMIOINFO lpmmioinfo,
  DWORD dwOpenFlags
)
{
   QFile* pFile = new QFile;
   QFile::OpenMode mode;
   bool mmioinfoCreated = false;
   
#if UNICODE
   pFile->setFileName(QString::fromWCharArray(szFilename));
#else
   pFile->setFileName(QString::fromLatin1(szFilename));
#endif

   if ( !lpmmioinfo )
   {
      lpmmioinfo = new MMIOINFO;
      memset(lpmmioinfo,0,sizeof(MMIOINFO));
      mmioinfoCreated = true;
   }
   lpmmioinfo->dwFlags = dwOpenFlags;
   
   if ( dwOpenFlags&MMIO_CREATE )
   {
      mode |= QIODevice::Truncate;
   }
   
   if ( dwOpenFlags&MMIO_READWRITE )
   {
      mode |= QIODevice::ReadWrite;
   }
   
   if ( dwOpenFlags&MMIO_ALLOCBUF )
   {
      lpmmioinfo->cchBuffer = MMIO_DEFAULTBUFFER;
      lpmmioinfo->pchBuffer = new char[MMIO_DEFAULTBUFFER];
      lpmmioinfo->pchNext = lpmmioinfo->pchBuffer;
      lpmmioinfo->pchEndRead = lpmmioinfo->pchBuffer+MMIO_DEFAULTBUFFER;
      lpmmioinfo->pchEndWrite = lpmmioinfo->pchBuffer+MMIO_DEFAULTBUFFER;
   }

   bool opened = pFile->open(mode);
   if ( opened )
   {
      lpmmioinfo->hmmio = (HMMIO)pFile;
      mmioInfos.insert(pFile,lpmmioinfo);
      return (HMMIO)pFile;
   }
   delete pFile;
   lpmmioinfo->wErrorRet = MMSYSERR_NODRIVER;
   if ( mmioinfoCreated )
   {
      delete lpmmioinfo;
   }
   return NULL;
}

MMRESULT mmioCreateChunk(
  HMMIO hmmio,
  LPMMCKINFO lpck,
  UINT wFlags
)
{
   QFile* pFile = (QFile*)hmmio;
   const char* data = "RIFF";
   
   if ( pFile->isOpen() )
   {
      lpck->dwFlags = 0;
      
      switch ( wFlags )
      {
      case MMIO_CREATERIFF:
         pFile->write(data,4);
         pFile->write((const char*)&(lpck->cksize),4);
         pFile->write((const char*)&(lpck->fccType),4);
         lpck->dwFlags |= MMIO_CREATERIFF;
         break;
      case MMIO_CREATELIST:
         break;
      default:
         pFile->write((const char*)&(lpck->ckid),4);
         pFile->write((const char*)&(lpck->cksize),4);
         break;
      }
      lpck->dwDataOffset = pFile->pos();
      lpck->dwFlags |= MMIO_DIRTY;
   
      return MMSYSERR_NOERROR;
   }
   else
   {
      return MMSYSERR_INVALHANDLE;
   }
}

LONG mmioWrite(
  HMMIO hmmio,
  char _huge *pch,
  LONG cch
)
{
   QFile* pFile = (QFile*)hmmio;
   
   if ( pFile && pFile->isOpen() )
   {
      pFile->write(pch,cch);
   }
   else
   {
      return MMSYSERR_INVALHANDLE;
   }
   return MMSYSERR_NOERROR;
}

MMRESULT mmioAscend(
  HMMIO hmmio,
  LPMMCKINFO lpck,
  UINT wFlags
)
{
   QFile* pFile = (QFile*)hmmio;
   UINT chunkSize;
   const char pad = 0;
   
   if ( pFile && pFile->isOpen() )
   {
      if ( lpck->dwFlags&MMIO_DIRTY )
      {
         chunkSize = pFile->pos()-lpck->dwDataOffset;
         if ( chunkSize&1 )
            pFile->write(&pad,1);
         if ( lpck->dwFlags&MMIO_CREATERIFF )
         {
            pFile->seek(lpck->dwDataOffset-8);
            pFile->write((const char*)&(chunkSize),4);         
            pFile->seek(lpck->dwDataOffset+chunkSize+(chunkSize&1?1:0));
         }
         else
         {
            pFile->seek(lpck->dwDataOffset-4);
            pFile->write((const char*)&(chunkSize),4);         
            pFile->seek(lpck->dwDataOffset+chunkSize+(chunkSize&1?1:0));
         }
      }
   }
   else
   {
      return MMSYSERR_INVALHANDLE;
   }
   return MMSYSERR_NOERROR;
}

MMRESULT mmioDescend(
  HMMIO hmmio, 
  LPMMCKINFO lpck,
  const MMCKINFO* lpckParent,
  UINT wFlags
)
{
   QFile* pFile = (QFile*)hmmio;
   
   if ( pFile && pFile->isOpen() )
   {
   }
   else
   {
      return MMSYSERR_INVALHANDLE;
   }
   return MMSYSERR_NOERROR;
}

LONG mmioSeek(
  HMMIO hmmio,
  LONG lOffset,
  int iOrigin
)
{
   QFile* pFile = (QFile*)hmmio;

   if ( pFile->isOpen() )
   {
      switch ( iOrigin )
      {
      case SEEK_SET:
         pFile->seek(lOffset);
         break;
      case SEEK_CUR:
         pFile->seek(pFile->pos()+lOffset);
         break;
      case SEEK_END:
         pFile->seek(pFile->size()+lOffset);
         break;
      }
      return pFile->pos();
   }
   else
   {
      return -1;            
   }
}

MMRESULT mmioAdvance(
  HMMIO hmmio,
  LPMMIOINFO lpmmioinfo,
  UINT wFlags
)
{
   QFile* pFile = (QFile*)hmmio;
   
   if ( pFile && pFile->isOpen() )
   {
      switch ( wFlags )
      {
      case MMIO_READ:
         pFile->read(lpmmioinfo->pchBuffer,lpmmioinfo->cchBuffer);
         lpmmioinfo->pchNext = lpmmioinfo->pchBuffer;
         break;
      case MMIO_WRITE:
         pFile->write(lpmmioinfo->pchBuffer,lpmmioinfo->cchBuffer);
         lpmmioinfo->pchNext = lpmmioinfo->pchBuffer;
         break;
      }
      lpmmioinfo->lBufOffset = 0;
      lpmmioinfo->lDiskOffset = pFile->pos();
   }
   else
   {
      return MMSYSERR_INVALHANDLE;
   }
   return MMSYSERR_NOERROR;
}

MMRESULT mmioGetInfo(
  HMMIO hmmio,
  LPMMIOINFO lpmmioinfo,
  UINT wFlags
)
{
   QFile* pFile = (QFile*)hmmio;
   
   if ( pFile && pFile->isOpen() )
   {
      memcpy(lpmmioinfo,mmioInfos.value(pFile),sizeof(MMIOINFO));      
   }
   else
   {
      return MMSYSERR_INVALHANDLE;
   }
   return MMSYSERR_NOERROR;
}

MMRESULT mmioSetInfo(
  HMMIO hmmio,
  LPMMIOINFO lpmmioinfo,
  UINT wFlags
)
{
   QFile* pFile = (QFile*)hmmio;
   
   if ( pFile && pFile->isOpen() )
   {
      mmioInfos.remove(pFile);
      mmioInfos.insert(pFile,lpmmioinfo);
   }
   else
   {
      return MMSYSERR_INVALHANDLE;
   }
   return MMSYSERR_NOERROR;
}

MMRESULT mmioClose(
  HMMIO hmmio,
  UINT wFlags
)
{
   QFile* pFile = (QFile*)hmmio;

   if ( pFile && pFile->isOpen() )
   {
      pFile->close();
      mmioInfos.remove(pFile);
      delete pFile;
      return MMSYSERR_NOERROR;
   }
   return MMSYSERR_INVALHANDLE;
}

#if !(defined(Q_OS_WIN) || defined(Q_OS_WIN32))
VOID WINAPI Sleep(
  DWORD dwMilliseconds
)
{
   QMutex dummy;
   dummy.lock();
   QWaitCondition waitCondition;
   waitCondition.wait(&dummy, dwMilliseconds);
}
#endif

VOID WINAPI ExitProcess(
  UINT uExitCode
)
{
   exit(uExitCode);
}

BOOL WINAPI TrackMouseEvent(
   LPTRACKMOUSEEVENT lpEventTrack
)
{
   // CP: Nothing to do here I think.
}

HINSTANCE ShellExecute(
   HWND hwnd,
   LPCTSTR lpOperation,
   LPCTSTR lpFile,
   LPCTSTR lpParameters,
   LPCTSTR lpDirectory,
   INT nShowCmd
)
{
   QString operation;
   QString file;
   QString parameters;
   QString directory;
#if UNICODE
   operation = QString::fromWCharArray(lpOperation);
   file = QString::fromWCharArray(lpFile);
   parameters = QString::fromWCharArray(lpParameters);
   directory = QString::fromWCharArray(lpDirectory);
#else
   operation = QString::fromLatin1(lpOperation);
   file = QString::fromLatin1(lpFile);
   parameters = QString::fromLatin1(lpParameters);
   directory = QString::fromLatin1(lpDirectory);
#endif
   QStringList args = parameters.split(" ");
   args.prepend(file);
   qDebug("ShellExecute: %s %s %s",operation.toLatin1().data(),args.join(" ").toLatin1().data(),directory.toLatin1().data());
   QProcess::startDetached(operation,args,directory);
}

BOOL WINAPI MoveFileEx(
  LPCTSTR lpExistingFileName,
  LPCTSTR lpNewFileName,
  DWORD dwFlags
)
{
   QFile file;
   
   if ( dwFlags&MOVEFILE_REPLACE_EXISTING )
   {
#if UNICODE
      file.setFileName(QString::fromWCharArray(lpNewFileName));
#else
      file.setFileName(QString::fromLatin1(lpNewFileName));
#endif   
      file.remove();      
   }
   
#if UNICODE
   file.setFileName(QString::fromWCharArray(lpExistingFileName));
   return file.rename(QString::fromWCharArray(lpNewFileName));
#else
   file.setFileName(QString::fromLatin1(lpExistingFileName));
   return file.rename(QString::fromLatin1(lpNewFileName));
#endif   
}

BOOL WINAPI DeleteFile(
  LPCTSTR lpFileName
)
{
   QFile file;
   
#if UNICODE
   file.setFileName(QString::fromWCharArray(lpFileName));
#else
   file.setFileName(QString::fromLatin1(lpFileName));
#endif   
   return file.remove();   
}

DWORD WINAPI GetTempPath(
  DWORD nBufferLength,
  LPTSTR lpBuffer
)
{
   QString path = QDir::tempPath();
#if UNICODE
   wcsncpy(lpBuffer,path.unicode(),nBufferLength);
   return wcslen(lpBuffer);
#else
   strncpy(lpBuffer,path.toLatin1().constData(),nBufferLength);
   return strlen(lpBuffer);
#endif
}

UINT WINAPI GetTempFileName(
  LPCTSTR lpPathName,
  LPCTSTR lpPrefixString,
  UINT uUnique,
  LPTSTR lpTempFileName
)
{
   UINT value = QDateTime::currentMSecsSinceEpoch();
   QDir dir;
   QString fileName;
   QString filePath;
   QFile file;
   
#if UNICODE
   dir.setPath(QString::fromWCharArray(lpPathName));
#else
   dir.setPath(QString::fromLatin1(lpPathName));
#endif   
   
   do
   {
#if UNICODE
      fileName.sprintf("%u.%s",value,QString::fromWCharArray(lpPrefixString).toLatin1().constData());
#else
      fileName.sprintf("%u.%s",value,lpPrefixString);
#endif
      filePath = dir.absoluteFilePath(fileName);
      file.setFileName(filePath);
      if ( !file.exists() )
      {
         break;
      }
      value++;
   } while ( 1 );
   
#if UNICODE
   wcscpy(lpTempFileName,filePath.unicode());
#else
   strcpy(lpTempFileName,filePath.toLatin1().constData());
#endif
   
   return value;
}

CDocument* openFile(QString fileName)
{
   QFileInfo fileInfo(fileName);
   if ( fileName.isEmpty() )
   {
      return ptrToTheApp->OpenDocumentFile(NULL);
   }
   else if ( !fileInfo.exists() )
   {
      return NULL;
   }
   else
   {
      return ptrToTheApp->OpenDocumentFile(CString(fileName));
   }
}

UINT qtToMfcKeycode(UINT qt)
{
   UINT mfc = qt; // Assume same...
   switch ( qt )
   {
   case Qt::Key_Escape: // 0x01000000
      mfc = VK_ESCAPE;
      break;
   case Qt::Key_Tab: // 0x01000001
      mfc = VK_TAB;
      break;
   case Qt::Key_Backtab: // 0x01000002
      break;
   case Qt::Key_Backspace: // 0x01000003
      mfc = VK_BACK;
      break;
   case Qt::Key_Return: // 0x01000004
      mfc = VK_RETURN;
      break;
   case Qt::Key_Enter: // 0x01000005	Typically located on the keypad.
      mfc = VK_RETURN;
      break;
   case Qt::Key_Insert: // 0x01000006
      mfc = VK_INSERT;
      break;
   case Qt::Key_Delete: // 0x01000007
      mfc = VK_DELETE;
      break;
   case Qt::Key_Pause: // 0x01000008	The Pause/Break key (Note: Not anything to do with pausing media)
      mfc = VK_PAUSE;
      break;
   case Qt::Key_Print: // 0x01000009
      mfc = VK_PRINT;
      break;
   case Qt::Key_SysReq: // 0x0100000a
      break;
   case Qt::Key_Clear: // 0x0100000b
      mfc = VK_CLEAR;
      break;
   case Qt::Key_Home: // 0x01000010
      mfc = VK_HOME;
      break;
   case Qt::Key_End: // 0x01000011
      mfc = VK_END;
      break;
   case Qt::Key_Left: // 0x01000012
      mfc = VK_LEFT;
      break;
   case Qt::Key_Up: // 0x01000013
      mfc = VK_UP;
      break;
   case Qt::Key_Right: // 0x01000014
      mfc = VK_RIGHT;
      break;
   case Qt::Key_Down: // 0x01000015
      mfc = VK_DOWN;
      break;
   case Qt::Key_PageUp: // 0x01000016
      mfc = VK_PRIOR;
      break;
   case Qt::Key_PageDown: // 0x01000017
      mfc = VK_NEXT;
      break;
   case Qt::Key_Shift: // 0x01000020
      mfc = VK_SHIFT;
      break;
   case Qt::Key_Control: // 0x01000021	On Mac OS X, this corresponds to the Command keys.
      mfc = VK_CONTROL;
      break;
   case Qt::Key_Meta: // 0x01000022	On Mac OS X, this corresponds to the Control keys. On Windows keyboards, this key is mapped to the Windows key.
      mfc = VK_CONTROL;
      break;
   case Qt::Key_Alt: // 0x01000023
      break;
   case Qt::Key_AltGr: // 0x01001103	On Windows, when the KeyDown event for this key is sent, the Ctrl+Alt modifiers are also set.
      break;
   case Qt::Key_CapsLock: // 0x01000024
      mfc = VK_CAPITAL;
      break;
   case Qt::Key_NumLock: // 0x01000025
      mfc = VK_NUMLOCK;
      break;
   case Qt::Key_ScrollLock: // 0x01000026
      mfc = VK_SCROLL;
      break;
   case Qt::Key_F1: // 0x01000030
      mfc = VK_F1;
      break;
   case Qt::Key_F2: // 0x01000031
      mfc = VK_F2;
      break;
   case Qt::Key_F3: // 0x01000032
      mfc = VK_F3;
      break;
   case Qt::Key_F4: // 0x01000033
      mfc = VK_F4;
      break;
   case Qt::Key_F5: // 0x01000034
      mfc = VK_F5;
      break;
   case Qt::Key_F6: // 0x01000035
      mfc = VK_F6;
      break;
   case Qt::Key_F7: // 0x01000036
      mfc = VK_F7;
      break;
   case Qt::Key_F8: // 0x01000037
      mfc = VK_F8;
      break;
   case Qt::Key_F9: // 0x01000038
      mfc = VK_F9;
      break;
   case Qt::Key_F10: // 0x01000039
      mfc = VK_F10;
      break;
   case Qt::Key_F11: // 0x0100003a
      mfc = VK_F11;
      break;
   case Qt::Key_F12: // 0x0100003b
      mfc = VK_F12;
      break;
   case Qt::Key_F13: // 0x0100003c
      mfc = VK_F13;
      break;
   case Qt::Key_F14: // 0x0100003d
      mfc = VK_F14;
      break;
   case Qt::Key_F15: // 0x0100003e
      mfc = VK_F15;
      break;
   case Qt::Key_F16: // 0x0100003f
      mfc = VK_F16;
      break;
   case Qt::Key_F17: // 0x01000040
      mfc = VK_F17;
      break;
   case Qt::Key_F18: // 0x01000041
      mfc = VK_F18;
      break;
   case Qt::Key_F19: // 0x01000042
      mfc = VK_F19;
      break;
   case Qt::Key_F20: // 0x01000043
      mfc = VK_F20;
      break;
   case Qt::Key_F21: // 0x01000044
      mfc = VK_F21;
      break;
   case Qt::Key_F22: // 0x01000045
      mfc = VK_F22;
      break;
   case Qt::Key_F23: // 0x01000046
      mfc = VK_F23;
      break;
   case Qt::Key_F24: // 0x01000047
      mfc = VK_F24;
      break;
   case Qt::Key_F25: // 0x01000048
      break;
   case Qt::Key_F26: // 0x01000049
      break;
   case Qt::Key_F27: // 0x0100004a
      break;
   case Qt::Key_F28: // 0x0100004b
      break;
   case Qt::Key_F29: // 0x0100004c
      break;
   case Qt::Key_F30: // 0x0100004d
      break;
   case Qt::Key_F31: // 0x0100004e
      break;
   case Qt::Key_F32: // 0x0100004f
      break;
   case Qt::Key_F33: // 0x01000050
      break;
   case Qt::Key_F34: // 0x01000051
      break;
   case Qt::Key_F35: // 0x01000052
      break;
   case Qt::Key_Super_L: // 0x01000053
      break;
   case Qt::Key_Super_R: // 0x01000054
      break;
   case Qt::Key_Menu: // 0x01000055
      mfc = VK_MENU;
      break;
   case Qt::Key_Hyper_L: // 0x01000056
      break;
   case Qt::Key_Hyper_R: // 0x01000057
      break;
   case Qt::Key_Help: // 0x01000058
      mfc = VK_HELP;
      break;
   case Qt::Key_Direction_L: // 0x01000059
      mfc = VK_LEFT;
      break;
   case Qt::Key_Direction_R: // 0x01000060
      mfc = VK_RIGHT;
      break;
   case Qt::Key_Space: // 0x20
      mfc = VK_SPACE;
      break;
//   case Qt::Key_Any: //	Key_Space
//      break;
   case Qt::Key_Exclam: // 0x21
      break;
   case Qt::Key_QuoteDbl: // 0x22
      break;
   case Qt::Key_NumberSign: // 0x23
      break;
   case Qt::Key_Dollar: // 0x24
      break;
   case Qt::Key_Percent: // 0x25
      break;
   case Qt::Key_Ampersand: // 0x26
      break;
   case Qt::Key_Apostrophe: // 0x27
      break;
   case Qt::Key_ParenLeft: // 0x28
      break;
   case Qt::Key_ParenRight: // 0x29
      break;
   case Qt::Key_Asterisk: // 0x2a
      break;
   case Qt::Key_Plus: // 0x2b
      break;
   case Qt::Key_Comma: // 0x2c
      break;
   case Qt::Key_Minus: // 0x2d
      break;
   case Qt::Key_Period: // 0x2e
      break;
   case Qt::Key_Slash: // 0x2f
      break;
   case Qt::Key_0: // 0x30
      break;
   case Qt::Key_1: // 0x31
      break;
   case Qt::Key_2: // 0x32
      break;
   case Qt::Key_3: // 0x33
      break;
   case Qt::Key_4: // 0x34
      break;
   case Qt::Key_5: // 0x35
      break;
   case Qt::Key_6: // 0x36
      break;
   case Qt::Key_7: // 0x37
      break;
   case Qt::Key_8: // 0x38
      break;
   case Qt::Key_9: // 0x39
      break;
   case Qt::Key_Colon: // 0x3a
      break;
   case Qt::Key_Semicolon: // 0x3b
      break;
   case Qt::Key_Less: // 0x3c
      break;
   case Qt::Key_Equal: // 0x3d
      break;
   case Qt::Key_Greater: // 0x3e
      break;
   case Qt::Key_Question: // 0x3f
      break;
   case Qt::Key_At: // 0x40
      break;
   case Qt::Key_A: // 0x41
      break;
   case Qt::Key_B: // 0x42
      break;
   case Qt::Key_C: // 0x43
      break;
   case Qt::Key_D: // 0x44
      break;
   case Qt::Key_E: // 0x45
      break;
   case Qt::Key_F: // 0x46
      break;
   case Qt::Key_G: // 0x47
      break;
   case Qt::Key_H: // 0x48
      break;
   case Qt::Key_I: // 0x49
      break;
   case Qt::Key_J: // 0x4a
      break;
   case Qt::Key_K: // 0x4b
      break;
   case Qt::Key_L: // 0x4c
      break;
   case Qt::Key_M: // 0x4d
      break;
   case Qt::Key_N: // 0x4e
      break;
   case Qt::Key_O: // 0x4f
      break;
   case Qt::Key_P: // 0x50
      break;
   case Qt::Key_Q: // 0x51
      break;
   case Qt::Key_R: // 0x52
      break;
   case Qt::Key_S: // 0x53
      break;
   case Qt::Key_T: // 0x54
      break;
   case Qt::Key_U: // 0x55
      break;
   case Qt::Key_V: // 0x56
      break;
   case Qt::Key_W: // 0x57
      break;
   case Qt::Key_X: // 0x58
      break;
   case Qt::Key_Y: // 0x59
      break;
   case Qt::Key_Z: // 0x5a
      break;
   case Qt::Key_BracketLeft: // 0x5b
      mfc = VK_OEM_4;
      break;
   case Qt::Key_Backslash: // 0x5c
      mfc = VK_OEM_5;
      break;
   case Qt::Key_BracketRight: // 0x5d
      mfc = VK_OEM_6;
      break;
   case Qt::Key_AsciiCircum: // 0x5e
      break;
   case Qt::Key_Underscore: // 0x5f
      break;
   case Qt::Key_QuoteLeft: // 0x60
      mfc = VK_OEM_3;
      break;
   case Qt::Key_BraceLeft: // 0x7b
      mfc = VK_OEM_4;
      break;
   case Qt::Key_Bar: // 0x7c
      mfc = VK_OEM_5;
      break;
   case Qt::Key_BraceRight: // 0x7d
      mfc = VK_OEM_6;
      break;
   case Qt::Key_AsciiTilde: // 0x7e
      mfc = VK_OEM_3;
      break;
   case Qt::Key_nobreakspace: // 0x0a0
      break;
   case Qt::Key_exclamdown: // 0x0a1
      break;
   case Qt::Key_cent: // 0x0a2
      break;
   case Qt::Key_sterling: // 0x0a3
      break;
   case Qt::Key_currency: // 0x0a4
      break;
   case Qt::Key_yen: // 0x0a5
      break;
   case Qt::Key_brokenbar: // 0x0a6
      break;
   case Qt::Key_section: // 0x0a7
      break;
   case Qt::Key_diaeresis: // 0x0a8
      break;
   case Qt::Key_copyright: // 0x0a9
      break;
   case Qt::Key_ordfeminine: // 0x0aa
      break;
   case Qt::Key_guillemotleft: // 0x0ab
      break;
   case Qt::Key_notsign: // 0x0ac
      break;
   case Qt::Key_hyphen: // 0x0ad
      break;
   case Qt::Key_registered: // 0x0ae
      break;
   case Qt::Key_macron: // 0x0af
      break;
   case Qt::Key_degree: // 0x0b0
      break;
   case Qt::Key_plusminus: // 0x0b1
      break;
   case Qt::Key_twosuperior: // 0x0b2
      break;
   case Qt::Key_threesuperior: // 0x0b3
      break;
   case Qt::Key_acute: // 0x0b4
      break;
   case Qt::Key_mu: // 0x0b5
      break;
   case Qt::Key_paragraph: // 0x0b6
      break;
   case Qt::Key_periodcentered: // 0x0b7
      break;
   case Qt::Key_cedilla: // 0x0b8
      break;
   case Qt::Key_onesuperior: // 0x0b9
      break;
   case Qt::Key_masculine: // 0x0ba
      break;
   case Qt::Key_guillemotright: // 0x0bb
      break;
   case Qt::Key_onequarter: // 0x0bc
      break;
   case Qt::Key_onehalf: // 0x0bd
      break;
   case Qt::Key_threequarters: // 0x0be
      break;
   case Qt::Key_questiondown: // 0x0bf
      break;
   case Qt::Key_Agrave: // 0x0c0
      break;
   case Qt::Key_Aacute: // 0x0c1
      break;
   case Qt::Key_Acircumflex: // 0x0c2
      break;
   case Qt::Key_Atilde: // 0x0c3
      break;
   case Qt::Key_Adiaeresis: // 0x0c4
      break;
   case Qt::Key_Aring: // 0x0c5
      break;
   case Qt::Key_AE: // 0x0c6
      break;
   case Qt::Key_Ccedilla: // 0x0c7
      break;
   case Qt::Key_Egrave: // 0x0c8
      break;
   case Qt::Key_Eacute: // 0x0c9
      break;
   case Qt::Key_Ecircumflex: // 0x0ca
      break;
   case Qt::Key_Ediaeresis: // 0x0cb
      break;
   case Qt::Key_Igrave: // 0x0cc
      break;
   case Qt::Key_Iacute: // 0x0cd
      break;
   case Qt::Key_Icircumflex: // 0x0ce
      break;
   case Qt::Key_Idiaeresis: // 0x0cf
      break;
   case Qt::Key_ETH: // 0x0d0
      break;
   case Qt::Key_Ntilde: // 0x0d1
      break;
   case Qt::Key_Ograve: // 0x0d2
      break;
   case Qt::Key_Oacute: // 0x0d3
      break;
   case Qt::Key_Ocircumflex: // 0x0d4
      break;
   case Qt::Key_Otilde: // 0x0d5
      break;
   case Qt::Key_Odiaeresis: // 0x0d6
      break;
   case Qt::Key_multiply: // 0x0d7
      break;
   case Qt::Key_Ooblique: // 0x0d8
      break;
   case Qt::Key_Ugrave: // 0x0d9
      break;
   case Qt::Key_Uacute: // 0x0da
      break;
   case Qt::Key_Ucircumflex: // 0x0db
      break;
   case Qt::Key_Udiaeresis: // 0x0dc
      break;
   case Qt::Key_Yacute: // 0x0dd
      break;
   case Qt::Key_THORN: // 0x0de
      break;
   case Qt::Key_ssharp: // 0x0df
      break;
   case Qt::Key_division: // 0x0f7
      break;
   case Qt::Key_ydiaeresis: // 0x0ff
      break;
   case Qt::Key_Multi_key: // 0x01001120
      break;
   case Qt::Key_Codeinput: // 0x01001137
      break;
   case Qt::Key_SingleCandidate: // 0x0100113c
      break;
   case Qt::Key_MultipleCandidate: // 0x0100113d
      break;
   case Qt::Key_PreviousCandidate: // 0x0100113e
      break;
   case Qt::Key_Mode_switch: // 0x0100117e
      break;
   case Qt::Key_Kanji: // 0x01001121
      break;
   case Qt::Key_Muhenkan: // 0x01001122
      break;
   case Qt::Key_Henkan: // 0x01001123
      break;
   case Qt::Key_Romaji: // 0x01001124
      break;
   case Qt::Key_Hiragana: // 0x01001125
      break;
   case Qt::Key_Katakana: // 0x01001126
      break;
   case Qt::Key_Hiragana_Katakana: // 0x01001127
      break;
   case Qt::Key_Zenkaku: // 0x01001128
      break;
   case Qt::Key_Hankaku: // 0x01001129
      break;
   case Qt::Key_Zenkaku_Hankaku: // 0x0100112a
      break;
   case Qt::Key_Touroku: // 0x0100112b
      break;
   case Qt::Key_Massyo: // 0x0100112c
      break;
   case Qt::Key_Kana_Lock: // 0x0100112d
      break;
   case Qt::Key_Kana_Shift: // 0x0100112e
      break;
   case Qt::Key_Eisu_Shift: // 0x0100112f
      break;
   case Qt::Key_Eisu_toggle: // 0x01001130
      break;
   case Qt::Key_Hangul: // 0x01001131
      break;
   case Qt::Key_Hangul_Start: // 0x01001132
      break;
   case Qt::Key_Hangul_End: // 0x01001133
      break;
   case Qt::Key_Hangul_Hanja: // 0x01001134
      break;
   case Qt::Key_Hangul_Jamo: // 0x01001135
      break;
   case Qt::Key_Hangul_Romaja: // 0x01001136
      break;
   case Qt::Key_Hangul_Jeonja: // 0x01001138
      break;
   case Qt::Key_Hangul_Banja: // 0x01001139
      break;
   case Qt::Key_Hangul_PreHanja: // 0x0100113a
      break;
   case Qt::Key_Hangul_PostHanja: // 0x0100113b
      break;
   case Qt::Key_Hangul_Special: // 0x0100113f
      break;
   case Qt::Key_Dead_Grave: // 0x01001250
      break;
   case Qt::Key_Dead_Acute: // 0x01001251
      break;
   case Qt::Key_Dead_Circumflex: // 0x01001252
      break;
   case Qt::Key_Dead_Tilde: // 0x01001253
      break;
   case Qt::Key_Dead_Macron: // 0x01001254
      break;
   case Qt::Key_Dead_Breve: // 0x01001255
      break;
   case Qt::Key_Dead_Abovedot: // 0x01001256
      break;
   case Qt::Key_Dead_Diaeresis: // 0x01001257
      break;
   case Qt::Key_Dead_Abovering: // 0x01001258
      break;
   case Qt::Key_Dead_Doubleacute: // 0x01001259
      break;
   case Qt::Key_Dead_Caron: // 0x0100125a
      break;
   case Qt::Key_Dead_Cedilla: // 0x0100125b
      break;
   case Qt::Key_Dead_Ogonek: // 0x0100125c
      break;
   case Qt::Key_Dead_Iota: // 0x0100125d
      break;
   case Qt::Key_Dead_Voiced_Sound: // 0x0100125e
      break;
   case Qt::Key_Dead_Semivoiced_Sound: // 0x0100125f
      break;
   case Qt::Key_Dead_Belowdot: // 0x01001260
      break;
   case Qt::Key_Dead_Hook: // 0x01001261
      break;
   case Qt::Key_Dead_Horn: // 0x01001262
      break;
   case Qt::Key_Back: // 0x01000061
      break;
   case Qt::Key_Forward: // 0x01000062
      break;
   case Qt::Key_Stop: // 0x01000063
      break;
   case Qt::Key_Refresh: // 0x01000064
      break;
   case Qt::Key_VolumeDown: // 0x01000070
      break;
   case Qt::Key_VolumeMute: // 0x01000071
      break;
   case Qt::Key_VolumeUp: // 0x01000072
      break;
   case Qt::Key_BassBoost: // 0x01000073
      break;
   case Qt::Key_BassUp: // 0x01000074
      break;
   case Qt::Key_BassDown: // 0x01000075
      break;
   case Qt::Key_TrebleUp: // 0x01000076
      break;
   case Qt::Key_TrebleDown: // 0x01000077
      break;
   case Qt::Key_MediaPlay: // 0x01000080	A key setting the state of the media player to play
      break;
   case Qt::Key_MediaStop: // 0x01000081	A key setting the state of the media player to stop
      break;
   case Qt::Key_MediaPrevious: // 0x01000082
      break;
   case Qt::Key_MediaNext: // 0x01000083
      break;
   case Qt::Key_MediaRecord: // 0x01000084
      break;
   case Qt::Key_MediaPause: // 0x1000085	A key setting the state of the media player to pause (Note: not the pause/break key)
      break;
   case Qt::Key_MediaTogglePlayPause: // 0x1000086	A key to toggle the play/pause state in the media player (rather than setting an absolute state)
      break;
   case Qt::Key_HomePage: // 0x01000090
      break;
   case Qt::Key_Favorites: // 0x01000091
      break;
   case Qt::Key_Search: // 0x01000092
      break;
   case Qt::Key_Standby: // 0x01000093
      break;
   case Qt::Key_OpenUrl: // 0x01000094
      break;
   case Qt::Key_LaunchMail: // 0x010000a0
      break;
   case Qt::Key_LaunchMedia: // 0x010000a1
      break;
   case Qt::Key_Launch0: // 0x010000a2	On X11 this key is mapped to "My Computer" (XF86XK_MyComputer) key for legacy reasons.
      break;
   case Qt::Key_Launch1: // 0x010000a3	On X11 this key is mapped to "Calculator" (XF86XK_Calculator) key for legacy reasons.
      break;
   case Qt::Key_Launch2: // 0x010000a4	On X11 this key is mapped to XF86XK_Launch0 key for legacy reasons.
      break;
   case Qt::Key_Launch3: // 0x010000a5	On X11 this key is mapped to XF86XK_Launch1 key for legacy reasons.
      break;
   case Qt::Key_Launch4: // 0x010000a6	On X11 this key is mapped to XF86XK_Launch2 key for legacy reasons.
      break;
   case Qt::Key_Launch5: // 0x010000a7	On X11 this key is mapped to XF86XK_Launch3 key for legacy reasons.
      break;
   case Qt::Key_Launch6: // 0x010000a8	On X11 this key is mapped to XF86XK_Launch4 key for legacy reasons.
      break;
   case Qt::Key_Launch7: // 0x010000a9	On X11 this key is mapped to XF86XK_Launch5 key for legacy reasons.
      break;
   case Qt::Key_Launch8: // 0x010000aa	On X11 this key is mapped to XF86XK_Launch6 key for legacy reasons.
      break;
   case Qt::Key_Launch9: // 0x010000ab	On X11 this key is mapped to XF86XK_Launch7 key for legacy reasons.
      break;
   case Qt::Key_LaunchA: // 0x010000ac	On X11 this key is mapped to XF86XK_Launch8 key for legacy reasons.
      break;
   case Qt::Key_LaunchB: // 0x010000ad	On X11 this key is mapped to XF86XK_Launch9 key for legacy reasons.
      break;
   case Qt::Key_LaunchC: // 0x010000ae	On X11 this key is mapped to XF86XK_LaunchA key for legacy reasons.
      break;
   case Qt::Key_LaunchD: // 0x010000af	On X11 this key is mapped to XF86XK_LaunchB key for legacy reasons.
      break;
   case Qt::Key_LaunchE: // 0x010000b0	On X11 this key is mapped to XF86XK_LaunchC key for legacy reasons.
      break;
   case Qt::Key_LaunchF: // 0x010000b1	On X11 this key is mapped to XF86XK_LaunchD key for legacy reasons.
      break;
   case Qt::Key_LaunchG: // 0x0100010e	On X11 this key is mapped to XF86XK_LaunchE key for legacy reasons.
      break;
   case Qt::Key_LaunchH: // 0x0100010f	On X11 this key is mapped to XF86XK_LaunchF key for legacy reasons.
      break;
   case Qt::Key_MonBrightnessUp: // 0x010000b2
      break;
   case Qt::Key_MonBrightnessDown: // 0x010000b3
      break;
   case Qt::Key_KeyboardLightOnOff: // 0x010000b4
      break;
   case Qt::Key_KeyboardBrightnessUp: // 0x010000b5
      break;
   case Qt::Key_KeyboardBrightnessDown: // 0x010000b6
      break;
   case Qt::Key_PowerOff: // 0x010000b7
      break;
   case Qt::Key_WakeUp: // 0x010000b8
      break;
   case Qt::Key_Eject: // 0x010000b9
      break;
   case Qt::Key_ScreenSaver: // 0x010000ba
      break;
   case Qt::Key_WWW: // 0x010000bb
      break;
   case Qt::Key_Memo: // 0x010000bc
      break;
   case Qt::Key_LightBulb: // 0x010000bd
      break;
   case Qt::Key_Shop: // 0x010000be
      break;
   case Qt::Key_History: // 0x010000bf
      break;
   case Qt::Key_AddFavorite: // 0x010000c0
      break;
   case Qt::Key_HotLinks: // 0x010000c1
      break;
   case Qt::Key_BrightnessAdjust: // 0x010000c2
      break;
   case Qt::Key_Finance: // 0x010000c3
      break;
   case Qt::Key_Community: // 0x010000c4
      break;
   case Qt::Key_AudioRewind: // 0x010000c5
      break;
   case Qt::Key_BackForward: // 0x010000c6
      break;
   case Qt::Key_ApplicationLeft: // 0x010000c7
      break;
   case Qt::Key_ApplicationRight: // 0x010000c8
      break;
   case Qt::Key_Book: // 0x010000c9
      break;
   case Qt::Key_CD: // 0x010000ca
      break;
   case Qt::Key_Calculator: // 0x010000cb	On X11 this key is not mapped for legacy reasons. Use Qt::Key_Launch1 instead.
      break;
   case Qt::Key_ToDoList: // 0x010000cc
      break;
   case Qt::Key_ClearGrab: // 0x010000cd
      break;
   case Qt::Key_Close: // 0x010000ce
      break;
   case Qt::Key_Copy: // 0x010000cf
      break;
   case Qt::Key_Cut: // 0x010000d0
      break;
   case Qt::Key_Display: // 0x010000d1
      break;
   case Qt::Key_DOS: // 0x010000d2
      break;
   case Qt::Key_Documents: // 0x010000d3
      break;
   case Qt::Key_Excel: // 0x010000d4
      break;
   case Qt::Key_Explorer: // 0x010000d5
      break;
   case Qt::Key_Game: // 0x010000d6
      break;
   case Qt::Key_Go: // 0x010000d7
      break;
   case Qt::Key_iTouch: // 0x010000d8
      break;
   case Qt::Key_LogOff: // 0x010000d9
      break;
   case Qt::Key_Market: // 0x010000da
      break;
   case Qt::Key_Meeting: // 0x010000db
      break;
   case Qt::Key_MenuKB: // 0x010000dc
      break;
   case Qt::Key_MenuPB: // 0x010000dd
      break;
   case Qt::Key_MySites: // 0x010000de
      break;
   case Qt::Key_News: // 0x010000df
      break;
   case Qt::Key_OfficeHome: // 0x010000e0
      break;
   case Qt::Key_Option: // 0x010000e1
      break;
   case Qt::Key_Paste: // 0x010000e2
      break;
   case Qt::Key_Phone: // 0x010000e3
      break;
   case Qt::Key_Calendar: // 0x010000e4
      break;
   case Qt::Key_Reply: // 0x010000e5
      break;
   case Qt::Key_Reload: // 0x010000e6
      break;
   case Qt::Key_RotateWindows: // 0x010000e7
      break;
   case Qt::Key_RotationPB: // 0x010000e8
      break;
   case Qt::Key_RotationKB: // 0x010000e9
      break;
   case Qt::Key_Save: // 0x010000ea
      break;
   case Qt::Key_Send: // 0x010000eb
      break;
   case Qt::Key_Spell: // 0x010000ec
      break;
   case Qt::Key_SplitScreen: // 0x010000ed
      break;
   case Qt::Key_Support: // 0x010000ee
      break;
   case Qt::Key_TaskPane: // 0x010000ef
      break;
   case Qt::Key_Terminal: // 0x010000f0
      break;
   case Qt::Key_Tools: // 0x010000f1
      break;
   case Qt::Key_Travel: // 0x010000f2
      break;
   case Qt::Key_Video: // 0x010000f3
      break;
   case Qt::Key_Word: // 0x010000f4
      break;
   case Qt::Key_Xfer: // 0x010000f5
      break;
   case Qt::Key_ZoomIn: // 0x010000f6
      break;
   case Qt::Key_ZoomOut: // 0x010000f7
      break;
   case Qt::Key_Away: // 0x010000f8
      break;
   case Qt::Key_Messenger: // 0x010000f9
      break;
   case Qt::Key_WebCam: // 0x010000fa
      break;
   case Qt::Key_MailForward: // 0x010000fb
      break;
   case Qt::Key_Pictures: // 0x010000fc
      break;
   case Qt::Key_Music: // 0x010000fd
      break;
   case Qt::Key_Battery: // 0x010000fe
      break;
   case Qt::Key_Bluetooth: // 0x010000ff
      break;
   case Qt::Key_WLAN: // 0x01000100
      break;
   case Qt::Key_UWB: // 0x01000101
      break;
   case Qt::Key_AudioForward: // 0x01000102
      break;
   case Qt::Key_AudioRepeat: // 0x01000103
      break;
   case Qt::Key_AudioRandomPlay: // 0x01000104
      break;
   case Qt::Key_Subtitle: // 0x01000105
      break;
   case Qt::Key_AudioCycleTrack: // 0x01000106
      break;
   case Qt::Key_Time: // 0x01000107
      break;
   case Qt::Key_Hibernate: // 0x01000108
      break;
   case Qt::Key_View: // 0x01000109
      break;
   case Qt::Key_TopMenu: // 0x0100010a
      break;
   case Qt::Key_PowerDown: // 0x0100010b
      break;
   case Qt::Key_Suspend: // 0x0100010c
      break;
   case Qt::Key_ContrastAdjust: // 0x0100010d
      break;
   case Qt::Key_MediaLast: // 0x0100ffff
      break;
   case Qt::Key_unknown: // 0x01ffffff
      break;
   case Qt::Key_Call: // 0x01100004	A key to answer or initiate a call (see Qt::Key_ToggleCallHangup for a key to toggle current call state)
      break;
   case Qt::Key_Camera: // 0x01100020	A key to activate the camera shutter
      break;
   case Qt::Key_CameraFocus: // 0x01100021	A key to focus the camera
      break;
   case Qt::Key_Context1: // 0x01100000
      break;
   case Qt::Key_Context2: // 0x01100001
      break;
   case Qt::Key_Context3: // 0x01100002
      break;
   case Qt::Key_Context4: // 0x01100003
      break;
   case Qt::Key_Flip: // 0x01100006
      break;
   case Qt::Key_Hangup: // 0x01100005	A key to end an ongoing call (see Qt::Key_ToggleCallHangup for a key to toggle current call state)
      break;
   case Qt::Key_No: // 0x01010002
      break;
   case Qt::Key_Select: // 0x01010000
      break;
   case Qt::Key_Yes: // 0x01010001
      break;
   case Qt::Key_ToggleCallHangup: // 0x01100007	A key to toggle the current call state (ie. either answer, or hangup) depending on current call state
      break;
   case Qt::Key_VoiceDial: // 0x01100008
      break;
   case Qt::Key_LastNumberRedial: // 0x01100009
      break;
   case Qt::Key_Execute: // 0x01020003
      break;
   case Qt::Key_Printer: // 0x01020002
      break;
   case Qt::Key_Play: // 0x01020005
      break;
   case Qt::Key_Sleep: // 0x01020004
      break;
   case Qt::Key_Zoom: // 0x01020006
      break;
   case Qt::Key_Cancel: // 0x01020001
      break;
   }
   return mfc;
}

UINT mfcToQtKeycode(UINT mfc)
{
   UINT qt = mfc; // Assume same...
   switch ( mfc )
   {
   case VK_DIVIDE:
      qt = Qt::Key_division;
      break;
   case VK_F1:
      qt = Qt::Key_F1;
      break;
   case VK_F2:
      qt = Qt::Key_F2;
      break;
   case VK_F3:
      qt = Qt::Key_F3;
      break;
   case VK_F4:
      qt = Qt::Key_F4;
      break;
   case VK_F5:
      qt = Qt::Key_F5;
      break;
   case VK_F6:
      qt = Qt::Key_F6;
      break;
   case VK_F7:
      qt = Qt::Key_F7;
      break;
   case VK_F8:
      qt = Qt::Key_F8;
      break;
   case VK_F9:
      qt = Qt::Key_F9;
      break;
   case VK_F10:
      qt = Qt::Key_F10;
      break;
   case VK_F11:
      qt = Qt::Key_F11;
      break;
   case VK_F12:
      qt = Qt::Key_F12;
      break;
   case VK_F13:
      qt = Qt::Key_F13;
      break;
   case VK_F14:
      qt = Qt::Key_F14;
      break;
   case VK_F15:
      qt = Qt::Key_F15;
      break;
   case VK_F16:
      qt = Qt::Key_F16;
      break;
   case VK_F17:
      qt = Qt::Key_F17;
      break;
   case VK_F18:
      qt = Qt::Key_F18;
      break;
   case VK_F19:
      qt = Qt::Key_F19;
      break;
   default:
      qDebug("mfcToQtKeycode...not sure what to do here yet mfc=%x.",mfc);
      break;      
   }
   return qt;
}

bool ideifiedFamiTracker = false;

void ideifyFamiTracker()
{
   ideifiedFamiTracker = true;
}

bool backgroundedFamiTracker = false;

void backgroundifyFamiTracker(QString applicationName)
{
   gApplicationName = applicationName;
   backgroundedFamiTracker = true;
}

bool invisibleFamiTracker = false;

void hideFamiTracker(QString applicationName)
{
   gApplicationName = applicationName;
   backgroundedFamiTracker = true;
   invisibleFamiTracker = true;
}
