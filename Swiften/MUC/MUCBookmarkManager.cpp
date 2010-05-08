/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "MUCBookmarkManager.h"

#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/Queries/IQRouter.h"


namespace Swift {

MUCBookmarkManager::MUCBookmarkManager(IQRouter* iqRouter) {
	iqRouter_ = iqRouter;
	boost::shared_ptr<GetPrivateStorageRequest<Storage> > request(new GetPrivateStorageRequest<Storage>(iqRouter_));
	request->onResponse.connect(boost::bind(&MUCBookmarkManager::handleBookmarksReceived, this, _1, _2));
	request->send();
}

void MUCBookmarkManager::handleBookmarksReceived(boost::shared_ptr<Storage> payload, const boost::optional<ErrorPayload>& error) {
	if (error) {
		return;
	}
	storage = payload;

	std::vector<MUCBookmark> receivedBookmarks;
	foreach (Storage::Conference conference, payload->getConferences()) {
		String name = (!conference.name.isEmpty()) ? conference.name : conference.jid.getNode();
		MUCBookmark bookmark(conference.jid, name);
		bookmark.setAutojoin(conference.autoJoin);
		if (!conference.nick.isEmpty()) {
			bookmark.setNick(conference.nick);
		}
		if (!conference.password.isEmpty()) {
			bookmark.setPassword(conference.password);
		}
		receivedBookmarks.push_back(bookmark);
	}

	std::vector<MUCBookmark> newBookmarks;
	foreach (const MUCBookmark& oldBookmark, bookmarks_) {
		if (containsEquivalent(receivedBookmarks, oldBookmark)) {
			newBookmarks.push_back(oldBookmark);
		} else {
			onBookmarkRemoved(oldBookmark);
		} 
	}

	foreach (const MUCBookmark& newBookmark, receivedBookmarks) {
		if (!containsEquivalent(bookmarks_, newBookmark)) {
			newBookmarks.push_back(newBookmark);
			onBookmarkAdded(newBookmark);
		}
	}
	bookmarks_ = newBookmarks;
}

bool MUCBookmarkManager::containsEquivalent(const std::vector<MUCBookmark>& list, const MUCBookmark& bookmark) {
	return std::find(list.begin(), list.end(), bookmark) != list.end();
}

void MUCBookmarkManager::replaceBookmark(const MUCBookmark& oldBookmark, const MUCBookmark& newBookmark) {
	for (size_t i = 0; i < bookmarks_.size(); i++) {
		if (bookmarks_[i] == oldBookmark) {
			bookmarks_[i] = newBookmark;
			flush();
			onBookmarkRemoved(oldBookmark);
			onBookmarkAdded(newBookmark);
			return;
		}
	}
}

void MUCBookmarkManager::addBookmark(const MUCBookmark& bookmark) {
	bookmarks_.push_back(bookmark);
	onBookmarkAdded(bookmark);
	flush();
}


void MUCBookmarkManager::removeBookmark(const MUCBookmark& bookmark) {
	std::vector<MUCBookmark>::iterator it;
	for (it = bookmarks_.begin(); it != bookmarks_.end(); it++) {
		if ((*it) == bookmark) {
			bookmarks_.erase(it);
			onBookmarkRemoved(bookmark);
			break;
		}
	}
	flush();
}

void MUCBookmarkManager::flush() {
	//FIXME: some code may be useful
}

const std::vector<MUCBookmark>& MUCBookmarkManager::getBookmarks() const {
	return bookmarks_;
}

}
