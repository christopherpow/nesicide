/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Dec 10 12:16:25 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtWebKit/QWebView>
#include <cprojecttreeview.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionE_xit;
    QAction *actionNew_Project;
    QAction *actionOpen_Project;
    QAction *actionSave_Project;
    QAction *actionSave_Project_As;
    QAction *action_Project_Browser;
    QAction *actionNew_Palette;
    QAction *actionProject_Properties;
    QAction *actionCreate_Project_from_ROM;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    QWebView *webView;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_View;
    QMenu *menu_Windows;
    QStatusBar *statusBar;
    QToolBar *projectToolBar;
    QDockWidget *projectBrowserDockWidget;
    QWidget *dockWidgetContents_3;
    QGridLayout *gridLayout;
    CProjectTreeView *projectTreeWidget;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(762, 577);
        actionE_xit = new QAction(MainWindow);
        actionE_xit->setObjectName(QString::fromUtf8("actionE_xit"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/stock_exit.svg"), QSize(), QIcon::Normal, QIcon::Off);
        actionE_xit->setIcon(icon);
        actionE_xit->setMenuRole(QAction::QuitRole);
        actionNew_Project = new QAction(MainWindow);
        actionNew_Project->setObjectName(QString::fromUtf8("actionNew_Project"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/resources/window_new.svg"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew_Project->setIcon(icon1);
        actionOpen_Project = new QAction(MainWindow);
        actionOpen_Project->setObjectName(QString::fromUtf8("actionOpen_Project"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/resources/document-open.svg"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen_Project->setIcon(icon2);
        actionSave_Project = new QAction(MainWindow);
        actionSave_Project->setObjectName(QString::fromUtf8("actionSave_Project"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/resources/stock_save.svg"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_Project->setIcon(icon3);
        actionSave_Project_As = new QAction(MainWindow);
        actionSave_Project_As->setObjectName(QString::fromUtf8("actionSave_Project_As"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/resources/stock_save-as.svg"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_Project_As->setIcon(icon4);
        action_Project_Browser = new QAction(MainWindow);
        action_Project_Browser->setObjectName(QString::fromUtf8("action_Project_Browser"));
        action_Project_Browser->setCheckable(true);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/resources/folder-system.svg"), QSize(), QIcon::Normal, QIcon::Off);
        action_Project_Browser->setIcon(icon5);
        actionNew_Palette = new QAction(MainWindow);
        actionNew_Palette->setObjectName(QString::fromUtf8("actionNew_Palette"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/resources/style.svg"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew_Palette->setIcon(icon6);
        actionProject_Properties = new QAction(MainWindow);
        actionProject_Properties->setObjectName(QString::fromUtf8("actionProject_Properties"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/resources/document-properties.svg"), QSize(), QIcon::Normal, QIcon::Off);
        actionProject_Properties->setIcon(icon7);
        actionCreate_Project_from_ROM = new QAction(MainWindow);
        actionCreate_Project_from_ROM->setObjectName(QString::fromUtf8("actionCreate_Project_from_ROM"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/resources/drive-harddisk.svg"), QSize(), QIcon::Normal, QIcon::Off);
        actionCreate_Project_from_ROM->setIcon(icon8);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setDocumentMode(false);
        tabWidget->setTabsClosable(true);
        tabWidget->setMovable(true);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_2 = new QGridLayout(tab);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        webView = new QWebView(tab);
        webView->setObjectName(QString::fromUtf8("webView"));
        webView->setUrl(QUrl("about:blank"));

        gridLayout_2->addWidget(webView, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 762, 21));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_View = new QMenu(menuBar);
        menu_View->setObjectName(QString::fromUtf8("menu_View"));
        menu_Windows = new QMenu(menu_View);
        menu_Windows->setObjectName(QString::fromUtf8("menu_Windows"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        projectToolBar = new QToolBar(MainWindow);
        projectToolBar->setObjectName(QString::fromUtf8("projectToolBar"));
        projectToolBar->setIconSize(QSize(24, 24));
        projectToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        projectToolBar->setFloatable(true);
        MainWindow->addToolBar(Qt::TopToolBarArea, projectToolBar);
        projectBrowserDockWidget = new QDockWidget(MainWindow);
        projectBrowserDockWidget->setObjectName(QString::fromUtf8("projectBrowserDockWidget"));
        projectBrowserDockWidget->setFloating(false);
        projectBrowserDockWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
        dockWidgetContents_3 = new QWidget();
        dockWidgetContents_3->setObjectName(QString::fromUtf8("dockWidgetContents_3"));
        gridLayout = new QGridLayout(dockWidgetContents_3);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        projectTreeWidget = new CProjectTreeView(dockWidgetContents_3);
        projectTreeWidget->setObjectName(QString::fromUtf8("projectTreeWidget"));
        projectTreeWidget->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(projectTreeWidget->sizePolicy().hasHeightForWidth());
        projectTreeWidget->setSizePolicy(sizePolicy);
        projectTreeWidget->setAutoFillBackground(false);
        projectTreeWidget->setAlternatingRowColors(false);
        projectTreeWidget->setUniformRowHeights(false);
        projectTreeWidget->setAnimated(true);
        projectTreeWidget->setHeaderHidden(true);
        projectTreeWidget->header()->setVisible(false);

        gridLayout->addWidget(projectTreeWidget, 0, 0, 1, 1);

        projectBrowserDockWidget->setWidget(dockWidgetContents_3);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), projectBrowserDockWidget);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_View->menuAction());
        menu_File->addAction(actionNew_Project);
        menu_File->addAction(actionCreate_Project_from_ROM);
        menu_File->addSeparator();
        menu_File->addAction(actionOpen_Project);
        menu_File->addSeparator();
        menu_File->addAction(actionProject_Properties);
        menu_File->addSeparator();
        menu_File->addAction(actionE_xit);
        menu_View->addAction(menu_Windows->menuAction());
        menu_Windows->addAction(action_Project_Browser);
        projectToolBar->addAction(actionNew_Project);
        projectToolBar->addSeparator();
        projectToolBar->addAction(actionOpen_Project);
        projectToolBar->addAction(actionSave_Project);
        projectToolBar->addAction(actionSave_Project_As);
        projectToolBar->addSeparator();
        projectToolBar->addAction(actionProject_Properties);

        retranslateUi(MainWindow);
        QObject::connect(actionE_xit, SIGNAL(activated()), MainWindow, SLOT(close()));
        QObject::connect(projectBrowserDockWidget, SIGNAL(visibilityChanged(bool)), action_Project_Browser, SLOT(setChecked(bool)));
        QObject::connect(action_Project_Browser, SIGNAL(toggled(bool)), projectBrowserDockWidget, SLOT(setVisible(bool)));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "NESICIDE2", 0, QApplication::UnicodeUTF8));
        actionE_xit->setText(QApplication::translate("MainWindow", "E&xit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionE_xit->setStatusTip(QApplication::translate("MainWindow", "Exit the application.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionNew_Project->setText(QApplication::translate("MainWindow", "New &Project...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionNew_Project->setToolTip(QApplication::translate("MainWindow", "New Project", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionNew_Project->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+N", 0, QApplication::UnicodeUTF8));
        actionOpen_Project->setText(QApplication::translate("MainWindow", "Open &Project...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionOpen_Project->setToolTip(QApplication::translate("MainWindow", "Open Project...", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionOpen_Project->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+O", 0, QApplication::UnicodeUTF8));
        actionSave_Project->setText(QApplication::translate("MainWindow", "Save &Project", 0, QApplication::UnicodeUTF8));
        actionSave_Project->setShortcut(QApplication::translate("MainWindow", "Ctrl+A", 0, QApplication::UnicodeUTF8));
        actionSave_Project_As->setText(QApplication::translate("MainWindow", "Save Project &As...", 0, QApplication::UnicodeUTF8));
        action_Project_Browser->setText(QApplication::translate("MainWindow", "&Project Browser", 0, QApplication::UnicodeUTF8));
        actionNew_Palette->setText(QApplication::translate("MainWindow", "New Palette", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionNew_Palette->setToolTip(QApplication::translate("MainWindow", "Create a new Palette", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionProject_Properties->setText(QApplication::translate("MainWindow", "Project &Properties...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionProject_Properties->setToolTip(QApplication::translate("MainWindow", "Project properties", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionProject_Properties->setShortcut(QApplication::translate("MainWindow", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        actionCreate_Project_from_ROM->setText(QApplication::translate("MainWindow", "Create Project from &ROM...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCreate_Project_from_ROM->setToolTip(QApplication::translate("MainWindow", "Create project from a ROM", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Welcome Page", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menu_View->setTitle(QApplication::translate("MainWindow", "&View", 0, QApplication::UnicodeUTF8));
        menu_Windows->setTitle(QApplication::translate("MainWindow", "&Windows", 0, QApplication::UnicodeUTF8));
        projectToolBar->setWindowTitle(QApplication::translate("MainWindow", "Project File Operations", 0, QApplication::UnicodeUTF8));
        projectBrowserDockWidget->setWindowTitle(QApplication::translate("MainWindow", "Project Browser", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
