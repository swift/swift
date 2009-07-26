#include "RosterDelegate.h"

#include <QPainter>

namespace Swift {
QSize RosterDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	return QSize(100,50);
}

void RosterDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex index) const {
	painter->save();
	QStyledItemDelegate::paint(painter, option, index);
	painter->restore();
}

}