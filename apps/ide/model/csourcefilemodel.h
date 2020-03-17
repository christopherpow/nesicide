#ifndef CSOURCEFILES_H
#define CSOURCEFILES_H

#include "model/csubmodel.h"

class CSourceFileModel : public CSubModel
{
   Q_OBJECT
signals:
   void sourceFileChanged(const QUuid& uuid);
   void sourceFileAdded  (const QUuid& uuid);
   void sourceFileRemoved(const QUuid& uuid);

public:
   // Add a new source file to the project.
   QUuid newSourceFile(const QString& path);

   // Add an existing source file to the project.
   QUuid addExistingSourceFile(const QString& path);

   // Remove a source file from the project.
   void removeSourceFile(const QUuid& uuid);

   // Properties handler.
   void fileProperties(const QUuid& uuid);

   QList<QUuid> getUuids() const;
   QString      getName(const QUuid &uuid) const;

   // -- Getters --
   QString getSourceCode(const QUuid& uuid) const;
   QString getRelativePath(const QUuid& uuid) const;
   QString getFileName(const QUuid& uuid) const;

   // -- Setters --
   void setSourceCode(const QUuid& uuid, const QString& source);
   void setRelativePath(const QUuid& uuid, const QString& path);

   CDesignerEditorBase* createEditorWidget(const QUuid&) const;

private:
   friend class CProjectModel;
   CSourceFileModel();
};

#endif // CSOURCEFILES_H
