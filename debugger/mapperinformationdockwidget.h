#ifndef MAPPERINFORMATIONDOCKWIDGET_H
#define MAPPERINFORMATIONDOCKWIDGET_H

#include "cdebuggerbase.h"

namespace Ui {
    class MapperInformationDockWidget;
}

class MapperInformationDockWidget : public CDebuggerBase
{
    Q_OBJECT

public:
    explicit MapperInformationDockWidget(QWidget *parent = 0);
    ~MapperInformationDockWidget();

protected:
    void showEvent(QShowEvent* e);
    void hideEvent(QHideEvent* e);
   void changeEvent(QEvent* e);

public slots:
   void updateInformation();
   void cartridgeLoaded();

private:
    Ui::MapperInformationDockWidget *ui;
};

#endif // MAPPERINFORMATIONDOCKWIDGET_H
