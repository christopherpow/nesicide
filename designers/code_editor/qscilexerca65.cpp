#include "qscilexerca65.h"

#include <QColor>
#include <QFont>
#include <QSettings>

// Styles
enum
{
   CA65_Default,
   CA65_Comment,
   CA65_Opcode,
   CA65_HighlightedLine
};

QsciLexerCA65::QsciLexerCA65(QObject *parent)
{
}

QsciLexerCA65::~QsciLexerCA65()
{
}

QString QsciLexerCA65::description(int style) const
{
   switch ( style )
   {
      case CA65_Default:
         return "Default";
      break;
      case CA65_Comment:
         return "Comment";
      break;
      case CA65_Opcode:
         return "Opcode";
      break;
      case CA65_HighlightedLine:
         return "Highlighted Line";
      break;
      default:
         return QString();
      break;
   }
}

void QsciLexerCA65::styleText(int start, int end)
{
   QByteArray  chars;
   QString     text;
   QStringList lines;
   int         textPos;
   int         pos;

   // Get the text that is being styled.
   chars.reserve((end-start)+1);
   editor()->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE,start,end,chars.data());

#if 0
   // Break the text into line chunks.
   text = text.append(chars);
   lines = text.split(QRegExp("[\r\n]"));

   // Style the lines.
   startStyling(start);
   textPos = start;
   int num = lines.count();

   setStyling(100,CA65_HighlightedLine);
   foreach ( const QString& line, lines )
   {
      qDebug(QString::number(num++).toAscii().constData());
      // Lastly, look for comments.
      pos = line.indexOf(';');
      if ( pos >= 0 )
      {
         qDebug(QString::number(pos).toAscii().constData());
         qDebug(QString::number(textPos).toAscii().constData());
//         setStyling(-(pos-line.length()),CA65_Default);
         setStyling(line.length()-pos,CA65_Comment);
      }
   }
#endif
}

const char* QsciLexerCA65::wordCharacters() const
{
   const char* chars = QsciLexerCustom::wordCharacters();
   QString st = "wordCharacters";
   qDebug(st.toAscii().constData());
   qDebug(chars);
   return chars;
}

QColor QsciLexerCA65::defaultColor() const
{
   return QColor(0,0,0);
}

QColor QsciLexerCA65::defaultColor(int style) const
{
   switch ( style )
   {
      case CA65_Default:
         return QColor(50,50,50);
      break;
      case CA65_Comment:
         return QColor(0,255,45);
      break;
      case CA65_Opcode:
         return QColor(0,0,0);
      break;
      case CA65_HighlightedLine:
         return QColor(0,0,0);
      break;
      default:
         return QColor(0,0,0);
      break;
   }
}

QColor QsciLexerCA65::color(int style) const
{
   QString st = "color(";
   st += QString::number(style);
   st += ")";
   qDebug(st.toAscii().constData());

   return QsciLexerCA65::defaultColor(style);
}

QColor QsciLexerCA65::defaultPaper() const
{
   return QColor(255,255,255);
}

QColor QsciLexerCA65::defaultPaper(int style) const
{
   switch ( style )
   {
      case CA65_Default:
         return QColor(245,245,245);
      break;
      case CA65_Comment:
         return QColor(245,245,245);
      break;
      case CA65_Opcode:
         return QColor(245,245,245);
      break;
      case CA65_HighlightedLine:
         return QColor(215,215,215);
      break;
      default:
         return QColor(255,255,255);
      break;
   }
}

QColor QsciLexerCA65::paper(int style) const
{
   QString st = "paper(";
   st += QString::number(style);
   st += ")";
   qDebug(st.toAscii().constData());

   return QsciLexerCA65::defaultPaper(style);
}

QFont QsciLexerCA65::defaultFont() const
{
#ifdef Q_WS_MAC
   return QFont("Monaco", 11);
#endif
#ifdef Q_WS_X11
   return QFont("Monospace", 10);
#endif
#ifdef Q_WS_WIN
   return QFont("Consolas", 11);
#endif
}

QFont QsciLexerCA65::defaultFont(int style) const
{
   QFont font = QsciLexerCA65::defaultFont();

   switch ( style )
   {
      case CA65_Comment:
         font.setItalic(true);
      break;
   }

   return font;
}

QFont QsciLexerCA65::font(int style) const
{
   QString st = "font(";
   st += QString::number(style);
   st += ")";
   qDebug(st.toAscii().constData());

   return QsciLexerCA65::defaultFont(style);
}
