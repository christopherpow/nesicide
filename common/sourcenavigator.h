#ifndef SOURCENAVIGATOR_H
#define SOURCENAVIGATOR_H

#include <QWidget>

namespace Ui {
    class SourceNavigator;
}

class SourceNavigator : public QWidget
{
   Q_OBJECT

public:
   explicit SourceNavigator(QWidget *parent = 0);
   virtual ~SourceNavigator();

   void shutdown();

private:
   Ui::SourceNavigator *ui;
   void updateSymbolsForFile(QString file);
   void updateFiles(bool doIt);
   QString m_loadedTarget;

signals:
   void snapTo(QString item);

private slots:
   void on_symbols_activated(QString );
   void on_files_activated(QString );

public slots:
   void compiler_compileDone(bool bOk);
   void emulator_emulatorPaused(bool show = true);
   void emulator_machineReady();
   void updateTargetMachine(QString target);
};

#endif // SOURCENAVIGATOR_H
