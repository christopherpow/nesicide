#ifndef QSCILEXERCA65_H
#define QSCILEXERCA65_H

#include <qobject.h>

#include <Qsci/qsciglobal.h>
#include <Qsci/qscilexercustom.h>

class QsciLexerCA65 : public QsciLexerCustom
{
   Q_OBJECT
public:
   QsciLexerCA65(QObject *parent = 0);
   virtual ~QsciLexerCA65();

   virtual const char* language() const { return "CA65"; }
   virtual QString description(int style) const;
   virtual void styleText(int start,int end);

   virtual QColor color(int style) const;
   virtual QColor paper(int style) const;
   virtual bool eolFill(int style) const { return true; }
};

#endif // QSCILEXERCA65_H
