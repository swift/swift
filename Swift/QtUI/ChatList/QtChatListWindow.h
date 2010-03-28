#pragma once

#include <QTreeView>

#include "Swift/Controllers/UIInterfaces/ChatListWindow.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/QtUI/ChatList/ChatListModel.h"
#include "Swift/QtUI/ChatList/ChatListDelegate.h"

namespace Swift {

	class QtChatListWindow : public QTreeView, public ChatListWindow {
		Q_OBJECT
		public:
			QtChatListWindow(UIEventStream *uiEventStream, QWidget* parent = NULL);
			virtual ~QtChatListWindow();
			void addMUCBookmark(boost::shared_ptr<MUCBookmark> bookmark);
			void removeMUCBookmark(boost::shared_ptr<MUCBookmark> bookmark);
		private slots:
			void handleItemActivated(const QModelIndex&);
		private:
			UIEventStream* eventStream_;
			ChatListModel* model_;
			ChatListDelegate* delegate_;
	};

}
