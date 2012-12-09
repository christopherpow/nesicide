#ifndef CTILESTAMPMODEL_H
#define CTILESTAMPMODEL_H

#include "model/csubmodel.h"
#include <QImage>

class CTileStampModel : public CSubModel
{
   Q_OBJECT
public:
   QUuid newTileStamp(const QString& name);
   void deleteTileStamp(const QUuid& uuid);

   // Retrieve a list of all UUIDs in this model.
   QList<QUuid> getUuids() const;

   // -- Getters --
   QByteArray getTileData(const QUuid& uuid) const;
   QByteArray getAttributeData(const QUuid& uuid) const;
   QSize getSize(const QUuid& uuid) const;
   QUuid getPaletteUuid(const QUuid& uuid) const;
   QImage getTileImage(const QUuid& uuid) const;
   //QList<PropertyItem> getTileProperties(const QUuid& uuid) const;

   // -- Setters --
   void setSize(const QUuid& uuid, QSize& size);

private:
   friend class CProjectModel;
   CTileStampModel();
};

#endif // CTILESTAMPMODEL_H
