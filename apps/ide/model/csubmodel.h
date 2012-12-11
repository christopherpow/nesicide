#ifndef CSUBMODEL_H
#define CSUBMODEL_H

#include <QObject>
#include <QUuid>

class CNesicideProject;
class CDesignerEditorBase;

class CSubModel : public QObject
{
   Q_OBJECT
signals:
   void reset();

public:
   virtual ~CSubModel() { }

   // Retrieve a list of all UUIDs in this model.
   virtual QList<QUuid> getUuids() const=0;

   // Get the name of an object.
   virtual QString getName(const QUuid&)const=0;

   // Workaround methods.
   virtual CDesignerEditorBase* createEditorWidget(const QUuid& uuid) const=0;

protected:
   CNesicideProject* m_pProject;

   friend class CProjectModel;

   CSubModel() : m_pProject(NULL) { }

   void setProject(CNesicideProject *project)
   {
      m_pProject = project;
      emit reset();
   }
};


#endif // CSUBMODEL_H
