/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/ChatList/QtChatListWindow.h>

#include <boost/bind.hpp>

#include <QContextMenuEvent>
#include <QMenu>
#include <QMimeData>
#include <QUrl>

#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/Controllers/UIEvents/AddMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIEvents/EditMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RemoveMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/ShowWhiteboardUIEvent.h>

#include <Swift/QtUI/ChatList/ChatListMUCItem.h>
#include <Swift/QtUI/ChatList/ChatListRecentItem.h>
#include <Swift/QtUI/ChatList/ChatListWhiteboardItem.h>
#include <Swift/QtUI/QtAddBookmarkWindow.h>
#include <Swift/QtUI/QtEditBookmarkWindow.h>
#include <Swift/QtUI/QtUISettingConstants.h>

namespace Swift {

QtChatListWindow::QtChatListWindow(UIEventStream *uiEventStream, SettingsProvider* settings, QWidget* parent) : QTreeView(parent), isOnline_(false) {
	eventStream_ = uiEventStream;
	settings_ = settings;
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
	setDragEnabled(true);
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

void QtChatListWindow::handleClearRecentsRequested() {
	onClearRecentsRequested();
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
	onlineOnlyActions_ << mucMenu_->addAction(tr("Add New Bookmark"), this, SLOT(handleAddBookmark()));
	onlineOnlyActions_ << mucMenu_->addAction(tr("Edit Bookmark"), this, SLOT(handleEditBookmark()));
	onlineOnlyActions_ << mucMenu_->addAction(tr("Remove Bookmark"), this, SLOT(handleRemoveBookmark()));
	emptyMenu_ = new QMenu();
	onlineOnlyActions_ << emptyMenu_->addAction(tr("Add New Bookmark"), this, SLOT(handleAddBookmark()));
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
	else if (ChatListWhiteboardItem* whiteboardItem = dynamic_cast<ChatListWhiteboardItem*>(item)) {
		if (!whiteboardItem->getChat().isMUC || bookmarksEnabled_) {
			eventStream_->send(boost::make_shared<ShowWhiteboardUIEvent>(whiteboardItem->getChat().jid));
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

void QtChatListWindow::addWhiteboardSession(const ChatListWindow::Chat& chat) {
	model_->addWhiteboardSession(chat);
}

void QtChatListWindow::removeWhiteboardSession(const JID& jid) {
	model_->removeWhiteboardSession(jid);
}

void QtChatListWindow::setRecents(const std::list<ChatListWindow::Chat>& recents) {
	model_->setRecents(recents);
}

void QtChatListWindow::setUnreadCount(int unread) {
	emit onCountUpdated(unread);
}

void QtChatListWindow::setOnline(bool isOnline) {
	isOnline_ = isOnline;
}

void QtChatListWindow::handleRemoveBookmark() {
	const ChatListMUCItem* mucItem = dynamic_cast<const ChatListMUCItem*>(contextMenuItem_);
	if (!mucItem) return;
	eventStream_->send(boost::shared_ptr<UIEvent>(new RemoveMUCBookmarkUIEvent(mucItem->getBookmark())));
}

void QtChatListWindow::handleAddBookmarkFromRecents() {
	const ChatListRecentItem* item = dynamic_cast<const ChatListRecentItem*>(contextMenuItem_);
	if (item) {
		const ChatListWindow::Chat& chat = item->getChat();
		MUCBookmark bookmark(chat.jid, chat.jid.toBare().toString());
		bookmark.setNick(chat.nick);
		bookmark.setPassword(chat.password);
		eventStream_->send(boost::shared_ptr<UIEvent>(new AddMUCBookmarkUIEvent(bookmark)));
	}
}

void QtChatListWindow::handleAddBookmark() {
	(new QtAddBookmarkWindow(eventStream_))->show();
}


void QtChatListWindow::handleEditBookmark() {
	const ChatListMUCItem* mucItem = dynamic_cast<const ChatListMUCItem*>(contextMenuItem_);
	if (!mucItem) return;
	QtEditBookmarkWindow* window = new QtEditBookmarkWindow(eventStream_, mucItem->getBookmark());
	window->show();
}

void QtChatListWindow::dragEnterEvent(QDragEnterEvent *event) {
	if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1) {
		event->acceptProposedAction();
	}
}

void QtChatListWindow::contextMenuEvent(QContextMenuEvent* event) {
	QModelIndex index = indexAt(event->pos());
	ChatListItem* baseItem = index.isValid() ? static_cast<ChatListItem*>(index.internalPointer()) : NULL;
	contextMenuItem_ = baseItem;

	foreach(QAction* action, onlineOnlyActions_) {
		action->setEnabled(isOnline_);
	}

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
		return;
	}

	ChatListRecentItem* recentItem = dynamic_cast<ChatListRecentItem*>(baseItem);
	if (recentItem) {
		const ChatListWindow::Chat& chat = recentItem->getChat();
		if (chat.isMUC) {
			QMenu mucRecentsMenu;
			QAction* bookmarkAction = NULL;
			const ChatListMUCItem* mucItem = model_->getChatListMUCItem(chat.jid);
			if (mucItem) {
				contextMenuItem_ = mucItem;
				bookmarkAction = mucRecentsMenu.addAction(tr("Edit Bookmark"), this, SLOT(handleEditBookmark()));
			}
			else {
				bookmarkAction = mucRecentsMenu.addAction(tr("Add to Bookmarks"), this, SLOT(handleAddBookmarkFromRecents()));
			}
			bookmarkAction->setEnabled(isOnline_);
			mucRecentsMenu.addAction(tr("Clear recents"), this, SLOT(handleClearRecentsRequested()));
			mucRecentsMenu.exec(QCursor::pos());
			return;
		}
	}

	QMenu menu;
	menu.addAction(tr("Clear recents"), this, SLOT(handleClearRecentsRequested()));
	menu.exec(event->globalPos());
}

}
