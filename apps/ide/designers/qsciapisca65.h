#ifndef QSCIAPISCA65_H
#define QSCIAPISCA65_H

#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciapis.h>
#include <QObject>

class QsciAPIsCA65 : public QsciAPIs
{
public:
   QsciAPIsCA65(QsciLexer *lexer);
   virtual ~QsciAPIsCA65();

   //! \reimp
   virtual void updateAutoCompletionList(const QStringList &context,
           QStringList &list);

   //! \reimp
   virtual void autoCompletionSelected(const QString &sel);

   //! \reimp
   virtual QStringList callTips(const QStringList &context, int commas,
           QsciScintilla::CallTipsStyle style, QList<int> &shifts);
};

#endif // QSCIAPISCA65_H
