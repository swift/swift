#include "RosterDelegate.h"

#include <QPainter>
#include <QColor>
#include <QBrush>
#include <QFontMetrics>
#include <qdebug.h>

#include "QtTreeWidgetItem.h"

namespace Swift {
QSize RosterDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	QtTreeWidgetItem* item = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (item && !item->isContact()) {
		return QStyledItemDelegate::sizeHint(option, index);
	}
	//Doesn't work, yay! FIXME: why?
	QSize size = (option.state & QStyle::State_Selected) ? QSize(150, 80) : QSize(150, avatarSize_ + margin_ * 2);
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
	QRect avatarRegion(QPoint(margin_, fullRegion.top() + margin_), QSize(avatarSize_, avatarSize_));
	QIcon icon = index.data(AvatarRole).isValid() && !index.data(AvatarRole).value<QIcon>().isNull()
		? index.data(AvatarRole).value<QIcon>()
		: QIcon(":/icons/avatar.png");
	icon.paint(painter, avatarRegion, Qt::AlignVCenter | Qt::AlignHCenter);
	
	QFont nameFont = painter->font();
	QFont statusFont = painter->font();
	
	painter->setFont(nameFont);
	QRect textRegion(fullRegion.adjusted(avatarSize_ + margin_ * 2, 0, 0, 0));
	
	QFontMetrics nameMetrics(nameFont);
	int nameHeight = nameMetrics.height() + margin_ * 2;
	QRect nameRegion(textRegion.adjusted(0, margin_, 0, 0));
	painter->drawText(nameRegion, Qt::AlignTop, index.data(Qt::DisplayRole).toString());
	
	statusFont.setStyle(QFont::StyleItalic);
	statusFont.setPointSize(10);
	painter->setFont(statusFont);
	painter->setPen(QPen(QColor(160,160,160)));
	QRect statusTextRegion(textRegion.adjusted(0, nameHeight, 0, 0));
	painter->drawText(statusTextRegion, Qt::AlignTop, index.data(StatusTextRole).toString());
	
	painter->restore();
}

}