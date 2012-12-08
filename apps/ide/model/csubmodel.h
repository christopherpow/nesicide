#ifndef CSUBMODEL_H
#define CSUBMODEL_H

#include <QObject>
#include <QUuid>

class CNesicideProject;

class CSubModel : public QObject
{
   Q_OBJECT
signals:
   void reset();

public:
   virtual ~CSubModel() { }

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
