#include "paletteeditorwindow.h"
#include "ui_paletteeditorwindow.h"

PaletteEditorWindow::PaletteEditorWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PaletteEditorWindow)
{
    ui->setupUi(this);
    ui->webView->setHtml("<html><head></head>"
                         "<body>"
                         "Palette Name: <input type='text' value='palette0' />"
                         "</body>"
                         "</html>", QUrl("localhost"));
}

PaletteEditorWindow::~PaletteEditorWindow()
{
    delete ui;
}

void PaletteEditorWindow::changeEvent(QEvent *e)
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

void PaletteEditorWindow::on_webView_titleChanged(QString title)
{

}
