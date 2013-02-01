#ifndef CMUSICFAMITRACKERPATTERNITEMDELEGATE_H
#define CMUSICFAMITRACKERPATTERNITEMDELEGATE_H

#include <QStyledItemDelegate>

class CMusicFamiTrackerPatternItemDelegate : public QStyledItemDelegate
{
   Q_OBJECT
public:
   explicit CMusicFamiTrackerPatternItemDelegate(QObject *parent = 0);
   
   void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

signals:
   
public slots:
   
};

#endif // CMUSICFAMITRACKERPATTERNITEMDELEGATE_H
