
#include "model/csourcefilemodel.h"
#include "model/projectsearcher.h"
#include "cnesicideproject.h"
#include "filepropertiesdialog.h"

CSourceFileModel::CSourceFileModel()
   : CSubModel()
{

}

QUuid CSourceFileModel::newSourceFile(const QString& path)
{
   if (m_pNesicideProject == NULL || path.isNull())
      return QUuid();

   CSourceItem *file = m_pNesicideProject->getProject()->getSources()->addSourceFile(path);
   m_pNesicideProject->setDirty(true);

   emit sourceFileAdded(file->uuid());
   return file->uuid();
}

QUuid CSourceFileModel::addExistingSourceFile(const QString &path)
{
   if (m_pNesicideProject == NULL || path.isNull())
      return QUuid();

   // TODO Does this work?
   CSourceItem *file = m_pNesicideProject->getProject()->getSources()->addSourceFile(path);
   m_pNesicideProject->setDirty(true);

   emit sourceFileAdded(file->uuid());
   return file->uuid();
}

void CSourceFileModel::fileProperties(const QUuid &uuid)
{
   if (m_pNesicideProject == NULL)
      return;

   // Make sure item has correct type before doing anything.
   CSourceItem* item = ProjectSearcher::findItemByUuid<CSourceItem>(m_pNesicideProject, uuid);
   if (item == NULL)
      return;

   FilePropertiesDialog fpd(item);
   int ret = fpd.exec();
   if ( ret == QDialog::Accepted )
   {
      m_pNesicideProject->setDirty(true);
   }
}

void CSourceFileModel::removeSourceFile(const QUuid &uuid)
{
   if (m_pNesicideProject == NULL)
      return;

   // Make sure item has correct type before doing anything.
   CSourceItem* item = ProjectSearcher::findItemByUuid<CSourceItem>(m_pNesicideProject, uuid);
   if (item == NULL)
      return;

   m_pNesicideProject->getProject()->getSources()->removeSourceFile(item);
   m_pNesicideProject->setDirty(true);

   emit sourceFileRemoved(uuid);
}

QList<QUuid> CSourceFileModel::getUuids() const
{
   if (m_pNesicideProject == NULL)
      return QList<QUuid>();

   QList<CSourceItem*> files = ProjectSearcher::findItemsOfType<CSourceItem>(m_pNesicideProject);

   QList<QUuid> uuids;
   foreach(CSourceItem* file, files)
   {
      uuids.append(file->uuid());
   }
   return uuids;
}

QString CSourceFileModel::getName(const QUuid &uuid) const
{
   return getFileName(uuid);
}

QString CSourceFileModel::getSourceCode(const QUuid &uuid) const
{
   if (m_pNesicideProject == NULL)
      return QString();

   CSourceItem* file = ProjectSearcher::findItemByUuid<CSourceItem>(m_pNesicideProject, uuid);
   return file != NULL ? file->sourceCode() : QString();
}

QString CSourceFileModel::getRelativePath(const QUuid &uuid) const
{
   if (m_pNesicideProject == NULL)
      return QString();

   CSourceItem* file = ProjectSearcher::findItemByUuid<CSourceItem>(m_pNesicideProject, uuid);
   return file != NULL ? file->path() : QString();
}

QString CSourceFileModel::getFileName(const QUuid &uuid) const
{
   if (m_pNesicideProject == NULL)
      return QString();

   CSourceItem* file = ProjectSearcher::findItemByUuid<CSourceItem>(m_pNesicideProject, uuid);
   return file != NULL ? file->caption() : QString();
}

void CSourceFileModel::setSourceCode(const QUuid &uuid, const QString &source)
{
   if (m_pNesicideProject == NULL)
      return;

   CSourceItem* file = ProjectSearcher::findItemByUuid<CSourceItem>(m_pNesicideProject, uuid);
   if (file == NULL)
      return;

   file->setSourceCode(source);
   emit sourceFileChanged(uuid);
}

void CSourceFileModel::setRelativePath(const QUuid &uuid, const QString &path)
{
   if (m_pNesicideProject == NULL)
      return;

   CSourceItem* file = ProjectSearcher::findItemByUuid<CSourceItem>(m_pNesicideProject, uuid);
   if (file == NULL)
      return;

   file->setName(path);
   file->setPath(path);
   emit sourceFileChanged(uuid);
}

CDesignerEditorBase *CSourceFileModel::createEditorWidget(const QUuid &uuid) const
{
   CSourceItem* file = ProjectSearcher::findItemByUuid<CSourceItem>(m_pNesicideProject, uuid);
   if (file == NULL)
      return NULL;

   // Source Item must know their editor widget due to current architecture.
   // Load before displaying...
   file->deserializeContent();
   
   file->setEditor(new CodeEditorForm(file->caption(), file->sourceCode(), file));
   return file->editor();
}
