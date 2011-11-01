/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QStyledItemDelegate>

#include "Swift/QtUI/Roster/GroupItemDelegate.h"

namespace Swift {
	class ChatListMUCItem;
	class ChatListRecentItem;
	class ChatListDelegate : public QStyledItemDelegate {
		public:
			ChatListDelegate(bool compact);
			~ChatListDelegate();
			QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
			void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
		public slots:
			void setCompact(bool compact);
		private:
			void paintMUC(QPainter* painter, const QStyleOptionViewItem& option, ChatListMUCItem* item) const;
			void paintRecent(QPainter* painter, const QStyleOptionViewItem& option, ChatListRecentItem* item) const;
			QSize mucSizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/ ) const;
			QSize recentSizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/ ) const;

			bool compact_;
			DelegateCommons common_;
			GroupItemDelegate* groupDelegate_;
	};

}

