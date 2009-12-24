#ifndef GRAPHICSBANKEDITORFORM_H
#define GRAPHICSBANKEDITORFORM_H

#include <QWidget>

namespace Ui {
    class GraphicsBankEditorForm;
}

class GraphicsBankEditorForm : public QWidget {
    Q_OBJECT
public:
    GraphicsBankEditorForm(QWidget *parent = 0);
    ~GraphicsBankEditorForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GraphicsBankEditorForm *ui;
};

#endif // GRAPHICSBANKEDITORFORM_H
