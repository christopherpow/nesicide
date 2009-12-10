/********************************************************************************
** Form generated from reading UI file 'paletteeditorwindow.ui'
**
** Created: Wed Dec 9 09:51:44 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PALETTEEDITORWINDOW_H
#define UI_PALETTEEDITORWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include <QtWebKit/QWebView>

QT_BEGIN_NAMESPACE

class Ui_PaletteEditorWindow
{
public:
    QGridLayout *gridLayout;
    QWebView *webView;

    void setupUi(QWidget *PaletteEditorWindow)
    {
        if (PaletteEditorWindow->objectName().isEmpty())
            PaletteEditorWindow->setObjectName(QString::fromUtf8("PaletteEditorWindow"));
        PaletteEditorWindow->resize(449, 338);
        gridLayout = new QGridLayout(PaletteEditorWindow);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        webView = new QWebView(PaletteEditorWindow);
        webView->setObjectName(QString::fromUtf8("webView"));
        webView->setAutoFillBackground(false);
        webView->setUrl(QUrl("about:blank"));

        gridLayout->addWidget(webView, 0, 0, 1, 1);


        retranslateUi(PaletteEditorWindow);

        QMetaObject::connectSlotsByName(PaletteEditorWindow);
    } // setupUi

    void retranslateUi(QWidget *PaletteEditorWindow)
    {
        PaletteEditorWindow->setWindowTitle(QString());
    } // retranslateUi

};

namespace Ui {
    class PaletteEditorWindow: public Ui_PaletteEditorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PALETTEEDITORWINDOW_H
