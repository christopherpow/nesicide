#ifndef CDESIGNEREDITORBASE_H
#define CDESIGNEREDITORBASE_H

#include <QMainWindow>
#include <QUndoStack>
#include <QToolBar>

#include "icenterwidgetitem.h"
#include "iprojecttreeviewitem.h"

class CDesignerEditorBase : public QMainWindow, public ICenterWidgetItem
{
   Q_OBJECT
public:
   explicit CDesignerEditorBase(IProjectTreeViewItem* link,QWidget *parent = 0);
   virtual ~CDesignerEditorBase() {};

   virtual bool isModified() { return m_isModified; }
   virtual void setModified(bool modified) { m_isModified = modified; emit editor_modified(modified); }

   // ICenterWidgetItem Interface Implmentation
   virtual QMessageBox::StandardButton onCloseQuery();
   virtual void onClose();
   virtual void onSave();
   virtual QMenu& editorMenu();

protected:
   void keyPressEvent(QKeyEvent *e);

public slots:
   virtual void snapTo(QString /*item*/) {}
   virtual void replaceText(QString /*from*/, QString /*to*/, bool /*replaceAll*/) {}
   virtual void applyChangesToTab(QString /*uuid*/) {}
   virtual void applyProjectPropertiesToTab() {}
   virtual void applyEnvironmentSettingsToTab() {}
   virtual void applyAppSettingsToTab() {}
   virtual void checkOpenFile(QDateTime /*lastActivationTime*/) {}
   virtual void itemAdded(QUuid /*uuid*/) {}
   virtual void itemRemoved(QUuid /*uuid*/) {}

signals:
   void editor_modified(bool m);
   void markProjectDirty(bool dirty);
   void breakpointsChanged();
   void activateSearchBar(QString item);
   void setSearchBarHint(bool found);
   void snapToTab(QString item);
   void addWatchedItem(QString item);
   void applyChanges(QString uuid);
   void addStatusBarWidget(QWidget* widget);
   void removeStatusBarWidget(QWidget* widget);
   void addToolBarWidget(QToolBar* toolBar);
   void removeToolBarWidget(QToolBar* toolBar);
   void setStatusBarMessage(QString message);

protected:
   QUndoStack m_undoStack;
   QMenu m_menu;
   bool m_isModified;
};

#endif // CDESIGNEREDITORBASE_H
