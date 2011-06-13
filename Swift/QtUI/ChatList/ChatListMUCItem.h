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
	class ChatListMUCItem : public ChatListItem {
		public:
			enum MUCItemRoles {
				DetailTextRole = Qt::UserRole/*,
				AvatarRole = Qt::UserRole + 1,
				PresenceIconRole = Qt::UserRole + 2,
				StatusShowTypeRole = Qt::UserRole + 3*/
			};
			ChatListMUCItem(const MUCBookmark& bookmark, ChatListGroupItem* parent);
			const MUCBookmark& getBookmark() const;
			QVariant data(int role) const;
		private:
			MUCBookmark bookmark_;
			QList<ChatListItem*> items_;
	};
}
