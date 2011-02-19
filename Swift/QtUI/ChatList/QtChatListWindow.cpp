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
	bookmarksEnabled_ = false;
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
	connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleClicked(const QModelIndex&)));
}

QtChatListWindow::~QtChatListWindow() {
	delete model_;
	delete delegate_;
	delete mucMenu_;
	delete emptyMenu_;
}

void QtChatListWindow::setBookmarksEnabled(bool enabled) {
	bookmarksEnabled_ = enabled;
}

void QtChatListWindow::handleClicked(const QModelIndex& index) {
	ChatListGroupItem* item = dynamic_cast<ChatListGroupItem*>(static_cast<ChatListItem*>(index.internalPointer()));
	if (item) {
		setExpanded(index, !isExpanded(index));
	}
}

void QtChatListWindow::setupContextMenus() {
	mucMenu_ = new QMenu();
	mucMenu_->addAction(tr("Add New Bookmark"), this, SLOT(handleAddBookmark()));
	mucMenu_->addAction(tr("Edit Bookmark"), this, SLOT(handleEditBookmark()));
	mucMenu_->addAction(tr("Remove Bookmark"), this, SLOT(handleRemoveBookmark()));
	emptyMenu_ = new QMenu();
	emptyMenu_->addAction(tr("Add New Bookmark"), this, SLOT(handleAddBookmark()));
	
}

void QtChatListWindow::handleItemActivated(const QModelIndex& index) {
	if (!bookmarksEnabled_) {
		return;
	}
	ChatListItem* item = model_->getItemForIndex(index);
	ChatListMUCItem* mucItem = dynamic_cast<ChatListMUCItem*>(item);
	if (mucItem) {
		boost::shared_ptr<UIEvent> event(new JoinMUCUIEvent(mucItem->getBookmark().getRoom(), mucItem->getBookmark().getNick()));
		eventStream_->send(event);
	}
}

void QtChatListWindow::clear() {
	model_->clear();
}

void QtChatListWindow::addMUCBookmark(const MUCBookmark& bookmark) {
	model_->addMUCBookmark(bookmark);
}

void QtChatListWindow::removeMUCBookmark(const MUCBookmark& bookmark) {
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
	if (!bookmarksEnabled_) {
		return;
	}
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
