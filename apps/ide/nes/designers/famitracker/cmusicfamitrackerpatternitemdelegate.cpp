#include "cmusicfamitrackerpatternitemdelegate.h"

#include <QPainter>

CMusicFamiTrackerPatternItemDelegate::CMusicFamiTrackerPatternItemDelegate(QObject *parent) :
   QStyledItemDelegate(parent)
{
}

void CMusicFamiTrackerPatternItemDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
   painter->fillRect(option.rect,QColor(200,200,200));
   
   if (option.state & QStyle::State_Selected)
       painter->fillRect(option.rect, option.palette.highlight());
}
