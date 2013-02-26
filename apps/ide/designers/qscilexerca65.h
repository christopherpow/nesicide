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
   // Styles
   enum
   {
      CA65_Default,
      CA65_Opcode,
      CA65_Label,
      CA65_Keyword,
      CA65_QuotedString,
      CA65_Comment,
      CA65_Number
   };

   QsciLexerCA65(QObject *parent = 0);
   virtual ~QsciLexerCA65();

   virtual const char* language() const { return "CA65"; }
   virtual QString description(int style) const;
   virtual void styleText(int start,int end);

   const char *blockEnd(int *style = 0) const;
   const char *blockStart(int *style = 0) const;
   const char *blockStartKeyword(int *style = 0) const;
   
   virtual bool eolFill(int /*style*/) const { return true; }

   virtual QColor defaultColor() const;
   virtual QColor defaultColor(int style) const;

   virtual QColor defaultPaper() const;
   virtual QColor defaultPaper(int style) const;
   virtual QColor paper(int style) const;

   virtual QFont defaultFont() const;
   virtual QFont defaultFont(int style) const;

   virtual const char *wordCharacters() const;

   virtual int styleBitsNeeded() const { return 8; }

   bool readSettings(QSettings &qs,const char *prefix = "/Scintilla");
   bool writeSettings(QSettings &qs,const char *prefix = "/Scintilla");

protected:
   bool readProperties(QSettings &qs,const QString &prefix);
   bool writeProperties(QSettings &qs,const QString &prefix) const;

protected:
   QRegExp opcodeRegex;
   QRegExp keywordRegex;
   QRegExp labelRegex;
   QRegExp numberRegex;
};

#endif // QSCILEXERCA65_H
