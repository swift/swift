#include "RosterDelegate.h"

#include <QPainter>
#include <QColor>
#include <QBrush>
#include <qdebug.h>

#include "QtTreeWidgetItem.h"

namespace Swift {
QSize RosterDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	QtTreeWidgetItem* item = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (item && !item->isContact()) {
		return QStyledItemDelegate::sizeHint(option, index);
	}
	//Doesn't work, yay! FIXME: why?
	QSize size = (option.state & QStyle::State_Selected) ? QSize(200, 50) : QSize(100,50);
	qDebug() << "Returning size" << size;
	return size;
}

void RosterDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QtTreeWidgetItem* item = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (item && !item->isContact()) {
		QStyledItemDelegate::paint(painter, option, index);
		return;
	}
	qDebug() << "painting" << index.data(Qt::DisplayRole).toString();
	painter->save();
	//QStyledItemDelegate::paint(painter, option, index);
	//initStyleOption(option, index);
	QRect fullRegion(option.rect);
	if ( option.state & QStyle::State_Selected ) {
		painter->fillRect(fullRegion, option.palette.highlight());
		painter->setPen(option.palette.highlightedText().color());
	} 
	QRect avatarRegion(QPoint(0, fullRegion.top()), QSize(32, 32));
	QIcon icon = index.data(AvatarRole).isValid() && !index.data(AvatarRole).value<QIcon>().isNull()
		? index.data(AvatarRole).value<QIcon>()
		: QIcon(":/icons/avatar.png");
	icon.paint(painter, avatarRegion, Qt::AlignVCenter | Qt::AlignHCenter);
	
	QFont nameFont = painter->font();
	QFont statusFont = painter->font();
	statusFont.setStyle(QFont::StyleItalic);
	//statusFont.setSize(-1);
	painter->setFont(nameFont);
	QRect textRegion(fullRegion.adjusted(avatarRegion.width(), 0, 0, 0));
	QRect nameRegion(textRegion.adjusted(0,0,0,-25));
	painter->drawText(nameRegion, Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());
	painter->setFont(statusFont);
	QRect statusTextRegion(textRegion.adjusted(0, 25, 0, 0));
	painter->drawText(statusTextRegion, Qt::AlignVCenter, index.data(StatusTextRole).toString());
	
	painter->restore();
}

}