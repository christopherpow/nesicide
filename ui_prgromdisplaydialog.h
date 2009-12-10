/********************************************************************************
** Form generated from reading UI file 'prgromdisplaydialog.ui'
**
** Created: Thu Dec 10 14:23:21 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRGROMDISPLAYDIALOG_H
#define UI_PRGROMDISPLAYDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_PRGROMDisplayDialog
{
public:
    QGridLayout *gridLayout;
    QTextBrowser *textBrowser;

    void setupUi(QDialog *PRGROMDisplayDialog)
    {
        if (PRGROMDisplayDialog->objectName().isEmpty())
            PRGROMDisplayDialog->setObjectName(QString::fromUtf8("PRGROMDisplayDialog"));
        PRGROMDisplayDialog->resize(400, 300);
        gridLayout = new QGridLayout(PRGROMDisplayDialog);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        textBrowser = new QTextBrowser(PRGROMDisplayDialog);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier New"));
        textBrowser->setFont(font);
        textBrowser->setAcceptRichText(false);
        textBrowser->setTextInteractionFlags(Qt::NoTextInteraction);

        gridLayout->addWidget(textBrowser, 0, 0, 1, 1);


        retranslateUi(PRGROMDisplayDialog);

        QMetaObject::connectSlotsByName(PRGROMDisplayDialog);
    } // setupUi

    void retranslateUi(QDialog *PRGROMDisplayDialog)
    {
        PRGROMDisplayDialog->setWindowTitle(QApplication::translate("PRGROMDisplayDialog", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PRGROMDisplayDialog: public Ui_PRGROMDisplayDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRGROMDISPLAYDIALOG_H
