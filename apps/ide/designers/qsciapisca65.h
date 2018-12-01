#ifndef QSCIAPISCA65_H
#define QSCIAPISCA65_H

#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciapis.h>
#include <QObject>

class QsciAPIsCA65 : public QsciAPIs
{
   Q_OBJECT

public:
   QsciAPIsCA65(QsciLexer *lexer);

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
