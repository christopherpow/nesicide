#ifndef CMUSICMODEL_H
#define CMUSICMODEL_H

#include "model/csubmodel.h"

class CDesignerEditorBase;

class CMusicModel : public CSubModel
{
   Q_OBJECT
signals:
   void musicAdded(const QUuid& uuid);
   void musicDeleted(const QUuid& uuid);

public:
   QUuid newMusic(const QString& name);
   void deleteMusic(const QUuid& uuid);

   QList<QUuid> getUuids() const;
   QString getName(const QUuid& uuid) const;

   // -- Getters --
   //QList<IChrRomBankItem*> getGraphics();
   //void exportAsPNG(const QUuid& uuid) const;

   // Workaround methods.
   CDesignerEditorBase* createEditorWidget(const QUuid& uuid) const;

private:
   friend class CProjectModel;
   CMusicModel();
};

#endif // CMUSICMODEL_H
