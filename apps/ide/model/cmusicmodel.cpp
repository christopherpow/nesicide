
#include "cmusicmodel.h"

#include "cnesicideproject.h"
#include "cdesignereditorbase.h"
#include "model/projectsearcher.h"

CMusicModel::CMusicModel()
   : CSubModel()
{
}


QList<QUuid> CMusicModel::getUuids() const
{
   if (m_pProject == NULL)
      return QList<QUuid>();

   return ProjectSearcher::findUuidsOfType<CMusic>(m_pProject);
}

QString CMusicModel::getName(const QUuid &uuid) const
{
   CMusic* music = ProjectSearcher::findItemByUuid<CMusic>(m_pProject, uuid);
   return music != NULL ? music->caption() : QString();
}


QUuid CMusicModel::newMusic(const QString &name)
{
   if (m_pProject == NULL)
      return QUuid();

   CMusics* musics = m_pProject->getProject()->getSounds()->getMusics();
   CMusic* pMusic = new CMusic(musics);
   pMusic->setName(name);
   musics->getMusicList().append(pMusic);
   musics->appendChild(pMusic);
   m_pProject->setDirty(true);

   emit musicAdded(pMusic->uuid());
   return pMusic->uuid();
}

void CMusicModel::deleteMusic(const QUuid &uuid)
{
   if (m_pProject == NULL)
      return;

   CMusic* music = ProjectSearcher::findItemByUuid<CMusic>(m_pProject, uuid);
   if (music == NULL)
      return;

   CMusics* musics = m_pProject->getProject()->getSounds()->getMusics();
   musics->removeChild(music);
   musics->getMusicList().removeAll(music);
   m_pProject->setDirty(true);
   delete music;

   emit musicDeleted(uuid);
}


CDesignerEditorBase *CMusicModel::createEditorWidget(const QUuid &uuid) const
{
   CMusic* music = ProjectSearcher::findItemByUuid<CMusic>(m_pProject, uuid);
   if (music == NULL)
      return NULL;

   // Item must know editor due to current architecture.
   music->setEditor(new MusicEditorForm(/*music->getMusic(),*/ music));
   return music->editor();
}
