#ifndef CTILESTAMP_H
#define CTILESTAMP_H

#include <stdint.h>

#include "cprojectbase.h"
#include "tilestampeditorform.h"

#include <QMessageBox>
#include <QTabWidget>

class CTileStamp : public CProjectBase
{
   Q_OBJECT
public:
   CTileStamp(IProjectTreeViewItem* parent);
   virtual ~CTileStamp();

   // Member getters
   QByteArray getTile();

   TileStampEditorForm* editor() { return dynamic_cast<TileStampEditorForm*>(m_editor); }


   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(CProjectTabWidget* parent);
   virtual void saveItemEvent();
   virtual bool canChangeName();
   virtual bool onNameChanged(QString newName);

private:
   QByteArray m_tile;
};

#endif // CTILESTAMP_H
