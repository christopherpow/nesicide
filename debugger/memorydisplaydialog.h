#ifndef MEMORYDISPLAYDIALOG_H
#define MEMORYDISPLAYDIALOG_H

#include <QDialog>
#include "cdebuggermemorydisplaymodel.h"

#include "cnesicidecommon.h"
#include "cregisterdata.h"

namespace Ui {
    class MemoryDisplayDialog;
}

class MemoryDisplayDialog : public QDialog {
    Q_OBJECT
public:
    MemoryDisplayDialog(QWidget *parent = 0, eMemoryType display = eMemory_CPU);
    ~MemoryDisplayDialog();

protected:
    void showEvent(QShowEvent *e);
    void contextMenuEvent(QContextMenuEvent *e);
    void changeEvent(QEvent *e);

public slots:
    void cartridgeLoaded();
    void updateMemory();

signals:
    void showMe(eMemoryType display);

private:
    Ui::MemoryDisplayDialog *ui;
    CDebuggerMemoryDisplayModel *model;
    eMemoryType m_display;
};

#endif // MEMORYDISPLAYDIALOG_H
