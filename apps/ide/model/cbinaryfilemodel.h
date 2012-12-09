#ifndef CBINARYFILEMODEL_H
#define CBINARYFILEMODEL_H

#include "model/csubmodel.h"
#include <QImage>

class CBinaryFileModel : public CSubModel
{
   Q_OBJECT
public:
   QUuid addExistingBinaryFile(const QString& path);
   void removeBinaryFile(const QUuid& uuid);

   // Retrieve a list of all UUIDs in this model.
   QList<QUuid> getUuids() const;

   // -- Getters --
   QByteArray getBinaryData(const QUuid& uuid) const;
   QImage getBinaryImage(const QUuid& uuid) const;
   QSize getSize(const QUuid& uuid) const;

   // -- Setters --
   void setBinaryData(const QUuid& uuid, const QByteArray& newBinaryData);

private:
   friend class CProjectModel;
   CBinaryFileModel();
};

#endif // CBINARYFILEMODEL_H
