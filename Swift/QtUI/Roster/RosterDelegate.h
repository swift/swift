/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QStyledItemDelegate>
#include <QColor>
#include <QFont>

#include "GroupItemDelegate.h"
#include "DelegateCommons.h"

namespace Swift {
	class QtTreeWidget;
	class RosterDelegate : public QStyledItemDelegate {
	public:
		RosterDelegate(QtTreeWidget* tree);
		~RosterDelegate();
		QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	private:
		QSize contactSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void paintGroup(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const; 
		void paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const; 
		DelegateCommons common_;
		GroupItemDelegate* groupDelegate_;
		QtTreeWidget* tree_;
		static const int avatarSize_;
		static const int presenceIconHeight_;
		static const int presenceIconWidth_;

	};
}
