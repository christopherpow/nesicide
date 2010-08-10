#include "nametabledisplaydialog.h"
#include "ui_nametabledisplaydialog.h"

#include "cnessystempalette.h"

#include "dbg_cnesppu.h"

#include "main.h"

NameTableDisplayDialog::NameTableDisplayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NameTableDisplayDialog)
{
    ui->setupUi(this);
    imgData = new char[512*512*3];

    CPPUDBG::NameTableInspectorTV((int8_t*)imgData);

    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(renderData()) );
    QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()) );

    renderer = new CNameTablePreviewRenderer(ui->frame,imgData);
    ui->frame->layout()->addWidget(renderer);
    ui->frame->layout()->update();

    ui->showVisible->setChecked ( true );
}

void NameTableDisplayDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    CPPUDBG::EnableNameTableInspector(true);
    renderData();
}

void NameTableDisplayDialog::hideEvent(QHideEvent *event)
{
    QDialog::hideEvent(event);
    CPPUDBG::EnableNameTableInspector(false);
}

void NameTableDisplayDialog::changeEvent(QEvent *e)
{
   QDialog::changeEvent(e);
   switch (e->type()) {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
      break;
      default:
      break;
   }
}

void NameTableDisplayDialog::renderData()
{
   if ( isVisible() )
   {
      CPPUDBG::RENDERNAMETABLE();
      renderer->updateGL ();
   }
}

NameTableDisplayDialog::~NameTableDisplayDialog()
{
   delete imgData;
   delete ui;
}

void NameTableDisplayDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateScrollbars();
}

void NameTableDisplayDialog::on_zoomSlider_valueChanged(int value)
{
    renderer->changeZoom(value);
    ui->zoomValueLabel->setText(QString::number(value).append("%"));
    updateScrollbars();
}

void NameTableDisplayDialog::updateScrollbars()
{
    int value = ui->zoomSlider->value();
    int viewWidth = (float)512 * ((float)value / 100.0f);
    int viewHeight = (float)480 * ((float)value / 100.0f);
    ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
    ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
    renderer->scrollX = ui->horizontalScrollBar->value();
    renderer->scrollY = ui->verticalScrollBar->value();
}

void NameTableDisplayDialog::on_horizontalScrollBar_valueChanged(int value)
{
    renderer->scrollX = ui->horizontalScrollBar->value();
    renderer->repaint();
}

void NameTableDisplayDialog::on_verticalScrollBar_valueChanged(int value)
{
    renderer->scrollY = ui->verticalScrollBar->value();
    renderer->repaint();
}

void NameTableDisplayDialog::on_showVisible_toggled(bool checked)
{
    CPPUDBG::SetPPUViewerShowVisible ( checked );
}
