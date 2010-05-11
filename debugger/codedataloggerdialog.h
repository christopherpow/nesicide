#ifndef CODEDATALOGGERDIALOG_H
#define CODEDATALOGGERDIALOG_H

#include <QDialog>
#include "ccodedataloggerrenderer.h"

namespace Ui {
    class CodeDataLoggerDialog;
}

class CodeDataLoggerDialog : public QDialog {
    Q_OBJECT
public:
    CodeDataLoggerDialog(QWidget *parent = 0);
    ~CodeDataLoggerDialog();
    void updateScrollbars();

protected:
    CCodeDataLoggerRenderer *renderer;

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *event);

private:
    Ui::CodeDataLoggerDialog *ui;
    char *cpuImgData;
    char *ppuImgData;

public slots:
    void renderData();

private slots:
    void on_displaySelect_currentIndexChanged(int index);
    void on_verticalScrollBar_valueChanged(int value);
    void on_horizontalScrollBar_valueChanged(int value);
    void on_zoomSlider_valueChanged(int value);
};

#endif // CODEDATALOGGERDIALOG_H
