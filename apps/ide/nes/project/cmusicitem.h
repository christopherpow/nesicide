#ifndef CMUSICITEM_H
#define CMUSICITEM_H

#include <stdint.h>

#include "cprojectbase.h"
#include "musiceditorform.h"

#include <QMessageBox>
#include <QTabWidget>

class CMusicItem : public CProjectBase
{
   Q_OBJECT
public:
   CMusicItem(IProjectTreeViewItem* parent);
   virtual ~CMusicItem();

   // Member Getters
   QByteArray musicData();

   // Member Setters
   void setMusicData(QByteArray musicData);

   MusicEditorForm* editor() { return dynamic_cast<MusicEditorForm*>(m_editor); }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool serializeContent();
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);
   virtual bool deserializeContent();

   // IProjectTreeViewItem Interface Implmentation
   virtual void openItemEvent(CProjectTabWidget* parent);
   virtual bool canChangeName() { return true; }
   virtual bool onNameChanged(QString newName);

   // CProjectBase Interface Implementation
   virtual bool exportData();

private:
   QByteArray m_musicData;
};

#endif // CMUSICITEM_H
