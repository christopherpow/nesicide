#ifndef CSOURCEITEM_H
#define CSOURCEITEM_H

#include "cprojectbase.h"
#include "codeeditorform.h"

#include <QMessageBox>
#include <QTabWidget>

class CSourceItem : public CProjectBase
{
   Q_OBJECT
public:
   CSourceItem(IProjectTreeViewItem* parent);
   virtual ~CSourceItem();

   // Member Getters
   QString sourceCode();

   // Member Setters
   void setSourceCode(QString sourceCode);

   CodeEditorForm* editor() { return dynamic_cast<CodeEditorForm*>(m_editor); }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool serializeContent();
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);
   virtual bool deserializeContent();

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void openItemEvent(CProjectTabWidget* parent);
   virtual void saveItemEvent();

private:
   // Attributes
   QString m_sourceCode;
};

#endif // CSOURCEITEM_H
