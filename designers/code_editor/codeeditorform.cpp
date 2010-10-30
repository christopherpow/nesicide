#include "codeeditorform.h"
#include "ui_codeeditorform.h"

#include <QToolTip>

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

void CodeEditorForm::on_textEdit_selectionChanged()
{
   // Figure out what is being selected and show an appropriate tooltip...
   // 1. If selection is 6502 opcode, show helpful information on it.
   // 2. If selection is a compiler-identified symbol, show its address and value.
   // 3. If selection is a macro, show its declaration.
   // 4. If selection is a label, show its address.
   // 5. If selection is an expression, evaluate and display its current value.
}
