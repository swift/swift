#pragma once

#include <QVariant>

namespace Swift {
	class ChatListGroupItem;
	class ChatListItem {
		public:
			ChatListItem(ChatListGroupItem* parent) {parent_ = parent;};
			ChatListGroupItem* parent() {return parent_;};
			virtual QVariant data(int role) = 0;
		private:
			ChatListGroupItem* parent_;
	};
}
