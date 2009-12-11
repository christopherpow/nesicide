#ifndef CHRROMDISPLAYDIALOG_H
#define CHRROMDISPLAYDIALOG_H

#include <QDialog>
#include "cchrrompreviewrenderer.h"

namespace Ui {
    class CHRROMDisplayDialog;
}

class CHRROMDisplayDialog : public QDialog {
    Q_OBJECT
public:
    CHRROMDisplayDialog(QWidget *parent, qint8 *data);
    ~CHRROMDisplayDialog();
    CCHRROMPreviewRenderer *renderer;
    qint8 *chrrom;
    qint8 palette[4][3]; // 4 palettes, r, g, and b each...
    void updateScrollbars();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *event);

private:
    Ui::CHRROMDisplayDialog *ui;

private slots:
    void on_verticalScrollBar_valueChanged(int value);
    void on_horizontalScrollBar_valueChanged(int value);
    void on_horizontalScrollBar_actionTriggered(int action);
    void on_verticalScrollBar_actionTriggered(int action);
    void on_zoomSlider_valueChanged(int value);
    void on_zoomSlider_actionTriggered(int action);
    void on_zoomSlider_sliderMoved(int position);
};

#endif // CHRROMDISPLAYDIALOG_H
