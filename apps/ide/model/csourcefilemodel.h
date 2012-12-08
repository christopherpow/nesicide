#ifndef CSOURCEFILES_H
#define CSOURCEFILES_H

#include <QObject>
#include <QUuid>

class CNesicideProject;

class CSourceFileModel : public QObject
{
   Q_OBJECT
public:
   // Add a new source file to the project.
   QUuid newSourceFile(const QString& path);

   // Remove a source file from the project.
   void removeSourceFile(const QUuid& uuid);

   // Retrieve a list of all UUIDs in this model.
   QList<QUuid> getUuids() const;

   // -- Getters --
   QString getSourceCode(const QUuid& uuid) const;
   QString getRelativePath(const QUuid& uuid) const;
   QString getFileName(const QUuid& uuid) const;

   // -- Setters --
   void setSourceCode(const QUuid& uuid, const QString& source);
   void setRelativePath(const QUuid& uuid, const QString& path);

signals:
   void sourceFileChanged(const QUuid& uuid);
   void sourceFileAdded  (const QUuid& uuid);
   void sourceFileRemoved(const QUuid& uuid);

   // Fired when the underlying project changes.
   void reset();

private:
   CNesicideProject* m_pProject;

   friend class CProjectModel;

   CSourceFileModel();
   void setProject(CNesicideProject* project);
};

#endif // CSOURCEFILES_H
