/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/ChatList/QtChatListWindow.h"

#include <QMenu>
#include <QContextMenuEvent>

#include "Swift/QtUI/ChatList/ChatListMUCItem.h"
#include "Swift/QtUI/QtAddBookmarkWindow.h"
#include "Swift/QtUI/QtEditBookmarkWindow.h"
#include "Swift/Controllers/UIEvents/JoinMUCUIEvent.h"
#include "Swift/Controllers/UIEvents/AddMUCBookmarkUIEvent.h"
#include "Swift/Controllers/UIEvents/RemoveMUCBookmarkUIEvent.h"
#include "Swift/Controllers/UIEvents/EditMUCBookmarkUIEvent.h"

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
	expandAll();
	setAnimated(true);
	setIndentation(0);
	setRootIsDecorated(true);
	setupContextMenus();
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleItemActivated(const QModelIndex&)));
}

QtChatListWindow::~QtChatListWindow() {
	delete model_;
	delete delegate_;
	delete mucMenu_;
	delete emptyMenu_;
}

void QtChatListWindow::setupContextMenus() {
	mucMenu_ = new QMenu();
	mucMenu_->addAction("Add New Bookmark", this, SLOT(handleAddBookmark()));
	mucMenu_->addAction("Edit Bookmark", this, SLOT(handleEditBookmark()));
	mucMenu_->addAction("Remove Bookmark", this, SLOT(handleRemoveBookmark()));
	emptyMenu_ = new QMenu();
	emptyMenu_->addAction("Add New Bookmark", this, SLOT(handleAddBookmark()));
	
}

void QtChatListWindow::handleItemActivated(const QModelIndex& index) {
	ChatListItem* item = model_->getItemForIndex(index);
	ChatListMUCItem* mucItem = dynamic_cast<ChatListMUCItem*>(item);
	if (mucItem) {
		boost::shared_ptr<UIEvent> event(new JoinMUCUIEvent(mucItem->getBookmark()->getRoom(), mucItem->getBookmark()->getNick()));
		eventStream_->send(event);
	}
}

void QtChatListWindow::addMUCBookmark(boost::shared_ptr<MUCBookmark> bookmark) {
	model_->addMUCBookmark(bookmark);
}

void QtChatListWindow::removeMUCBookmark(boost::shared_ptr<MUCBookmark> bookmark) {
	model_->removeMUCBookmark(bookmark);
}

void QtChatListWindow::handleRemoveBookmark() {
	ChatListMUCItem* mucItem = dynamic_cast<ChatListMUCItem*>(contextMenuItem_);
	if (!mucItem) return;
	eventStream_->send(boost::shared_ptr<UIEvent>(new RemoveMUCBookmarkUIEvent(mucItem->getBookmark())));
}

void QtChatListWindow::handleAddBookmark() {
	(new QtAddBookmarkWindow(eventStream_))->show();
}


void QtChatListWindow::handleEditBookmark() {
	ChatListMUCItem* mucItem = dynamic_cast<ChatListMUCItem*>(contextMenuItem_);
	if (!mucItem) return;
	QtEditBookmarkWindow* window = new QtEditBookmarkWindow(eventStream_, mucItem->getBookmark());
	window->show();
}


void QtChatListWindow::contextMenuEvent(QContextMenuEvent* event) {
	QModelIndex index = indexAt(event->pos());
	ChatListItem* baseItem = index.isValid() ? static_cast<ChatListItem*>(index.internalPointer()) : NULL;
	contextMenuItem_ = baseItem;
	if (!baseItem) {
		emptyMenu_->exec(QCursor::pos());
		return;
	}
	ChatListMUCItem* mucItem = dynamic_cast<ChatListMUCItem*>(baseItem);
	if (mucItem) {
		mucMenu_->exec(QCursor::pos());
	}
}

}
