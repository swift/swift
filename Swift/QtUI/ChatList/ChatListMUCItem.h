/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QList>

#include <boost/shared_ptr.hpp>

#include "Swiften/MUC/MUCBookmark.h"

#include "Swift/QtUI/ChatList/ChatListItem.h"

namespace Swift {
	enum MUCItemRoles {
		DetailTextRole = Qt::UserRole/*,
		AvatarRole = Qt::UserRole + 1,
		PresenceIconRole = Qt::UserRole + 2,
		StatusShowTypeRole = Qt::UserRole + 3*/
	};
	class ChatListMUCItem : public ChatListItem {
		public:
			ChatListMUCItem(boost::shared_ptr<MUCBookmark> bookmark, ChatListGroupItem* parent);
			boost::shared_ptr<MUCBookmark> getBookmark();
			QVariant data(int role);
		private:
			boost::shared_ptr<MUCBookmark> bookmark_;
			QList<ChatListItem*> items_;
	};
}
