#ifndef CATTRIBUTEMODEL_H
#define CATTRIBUTEMODEL_H

#include "model/csubmodel.h"

class CAttributeModel : public CSubModel
{
   Q_OBJECT
public:
   QUuid newPalette(const QString& name);
   void deletePalette(const QUuid& uuid);

   // Retrieve a list of all UUIDs in this model.
   QList<QUuid> getUuids() const;

   // -- Getters --
   QString getName(const QUuid& uuid) const;
   QList<uint8_t> getPaletteData(const QUuid& uuid) const;

   // -- Setters --
   void setPaletteData(const QUuid& uuid, const QList<uint8_t>& data);

private:
   friend class CProjectModel;
   CAttributeModel();
};

#endif // CATTRIBUTEMODEL_H
