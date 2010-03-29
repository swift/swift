#pragma once

#include <QList>

#include <boost/shared_ptr.hpp>

#include "Swiften/MUC/MUCBookmark.h"

#include "Swift/QtUI/ChatList/ChatListItem.h"

namespace Swift {
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
