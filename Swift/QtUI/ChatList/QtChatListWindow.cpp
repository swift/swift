/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/ChatList/QtChatListWindow.h"

namespace Swift {

QtChatListWindow::QtChatListWindow(UIEventStream *uiEventStream, QWidget* parent) : QTreeView(parent) {
	eventStream_ = uiEventStream;
	model_ = new ChatListModel();
	setModel(model_);
	delegate_ = new ChatListDelegate();
	setItemDelegate(delegate_);
	setHeaderHidden(true);
	#ifdef SWIFT_PLATFORM_MACOSX
	setAlternatingRowColors(true);
#endif
	setAnimated(true);
	setIndentation(0);
	setRootIsDecorated(true);
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleItemActivated(const QModelIndex&)));
}

QtChatListWindow::~QtChatListWindow() {
	delete model_;
	delete delegate_;
}

void QtChatListWindow::handleItemActivated(const QModelIndex& item) {

}

void QtChatListWindow::addMUCBookmark(boost::shared_ptr<MUCBookmark> bookmark) {
	model_->addMUCBookmark(bookmark);
}

void QtChatListWindow::removeMUCBookmark(boost::shared_ptr<MUCBookmark> bookmark) {
	model_->removeMUCBookmark(bookmark);
}

}
