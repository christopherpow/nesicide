#include "qscilexercc65.h"

#include <QColor>
#include <QFont>
#include <QSettings>

QsciLexerCC65::QsciLexerCC65(QObject */*parent*/)
{
#ifdef Q_WS_MAC
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
