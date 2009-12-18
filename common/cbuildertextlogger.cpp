#include "cbuildertextlogger.h"

CBuilderTextLogger builderTextLogger;

CBuilderTextLogger::CBuilderTextLogger()
{
    m_textEdit = (QPlainTextEdit *)NULL;
}

void CBuilderTextLogger::setTextEditControl(QPlainTextEdit *control)
{
    m_textEdit = control;
}

void CBuilderTextLogger::clear()
{
    if (m_textEdit)
        m_textEdit->clear();
}

void CBuilderTextLogger::write(QString text)
{
    if (m_textEdit)
        m_textEdit->appendHtml(text);
}
