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
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *event);

private:
    Ui::OAMDisplayDialog *ui;
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
    void colorChanged (const QColor &color);
};

#endif // OAMDISPLAYDIALOG_H
