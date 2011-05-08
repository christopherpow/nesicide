#ifndef QSCILEXERCA65_H
#define QSCILEXERCA65_H

#include "Qsci/qscilexercustom.h"

class QsciLexerCA65 : public QsciLexerCustom
{
   Q_OBJECT

public:
    QsciLexerCA65(QObject* parent = 0);

    virtual void styleText(int start,int end);

    virtual const char* language() const { return "CA65"; }
    virtual QString description(int param) const { return "description"; }
};

#endif // QSCILEXERCA65_H
