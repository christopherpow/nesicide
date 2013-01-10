#ifndef CMUSICMODEL_H
#define CMUSICMODEL_H

#include "model/csubmodel.h"

class CDesignerEditorBase;

class CMusicModel : public CSubModel
{
   Q_OBJECT
signals:
   void musicFileChanged(const QUuid& uuid);
   void musicFileAdded  (const QUuid& uuid);
   void musicFileRemoved(const QUuid& uuid);

public:
   // Add a new music file to the project.
   QUuid newMusicFile(const QString& path);

   // Add an existing music file to the project.
   QUuid addExistingMusicFile(const QString& path);

   // Remove a music file from the project.
   void removeMusicFile(const QUuid& uuid);

   QList<QUuid> getUuids() const;
   QString      getName(const QUuid &uuid) const;

   // -- Getters --
   QByteArray getMusicData(const QUuid& uuid) const;
   QString getRelativePath(const QUuid& uuid) const;
   QString getFileName(const QUuid& uuid) const;

   // -- Setters --
   void setMusicData(const QUuid& uuid, const QByteArray& source);
   void setRelativePath(const QUuid& uuid, const QString& path);

   CDesignerEditorBase* createEditorWidget(const QUuid&) const;
private:
   friend class CProjectModel;
   CMusicModel();
};

#endif // CMUSICMODEL_H
