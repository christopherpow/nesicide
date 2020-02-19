#ifndef CPROJECTTABWIDGET_H
#define CPROJECTTABWIDGET_H

#include <QTabWidget>
#include <QDateTime>
#include <QToolBar>
#include <QDockWidget>

// The IDE uses a system of signals named "snapTo" and "snapToTab" to
// communicate information between various debuggers, IDE elements, or
// designer forms when things change in the IDE environment that the
// open windows should react to.  The single parameter of these signals
// is a QString representing the information to be broadcast.
// For example, when the user types into the search bar on the toolbar,
// the search bar widget emits:
//
// snapTo("SearchBar,<current-text>")
//
// Now any IDE element that wants to react to real-time search bar updates
// can do so simply by getting the search bar widget from the widget registry
// (see cdockwidgetregistry.h), and connecting its snapTo(QString) signal
// to an appropriate slot with a QString parameter.  Then the slot handler
// can process the string it receives and easily parse out the information
// it needs.  In the above example, a simple split(QRegExp("[,]") operation
// on the QString parameter will produce the two pieces of information the
// receiving slot needs.  First, it'll get the type of the information, namely
// "SearchBar".  Second, it'll get the information, namely the current text
// present in the search bar window.  From there it's up to the IDE element
// how it responds to the signal.
//
// snapToTab is an extension of snapTo that is used by the project tab widget
// (this file) because I couldn't get it to let me use a method in a class as
// both a signal *and* a slot.  Most often the signal emitted by an IDE element
// should be snapTo(QString).
//
// As the snapTo usage is expanded I'll try to remember to document its usage
// and formats here.
//
// snapTo formats:  emit snapTo(<string>) where <string is one of:
// OutputPaneFile,<file>,<line>
//    Sent from the output pane in response to the user clicking on a line of
//    a search result or an error in a compile log.  This tells the other IDE
//    elements to show <file> and scroll so <line> is in view.
// SourceNavigatorFile,<file>,[<line>]
//    Sent from various places, one being the source navigator toolbar widget,
//    indicating the user has done some action (selected a file, double-clicked
//    on a table item containing a file reference, etc.) requiring the display
//    of <file>.  If <line> is present it indicates to the receiving IDE elements
//    that they should indicate execution of the emulator is currently at <line>.
// SourceNavigatorSymbol,<symbol>
//    Sent from various places, one being the source navigator toolbar widget,
//    indicating the user has done some action (selected a symbol, double-clicked
//    on a table item containing a symbol reference, etc.) requiring the display
//    of <symbol>.  Most often this is paired with emitting SourceNavigatorFile
//    in order to ensure the file is made visible so the symbol will also be visible.
// Address,<address>
//    Sent from widgets that do not have symbolic debug reference capability, such
//    as the execution visualizer.  Indicates to the other IDE elements that they should
//    try to display whatever they can about the particular <address>.
// SearchBar,<case-sensitive>,<regular-expression>,<search-direction>,<search-text>
//    Sent from the search bar widget and also from the search bar widget embedded
//    in the search dock widget.  Indicates to the receiving IDE elements that the
//    user is searching for <search-text> and that they should do whatever they can
//    to help.  If <case-sensitive> is 1, the search should not ignore case. If
//    <regular-expression> is 1, the search string should be interpreted as a
//    QRegExp.  If <search-direction> is 1, the search should go from the first item
//    to the last item.

class CProjectTabWidget : public QTabWidget
{
   Q_OBJECT
public:
   explicit CProjectTabWidget(QWidget *parent = 0);

   int addTab(QWidget *page, const QString &label);
   int addTab(QWidget *page, const QIcon &icon, const QString &label);
   void removeTab(int index);

protected:
   void tabBar_contextMenuEvent(QContextMenuEvent *event);
   void tabBar_mouseMoveEvent(QMouseEvent* event);
   bool eventFilter(QObject *object, QEvent *event);

signals:
   void tabModified(int tab,bool modified);
   void tabAdded(int tab);
   void tabAboutToBeRemoved(int tabIndex);
   void tabRemoved(int index);
   void snapTo(QString item);
   void applyChangesToTab(QString uuid);
   void markProjectDirty(bool dirty);
   void applyProjectPropertiesToTab();
   void applyEnvironmentSettingsToTab();
   void applyAppSettingsToTab();
   void addStatusBarWidget(QWidget* widget);
   void removeStatusBarWidget(QWidget* widget);
   void addToolBarWidget(QToolBar* toolBar);
   void removeToolBarWidget(QToolBar* toolBar);
   void setStatusBarMessage(QString message);
   void updateTargetMachine(QString target);
   void checkOpenFile(QDateTime lastActivationTime);

public slots:
   void unDockTab();
   void reDockTab();
   void checkOpenFiles(QDateTime lastActivationTime);
   void tabModified(bool modified);
   void projectDirtied(bool dirtied);
   void snapToTab(QString item);
   void applyChanges(QString uuid);
   void applyProjectProperties();
   void applyEnvironmentSettings();
   void applyAppSettings();

protected:
   QMap<QString,QWidget*> tearOffs;
};

#endif // CPROJECTTABWIDGET_H
