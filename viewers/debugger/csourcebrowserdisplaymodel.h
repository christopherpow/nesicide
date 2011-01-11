#ifndef CSOURCEBROWSERDISPLAYMODEL_H
#define CSOURCEBROWSERDISPLAYMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class CSourceBrowserDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CSourceBrowserDisplayModel(QObject* parent);
   virtual ~CSourceBrowserDisplayModel();
   QVariant data(const QModelIndex& index, int role) const;
   Qt::ItemFlags flags(const QModelIndex& index) const;
   QVariant headerData(int section, Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                     const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   void layoutChangedEvent();
   void setSource(QStringList& source) { m_source = source; }
   void setSourceFilename(char* fileName) { m_sourceFilename = fileName; }

private:
   QStringList m_source;
   QString     m_sourceFilename;
};

#endif // CSOURCEBROWSERDISPLAYMODEL_H
