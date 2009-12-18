#ifndef CBUILDERTEXTLOGGER_H
#define CBUILDERTEXTLOGGER_H

#include <QPlainTextEdit>

class CBuilderTextLogger
{
public:
    CBuilderTextLogger();
    void setTextEditControl(QPlainTextEdit *control);
    void clear();
    void write(QString text);
private:
    QPlainTextEdit *m_textEdit;
};

extern CBuilderTextLogger builderTextLogger;

#endif // CBUILDERTEXTLOGGER_H
