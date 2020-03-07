#include "nesemulatordockwidget.h"
#include "ui_nesemulatordockwidget.h"

#include "emulatorprefsdialog.h"
#include "cobjectregistry.h"

#include "nes_emulator_core.h"

NESEmulatorDockWidget::NESEmulatorDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::NESEmulatorDockWidget)
{
   int32_t i;

   imgData = new char[256*256*4];

   // Clear image to set alpha channel...
   for ( i = 0; i < 256*256*4; i+=4 )
   {
      imgData[i] = 0;
      imgData[i+1] = 0;
      imgData[i+2] = 0;
      imgData[i+3] = 0xFF;
   }

   ui->setupUi(this);

   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

   fakeTitleBar = new QWidget();
   fakeTitleBar->setMaximumHeight(0);
   savedTitleBar = titleBarWidget();
   setTitleBarWidget(fakeTitleBar);

   renderer = new CNESEmulatorRenderer(ui->frame, imgData);
   renderer->setMouseTracking(true);

   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   nesSetTVOut((int8_t*)imgData);

   QObject::connect(emulator, SIGNAL(emulatedFrame()), this, SLOT(renderData()));
   QObject::connect(this,SIGNAL(controllerInput(uint32_t*)),emulator,SLOT(controllerInput(uint32_t*)));

   m_joy [ CONTROLLER1 ] = 0;
   m_joy [ CONTROLLER2 ] = 0;
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

void NESEmulatorDockWidget::fixTitleBar()
{
   QWidget* swap = titleBarWidget();
   setTitleBarWidget(savedTitleBar);
   savedTitleBar = swap;
}

void NESEmulatorDockWidget::mousePressEvent(QMouseEvent* event)
{
   if ( event->button()&Qt::LeftButton )
   {
      // Check for Zapper connected to mouse clicks.
      if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_Zapper) &&
           (EmulatorPrefsDialog::getControllerMouseMap(CONTROLLER1,IO_Zapper_FIRE)) )
      {
         m_joy [ CONTROLLER1 ] |= ZAPPER_TRIGGER;
      }
      if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_Zapper) &&
           (EmulatorPrefsDialog::getControllerMouseMap(CONTROLLER2,IO_Zapper_FIRE)) )
      {
         m_joy [ CONTROLLER2 ] |= ZAPPER_TRIGGER;
      }

      // Check for Vaus(Arkanoid) connected to mouse clicks.
      if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_Vaus) &&
           (EmulatorPrefsDialog::getControllerMouseMap(CONTROLLER1,IO_Vaus_FIRE)) )
      {
         m_joy [ CONTROLLER1 ] |= VAUS_FIRE;
      }
      if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_Vaus) &&
           (EmulatorPrefsDialog::getControllerMouseMap(CONTROLLER2,IO_Vaus_FIRE)) )
      {
         m_joy [ CONTROLLER2 ] |= VAUS_FIRE;
      }

      emit controllerInput ( m_joy );

      event->accept();
   }
}

void NESEmulatorDockWidget::mouseReleaseEvent(QMouseEvent* event)
{
   if ( event->button()&Qt::LeftButton )
   {
      // Check for Zappers connected to mouse clicks.
      if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_Zapper) &&
           (EmulatorPrefsDialog::getControllerMouseMap(CONTROLLER1,IO_Zapper_FIRE)) )
      {
         m_joy [ CONTROLLER1 ] &= (~(ZAPPER_TRIGGER));
      }
      if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_Zapper) &&
           (EmulatorPrefsDialog::getControllerMouseMap(CONTROLLER2,IO_Zapper_FIRE)) )
      {
         m_joy [ CONTROLLER2 ] &= (~(ZAPPER_TRIGGER));
      }

      // Check for Vaus(Arkanoid) connected to mouse clicks.
      if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_Vaus) &&
           (EmulatorPrefsDialog::getControllerMouseMap(CONTROLLER1,IO_Vaus_FIRE)) )
      {
         m_joy [ CONTROLLER1 ] &= (~(VAUS_FIRE));
      }
      if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_Vaus) &&
           (EmulatorPrefsDialog::getControllerMouseMap(CONTROLLER2,IO_Vaus_FIRE)) )
      {
         m_joy [ CONTROLLER2 ] &= (~(VAUS_FIRE));
      }

      emit controllerInput ( m_joy );

      event->accept();
   }
}

void NESEmulatorDockWidget::keyPressEvent(QKeyEvent* event)
{
   if ( EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_TurboJoypad )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_TurboJoypad_ATURBO) )
      {
         m_joy [ CONTROLLER1 ] |= JOY_ATURBO;
      }
      else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_TurboJoypad_BTURBO) )
      {
         m_joy [ CONTROLLER1 ] |= JOY_BTURBO;
      }
   }
   if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_StandardJoypad) ||
        (EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_TurboJoypad) )
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
   }
   else if ( EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_Zapper )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_Zapper_FIRE) )
      {
         m_joy [ CONTROLLER1 ] |= ZAPPER_TRIGGER;
      }
   }
   else if ( EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_Vaus )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_Vaus_FIRE) )
      {
         m_joy [ CONTROLLER1 ] |= VAUS_FIRE;
      }
   }

   if ( EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_TurboJoypad )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_TurboJoypad_ATURBO) )
      {
         m_joy [ CONTROLLER2 ] |= JOY_ATURBO;
      }
      else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_TurboJoypad_BTURBO) )
      {
         m_joy [ CONTROLLER2 ] |= JOY_BTURBO;
      }
   }
   if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_StandardJoypad) ||
        (EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_TurboJoypad) )
   {
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
   }
   else if ( EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_Zapper )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_Zapper_FIRE) )
      {
         m_joy [ CONTROLLER2 ] |= ZAPPER_TRIGGER;
      }
   }
   else if ( EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_Vaus )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_Vaus_FIRE) )
      {
         m_joy [ CONTROLLER2 ] |= VAUS_FIRE;
      }
   }

   emit controllerInput ( m_joy );

   event->accept();
}

void NESEmulatorDockWidget::keyReleaseEvent(QKeyEvent* event)
{
   if ( EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_TurboJoypad )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_TurboJoypad_ATURBO) )
      {
         m_joy [ CONTROLLER1 ] &= (~JOY_ATURBO);
      }
      else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_TurboJoypad_BTURBO) )
      {
         m_joy [ CONTROLLER1 ] &= (~JOY_BTURBO);
      }
   }
   if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_StandardJoypad) ||
        (EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_TurboJoypad) )
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
   }
   else if ( EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_Zapper )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_Zapper_FIRE) )
      {
         m_joy [ CONTROLLER1 ] &= (~ZAPPER_TRIGGER);
      }
   }
   else if ( EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_Vaus )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER1,IO_Vaus_FIRE) )
      {
         m_joy [ CONTROLLER1 ] &= (~VAUS_FIRE);
      }
   }

   if ( EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_TurboJoypad )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_TurboJoypad_ATURBO) )
      {
         m_joy [ CONTROLLER2 ] &= (~JOY_ATURBO);
      }
      else if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_TurboJoypad_BTURBO) )
      {
         m_joy [ CONTROLLER2 ] &= (~JOY_BTURBO);
      }
   }
   if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_StandardJoypad) ||
        (EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_TurboJoypad) )
   {
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
   }
   else if ( EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_Zapper )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_Zapper_FIRE) )
      {
         m_joy [ CONTROLLER2 ] &= (~ZAPPER_TRIGGER);
      }
   }
   else if ( EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_Vaus )
   {
      if ( event->key() == EmulatorPrefsDialog::getControllerKeyMap(CONTROLLER2,IO_Vaus_FIRE) )
      {
         m_joy [ CONTROLLER2 ] &= (~VAUS_FIRE);
      }
   }

   emit controllerInput ( m_joy );

   event->accept();
}

void NESEmulatorDockWidget::renderData()
{
   renderer->update();
}
