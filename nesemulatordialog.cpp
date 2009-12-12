#include "nesemulatordialog.h"
#include "ui_nesemulatordialog.h"

NESEmulatorDialog::NESEmulatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NESEmulatorDialog)
{
    ui->setupUi(this);
    ui->stopButton->setVisible(false);
    ui->pauseButton->setVisible(false);
    ui->stepButton->setVisible(false);
}

NESEmulatorDialog::~NESEmulatorDialog()
{
    delete ui;
}

void NESEmulatorDialog::stopEmulation()
{
    emit on_stopButton_clicked();
}

void NESEmulatorDialog::changeEvent(QEvent *e)
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

void NESEmulatorDialog::on_playButton_clicked()
{
    ui->playButton->setVisible(false);
    ui->stopButton->setVisible(true);
    ui->pauseButton->setVisible(true);
    ui->stepButton->setVisible(false);

    // TODO: Start or continue emulation


}

void NESEmulatorDialog::on_stopButton_clicked()
{
    // The following happens if the user closes the emulation window from the main UI. The main
    // UI calls a stop method to halt emulation, which method generates a signal that hits this slot.
    // In some cases the emulator is already stopped (in which the stop button will be hidden) so
    // we simply check for this condition before trying to do anything else.
    if (!ui->stopButton->isVisible())
        return;

    ui->playButton->setVisible(true);
    ui->stopButton->setVisible(false);
    ui->pauseButton->setVisible(false);
    ui->stepButton->setVisible(false);

    // TODO: Stop emulation (and reset)


}

void NESEmulatorDialog::on_pauseButton_clicked()
{
    ui->playButton->setVisible(true);
    ui->stopButton->setVisible(true);
    ui->pauseButton->setVisible(false);
    ui->stepButton->setVisible(true);

    // TODO: Pause emulation


}

void NESEmulatorDialog::on_stepButton_clicked()
{
    // TODO: Run one PPU cycle's worth of clocks on the PPU.
    //       Since PPU executes faster it makes sense to align the steps
    //       on PPU cycles but you can do it on CPU cycles if you really want to.
}
