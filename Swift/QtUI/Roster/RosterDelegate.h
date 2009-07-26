#pragma once

#import <QStyledItemDelegate>

namespace Swift {
	class RosterDelegate : public QStyledItemDelegate {
	public:
		QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	};
}