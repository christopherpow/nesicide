#ifndef PROJECTPROPERTIESDIALOG_H
#define PROJECTPROPERTIESDIALOG_H

#include <QDialog>
#include <QtXml>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include "cpaletteitemdelegate.h"
#include "defaultnespalette.h"

namespace Ui {
    class ProjectPropertiesDialog;
}

class ProjectPropertiesDialog : public QDialog {
    Q_OBJECT
public:
    ProjectPropertiesDialog(QWidget *parent, QList<QColor> pal);
    ~ProjectPropertiesDialog();
    void setProjectName(QString newName);
    QString getProjectName();
    QList<QColor> currentPalette;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ProjectPropertiesDialog *ui;
    void updateUI();
    QDomElement addElement( QDomDocument &doc, QDomNode &node,
                            const QString &tag,
                            const QString &value = QString::null )
    {
        QDomElement el = doc.createElement( tag );
        node.appendChild( el );
        if ( !value.isNull() ) {
            QDomText txt = doc.createTextNode( value );
            el.appendChild( txt );
        }
        return el;
    }

private slots:
    void on_blueDial_sliderMoved(int position);
    void on_greenDial_sliderMoved(int position);
    void on_redDial_sliderMoved(int position);
    void on_ImportPalettePushButton_clicked();
    void on_exportPalettePushButton_clicked();
    void on_resetPalettePushButton_clicked();
    void on_tableWidget_cellActivated(int row, int column);
    void on_redDial_valueChanged(int value);
    void on_blueDial_valueChanged(int value);
    void on_greenDial_valueChanged(int value);
    void on_tableWidget_cellClicked(int row, int column);
};

#endif // PROJECTPROPERTIESDIALOG_H
