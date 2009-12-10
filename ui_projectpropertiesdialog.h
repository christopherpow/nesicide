/********************************************************************************
** Form generated from reading UI file 'projectpropertiesdialog.ui'
**
** Created: Thu Dec 10 11:55:52 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROJECTPROPERTIESDIALOG_H
#define UI_PROJECTPROPERTIESDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDial>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProjectPropertiesDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    QLabel *label_4;
    QLineEdit *projectNameLineEdit;
    QSpacerItem *verticalSpacer;
    QWidget *tab_2;
    QGridLayout *gridLayout;
    QTableWidget *tableWidget;
    QCheckBox *checkBox_2;
    QFrame *line;
    QLabel *colorValueLabel;
    QPushButton *exportPalettePushButton;
    QPushButton *ImportPalettePushButton;
    QDial *blueDial;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label;
    QDial *redDial;
    QDial *greenDial;
    QCheckBox *checkBox_3;
    QCheckBox *checkBox_4;
    QCheckBox *checkBox;
    QPushButton *resetPalettePushButton;
    QLabel *label_5;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ProjectPropertiesDialog)
    {
        if (ProjectPropertiesDialog->objectName().isEmpty())
            ProjectPropertiesDialog->setObjectName(QString::fromUtf8("ProjectPropertiesDialog"));
        ProjectPropertiesDialog->setWindowModality(Qt::WindowModal);
        ProjectPropertiesDialog->resize(491, 447);
        ProjectPropertiesDialog->setModal(true);
        verticalLayout = new QVBoxLayout(ProjectPropertiesDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(ProjectPropertiesDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_2 = new QGridLayout(tab);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_4 = new QLabel(tab);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 0, 0, 1, 1);

        projectNameLineEdit = new QLineEdit(tab);
        projectNameLineEdit->setObjectName(QString::fromUtf8("projectNameLineEdit"));

        gridLayout_2->addWidget(projectNameLineEdit, 0, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 1, 0, 1, 2);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout = new QGridLayout(tab_2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tableWidget = new QTableWidget(tab_2);
        if (tableWidget->columnCount() < 16)
            tableWidget->setColumnCount(16);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(10, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(11, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(12, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(13, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(14, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(15, __qtablewidgetitem15);
        if (tableWidget->rowCount() < 4)
            tableWidget->setRowCount(4);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(0, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(1, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(2, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(3, __qtablewidgetitem19);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tableWidget->sizePolicy().hasHeightForWidth());
        tableWidget->setSizePolicy(sizePolicy);
        tableWidget->setProperty("showDropIndicator", QVariant(false));
        tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidget->setShowGrid(true);
        tableWidget->setGridStyle(Qt::SolidLine);
        tableWidget->setWordWrap(false);
        tableWidget->setCornerButtonEnabled(false);
        tableWidget->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        tableWidget->verticalHeader()->setMinimumSectionSize(15);

        gridLayout->addWidget(tableWidget, 1, 0, 1, 3);

        checkBox_2 = new QCheckBox(tab_2);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        gridLayout->addWidget(checkBox_2, 8, 1, 1, 1);

        line = new QFrame(tab_2);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 7, 0, 1, 3);

        colorValueLabel = new QLabel(tab_2);
        colorValueLabel->setObjectName(QString::fromUtf8("colorValueLabel"));
        colorValueLabel->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(colorValueLabel, 5, 1, 1, 1);

        exportPalettePushButton = new QPushButton(tab_2);
        exportPalettePushButton->setObjectName(QString::fromUtf8("exportPalettePushButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/document-export.svg"), QSize(), QIcon::Normal, QIcon::Off);
        exportPalettePushButton->setIcon(icon);

        gridLayout->addWidget(exportPalettePushButton, 0, 2, 1, 1);

        ImportPalettePushButton = new QPushButton(tab_2);
        ImportPalettePushButton->setObjectName(QString::fromUtf8("ImportPalettePushButton"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/resources/document-import.svg"), QSize(), QIcon::Normal, QIcon::Off);
        ImportPalettePushButton->setIcon(icon1);

        gridLayout->addWidget(ImportPalettePushButton, 0, 1, 1, 1);

        blueDial = new QDial(tab_2);
        blueDial->setObjectName(QString::fromUtf8("blueDial"));
        blueDial->setMaximum(255);
        blueDial->setOrientation(Qt::Horizontal);
        blueDial->setInvertedAppearance(false);
        blueDial->setInvertedControls(false);
        blueDial->setWrapping(false);
        blueDial->setNotchTarget(8);
        blueDial->setNotchesVisible(true);

        gridLayout->addWidget(blueDial, 3, 2, 1, 1);

        label_3 = new QLabel(tab_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_3, 2, 2, 1, 1);

        label_2 = new QLabel(tab_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 2, 1, 1, 1);

        label = new QLabel(tab_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 2, 0, 1, 1);

        redDial = new QDial(tab_2);
        redDial->setObjectName(QString::fromUtf8("redDial"));
        redDial->setMaximum(255);
        redDial->setWrapping(false);
        redDial->setNotchTarget(8);
        redDial->setNotchesVisible(true);

        gridLayout->addWidget(redDial, 3, 0, 1, 1);

        greenDial = new QDial(tab_2);
        greenDial->setObjectName(QString::fromUtf8("greenDial"));
        greenDial->setMaximum(255);
        greenDial->setWrapping(false);
        greenDial->setNotchTarget(8);
        greenDial->setNotchesVisible(true);

        gridLayout->addWidget(greenDial, 3, 1, 1, 1);

        checkBox_3 = new QCheckBox(tab_2);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));

        gridLayout->addWidget(checkBox_3, 8, 2, 1, 1);

        checkBox_4 = new QCheckBox(tab_2);
        checkBox_4->setObjectName(QString::fromUtf8("checkBox_4"));

        gridLayout->addWidget(checkBox_4, 8, 0, 1, 1);

        checkBox = new QCheckBox(tab_2);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout->addWidget(checkBox, 10, 0, 1, 1);

        resetPalettePushButton = new QPushButton(tab_2);
        resetPalettePushButton->setObjectName(QString::fromUtf8("resetPalettePushButton"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/resources/style.svg"), QSize(), QIcon::Normal, QIcon::Off);
        resetPalettePushButton->setIcon(icon2);

        gridLayout->addWidget(resetPalettePushButton, 0, 0, 1, 1);

        label_5 = new QLabel(tab_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        QFont font;
        font.setItalic(true);
        label_5->setFont(font);
        label_5->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_5, 11, 0, 1, 3);

        tabWidget->addTab(tab_2, QString());

        verticalLayout->addWidget(tabWidget);

        buttonBox = new QDialogButtonBox(ProjectPropertiesDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(ProjectPropertiesDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ProjectPropertiesDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ProjectPropertiesDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ProjectPropertiesDialog);
    } // setupUi

    void retranslateUi(QDialog *ProjectPropertiesDialog)
    {
        ProjectPropertiesDialog->setWindowTitle(QApplication::translate("ProjectPropertiesDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ProjectPropertiesDialog", "Project Name:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("ProjectPropertiesDialog", "Project", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("ProjectPropertiesDialog", "x0", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("ProjectPropertiesDialog", "x1", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("ProjectPropertiesDialog", "x2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("ProjectPropertiesDialog", "x3", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("ProjectPropertiesDialog", "x4", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("ProjectPropertiesDialog", "x5", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = tableWidget->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QApplication::translate("ProjectPropertiesDialog", "x6", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = tableWidget->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QApplication::translate("ProjectPropertiesDialog", "x7", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = tableWidget->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QApplication::translate("ProjectPropertiesDialog", "x8", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = tableWidget->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QApplication::translate("ProjectPropertiesDialog", "x9", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem10 = tableWidget->horizontalHeaderItem(10);
        ___qtablewidgetitem10->setText(QApplication::translate("ProjectPropertiesDialog", "xA", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem11 = tableWidget->horizontalHeaderItem(11);
        ___qtablewidgetitem11->setText(QApplication::translate("ProjectPropertiesDialog", "xB", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem12 = tableWidget->horizontalHeaderItem(12);
        ___qtablewidgetitem12->setText(QApplication::translate("ProjectPropertiesDialog", "xC", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem13 = tableWidget->horizontalHeaderItem(13);
        ___qtablewidgetitem13->setText(QApplication::translate("ProjectPropertiesDialog", "xD", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem14 = tableWidget->horizontalHeaderItem(14);
        ___qtablewidgetitem14->setText(QApplication::translate("ProjectPropertiesDialog", "xE", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem15 = tableWidget->horizontalHeaderItem(15);
        ___qtablewidgetitem15->setText(QApplication::translate("ProjectPropertiesDialog", "xF", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem16 = tableWidget->verticalHeaderItem(0);
        ___qtablewidgetitem16->setText(QApplication::translate("ProjectPropertiesDialog", "0x", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem17 = tableWidget->verticalHeaderItem(1);
        ___qtablewidgetitem17->setText(QApplication::translate("ProjectPropertiesDialog", "1x", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem18 = tableWidget->verticalHeaderItem(2);
        ___qtablewidgetitem18->setText(QApplication::translate("ProjectPropertiesDialog", "2x", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem19 = tableWidget->verticalHeaderItem(3);
        ___qtablewidgetitem19->setText(QApplication::translate("ProjectPropertiesDialog", "3x", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("ProjectPropertiesDialog", "Emphasize Red", 0, QApplication::UnicodeUTF8));
        colorValueLabel->setText(QApplication::translate("ProjectPropertiesDialog", "#000000", 0, QApplication::UnicodeUTF8));
        exportPalettePushButton->setText(QApplication::translate("ProjectPropertiesDialog", "Export Palette...", 0, QApplication::UnicodeUTF8));
        ImportPalettePushButton->setText(QApplication::translate("ProjectPropertiesDialog", "Import Palette...", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ProjectPropertiesDialog", "Blue", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ProjectPropertiesDialog", "Green", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ProjectPropertiesDialog", "Red", 0, QApplication::UnicodeUTF8));
        checkBox_3->setText(QApplication::translate("ProjectPropertiesDialog", "Emphasize Blue", 0, QApplication::UnicodeUTF8));
        checkBox_4->setText(QApplication::translate("ProjectPropertiesDialog", "Emphasize Green", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("ProjectPropertiesDialog", "Monochrome", 0, QApplication::UnicodeUTF8));
        resetPalettePushButton->setText(QApplication::translate("ProjectPropertiesDialog", "Reset Palette", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ProjectPropertiesDialog", "This palette is for internal emulation purposes only.", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("ProjectPropertiesDialog", "System Palette", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ProjectPropertiesDialog: public Ui_ProjectPropertiesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROJECTPROPERTIESDIALOG_H
