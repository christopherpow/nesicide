#include "cqtmfc.h"
//#include "resource.h"

#include <stdarg.h>

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

extern CWinApp* ptrToTheApp;

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

QHash<int,QIcon*> qtIconResources;

QIcon* qtIconResource(int id)
{
   return qtIconResources.value(id);
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

int AfxMessageBox(
   LPCTSTR lpszText,
   UINT nType,
   UINT nIDHelp
)
{
   QString button1;
   QString button2;
   QString button3;
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
   // CP: not really needed...
   return 0;
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
   LONG modParam = 0;
   switch ( (lParam>>16)&0xFF )
   {
      // Mappable
//   case VK_ABNT_C1:		// 0xC1	Abnt C1
//      break;
//   case VK_ABNT_C2:		// 0xC2	Abnt C2
//      break;
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
      return 0xf0f0f0;
      break;
   case COLOR_BTNHIGHLIGHT:
      return 0xffffff;
      break;
   case COLOR_APPWORKSPACE:
      return 0xababab;
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
   QByteArray value = QString::number((int)pMem,16).toLatin1();
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

   pMem->lock();
   pData = pMem->data();
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
   UINT id = (UINT)lpTableName;
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
   while ( pAccel->cmd )
   {
      if ( lpMsg->wParam == pAccel->key )
      {
         CWnd* pWnd = (CWnd*)hWnd;
         pWnd->SendMessage(WM_COMMAND,pAccel->cmd);
         qDebug("Translating and sending %d message...",pAccel->key);
         return 1;
      }
      pAccel++;
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

//IMPLEMENT_DYNAMIC(CObject,NULL) <- CObject is base...treat it special.
CRuntimeClass CObject::classCObject = 
{
"CObject",
sizeof(CObject),
-1,
NULL,
NULL,
NULL
};

CRuntimeClass* CObject::GetRuntimeClass()
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
   pClass = pClass->m_pBaseClass;
   while ( pClass )
   {
      if ( pClass == GetRuntimeClass() )
      {
         return TRUE;
      }
      pClass = pClass->m_pBaseClass;
   }
   return FALSE;
}

CRuntimeClass* CObject::GetRuntimeClass() const
{
   return &classCObject;
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

BOOL CString::IsEmpty() const
{
   return _qstr.isEmpty();
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
   _qstr = QString::fromLatin1(str);
#endif
   UpdateScratch();
   return *this;
}

CString& CString::operator=(TCHAR c)
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
   return _qstr.compare(QString(lpsz),Qt::CaseInsensitive);
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
   _qfile.setFileName(lpszFileName);
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
   _qfile.setFileName(lpszFileName);
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
      p.drawPixmap(0,0,_qpixmap);
      p.end();
   }
}

void CDC::attach()
{
   _qpixmap = QPixmap(1,1);
   _qpainter.begin(&_qpixmap);
   m_hDC = (HDC)&_qpixmap;
   attached = true;
}

void CDC::attach(QWidget* parent)
{
   _qwidget = parent;
   _qpixmap = QPixmap(_qwidget->size());
   _qpixmap.fill(_qwidget->palette().color(QPalette::Window)); // CP: hack to initialize pixmap with widget's background color.
   _qpainter.begin(&_qpixmap);
   m_hDC = (HDC)&_qpixmap;
   attached = true;
   _doFlush = true;
}

void CDC::detach()
{
   if ( attached )
   {
      if ( _qpainter.isActive() )
         _qpainter.end();
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
   QSize pixmapSize = pSrcDC->pixmapSize();
   pixmapSize = pixmapSize.boundedTo(QSize(nWidth,nHeight));
   if ( pixmap && (pixmapSize.width() >= 0) )
      _qpainter.drawPixmap(x,y,pixmapSize.width(),pixmapSize.height(),*pixmap,xSrc,ySrc,pixmapSize.width(),pixmapSize.height());
   else
      _qpainter.drawPixmap(x,y,nWidth,nHeight,*pixmap,xSrc,ySrc,nWidth,nHeight);
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
//   dc.flush();
   return 0;
}

BOOL CDC::DrawEdge(
   LPRECT lpRect,
   UINT nEdge,
   UINT nFlags
)
{
   QRect rect(lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top);
   _qpainter.drawRect(rect);
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
   QString qstr = (LPCTSTR)str;
#endif
   QPen origPen = _qpainter.pen();
   _qpainter.setPen(QPen(_textColor));
//   _qpainter.setFont((QFont)*_font);
   _qpainter.drawText(rect,qstr.toLatin1().constData());
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
   QString qstr(lpszString);
#endif
   QPen origPen = _qpainter.pen();
   _qpainter.setPen(QPen(_textColor));
//   _qpainter.setFont((QFont)*_font);
   _qpainter.drawText(rect,qstr.left(nCount).toLatin1().constData());
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
   QString qstr(lpszString);
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

CComboBox::CComboBox(CWnd *parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;

   _grid = NULL;

   _qt = new QComboBox(parent->toQWidget());

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QComboBox*>(_qt);

   _qtd->setMouseTracking(true);

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
   _qtd->addItem(lpszString);
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
   _qtd->setMouseTracking(true);

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

IMPLEMENT_DYNAMIC(CCheckListBox,CListBox)

CCheckListBox::CCheckListBox(CWnd* parent)
   : CListBox(parent)
{
}

CCheckListBox::~CCheckListBox()
{
}

IMPLEMENT_DYNAMIC(CListCtrl,CWnd)

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
   _qtd->setMouseTracking(true);

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

IMPLEMENT_DYNAMIC(CScrollBar,CWnd)

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

   _qtd->setMouseTracking(true);

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

IMPLEMENT_DYNCREATE(CCmdTarget,CObject)

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

IMPLEMENT_DYNAMIC(CWnd,CCmdTarget)

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
   DestroyWindow();
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

CWnd* CWnd::SetFocus()
{
   CWnd* pWnd = focusWnd;
   if ( focusWnd )
      focusWnd->OnKillFocus(this);
   focusWnd = this;
   _qt->setFocus();
   OnSetFocus(pWnd);
   return pWnd;
}

BOOL CWnd::PreTranslateMessage(
   MSG* pMsg
)
{
   BOOL processed = FALSE;
   switch ( pMsg->message )
   {
   case WM_NULL:
      processed = TRUE;
      break;
   default:
      processed = FALSE;
      break;
   }
   return processed;
}

CWnd* CWnd::GetFocus()
{
   return focusWnd;
}

void CWnd::SetOwner(
   CWnd* pOwnerWnd
)
{
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
   return _myDC;
}

void CWnd::ReleaseDC(CDC* pDC)
{
   update();
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

void CWnd::focusInEvent(QFocusEvent *event)
{
   SetFocus();
}

bool CWnd::eventFilter(QObject *object, QEvent *event)
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
//   qDebug("eventFilter: unhandled %d object %s", event->type(), object->objectName().toLatin1().constData());
   return false;
}

BOOL CWnd::IsWindowVisible( ) const
{
   return _qt->isVisible();
}

BOOL CWnd::DestroyWindow()
{
   if ( _qt )
   {
      QList<QWidget *> widgets = _qt->findChildren<QWidget *>();
      foreach ( QWidget* widget, widgets ) widget->deleteLater();
      _qt->close();
   }
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
//   _qt->setFixedSize(createStruct.cx,createStruct.cy);
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
//   if ( bRepaint )
//      repaint();
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
   strncpy(lpszStringBuf,(LPTSTR)_qt->windowTitle().toLatin1().constData(),nMaxCount);
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

IMPLEMENT_DYNCREATE(CFrameWnd,CWnd)

CFrameWnd::CFrameWnd(CWnd *parent)
   : CWnd(parent),
     m_pViewActive(NULL),
     m_pDocument(NULL),
     m_bInRecalcLayout(FALSE)
{
   int idx;

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
   realCentralWidget->setObjectName(QString::fromUtf8("realCentralWidget"));

   gridLayout->addWidget(realCentralWidget, 1, 1, 1, 1);

   gridLayout->setRowStretch(1,1);
   gridLayout->setColumnStretch(1,1);

   centralWidget->setLayout(gridLayout);

   m_pMenu = new CMenu;
   m_pMenu->LoadMenu(128);
   for ( idx = 0; idx < m_pMenu->GetMenuItemCount(); idx++ )
   {
      ptrToTheApp->qtMainWindow->menuBar()->addMenu(m_pMenu->GetSubMenu(idx)->toQMenu());
   }
}

CFrameWnd::~CFrameWnd()
{
   delete m_pMenu;
}

void CFrameWnd::menuAction_triggered(int id)
{
   // Pass up the chain.
   AfxGetApp()->menuAction_triggered(id);
}

void CFrameWnd::menuAboutToShow(CMenu* menu)
{
   // Pass up the chain.
   AfxGetApp()->menuAboutToShow(menu);
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
   if ( !CWnd::Create(lpszClassName,lpszWindowName,dwStyle,rect,pParentWnd,lpszMenuName,dwExStyle,pContext) )
      return FALSE;
   
   // Set this frame's document.
   m_pDocument = pContext->m_pCurrentDoc;
   
   // The view is actually created in CFrameWnd::CreateView but I'm being lazy...
   m_pViewActive = (CView*)pContext->m_pNewViewClass->CreateObject();   
   
   // Set frame in hijacked context.
   pContext->m_pCurrentFrame = this;
   
   // Create the view!
   m_pViewActive->Create(lpszClassName,lpszWindowName,dwStyle|WS_VSCROLL|WS_HSCROLL,rect,pParentWnd,lpszMenuName,dwExStyle,pContext);

   mfcToQtWidgetMap()->insert(AFX_IDW_PANE_FIRST,m_pViewActive);
   
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
   m_pDocument->SetTitle(title);
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
#if UNICODE
   ptrToTheApp->qtMainWindow->statusBar()->showMessage(QString::fromWCharArray(message));
#else
   ptrToTheApp->qtMainWindow->statusBar()->showMessage(QString::fromLatin1(message));
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

void CFrameWnd::OnUpdateRecentFileList(CCmdUI *pCmdUI)
{
   CRecentFileList* pRecentFileList = AfxGetApp()->m_pRecentFileList;
   CMenu* pMRU = GetMenu()->GetSubMenu(0);
   CString mruPath;
   CString mruString;
   int mru;
   
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
         pMRU->InsertMenu(ID_APP_EXIT,MF_STRING|MF_BYCOMMAND,ID_FILE_MRU_FILE1+mru,mruString);
      }
   }
   else
   {
      pMRU->InsertMenu(ID_APP_EXIT,MF_STRING|MF_BYCOMMAND,ID_FILE_MRU_FILE1,"Recent File");
      pMRU->EnableMenuItem(ID_FILE_MRU_FILE1,false);
   }
   // CP: Not sure why separator disappears...
   pMRU->InsertMenu(ID_APP_EXIT,MF_SEPARATOR|MF_BYCOMMAND);
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

IMPLEMENT_DYNCREATE(CView,CWnd)

CView::CView()
   : m_pDocument(NULL)
{
}

CView::~CView()
{
}

void CView::menuAction_triggered(int id)
{
   // Pass up the chain.
   m_pDocument->menuAction_triggered(id);
}

void CView::menuAboutToShow(CMenu* menu)
{
   // Pass up the chain.
   m_pDocument->menuAboutToShow(menu);
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
   
   // Add document to view.
   m_pDocument = pContext->m_pCurrentDoc;
   
   // Add view to document.
   m_pDocument->AddView(this);
   
   // Set parent frame.
   m_pFrameWnd = pContext->m_pCurrentFrame;

   return TRUE;
}

IMPLEMENT_DYNAMIC(CControlBar,CWnd)

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

IMPLEMENT_DYNAMIC(CReBarCtrl,CWnd)

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

   _qtd->setMouseTracking(true);
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
   emit toolBarAction_triggered(_qtd->actions().indexOf(qobject_cast<QAction*>(sender())));
}

IMPLEMENT_DYNAMIC(CReBar,CControlBar)

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

IMPLEMENT_DYNAMIC(CToolBar,CControlBar)

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
   QAction* cur;
   QMenu* menu;
   QToolButton* ptb;
   QList<QAction*> actions;
   switch ( nStyle )
   {
   case TBBS_DROPDOWN:
      actions = _qtd->actions();
      cur = actions.at(nIndex);
      menu = new QMenu;
      ptb = dynamic_cast<QToolButton*>(_qtd->widgetForAction(cur));
      ptb->setMenu(menu);
      ptb->setPopupMode(QToolButton::MenuButtonPopup);
      cur->setMenu(menu);
      QObject::connect(menu,SIGNAL(aboutToShow()),this,SLOT(menu_aboutToShow()));
      break;
   default:
      qDebug("CToolBar::SetButtonStyle %d not implemented",nStyle);
      break;
   }
}

void CToolBar::toolBarAction_triggered()
{
   emit toolBarAction_triggered(_qtd->actions().indexOf(qobject_cast<QAction*>(sender())));
}

void CToolBar::menu_aboutToShow()
{
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
      emit toolBarAction_menu_aboutToShow(actions.indexOf(origin));
   }
}

IMPLEMENT_DYNAMIC(CStatusBar,CControlBar)

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
   ptrToTheApp->qtMainWindow->statusBar()->showMessage(QString::fromWCharArray(lpszString));
#else
   ptrToTheApp->qtMainWindow->statusBar()->showMessage(lpszString);
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
      CString lpszText = qtMfcStringResource(lpIDArray[pane]);
#if UNICODE
      ((QLabel*)newPane->toQWidget())->setText(QString::fromWCharArray(lpszText));
#else
      ((QLabel*)newPane->toQWidget())->setText(QString::fromLatin1(lpszText));
#endif
      if ( lpIDArray[pane] != ID_SEPARATOR )
      {
         mfcToQtWidget.insert(lpIDArray[pane],newPane);
      }
      ptrToTheApp->qtMainWindow->statusBar()->addPermanentWidget(newPane->toQWidget());
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

IMPLEMENT_DYNAMIC(CDialogBar,CControlBar)

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

IMPLEMENT_DYNAMIC(CDialog,CWnd)

CDialog::CDialog()
{
   if ( _qt )
      delete _qt;

   _qt = new QDialog;

   _qtd = dynamic_cast<QDialog*>(_qt);
   _inited = false;

   _qtd->installEventFilter(this);
   _qtd->setMouseTracking(true);

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

   _qtd->installEventFilter(this);
   _qtd->setMouseTracking(true);

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
   {
      _qtd->setWindowFlags(_qtd->windowFlags()|Qt::Dialog);
   }

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

IMPLEMENT_DYNAMIC(CCommonDialog,CDialog)

CCommonDialog::CCommonDialog(CWnd *pParentWnd)
 : CDialog(0,pParentWnd)
{
}

CCommonDialog::~CCommonDialog()
{
}

IMPLEMENT_DYNCREATE(CWinThread,CCmdTarget)

CWinThread::CWinThread()
{
   m_hThread = (HANDLE)this;
   m_nThreadID = (DWORD)QThread::currentThreadId();
   
   m_pMainWnd = NULL;
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
   m_pMainWnd = AfxGetMainWnd();

   if ( !(dwCreateFlags&CREATE_SUSPENDED) )
   {
      ResumeThread();
   }

   return TRUE;
}

DWORD CWinThread::ResumeThread( )
{
   InitInstance();
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

IMPLEMENT_DYNCREATE(CDocument,CCmdTarget)

CDocument::CDocument()
   : m_pDocTemplate(NULL), m_bModified(FALSE), m_bAutoDelete(TRUE)
{
   QObject::connect(this,SIGNAL(documentSaved()),ptrToTheApp->qtMainWindow,SLOT(documentSaved()));
   QObject::connect(this,SIGNAL(documentClosed()),ptrToTheApp->qtMainWindow,SLOT(documentClosed()));
}

void CDocument::menuAction_triggered(int id)
{
   // Pass up the chain.
   AfxGetMainWnd()->menuAction_triggered(id);
}

void CDocument::menuAboutToShow(CMenu* menu)
{
   // Pass up the chain.
   AfxGetMainWnd()->menuAboutToShow(menu);
}

void CDocument::OnCloseDocument()
{ 
   emit documentClosed(); 
}

void CDocument::AddView( 
   CView* pView  
)
{
   _views.append(pView);
}

void CDocument::SetTitle(CString title )
{
   CString appName;
   QString windowTitle;
   QFileInfo fileInfo;

   if ( title.Right(1) == "*" )
   {
      m_strTitle = title.Left(title.GetLength()-1);
   }
   else
   {
      m_strTitle = title;
   }

#if UNICODE
   windowTitle = QString::fromWCharArray((LPCTSTR)title);
#else
   windowTitle = QString::fromLatin1((LPCTSTR)title);
#endif
   fileInfo.setFile(windowTitle);
   m_pDocTemplate->GetDocString(appName,CDocTemplate::windowTitle);
#if UNICODE
   windowTitle = fileInfo.fileName() + QString::fromWCharArray((LPCTSTR)appName);
#else
   windowTitle = fileInfo.fileName() + QString::fromLatin1((LPCTSTR)appName);
#endif
   ptrToTheApp->qtMainWindow->setWindowTitle(windowTitle);
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

void CDocument::SetPathName(
   LPCTSTR lpszPathName,
   BOOL bAddToMRU
)
{
   m_strPathName = lpszPathName;
   
   if ( bAddToMRU )
   {
      AfxGetApp()->AddToRecentFileList(lpszPathName);
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
	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
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
      rString = " - FamiTracker";
      break;

   case docName:
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
   GetDocString(title,fileNewName);
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
}

void CCommandLineInfo::ParseParam(
   const TCHAR* pszParam,
   BOOL bFlag,
   BOOL bLast
)
{
   qDebug("CCommandLineInfo::ParseParam");
}

IMPLEMENT_DYNCREATE(CWinApp,CWinThread)

CWinApp::CWinApp() 
   : m_pRecentFileList(NULL) 
{
}

CWinApp::~CWinApp()
{
   delete m_pRecentFileList;
}

void CWinApp::menuAction_triggered(int id)
{
   // Handle MRU.
   if ( (id >= ID_FILE_MRU_FILE1) &&
        (id <= ID_FILE_MRU_FILE16))
   {
      OnOpenRecentFile(id);
   }
}

void CWinApp::menuAboutToShow(CMenu* menu)
{
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
      qDebug("Not really using CDocTemplate...");
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
   // CP: Just do New file for now...
   OpenDocumentFile(NULL);
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
   qDebug("LoadStandardCursor needs work...");
//   setCursor()
   return (HCURSOR)NULL;
}

IMPLEMENT_DYNAMIC(CMenu,CCmdTarget)

CMenu::CMenu()
   : m_hMenu(NULL)
{
   _qtd = new QMenu;
   _cmenu = new QHash<int,CMenu*>;
   m_hMenu = (HMENU)this;
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

void CMenu::menuAction_triggered()
{
   emit menuAction_triggered(qtToMfcMenu.value((QAction*)sender()));
}

void CMenu::menuAboutToShow()
{
   emit menuAboutToShow(this);
}

QAction* CMenu::findMenuItem(UINT id) const
{
   int subMenu;
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

UINT CMenu::findMenuID(QAction* action) const
{
   int subMenu;
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
   switch ( nFlags&MF_BYPOSITION )
   {
   case MF_BYPOSITION:
      _qtd->removeAction(_qtd->actions().at(nPosition));
      break;
   default:
      _qtd->removeAction(findMenuItem(nPosition));
      break;
   }
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
   if ( nFlags&MF_POPUP )
   {
      _cmenu->insert(_qtd->actions().count(),pMenu);
      _qtd->addMenu(pMenu->toQMenu());

#if UNICODE
      pMenu->toQMenu()->setTitle(QString::fromWCharArray(lpszNewItem));
#else
      pMenu->toQMenu()->setTitle(QString::fromLatin1(lpszNewItem));
#endif
      QObject::connect(pMenu,SIGNAL(menuAction_triggered(int)),this,SIGNAL(menuAction_triggered(int)));
   }
   else if ( nFlags&MF_SEPARATOR )
   {
      _qtd->addSeparator();
   }
   else
   {
#if UNICODE
      QAction* action = _qtd->addAction(QString::fromWCharArray(lpszNewItem));
#else
      QAction* action = _qtd->addAction(QString::fromLatin1(lpszNewItem));
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
      QObject::connect(action,SIGNAL(triggered()),this,SLOT(menuAction_triggered()));
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
      action = _qtd->actions().at(nPosition);
      break;
   default:
      action = findMenuItem(nPosition);
      break;
   }
   if ( action )
   {
      if ( nFlags&MF_POPUP )
      {
         _cmenu->insert(_qtd->actions().indexOf(action),pMenu);
         _qtd->insertMenu(action,pMenu->toQMenu());
   
#if UNICODE
         pMenu->toQMenu()->setTitle(QString::fromWCharArray(lpszNewItem));
#else
         pMenu->toQMenu()->setTitle(QString::fromLatin1(lpszNewItem));
#endif
         QObject::connect(pMenu,SIGNAL(menuAction_triggered(int)),this,SIGNAL(menuAction_triggered(int)));
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
         if ( nFlags&MF_ENABLED )
         {
            action->setEnabled(true);
         }
         if ( (nFlags&MF_DISABLED) ||
              (nFlags&MF_GRAYED) )
         {
            action->setDisabled(true);
         }
         QObject::connect(action,SIGNAL(triggered()),this,SLOT(menuAction_triggered()));
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
   return findMenuID(_qtd->actions().at(nPos));
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
         action = _qtd->actions().at(nID);
      }
      break;
   default:
      action = findMenuItem(nID);
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
      action = _qtd->actions().at(nIDItem);
      break;
   default:
      action = findMenuItem(nIDItem);
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
      action = _qtd->actions().at(nIDItem);
      break;
   default:
      action = findMenuItem(nIDItem);
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
      action = _qtd->actions().at(nPosition);
      break;
   default:
      action = findMenuItem(nPosition);
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
      action = _qtd->actions().at(uItem);
   }
   else
   {
      action = findMenuItem(uItem);
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
   QAction* action = findMenuItem(nIDCheckItem);
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
   if ( !(nFlags&TPM_RETURNCMD) )
   {
      QObject::connect(this,SIGNAL(menuAction_triggered(int)),pWnd,SLOT(menuAction_triggered(int)));
      QObject::connect(this,SIGNAL(menuAboutToShow(CMenu*)),pWnd,SLOT(menuAboutToShow(CMenu*)));
   }
   QAction* action = _qtd->exec(QCursor::pos());
   int result = 0;
   if ( action && (nFlags&TPM_RETURNCMD) )
   {
      result = findMenuID(action);
   }
   else if ( action )
   {
      result = 1;
   }
   if ( !(nFlags&TPM_RETURNCMD) )
   {
      QObject::disconnect(this,SIGNAL(menuAction_triggered(int)),pWnd,SLOT(menuAction_triggered(int)));
      QObject::disconnect(this,SIGNAL(menuAboutToShow(CMenu*)),pWnd,SLOT(menuAboutToShow(CMenu*)));
   }
   return result;
}

UINT CMenu::EnableMenuItem(
   UINT nIDEnableItem,
   UINT nEnable
)
{
   QAction* action = findMenuItem(nIDEnableItem);
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
   _qtd = NULL;
   return TRUE;
}

IMPLEMENT_DYNAMIC(CTabCtrl,CWnd)

CTabCtrl::CTabCtrl(CWnd* parent)
   : CWnd(parent)
{
   if ( _qt )
      delete _qt;

   _grid = NULL;

   _qt = new QTabWidget(parent->toQWidget());

   // Downcast to save having to do it all over the place...
   _qtd = dynamic_cast<QTabWidget*>(_qt);

   _qtd->setMouseTracking(true);

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

IMPLEMENT_DYNAMIC(CEdit,CWnd)

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
      QObject::connect(_qtd_ptedit,SIGNAL(textChanged()),this,SIGNAL(textChanged()));

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

   _qtd->setMouseTracking(true);
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
   return (HBITMAP)0;
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
   strncpy(lpStr,_qtd->text().toLatin1().constData(),nMaxCount);
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

IMPLEMENT_DYNAMIC(CBitmapButton,CButton)

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

   _qtd->setMouseTracking(true);
   _qtd->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   _qtd->setVisible(dwStyle&WS_VISIBLE);

   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(clicked()),this,SIGNAL(clicked()));

   return TRUE;
}

IMPLEMENT_DYNAMIC(CSpinButtonCtrl,CWnd)

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

//   _qt = new QSpinBox_MFC(pParentWnd->toQWidget());
   _qt = new QSpinBox(pParentWnd->toQWidget());

   // Downcast to save having to do it all over the place...
//   _qtd = dynamic_cast<QSpinBox_MFC*>(_qt);
   _qtd = dynamic_cast<QSpinBox*>(_qt);

   // Pass-through signals
   QObject::connect(_qtd,SIGNAL(valueChanged(int)),this,SLOT(control_edited(int)));

   _qtd->setMouseTracking(true);
   _qtd->setGeometry(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
   _qtd->setMaximum(65536);
   _qtd->setVisible(dwStyle&WS_VISIBLE);

   return TRUE;
}

void CSpinButtonCtrl::control_edited(int value)
{
   emit valueChanged(_oldValue,value);
   _oldValue = value;
}

int CSpinButtonCtrl::SetPos(
   int nPos
)
{
   int pos = _qtd->value();
   _qtd->blockSignals(true);
   _oldValue = nPos;
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
   int val = _qtd->value();
   _qtd->blockSignals(true);
   _qtd->setRange(nLower,nUpper);
   if ( val < nLower )
   {
      SetPos(nLower);
   }
   if ( val > nUpper )
   {
      SetPos(nUpper);
   }
   _qtd->blockSignals(false);
}

void CSpinButtonCtrl::SetDlgItemInt(
   int nID,
   UINT nValue,
   BOOL bSigned
)
{
   _oldValue = nValue;
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
   val = QString::fromLatin1(lpszString);
#endif
   _oldValue = val.toInt();
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
   strncpy(lpStr,_qtd->text().toLatin1().constData(),nMaxCount);
#endif
   return _qtd->text().length();
}

IMPLEMENT_DYNAMIC(CSliderCtrl,CWnd)

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
   _qtd->setMouseTracking(true);

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

IMPLEMENT_DYNAMIC(CStatic,CWnd)

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
   _qtd->setMouseTracking(true);
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
   strncpy(lpStr,_qtd->text().toLatin1().constData(),nMaxCount);
#endif
   return _qtd->text().length();
}

IMPLEMENT_DYNAMIC(CGroupBox,CWnd)

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
   _qtd->setMouseTracking(true);

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
   strncpy(lpStr,_qtd->title().toLatin1().constData(),nMaxCount);
#endif
   return _qtd->title().length();
}

IMPLEMENT_DYNAMIC(CFileDialog,CCommonDialog)

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
   _qtd->hide();

#if UNICODE
   _qtd->setDefaultSuffix(QString::fromWCharArray(lpszDefExt));
   _qtd->selectFile(QString::fromWCharArray(lpszFileName));
#else
   _qtd->setDefaultSuffix(lpszDefExt);
   _qtd->selectFile(lpszFileName);
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
      return 1;
   }
   else
   {
      return 0;
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
   return TRUE;
}

BOOL CEvent::ResetEvent()
{
   return TRUE;
}

BOOL CEvent::PulseEvent()
{
   return TRUE;
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
      return 1;
   else
      return 0;
}

IMPLEMENT_DYNAMIC(CPropertyPage,CDialog)

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

   _qtd->installEventFilter(this);
   _qtd->setMouseTracking(true);

   // Pass-through signals
}

CPropertyPage::~CPropertyPage()
{
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
   _tippers.append(pWnd);
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
   _tippers.append(pWnd);
   pWnd->toQWidget()->setToolTip(lpszText);
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
     m_nIndex(0)
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

CRecentFileList::CRecentFileList( 
   UINT nStart, 
   LPCTSTR lpszSection, 
   LPCTSTR lpszEntryFormat, 
   int nSize, 
   int nMaxDispLen  
)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
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
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
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
      break;
   case Qt::Key_Alt: // 0x01000023
      break;
   case Qt::Key_AltGr: // 0x01001103	On Windows, when the KeyDown event for this key is sent, the Ctrl+Alt modifiers are also set.
      break;
   case Qt::Key_CapsLock: // 0x01000024
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
      break;
   case Qt::Key_Direction_R: // 0x01000060
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
      break;
   case Qt::Key_Backslash: // 0x5c
      break;
   case Qt::Key_BracketRight: // 0x5d
      break;
   case Qt::Key_AsciiCircum: // 0x5e
      break;
   case Qt::Key_Underscore: // 0x5f
      break;
   case Qt::Key_QuoteLeft: // 0x60
      break;
   case Qt::Key_BraceLeft: // 0x7b
      break;
   case Qt::Key_Bar: // 0x7c
      break;
   case Qt::Key_BraceRight: // 0x7d
      break;
   case Qt::Key_AsciiTilde: // 0x7e
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
