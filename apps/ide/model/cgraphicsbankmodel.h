#ifndef CGRAPHICSBANKMODEL_H
#define CGRAPHICSBANKMODEL_H

#include "model/csubmodel.h"

class CGraphicsBankModel : public CSubModel
{
   Q_OBJECT
signals:
   void graphicsBankAdded(const QUuid& uuid);
   void graphicsBankDeleted(const QUuid& uuid);

public:
   QUuid newGraphicsBank(const QString& name);
   void deleteGraphicsBank(const QUuid& uuid);

   // Retrieve a list of all UUIDs in this model.
   QList<QUuid> getUuids() const;

   // -- Getters --
   QString getName(const QUuid& uuid) const;

   //QList<IChrRomBankItem*> getGraphics();
   void exportAsPNG(const QUuid& uuid) const;

private:
   friend class CProjectModel;
   CGraphicsBankModel();
};

#endif // CGRAPHICSBANKMODEL_H
