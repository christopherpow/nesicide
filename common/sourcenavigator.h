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
   ~SourceNavigator();

   void changeFile(QString file);

private:
   Ui::SourceNavigator *ui;
   void updateSymbolsForFile(int file);
    
signals:
   void fileNavigator_fileChanged(QString file);
   void fileNavigator_symbolChanged(QString file, QString symbol, int linenumber);

private slots:
   void on_symbols_currentIndexChanged(QString file);
   void on_files_currentIndexChanged(QString symbol);

public slots:
   void compiler_compileDone();
   void projectTreeView_openItem(QString item);
};

#endif // SOURCENAVIGATOR_H
