#ifndef CCODEBROWSERDISPLAYMODEL_H
#define CCODEBROWSERDISPLAYMODEL_H

#include <QAbstractTableModel>

#include "cbreakpointinfo.h"

enum
{
   CodeBrowserCol_Decoration = 0,
   CodeBrowserCol_Address,
   CodeBrowserCol_Opcode,
   CodeBrowserCol_Operand1,
   CodeBrowserCol_Operand2,
   CodeBrowserCol_Disassembly,
   CodeBrowserCol_Max
};

class CCodeBrowserDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CCodeBrowserDisplayModel(CBreakpointInfo* pBreakpoints,QObject* parent);
   virtual ~CCodeBrowserDisplayModel();
   QVariant data(const QModelIndex& index, int role) const;
   Qt::ItemFlags flags(const QModelIndex& index) const;
   QVariant headerData(int section, Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                     const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;

private:
   CBreakpointInfo *m_pBreakpoints;
   char            *m_modelStringBuffer;

public slots:
   void update(void);
};

#endif // CCODEBROWSERDISPLAYMODEL_H
