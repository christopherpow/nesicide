#ifndef QSCILEXERBIN_H
#define QSCILEXERBIN_H

#include <qobject.h>

#include "Qsci/qsciglobal.h"
#include "Qsci/qsciscintilla.h"
#include "Qsci/qscilexercustom.h"

class QsciLexerBin : public QsciLexerCustom
{
   Q_OBJECT
public:
   // Styles
   enum
   {
      Bin_Default
   };

   QsciLexerBin(QObject *parent = 0);
   virtual ~QsciLexerBin();

   virtual const char* language() const { return "Bin"; }
   virtual QString description(int style) const;

   virtual void styleText(int start, int end);

   virtual bool eolFill(int /*style*/) const { return true; }

   virtual QColor defaultColor() const;
   virtual QColor defaultColor(int style) const;

   virtual QColor defaultPaper() const;
   virtual QColor defaultPaper(int style) const;
   virtual QColor paper(int style) const;

   virtual QFont defaultFont() const;
   virtual QFont defaultFont(int style) const;

   virtual int styleBitsNeeded() const { return 1; }
};

#endif // QSCILEXERBIN_H
