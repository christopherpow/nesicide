#ifndef QSCILEXERCA65_H
#define QSCILEXERCA65_H

#include <qobject.h>

#include "Qsci/qsciglobal.h"
#include "Qsci/qsciscintilla.h"
#include "Qsci/qscilexercustom.h"

class QsciLexerCA65 : public QsciLexerCustom
{
   Q_OBJECT
public:
   QsciLexerCA65(QObject *parent = 0);
   virtual ~QsciLexerCA65();

   virtual const char* language() const { return "CA65"; }
   virtual QString description(int style) const;
   virtual void styleText(int start,int end);

   virtual bool eolFill(int style) const { return true; }

   virtual QColor defaultColor() const;
   virtual QColor defaultColor(int style) const;
   virtual QColor color(int style) const;

   virtual QColor defaultPaper() const;
   virtual QColor defaultPaper(int style) const;
   virtual QColor paper(int style) const;

   virtual QFont defaultFont() const;
   virtual QFont defaultFont(int style) const;
   virtual QFont font(int style) const;

   virtual const char *wordCharacters() const;

   virtual int styleBitsNeeded() const { return 6; }

protected:
   QRegExp opcodeRegex;
   QRegExp keywordRegex;
};

#endif // QSCILEXERCA65_H
