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
		QSize groupSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		QSize contactSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void paintGroup(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const; 
		void paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const; 
		QFont nameFont_;
		QFont statusFont_;
		QFont groupFont_;
		static const int avatarSize_;
		static const int presenceIconHeight_;
		static const int presenceIconWidth_;
		static const int groupCornerRadius_;
		static const int horizontalMargin_;
		static const int verticalMargin_ ;
		static const int farLeftMargin_;
	};
}
