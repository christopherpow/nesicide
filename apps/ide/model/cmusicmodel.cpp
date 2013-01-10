
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
   if (m_pProject == NULL || path.isNull())
      return QUuid();

   CMusicItem *file = m_pProject->getProject()->getSounds()->getMusics()->addMusicFile(path);
   m_pProject->setDirty(true);

   emit musicFileAdded(file->uuid());
   return file->uuid();
}

QUuid CMusicModel::addExistingMusicFile(const QString &path)
{
   if (m_pProject == NULL || path.isNull())
      return QUuid();

   // TODO Does this work?
   CMusicItem *file = m_pProject->getProject()->getSounds()->getMusics()->addMusicFile(path);
   m_pProject->setDirty(true);

   emit musicFileAdded(file->uuid());
   return file->uuid();
}

void CMusicModel::removeMusicFile(const QUuid &uuid)
{
   if (m_pProject == NULL)
      return;

   // Make sure item has correct type before doing anything.
   CMusicItem* item = ProjectSearcher::findItemByUuid<CMusicItem>(m_pProject, uuid);
   if (item == NULL)
      return;

   m_pProject->getProject()->getSounds()->getMusics()->removeMusicFile(item);
   m_pProject->setDirty(true);

   emit musicFileRemoved(uuid);
}

QList<QUuid> CMusicModel::getUuids() const
{
   if (m_pProject == NULL)
      return QList<QUuid>();

   QList<CMusicItem*> files = ProjectSearcher::findItemsOfType<CMusicItem>(m_pProject);

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
   if (m_pProject == NULL)
      return QByteArray();

   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pProject, uuid);
   return file != NULL ? file->musicData() : QByteArray();
}

QString CMusicModel::getRelativePath(const QUuid &uuid) const
{
   if (m_pProject == NULL)
      return QString();

   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pProject, uuid);
   return file != NULL ? file->path() : QString();
}

QString CMusicModel::getFileName(const QUuid &uuid) const
{
   if (m_pProject == NULL)
      return QString();

   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pProject, uuid);
   return file != NULL ? file->caption() : QString();
}

void CMusicModel::setMusicData(const QUuid &uuid, const QByteArray &data)
{
   if (m_pProject == NULL)
      return;

   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pProject, uuid);
   if (file == NULL)
      return;

   file->setMusicData(data);
   emit musicFileChanged(uuid);
}

void CMusicModel::setRelativePath(const QUuid &uuid, const QString &path)
{
   if (m_pProject == NULL)
      return;

   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pProject, uuid);
   if (file == NULL)
      return;

   file->setName(path);
   file->setPath(path);
   emit musicFileChanged(uuid);
}

CDesignerEditorBase *CMusicModel::createEditorWidget(const QUuid &uuid) const
{

   CMusicItem* file = ProjectSearcher::findItemByUuid<CMusicItem>(m_pProject, uuid);
   if (file == NULL)
      return NULL;

   // Source Item must know their editor widget due to current architecture.
   file->setEditor(new MusicEditorForm(file->caption(), file->musicData(), file));
   return file->editor();
}




