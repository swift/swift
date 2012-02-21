/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/ChatList/QtChatListWindow.h"

#include <boost/bind.hpp>

#include <QMenu>
#include <QContextMenuEvent>

#include <Swift/QtUI/ChatList/ChatListMUCItem.h>
#include <Swift/QtUI/ChatList/ChatListRecentItem.h>
#include <Swift/QtUI/QtAddBookmarkWindow.h>
#include <Swift/QtUI/QtEditBookmarkWindow.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/AddMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIEvents/RemoveMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIEvents/EditMUCBookmarkUIEvent.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>


namespace Swift {

QtChatListWindow::QtChatListWindow(UIEventStream *uiEventStream, SettingsProvider* settings, QWidget* parent) : QTreeView(parent) {
	eventStream_ = uiEventStream;
	settings_ = settings;;
	bookmarksEnabled_ = false;
	model_ = new ChatListModel();
	setModel(model_);
	delegate_ = new ChatListDelegate(settings_->getSetting(QtUISettingConstants::COMPACT_ROSTER));
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

	settings_->onSettingChanged.connect(boost::bind(&QtChatListWindow::handleSettingChanged, this, _1));
}

QtChatListWindow::~QtChatListWindow() {
	settings_->onSettingChanged.disconnect(boost::bind(&QtChatListWindow::handleSettingChanged, this, _1));
	delete model_;
	delete delegate_;
	delete mucMenu_;
	delete emptyMenu_;
}

void QtChatListWindow::handleSettingChanged(const std::string& setting) {
	if (setting == QtUISettingConstants::COMPACT_ROSTER.getKey()) {
		delegate_->setCompact(settings_->getSetting(QtUISettingConstants::COMPACT_ROSTER));
		repaint();
	}
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
	ChatListItem* item = model_->getItemForIndex(index);
	if (ChatListMUCItem* mucItem = dynamic_cast<ChatListMUCItem*>(item)) {
		if (bookmarksEnabled_) {
			onMUCBookmarkActivated(mucItem->getBookmark());
		}
	}
	else if (ChatListRecentItem* recentItem = dynamic_cast<ChatListRecentItem*>(item)) {
		if (!recentItem->getChat().isMUC || bookmarksEnabled_) {
			onRecentActivated(recentItem->getChat());
		}
	}
}

void QtChatListWindow::clearBookmarks() {
	model_->clearBookmarks();
}

void QtChatListWindow::addMUCBookmark(const MUCBookmark& bookmark) {
	model_->addMUCBookmark(bookmark);
}

void QtChatListWindow::removeMUCBookmark(const MUCBookmark& bookmark) {
	model_->removeMUCBookmark(bookmark);
}

void QtChatListWindow::setRecents(const std::list<ChatListWindow::Chat>& recents) {
	model_->setRecents(recents);
}

void QtChatListWindow::setUnreadCount(int unread) {
	emit onCountUpdated(unread);
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
		if (!bookmarksEnabled_) {
			return;
		}
		mucMenu_->exec(QCursor::pos());
	}
	else {
		QMenu menu;
		QAction* clearRecents = menu.addAction(tr("Clear recents"));
		menu.addAction(clearRecents);
		QAction* result = menu.exec(event->globalPos());
		if (result == clearRecents) {
			onClearRecentsRequested();
		}
	}
}

}
