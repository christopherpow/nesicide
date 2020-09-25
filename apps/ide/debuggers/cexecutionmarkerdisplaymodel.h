#ifndef CEXECUTIONMARKERDISPLAYMODEL_H
#define CEXECUTIONMARKERDISPLAYMODEL_H

#include <QAbstractTableModel>

enum
{
   ExecutionVisualizerCol_Color = 0,
   ExecutionVisualizerCol_MinCycles,
   ExecutionVisualizerCol_CurCycles,
   ExecutionVisualizerCol_MaxCycles,
   ExecutionVisualizerCol_StartAddr,
   ExecutionVisualizerCol_EndAddr,
   ExecutionVisualizerCol_Status,
   ExecutionVisualizerCol_MAX
};

class CExecutionMarkerDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   explicit CExecutionMarkerDisplayModel(QObject *parent = 0);
   virtual ~CExecutionMarkerDisplayModel();
   Qt::ItemFlags flags(const QModelIndex& index) const;
   QVariant data(const QModelIndex& index, int role) const;
   QVariant headerData(int section, Qt::Orientation orientation, int role) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;

public slots:
   void update();

private:
   char *m_modelStringBuffer;
};

#endif // CEXECUTIONMARKERDISPLAYMODEL_H
