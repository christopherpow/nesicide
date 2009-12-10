#ifndef PALETTEEDITORWINDOW_H
#define PALETTEEDITORWINDOW_H

#include <QWidget>

namespace Ui {
    class PaletteEditorWindow;
}

class PaletteEditorWindow : public QWidget {
    Q_OBJECT
public:
    PaletteEditorWindow(QWidget *parent = 0);
    ~PaletteEditorWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PaletteEditorWindow *ui;

private slots:
    void on_webView_titleChanged(QString title);
};

#endif // PALETTEEDITORWINDOW_H
