#ifndef CBREAKPOINTDISPLAYMODEL_H
#define CBREAKPOINTDISPLAYMODEL_H

#include <QAbstractTableModel>

#include "cbreakpointinfo.h"

class CBreakpointDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CBreakpointDisplayModel(CBreakpointInfo* pBreakpoints,QObject* parent = 0);
   virtual ~CBreakpointDisplayModel();
   Qt::ItemFlags flags(const QModelIndex& index) const;
   QVariant data(const QModelIndex& index, int role) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;

private:
   CBreakpointInfo *m_pBreakpoints;
   char            *m_modelStringBuffer;

public slots:
   void update();
};

#endif // CBREAKPOINTDISPLAYMODEL_H
