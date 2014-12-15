/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>

#include <Swift/QtUI/Roster/DelegateCommons.h>

namespace Swift {
	class UserSearchDelegate : public QStyledItemDelegate {
		public:
			UserSearchDelegate();
			~UserSearchDelegate();
			void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
			QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const;
		private:
			DelegateCommons common_;
	};

}

