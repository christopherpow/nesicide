#ifndef QSCILEXERCC65_H
#define QSCILEXERCC65_H

#include <qobject.h>

#include "Qsci/qsciglobal.h"
#include "Qsci/qsciscintilla.h"
#include "Qsci/qscilexercpp.h"

class QsciLexerCC65 : public QsciLexerCPP
{
   Q_OBJECT
public:
   QsciLexerCC65(QObject *parent = 0);
   virtual ~QsciLexerCC65();

   virtual QFont defaultFont() const;
   virtual QFont defaultFont(int style) const;

   virtual QFont font(int style) const;
};

#endif // QSCILEXERCC65_H
