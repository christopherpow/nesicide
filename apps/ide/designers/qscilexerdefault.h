#ifndef QSCILEXERDEFAULT_H
#define QSCILEXERDEFAULT_H

#include <qobject.h>

#include "Qsci/qsciglobal.h"
#include "Qsci/qsciscintilla.h"
#include "Qsci/qscilexercustom.h"

class QsciLexerDefault : public QsciLexerCustom
{
   Q_OBJECT
public:
   // Styles
   enum
   {
      Default_Default
   };

   QsciLexerDefault(QObject *parent = 0);
   virtual ~QsciLexerDefault();

   virtual const char* language() const { return "Default"; }
   virtual QString description(int style) const;
   virtual void styleText(int start,int end);

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
};

#endif // QSCILEXERDEFAULT_H
