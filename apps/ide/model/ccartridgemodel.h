#ifndef CCARTRIDGEMODEL_H
#define CCARTRIDGEMODEL_H

#include "model/csubmodel.h"

class CCartridgeModel : public CSubModel
{
public:
   CCartridgeModel();

   QList<QUuid> getPrgRomUuids() const;
   QList<QUuid> getChrRomUuids() const;

   QList<QUuid> getUuids() const;
   QString getName(const QUuid& uuid) const;

   CDesignerEditorBase* createEditorWidget(const QUuid&) const;
};

#endif // CCARTRIDGEMODEL_H
