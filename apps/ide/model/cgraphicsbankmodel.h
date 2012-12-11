#ifndef CGRAPHICSBANKMODEL_H
#define CGRAPHICSBANKMODEL_H

#include "model/csubmodel.h"

class CDesignerEditorBase;

class CGraphicsBankModel : public CSubModel
{
   Q_OBJECT
signals:
   void graphicsBankAdded(const QUuid& uuid);
   void graphicsBankDeleted(const QUuid& uuid);

public:
   QUuid newGraphicsBank(const QString& name);
   void deleteGraphicsBank(const QUuid& uuid);

   QList<QUuid> getUuids() const;
   QString getName(const QUuid& uuid) const;

   // -- Getters --
   //QList<IChrRomBankItem*> getGraphics();
   void exportAsPNG(const QUuid& uuid) const;

   // Workaround methods.
   CDesignerEditorBase* createEditorWidget(const QUuid& uuid) const;

private:
   friend class CProjectModel;
   CGraphicsBankModel();
};

#endif // CGRAPHICSBANKMODEL_H
