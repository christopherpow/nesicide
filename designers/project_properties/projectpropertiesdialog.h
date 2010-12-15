#ifndef PROJECTPROPERTIESDIALOG_H
#define PROJECTPROPERTIESDIALOG_H

#include <QDialog>
#include <QtXml>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include "cpaletteitemdelegate.h"
#include "cnesicideproject.h"
#include "csourceitem.h"

namespace Ui {
    class ProjectPropertiesDialog;
}

class ProjectPropertiesDialog : public QDialog {
    Q_OBJECT
public:
    ProjectPropertiesDialog(QWidget *parent);
    ~ProjectPropertiesDialog();
    QString getProjectName();
    void setMainSource(QString mainSource);
    QString getMainSource();
    QList<QColor> currentPalette;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ProjectPropertiesDialog *ui;
    void updateUI(int colid = -1);
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
    void on_blueHorizontalSlider_actionTriggered(int action);
    void on_greenHorizontalSlider_actionTriggered(int action);
    void on_redHorizontalSlider_actionTriggered(int action);
    void on_ImportPalettePushButton_clicked();
    void on_exportPalettePushButton_clicked();
    void on_resetPalettePushButton_clicked();
    void on_tableWidget_cellClicked(int row, int column);
};

#endif // PROJECTPROPERTIESDIALOG_H
