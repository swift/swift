/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
		void paintShadowText(QPainter* painter, const QRect& region, const QString& text) const;
		void paintExpansionTriangle(QPainter* painter, const QRect& region, int width, int height, bool expanded) const;
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
