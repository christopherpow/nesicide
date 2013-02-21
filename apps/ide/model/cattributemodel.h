#ifndef CATTRIBUTEMODEL_H
#define CATTRIBUTEMODEL_H

#include "model/csubmodel.h"

#include <stdint.h>

class CDesignerEditorBase;

class CAttributeModel : public CSubModel
{
   Q_OBJECT
signals:
   void paletteAdded(const QUuid& uuid);
   void paletteDeleted(const QUuid& uuid);

public:
   QUuid newPalette(const QString& name);
   void deletePalette(const QUuid& uuid);

   QList<QUuid> getUuids() const;
   QString getName(const QUuid& uuid) const;

   // -- Getters --
   QList<uint8_t> getPaletteData(const QUuid& uuid) const;

   // -- Setters --
   void setPaletteData(const QUuid& uuid, const QList<uint8_t>& data);

   // Workaround methods.
   CDesignerEditorBase* createEditorWidget(const QUuid& uuid) const;

private:
   friend class CProjectModel;
   CAttributeModel();
};

#endif // CATTRIBUTEMODEL_H
