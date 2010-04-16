/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QStyledItemDelegate>

#include "Swift/QtUI/Roster/GroupItemDelegate.h"

namespace Swift {
	class ChatListMUCItem;
	class ChatListDelegate : public QStyledItemDelegate {
		public:
			ChatListDelegate();
			~ChatListDelegate();
			QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
			void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
		private:
			void paintMUC(QPainter* painter, const QStyleOptionViewItem& option, ChatListMUCItem* item) const;
			QSize mucSizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/ ) const;

			DelegateCommons common_;
			GroupItemDelegate* groupDelegate_;
	};

}

