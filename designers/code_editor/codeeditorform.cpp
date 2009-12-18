#include "codeeditorform.h"
#include "ui_codeeditorform.h"

CodeEditorForm::CodeEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CodeEditorForm)
{
    ui->setupUi(this);
}

CodeEditorForm::~CodeEditorForm()
{
    delete ui;
}

void CodeEditorForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString CodeEditorForm::get_sourceCode()
{
    return ui->textEdit->toPlainText();
}

void CodeEditorForm::set_sourceCode(QString source)
{
    ui->textEdit->setPlainText(source);
}

void CodeEditorForm::on_textEdit_textChanged()
{

}
