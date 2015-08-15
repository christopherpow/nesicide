#include "qscilexercc65.h"

#include <QColor>
#include <QFont>
#include <QSettings>

QsciLexerCC65::QsciLexerCC65(QObject */*parent*/)
{
#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   setDefaultFont(QFont("Monaco", 11));
#endif
#ifdef Q_WS_X11
   setDefaultFont(QFont("Monospace", 10));
#endif
#ifdef Q_WS_WIN
   setDefaultFont(QFont("Consolas", 11));
#endif
}

QsciLexerCC65::~QsciLexerCC65()
{
}
