#include "qscilexercc65.h"

#include <QColor>
#include <QFont>
#include <QSettings>

QsciLexerCC65::QsciLexerCC65(QObject */*parent*/)
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

QsciLexerCC65::~QsciLexerCC65()
{
}

QFont QsciLexerCC65::defaultFont() const
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

QFont QsciLexerCC65::defaultFont(int style) const
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

