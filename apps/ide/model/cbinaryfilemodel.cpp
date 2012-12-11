
#include "cbinaryfilemodel.h"

#include "cnesicideproject.h"
#include "model/projectsearcher.h"

CBinaryFileModel::CBinaryFileModel()
   : CSubModel()
{
}

QList<QUuid> CBinaryFileModel::getUuids() const
{
   if (m_pProject == NULL)
       return QList<QUuid>();

   return ProjectSearcher::findUuidsOfType<CBinaryFile>(m_pProject);
}

QString CBinaryFileModel::getName(const QUuid &uuid) const
{
   return getFileName(uuid);
}


QString CBinaryFileModel::getFileName(const QUuid &uuid) const
{
   CBinaryFile* file = ProjectSearcher::findItemByUuid<CBinaryFile>(m_pProject, uuid);
   return file->caption();
}


QUuid CBinaryFileModel::addExistingBinaryFile(const QString &path)
{
   if (m_pProject == NULL)
      return QUuid();

   QDir dir(QDir::currentPath());

   CBinaryFiles* binaryFiles = m_pProject->getProject()->getBinaryFiles();

   CBinaryFile* pBinaryFile = new CBinaryFile(binaryFiles);
   pBinaryFile->setName(dir.fromNativeSeparators(dir.relativeFilePath(path)));
   pBinaryFile->setPath(dir.fromNativeSeparators(dir.relativeFilePath(path)));
   pBinaryFile->deserializeContent();

   binaryFiles->getBinaryFileList().append(pBinaryFile);
   binaryFiles->appendChild(pBinaryFile);

   emit binaryFileAdded(pBinaryFile->uuid());
   return pBinaryFile->uuid();
}

void CBinaryFileModel::removeBinaryFile(const QUuid &uuid)
{
   if (m_pProject == NULL)
      return;

   CBinaryFile* file = ProjectSearcher::findItemByUuid<CBinaryFile>(m_pProject, uuid);
   if (file == NULL)
      return;

   CBinaryFiles* binaryFiles = m_pProject->getProject()->getBinaryFiles();
   binaryFiles->removeChild(file);
   binaryFiles->getBinaryFileList().removeAll(file);
   m_pProject->setDirty(true);
   delete file;

   emit binaryFileRemoved(uuid);
}
