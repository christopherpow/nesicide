#include "qscilexerdefault.h"

#include <QColor>
#include <QFont>
#include <QSettings>

QsciLexerDefault::QsciLexerDefault(QObject */*parent*/)
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

QsciLexerDefault::~QsciLexerDefault()
{
}

QString QsciLexerDefault::description(int style) const
{
   // Note: this function MUST return a non-empty string
   //       for a style otherwise that style is ignored!
   switch ( style )
   {
      case Default_Default:
         return "Default";
      break;
      default:
         return QString();
      break;
   }
}

void QsciLexerDefault::styleText(int start, int end)
{
   startStyling(start,0xFF);
   setStyling(end-start,Default_Default);
}

const char* QsciLexerDefault::wordCharacters() const
{
   const char* chars = QsciLexerCustom::wordCharacters();
   return chars;
}

QColor QsciLexerDefault::defaultColor() const
{
   return QColor(0,0,0);
}

QColor QsciLexerDefault::defaultColor(int style) const
{
   switch ( style )
   {
      case Default_Default:
         return QColor(50,50,50);
      break;
      default:
         return defaultColor();
      break;
   }
}

QColor QsciLexerDefault::defaultPaper() const
{
   return QColor(255,255,255);
}

QColor QsciLexerDefault::defaultPaper(int /*style*/) const
{
   return QsciLexer::defaultPaper();
}

QColor QsciLexerDefault::paper(int /*style*/) const
{
   return QsciLexer::defaultPaper();
}

QFont QsciLexerDefault::defaultFont() const
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

QFont QsciLexerDefault::defaultFont(int /*style*/) const
{
   return QsciLexerDefault::defaultFont();
}

bool QsciLexerDefault::readProperties(QSettings &qs,const QString &prefix)
{
   return QsciLexer::readProperties(qs,prefix);
}

bool QsciLexerDefault::writeProperties(QSettings &qs,const QString &prefix) const
{
   return QsciLexer::writeProperties(qs,prefix);
}

bool QsciLexerDefault::readSettings(QSettings &qs,const char *prefix)
{
   return QsciLexer::readSettings(qs,prefix);
}

bool QsciLexerDefault::writeSettings(QSettings &qs,const char *prefix)
{
   return QsciLexer::writeSettings(qs,prefix);
}
