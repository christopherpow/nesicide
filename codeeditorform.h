#ifndef CODEEDITORFORM_H
#define CODEEDITORFORM_H

#include <QWidget>

namespace Ui {
    class CodeEditorForm;
}

class CodeEditorForm : public QWidget {
    Q_OBJECT
public:
    CodeEditorForm(QWidget *parent = 0);
    ~CodeEditorForm();

    QString get_sourceCode();
    void set_sourceCode(QString source);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CodeEditorForm *ui;
};

#endif // CODEEDITORFORM_H
