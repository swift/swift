#pragma once

#include <QList>

#include <boost/shared_ptr.hpp>

#include "Swiften/MUC/MUCBookmark.h"

#include "Swift/QtUI/ChatList/ChatListItem.h"

namespace Swift {
	class ChatListMUCItem : public ChatListItem {
		public:
			ChatListMUCItem(boost::shared_ptr<MUCBookmark> bookmark, ChatListGroupItem* parent) : ChatListItem(parent), bookmark_(bookmark) {};
			boost::shared_ptr<MUCBookmark> getBookmark() {return bookmark_;};
			QVariant data(int role) { return QVariant(); };
		private:
			boost::shared_ptr<MUCBookmark> bookmark_;
			QList<ChatListItem*> items_;
	};
}
