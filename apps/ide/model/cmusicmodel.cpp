
#include "cmusicmodel.h"

#include "cnesicideproject.h"
#include "cdesignereditorbase.h"
#include "model/projectsearcher.h"

CMusicModel::CMusicModel()
   : CSubModel()
{

}

QUuid CMusicModel::newMusicFile(const QString& path)
{
   if (m_pNesicideProject == NULL || path.isNull())
      return QUuid();

   CMusicItem *file = m_pNesicideProject->getProject()->getSounds()->getMusics()->addMusicFile(path);
   m_pNesicideProject->setDirty(true);

   emit musicFileAdded(file->uuid());
   return file->uuid();
}

QUuid CMusicModel::addExistingMusicFile(const QString &path)
{
   if (m_pNesicideProject == NULL || path.isNull())
      return QUuid();

   // TODO Does this work?
   CMusicItem *file = m_pNesicideProject->getProject()->getSounds()->getMusics()->addMusicFile(path);
   m_pNesicideProject->setDirty(true);

   emit musicFileAdded(file->uuid());
   return file->uuid();
}

void CMusicModel::removeMusicFile(const QUuid &uuid)
{
   if (m_pNesicideProject == NULL)
      return;

   // Make sure item has correct type before doing anything.
   CMusicItem* item = ProjectSearcher::findItemByUuid<CMusicItem>(m_pNesicideProject, uuid);
   if (item == NULL)
      return;

   m_pNesicideProject->getProject()->getSounds()->getMusics()->removeMusicFile(item);
   m_pNesicideProject->setDirty(true);

   emit musicFileRemoved(uuid);
}

QList<QUuid> CMusicModel::getUuids() const
{
   if (m_pNesicideProject == NULL)
      return QList<QUuid>();

   QList<CMusicItem*> files = ProjectSearcher::findItemsOfType<CMusicItem>(m_pNesicideProject);

   QList<QUuid> uuids;
   foreach(CMusicItem* file, files)
   {
      uuids.append(file->uuid());
   }
   return uuids;
}

QString CMusicModel::getName(const QUuid &uuid) const
{
   return getFileName(uuid);
}

QByteArray CMusicModel::getMusicData(const QUuid &uuid) const
{
   if (m_pNesicideProject == NULL)
      return QByteArray();

   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pNesicideProject, uuid);
   return file != NULL ? file->musicData() : QByteArray();
}

QString CMusicModel::getRelativePath(const QUuid &uuid) const
{
   if (m_pNesicideProject == NULL)
      return QString();

   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pNesicideProject, uuid);
   return file != NULL ? file->path() : QString();
}

QString CMusicModel::getFileName(const QUuid &uuid) const
{
   if (m_pNesicideProject == NULL)
      return QString();

   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pNesicideProject, uuid);
   return file != NULL ? file->caption() : QString();
}

void CMusicModel::setMusicData(const QUuid &uuid, const QByteArray &data)
{
   if (m_pNesicideProject == NULL)
      return;

   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pNesicideProject, uuid);
   if (file == NULL)
      return;

   file->setMusicData(data);
   emit musicFileChanged(uuid);
}

void CMusicModel::setRelativePath(const QUuid &uuid, const QString &path)
{
   if (m_pNesicideProject == NULL)
      return;

   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pNesicideProject, uuid);
   if (file == NULL)
      return;

   file->setName(path);
   file->setPath(path);
   emit musicFileChanged(uuid);
}

CDesignerEditorBase *CMusicModel::createEditorWidget(const QUuid &uuid) const
{
   MusicEditorForm* editor = MusicEditorForm::instance();
   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pNesicideProject, uuid);
   if (file == NULL)
      return NULL;

   // Source Item must know their editor widget due to current architecture.
   openFile(file->caption());
   editor->setFileName(file->caption());
   editor->setTreeLink(file);
   file->setEditor(editor);
   return file->editor();
}
