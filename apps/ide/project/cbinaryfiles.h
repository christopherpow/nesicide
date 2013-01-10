#ifndef CBINARYFILES_H
#define CBINARYFILES_H

#include "cprojectbase.h"
#include "cbinaryfile.h"

#include <QList>
#include <QMenu>
#include <QFileDialog>

class CBinaryFiles : public CProjectBase
{
   Q_OBJECT
public:
   CBinaryFiles(IProjectTreeViewItem* parent);
   virtual ~CBinaryFiles();

   // Helper functions
   void initializeProject();
   void terminateProject();

   QList<CBinaryFile*>& getBinaryFileList() { return m_binaryFiles; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void openItemEvent(CProjectTabWidget*) {}

   // ICenterWidgetItem Interface Implementation

private:
   // Contained children
   QList<CBinaryFile*> m_binaryFiles;
};

#endif // CBINARYFILES_H
