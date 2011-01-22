#ifndef NESEMULATORDOCKWIDGET_H
#define NESEMULATORDOCKWIDGET_H

#include "emulator_core.h"
#include "nesemulatorrenderer.h"

#include <QDockWidget>
#include <QKeyEvent>

namespace Ui {
   class NESEmulatorDockWidget;
}

class NESEmulatorDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit NESEmulatorDockWidget(QWidget *parent = 0);
   ~NESEmulatorDockWidget();

protected:
   void changeEvent(QEvent* e);
   void mousePressEvent(QMouseEvent* event);
   void mouseReleaseEvent(QMouseEvent* event);
   void mouseMoveEvent(QMouseEvent* event);
   void keyPressEvent(QKeyEvent* event);
   void keyReleaseEvent(QKeyEvent* event);

private:
   Ui::NESEmulatorDockWidget *ui;
   CNESEmulatorRenderer* renderer;
   char* imgData;
   unsigned char m_joy [ NUM_CONTROLLERS ];

private slots:
   void on_stepCPUButton_clicked();
   void on_stepPPUButton_clicked();
   void on_pauseButton_clicked();
   void on_playButton_clicked();
   void on_resetButton_clicked();
   void internalPause(bool);
   void internalPauseWithoutShow();
   void internalPlay();
   void renderData();
};

#endif // NESEMULATORDOCKWIDGET_H
