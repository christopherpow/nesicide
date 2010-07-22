#ifndef OAMDISPLAYDIALOG_H
#define OAMDISPLAYDIALOG_H

#include <QDialog>
#include "coampreviewrenderer.h"

namespace Ui {
    class OAMDisplayDialog;
}

class OAMDisplayDialog : public QDialog {
    Q_OBJECT
public:
    OAMDisplayDialog(QWidget *parent = 0);
    ~OAMDisplayDialog();
    void updateScrollbars();

protected:
    COAMPreviewRenderer *renderer;

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *event);

private:
    Ui::OAMDisplayDialog *ui;
    char *imgData;

public slots:
    void renderData();

private slots:
    void on_showVisible_toggled(bool checked);
    void on_updateScanline_editingFinished();
    void on_verticalScrollBar_valueChanged(int value);
    void on_horizontalScrollBar_valueChanged(int value);
    void on_zoomSlider_valueChanged(int value);
};

#endif // OAMDISPLAYDIALOG_H
