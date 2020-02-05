#include "qsciapisca65.h"

QsciAPIsCA65::QsciAPIsCA65(QsciLexer *lexer)
   : QsciAPIs(lexer)
{

}

QsciAPIsCA65::~QsciAPIsCA65()
{

}

//! \reimp
void QsciAPIsCA65::updateAutoCompletionList(const QStringList &context,
        QStringList &list)
{
   QsciAPIs::updateAutoCompletionList(context,list);
}

//! \reimp
void QsciAPIsCA65::autoCompletionSelected(const QString &sel)
{
   QString str = sel.split(":")[0];
   qDebug("autoCompletionSelected: %s",str.toUtf8().data());
   QsciAPIs::autoCompletionSelected(str);
}

//! \reimp
QStringList QsciAPIsCA65::callTips(const QStringList &context, int commas,
        QsciScintilla::CallTipsStyle style, QList<int> &shifts)
{
   qDebug("callTips: commas=%d",commas);
   foreach ( QString str, context )
   {
      qDebug(str.toUtf8().data());
   }
   return QsciAPIs::callTips(context,commas,style,shifts);
}
