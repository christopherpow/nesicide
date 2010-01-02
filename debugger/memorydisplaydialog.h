#ifndef MEMORYDISPLAYDIALOG_H
#define MEMORYDISPLAYDIALOG_H

#include <QDialog>
#include "cdebuggermemorydisplaymodel.h"

#include "cnesicidecommon.h"

namespace Ui {
    class MemoryDisplayDialog;
}

class MemoryDisplayDialog : public QDialog {
    Q_OBJECT
public:
    MemoryDisplayDialog(QWidget *parent = 0, eMemoryType display = eMemory_CPU);
    ~MemoryDisplayDialog();

protected:
    void changeEvent(QEvent *e);

public slots:
    void updateMemory();

private:
    Ui::MemoryDisplayDialog *ui;
    CDebuggerMemoryDisplayModel *model;
};

#endif // MEMORYDISPLAYDIALOG_H
