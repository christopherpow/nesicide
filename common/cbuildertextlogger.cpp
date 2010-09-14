#include "cbuildertextlogger.h"

CTextLogger generalTextLogger;
CTextLogger buildTextLogger;
CTextLogger debugTextLogger;

CTextLogger::CTextLogger()
: m_textEdit(NULL)
{
}

void CTextLogger::clear()
{
    if (m_textEdit)
        m_textEdit->clear();
}

void CTextLogger::write(QString text)
{
    if (m_textEdit)
        m_textEdit->appendHtml(text);
}
