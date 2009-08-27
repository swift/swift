#pragma once

#include <QStyledItemDelegate>
#include <QColor>
#include <QFont>

namespace Swift {
	class QtTreeWidgetItem;
	class RosterDelegate : public QStyledItemDelegate {
	public:
		RosterDelegate();
		QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	private:
		void paintGroup(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const; 
		void paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const; 
		QFont nameFont_;
		QFont statusFont_;
		static const int avatarSize_ = 20;
		static const int presenceIconHeight_ = 16;
		static const int presenceIconWidth_ = 16;
		static const int groupCornerRadius_ = 4;
		static const int horizontalMargin_ = 4;
		static const int verticalMargin_ = 1;
	};
}
