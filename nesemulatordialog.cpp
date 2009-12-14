#include "nesemulatordialog.h"
#include "ui_nesemulatordialog.h"
#include "main.h"

#include "PPU.h"

NESEmulatorDialog::NESEmulatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NESEmulatorDialog)
{
    imgData = new char[256*256*3];

    ui->setupUi(this);
    ui->stopButton->setVisible(false);
    ui->pauseButton->setVisible(false);
    ui->stepButton->setVisible(false);

    timer = new QBasicTimer();

    renderer = new CNESEmulatorRenderer(ui->frame, imgData);
    ui->frame->layout()->addWidget(renderer);
    ui->frame->layout()->update();
    CPPU::TV ( imgData );
}

NESEmulatorDialog::~NESEmulatorDialog()
{
    delete ui;
    delete timer;
}

void NESEmulatorDialog::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer->timerId())
    {
        renderer->updateGL ();
        timer->start(16, this);
    }
    else
    {
        QDialog::timerEvent(event);
    }
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

    emulator->setRunning ( true );

    timer->start ( 16, this );
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

    timer->stop ();

    emulator->setRunning ( false );
}

void NESEmulatorDialog::on_pauseButton_clicked()
{
    ui->playButton->setVisible(true);
    ui->stopButton->setVisible(true);
    ui->pauseButton->setVisible(false);
    ui->stepButton->setVisible(true);

    timer->stop ();

    emulator->setRunning ( false );
}

void NESEmulatorDialog::on_stepButton_clicked()
{
    // TODO: Run one PPU cycle's worth of clocks on the PPU.
    //       Since PPU executes faster it makes sense to align the steps
    //       on PPU cycles but you can do it on CPU cycles if you really want to.
    // CPTODO: re-factor stepping routine first...
}
