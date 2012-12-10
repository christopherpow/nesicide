#ifndef CPROJECTTREEWIDGET_H
#define CPROJECTTREEWIDGET_H

#include <QTreeWidget>
#include <QUuid>

class CProjectModel;

class CProjectTreeWidget : public QTreeWidget
{
   Q_OBJECT
public:
   explicit CProjectTreeWidget(QWidget *parent = 0);

   QUuid getUuidAt(const QPoint& pos);
   QUuid getUuidOf(const QTreeWidgetItem* item);

   bool containsUuid(const QUuid& uuid);
   void setCurrentUuid(const QUuid& uuid);

   void addItem(CProjectModel *project, const QUuid &uuid, const QUuid &parent);
   void setParent(const QUuid &child, const QUuid &parent);
   void removeItem(const QUuid &uuid);

private:   
   QTreeWidgetItem * buildNodeFromUuid(CProjectModel *project, const QUuid &uuid);
   QTreeWidgetItem * findTreeWidgetItem(const QUuid &uuid);
};

#endif // CPROJECTTREEWIDGET_H
