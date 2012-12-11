#ifndef CFILTERMODEL_H
#define CFILTERMODEL_H

#include "model/csubmodel.h"

class CProjectModel;

class CFilterModel : public CSubModel
{
   Q_OBJECT
public:
   QList<QUuid> getUuids() const;
   QString getName(const QUuid &) const;

   // Create a new filter.
   QUuid newFilter(const QString& name);

   // Check wether a uuid describes a filter.
   bool isFilter(const QUuid& uuid) const;

   // Returns true if uuid1 should be placed before uuid2 in the project browser.
   // JSTODO Switch to some other filter ordering scheme.
   bool compare(const QUuid& uuid1, const QUuid& uuid2) const;

   // Data Getters
   QList<QUuid> getFilteredItems(const QUuid& uuid) const;
   QString      getFilterName(const QUuid& uuid) const;

   // Data Setters
   void addToFilter(const QUuid& filterUuid, const QUuid& uuid);

   CDesignerEditorBase* createEditorWidget(const QUuid&) const { return NULL; }

private:
   friend class CProjectModel;
   CFilterModel(CProjectModel* project);

   CProjectModel* m_pProjectModel;
   QList<QUuid>   m_filters;

   int getFilterId(const QUuid& filterUuid) const;
};

#endif // CFILTERMODEL_H
