#ifndef CSOUNDS_H
#define CSOUNDS_H

#include "cprojectbase.h"
#include "cmusics.h"

#include <QString>

class CSounds : public CProjectBase
{
   Q_OBJECT
public:
   CSounds(IProjectTreeViewItem* parent);
   virtual ~CSounds();

   // Helper functions
   void initializeProject();
   void terminateProject();

   CMusics* getMusics() { return m_pMusics; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void openItemEvent(CProjectTabWidget*) {}

   // ICenterWidgetItem Interface Implementation

private:
   // Contained children
   CMusics* m_pMusics;
};

#endif // CSOUNDS_H
