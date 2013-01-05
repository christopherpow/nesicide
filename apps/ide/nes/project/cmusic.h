#ifndef CMUSIC_H
#define CMUSIC_H

#include <stdint.h>

#include "cprojectbase.h"
#include "musiceditorform.h"

#include <QMessageBox>
#include <QTabWidget>

class CMusic : public CProjectBase
{
   Q_OBJECT
public:
   CMusic(IProjectTreeViewItem* parent);
   virtual ~CMusic();

   // Member getters
   QList<uint8_t>& getMusic();

   MusicEditorForm* editor() { return dynamic_cast<MusicEditorForm*>(m_editor); }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(CProjectTabWidget* parent);
   virtual void saveItemEvent();
   virtual bool canChangeName() { return true; }
   virtual bool onNameChanged(QString newName);

private:
   QList<uint8_t> m_music;
};

#endif // CMUSIC_H
