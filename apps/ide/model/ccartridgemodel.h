#ifndef CCARTRIDGEMODEL_H
#define CCARTRIDGEMODEL_H

#include "model/csubmodel.h"

class CCartridgeModel : public CSubModel
{
public:
   CCartridgeModel();

   QList<QUuid> getPrgRomUuids() const;
   QList<QUuid> getChrRomUuids() const;

   QString getName(const QUuid& uuid) const;
};

#endif // CCARTRIDGEMODEL_H
