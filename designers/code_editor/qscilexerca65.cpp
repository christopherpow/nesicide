#include "qscilexerca65.h"

#include <QColor>
#include <QFont>
#include <QSettings>

// Styles
enum
{
   CA65_Default,
   CA65_Opcode
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
      case CA65_Opcode:
         return "Opcode";
      break;
      default:
         return QString();
      break;
   }
}

void QsciLexerCA65::styleText(int start, int end)
{
   QString st = "styleText(";
   st += QString::number(start);
   st += ",";
   st += QString::number(end);
   st += ")";
   qDebug(st.toAscii().constData());
   startStyling(start);
   setStyling(end-start,CA65_Opcode);
}

QColor QsciLexerCA65::color(int style) const
{
   QString st = "color(";
   st += QString::number(style);
   st += ")";
   qDebug(st.toAscii().constData());
   switch ( style )
   {
      case CA65_Default:
         return QColor(0,0,0);
      break;
      case CA65_Opcode:
         return QColor(0,0,0);
      break;
      default:
         return QColor(255,255,255);
      break;
   }
}

QColor QsciLexerCA65::paper(int style) const
{
   QString st = "color(";
   st += QString::number(style);
   st += ")";
   qDebug(st.toAscii().constData());
   switch ( style )
   {
      case CA65_Default:
         return QColor(225,225,225);
      break;
      case CA65_Opcode:
         return QColor(225,225,225);
      break;
      default:
         return QColor(255,255,255);
      break;
   }
}
