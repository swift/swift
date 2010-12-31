/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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

