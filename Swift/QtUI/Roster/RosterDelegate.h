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
		RosterDelegate(QtTreeWidget* tree, bool compact);
		~RosterDelegate();
		QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	public slots:
		void setCompact(bool compact);
	private:
		QSize contactSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void paintGroup(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const; 
		void paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const; 
		bool compact_;
		DelegateCommons common_;
		GroupItemDelegate* groupDelegate_;
		QtTreeWidget* tree_;
	};
}
