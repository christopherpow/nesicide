#include "nesemulatordockwidget.h"
#include "ui_nesemulatordockwidget.h"

#include "main.h"

#include "emulatorprefsdialog.h"

#include "emulator_core.h"

NESEmulatorDockWidget::NESEmulatorDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::NESEmulatorDockWidget)
{
   int32_t i;

   imgData = new char[256*256*4];

   ui->setupUi(this);

   renderer = new CNESEmulatorRenderer(ui->frame, imgData);
   renderer->setMouseTracking(true);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   QObject::connect(emulator, SIGNAL(emulatedFrame()), this, SLOT(renderData()));
   QObject::connect(breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()));

   m_joy [ CONTROLLER1 ] = 0x00;
   m_joy [ CONTROLLER2 ] = 0x00;

   // Clear image...
   for ( i = 0; i < 256*256*4; i+=4 )
   {
      imgData[i] = 0;
      imgData[i+1] = 0;
      imgData[i+2] = 0;
      imgData[i+3] = 0xFF;
   }
   nesSetTVOut((int8_t*)imgData);
}

NESEmulatorDockWidget::~NESEmulatorDockWidget()
{
    delete ui;
    delete renderer;
    delete imgData;
}

void NESEmulatorDockWidget::changeEvent(QEvent* e)
{
   QDockWidget::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void NESEmulatorDockWidget::mousePressEvent(QMouseEvent* event)
{
   // CPTODO: defaulted controller 2 to zapper for now
   if ( event->button()&Qt::LeftButton )
   {
      m_joy [ CONTROLLER2 ] |= JOY_TRIGGER;

      nesSetControllerScreenPosition(CONTROLLER2,event->pos().x()-ui->frame->pos().x(),event->pos().y()-ui->frame->pos().y());

      emulator->controllerInput ( m_joy );

      event->accept();
   }
}

void NESEmulatorDockWidget::mouseReleaseEvent(QMouseEvent* event)
{
   // CPTODO: defaulted controller 2 to zapper for now
   if ( event->button()&Qt::LeftButton )
   {
      m_joy [ CONTROLLER2 ] &= (~(JOY_TRIGGER));

      nesSetControllerScreenPosition(CONTROLLER2,event->pos().x()-ui->frame->pos().x(),event->pos().y()-ui->frame->pos().y());

      emulator->controllerInput ( m_joy );

      event->accept();
   }
}

void NESEmulatorDockWidget::mouseMoveEvent(QMouseEvent* event)
{
   // CPTODO: defaulted controller 2 to zapper for now
   nesSetControllerScreenPosition(CONTROLLER2,event->pos().x()-ui->frame->pos().x(),event->pos().y()-ui->frame->pos().y());
   event->accept();
}

void NESEmulatorDockWidget::keyPressEvent(QKeyEvent* event)
{
   if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_LEFT) )
   {
      m_joy [ CONTROLLER1 ] |= JOY_LEFT;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_RIGHT) )
   {
      m_joy [ CONTROLLER1 ] |= JOY_RIGHT;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_UP) )
   {
      m_joy [ CONTROLLER1 ] |= JOY_UP;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_DOWN) )
   {
      m_joy [ CONTROLLER1 ] |= JOY_DOWN;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_SELECT) )
   {
      m_joy [ CONTROLLER1 ] |= JOY_SELECT;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_START) )
   {
      m_joy [ CONTROLLER1 ] |= JOY_START;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_B) )
   {
      m_joy [ CONTROLLER1 ] |= JOY_B;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_A) )
   {
      m_joy [ CONTROLLER1 ] |= JOY_A;
   }

   if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_LEFT) )
   {
      m_joy [ CONTROLLER2 ] |= JOY_LEFT;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_RIGHT) )
   {
      m_joy [ CONTROLLER2 ] |= JOY_RIGHT;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_UP) )
   {
      m_joy [ CONTROLLER2 ] |= JOY_UP;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_DOWN) )
   {
      m_joy [ CONTROLLER2 ] |= JOY_DOWN;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_SELECT) )
   {
      m_joy [ CONTROLLER2 ] |= JOY_SELECT;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_START) )
   {
      m_joy [ CONTROLLER2 ] |= JOY_START;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_B) )
   {
      m_joy [ CONTROLLER2 ] |= JOY_B;
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_A) )
   {
      m_joy [ CONTROLLER2 ] |= JOY_A;
   }

   emulator->controllerInput ( m_joy );

   event->accept();
}

void NESEmulatorDockWidget::keyReleaseEvent(QKeyEvent* event)
{
   if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_LEFT) )
   {
      m_joy [ CONTROLLER1 ] &= (~JOY_LEFT);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_RIGHT) )
   {
      m_joy [ CONTROLLER1 ] &= (~JOY_RIGHT);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_UP) )
   {
      m_joy [ CONTROLLER1 ] &= (~JOY_UP);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_DOWN) )
   {
      m_joy [ CONTROLLER1 ] &= (~JOY_DOWN);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_SELECT) )
   {
      m_joy [ CONTROLLER1 ] &= (~JOY_SELECT);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_START) )
   {
      m_joy [ CONTROLLER1 ] &= (~JOY_START);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_B) )
   {
      m_joy [ CONTROLLER1 ] &= (~JOY_B);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_StandardJoypad_A) )
   {
      m_joy [ CONTROLLER1 ] &= (~JOY_A);
   }

   if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_LEFT) )
   {
      m_joy [ CONTROLLER2 ] &= (~JOY_LEFT);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_RIGHT) )
   {
      m_joy [ CONTROLLER2 ] &= (~JOY_RIGHT);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_UP) )
   {
      m_joy [ CONTROLLER2 ] &= (~JOY_UP);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_DOWN) )
   {
      m_joy [ CONTROLLER2 ] &= (~JOY_DOWN);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_SELECT) )
   {
      m_joy [ CONTROLLER2 ] &= (~JOY_SELECT);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_START) )
   {
      m_joy [ CONTROLLER2 ] &= (~JOY_START);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_B) )
   {
      m_joy [ CONTROLLER2 ] &= (~JOY_B);
   }
   else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_StandardJoypad_A) )
   {
      m_joy [ CONTROLLER2 ] &= (~JOY_A);
   }

   emulator->controllerInput ( m_joy );

   event->accept();
}

void NESEmulatorDockWidget::renderData()
{
   renderer->updateGL();
}
