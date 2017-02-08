#include "qscilexerbin.h"

#include <QColor>
#include <QFont>
#include <QSettings>

QsciLexerBin::QsciLexerBin(QObject */*parent*/)
{
#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   setDefaultFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   setDefaultFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   setDefaultFont(QFont("Consolas", 11));
#endif
}

QsciLexerBin::~QsciLexerBin()
{
}

QString QsciLexerBin::description(int style) const
{
   // Note: this function MUST return a non-empty string
   //       for a style otherwise that style is ignored!
   switch ( style )
   {
      case Bin_Default:
         return "Default";
      break;
   }
   return "";
}

void QsciLexerBin::styleText(int start, int end)
{
   // Reset line styling.
   startStyling(start,0xFF);
   setStyling(end-start,Bin_Default);
}

QColor QsciLexerBin::defaultColor() const
{
   return QColor(0,0,0);
}

QColor QsciLexerBin::defaultColor(int style) const
{
   switch ( style )
   {
      case Bin_Default:
         return QColor(50,50,50);
      break;
      default:
         return defaultColor();
      break;
   }
}

QColor QsciLexerBin::defaultPaper() const
{
   return QColor(255,255,255);
}

QColor QsciLexerBin::defaultPaper(int /*style*/) const
{
   return QsciLexer::defaultPaper();
}

QColor QsciLexerBin::paper(int /*style*/) const
{
   return QsciLexer::defaultPaper();
}

QFont QsciLexerBin::defaultFont() const
{
#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   return QFont("Monaco", 11);
#endif
#ifdef Q_OS_LINUX
   return QFont("Monospace", 10);
#endif
#ifdef Q_OS_WIN
   return QFont("Consolas", 11);
#endif
}

QFont QsciLexerBin::defaultFont(int style) const
{
   QFont font = QsciLexerBin::defaultFont();

   return font;
}
