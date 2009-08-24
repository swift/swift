#pragma once

#include <QStyledItemDelegate>
#include <QColor>

namespace Swift {
	class RosterDelegate : public QStyledItemDelegate {
	public:
		QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	private:
		static const int avatarSize_ = 28;
		static const int presenceIconHeight_ = 16;
		static const int presenceIconWidth_ = 16;
		static const int margin_ = 4;
	};
}
