#ifndef CMUSICS_H
#define CMUSICS_H

#include "cmusic.h"

#include <QInputDialog>
#include <QList>

class CMusics : public CProjectBase
{
   Q_OBJECT
public:
   CMusics(IProjectTreeViewItem* parent);
   virtual ~CMusics();

   // Helper functions
   void initializeProject();
   void terminateProject();

   QList<CMusic*>& getMusicList() { return m_musics; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(CProjectTabWidget*) {}

   // ICenterWidgetItem Interface Implementation

private:
   // Contained children
   QList<CMusic*> m_musics;
};

#endif // CMUSICS_H
