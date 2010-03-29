#pragma once

#include <QList>

#include "Swift/QtUI/ChatList/ChatListItem.h"

namespace Swift {
	class ChatListGroupItem : public ChatListItem {
		public:
			ChatListGroupItem(const QString& name, ChatListGroupItem* parent) : ChatListItem(parent), name_(name) {};
			void addItem(ChatListItem* item) {items_.push_back(item);};
			void remove(int index) {items_.removeAt(index);};
			int rowCount() {return items_.size();};
			ChatListItem* item(int i) {return items_[i];};
			int row(ChatListItem* item) {return items_.indexOf(item);};
			QVariant data(int role) {return (role == Qt::DisplayRole) ? name_ : QVariant();};
		private:
			QString name_;
			QList<ChatListItem*> items_;
	};
}
