
#include "model/cprojectmodel.h"
#include "model/projectsearcher.h"

#include "model/cattributemodel.h"
#include "model/cbinaryfilemodel.h"
#include "model/ccartridgemodel.h"
#include "model/cfiltermodel.h"
#include "model/cgraphicsbankmodel.h"
#include "model/csourcefilemodel.h"
#include "model/ctilestampmodel.h"
#include "model/cmusicmodel.h"

#include "cnesicideproject.h"
#include "iprojecttreeviewitem.h"

//--------------------------------------------------------------------------------------
// CProjectModel implementation
//--------------------------------------------------------------------------------------
CProjectModel::CProjectModel()
{
   m_pNesicideProject = NULL;

   m_pAttributeModel    = new CAttributeModel();
   m_pBinaryFileModel   = new CBinaryFileModel();
   m_pCartridgeModel    = new CCartridgeModel();
   m_pFilterModel       = new CFilterModel(this);
   m_pGraphicsBankModel = new CGraphicsBankModel();
   m_pSourceFileModel   = new CSourceFileModel();
   m_pTileStampModel    = new CTileStampModel();
   m_pMusicModel        = new CMusicModel();

   // Reroute signals from submodels.
   QObject::connect(m_pAttributeModel,    SIGNAL(paletteAdded(QUuid)),        this, SLOT(onItemAdded(QUuid)));
   QObject::connect(m_pAttributeModel,    SIGNAL(paletteDeleted(QUuid)),      this, SLOT(onItemRemoved(QUuid)));
   QObject::connect(m_pBinaryFileModel,   SIGNAL(binaryFileAdded(QUuid)),     this, SLOT(onItemAdded(QUuid)));
   QObject::connect(m_pBinaryFileModel,   SIGNAL(binaryFileRemoved(QUuid)),   this, SLOT(onItemRemoved(QUuid)));
   QObject::connect(m_pGraphicsBankModel, SIGNAL(graphicsBankAdded(QUuid)),   this, SLOT(onItemAdded(QUuid)));
   QObject::connect(m_pGraphicsBankModel, SIGNAL(graphicsBankDeleted(QUuid)), this, SLOT(onItemRemoved(QUuid)));
   QObject::connect(m_pSourceFileModel,   SIGNAL(sourceFileAdded(QUuid)),     this, SLOT(onItemAdded(QUuid)));
   QObject::connect(m_pSourceFileModel,   SIGNAL(sourceFileRemoved(QUuid)),   this, SLOT(onItemRemoved(QUuid)));
   QObject::connect(m_pTileStampModel,    SIGNAL(tileStampAdded(QUuid)),      this, SLOT(onItemAdded(QUuid)));
   QObject::connect(m_pTileStampModel,    SIGNAL(tileStampRemoved(QUuid)),    this, SLOT(onItemRemoved(QUuid)));
   QObject::connect(m_pMusicModel,        SIGNAL(musicFileAdded(QUuid)),      this, SLOT(onItemAdded(QUuid)));
   QObject::connect(m_pMusicModel,        SIGNAL(musicFileRemoved(QUuid)),    this, SLOT(onItemRemoved(QUuid)));
}

CProjectModel::~CProjectModel()
{
   delete m_pAttributeModel;
   delete m_pBinaryFileModel;
   delete m_pCartridgeModel;
   delete m_pFilterModel;
   delete m_pGraphicsBankModel;
   delete m_pSourceFileModel;
   delete m_pTileStampModel;
   delete m_pMusicModel;
}

void CProjectModel::setProject(CNesicideProject *project)
{
   m_pNesicideProject = project;

   // Propagate data changes through all child models.
   m_pAttributeModel->setProject(project);
   m_pCartridgeModel->setProject(project);
   m_pBinaryFileModel->setProject(project);
   m_pFilterModel->setProject(project);
   m_pGraphicsBankModel->setProject(project);
   m_pSourceFileModel->setProject(project);
   m_pTileStampModel->setProject(project);
   m_pMusicModel->setProject(project);

   emit reset();
}

QList<QUuid> CProjectModel::getUuids() const
{
   QList<QUuid> uuids;
   if (m_pNesicideProject == NULL)
      return uuids;

   // Extract the uuids of all project items, but
   // exclude "Filters" like CSourceFiles, CGraphicsBanks, etc.
   uuids.append(ProjectSearcher::findUuidsOfType<CAttributeTable>(m_pNesicideProject));
   uuids.append(ProjectSearcher::findUuidsOfType<CBinaryFile>(m_pNesicideProject));
   uuids.append(ProjectSearcher::findUuidsOfType<CGraphicsBank>(m_pNesicideProject));
   uuids.append(ProjectSearcher::findUuidsOfType<CSourceItem>(m_pNesicideProject));
   uuids.append(ProjectSearcher::findUuidsOfType<CTileStamp>(m_pNesicideProject));
   uuids.append(ProjectSearcher::findUuidsOfType<CMusicItem>(m_pNesicideProject));

   uuids.append(ProjectSearcher::findUuidsOfType<CCHRROMBank>(m_pNesicideProject));
   uuids.append(ProjectSearcher::findUuidsOfType<CPRGROMBank>(m_pNesicideProject));

   // Add filters; they're not present in the Project right now.
   uuids.append(m_pFilterModel->getUuids());

   return uuids;
}

void CProjectModel::visitDataItem(const QUuid &uuid, IUuidVisitor &visitor)
{
   if (m_pNesicideProject == NULL)
      return;

   // If a filter, not in the project tree; search for it first.
   if (this->getFilterModel()->isFilter(uuid))
   {
      CFilterUuid id(uuid);
      visitor.visit(id);
      return;
   }

   // Find project item with target uuid
   IProjectTreeViewItemIterator iter(m_pNesicideProject);
   while ( iter.current() != NULL )
   {
      if (iter.current()->uuid() == uuid.toString())
      {
         // Convert item to ProjectBase.
         CProjectBase* item = dynamic_cast<CProjectBase*>(iter.current());
         if (item == NULL)
            return;

         // Construct Uuid of correct type depending on the class name.
         const char* className = item->metaObject()->className();
         if (strcmp(className,"CBinaryFile") == 0)
         {
            CBinaryFileUuid id(uuid);
            visitor.visit(id);
         }
         else if(strcmp(className,"CSourceItem") == 0)
         {
            CSourceFileUuid id(uuid);
            visitor.visit(id);
         }
         else if (strcmp(className,"CAttributeTable") == 0)
         {
            CAttributeUuid id(uuid);
            visitor.visit(id);
         }
         else if (strcmp(className,"CGraphicsBank") == 0)
         {
            CGraphicsBankUuid id(uuid);
            visitor.visit(id);
         }
         else if (strcmp(className,"CTileStamp") == 0)
         {
            CTileStampUuid id(uuid);
            visitor.visit(id);
         }
         else if (strcmp(className,"CMusicItem") == 0)
         {
            CMusicFileUuid id(uuid);
            visitor.visit(id);
         }
         else if (strcmp(className,"CPRGROMBank") == 0)
         {
            CPrgRomUuid id(uuid);
            visitor.visit(id);
         }
         else if (strcmp(className,"CCHRROMBank") == 0)
         {
            CChrRomUuid id(uuid);
            visitor.visit(id);
         }
         return;
      }
      iter.next();
   }
}

void CProjectModel::onItemAdded(const QUuid &item)
{
   m_pNesicideProject->setDirty(true);
   emit itemAdded(item);
}

void CProjectModel::onItemRemoved(const QUuid &item)
{
   m_pNesicideProject->setDirty(true);
   emit itemRemoved(item);
}
