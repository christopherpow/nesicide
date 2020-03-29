#ifndef CPROJECTTREEOPENACTION_H
#define CPROJECTTREEOPENACTION_H

#include "model/iuuidvisitor.h"
#include <QObject>
#include <QTreeWidget>

class CProjectModel;
class CSubModel;
class CProjectTabWidget;

class CProjectTreeOpenAction : public QObject, public IUuidVisitor
{
   Q_OBJECT
public:
   CProjectTreeOpenAction(CProjectTabWidget* tabWidget, CProjectModel* project);

   virtual void visit(CUuid&);
   virtual void visit(CAttributeUuid&);
   virtual void visit(CBinaryFileUuid&);
   virtual void visit(CFilterUuid&);
   virtual void visit(CGraphicsBankUuid&);
   virtual void visit(CSourceFileUuid&);
   virtual void visit(CTileStampUuid&);
   virtual void visit(CMusicFileUuid&);

   virtual void visit(CChrRomUuid&);
   virtual void visit(CPrgRomUuid&);

private:
   CProjectTabWidget*  m_tabWidget;
   CProjectModel*      m_project;

   void doVisit(CSubModel* model, const QUuid& uuid);
};

#endif // CPROJECTTREEOPENACTION_H
