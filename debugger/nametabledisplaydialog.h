#ifndef NAMETABLEDISPLAYDIALOG_H
#define NAMETABLEDISPLAYDIALOG_H

#include <QDialog>
#include "cnametablepreviewrenderer.h"
#include "cnesppu.h"

namespace Ui {
    class NameTableDisplayDialog;
}

class NameTableDisplayDialog : public QDialog {
    Q_OBJECT
public:
    NameTableDisplayDialog(QWidget *parent = 0);
    ~NameTableDisplayDialog();
    void updateScrollbars();
protected:
    CNameTablePreviewRenderer *renderer;

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *event);

private:
    Ui::NameTableDisplayDialog *ui;
    char *imgData;

public slots:
    void renderData();

private slots:
    void on_verticalScrollBar_valueChanged(int value);
    void on_horizontalScrollBar_valueChanged(int value);
    void on_horizontalScrollBar_actionTriggered(int action);
    void on_verticalScrollBar_actionTriggered(int action);
    void on_zoomSlider_valueChanged(int value);
    void on_zoomSlider_actionTriggered(int action);
    void on_zoomSlider_sliderMoved(int position);
};

#endif // NAMETABLEDISPLAYDIALOG_H
