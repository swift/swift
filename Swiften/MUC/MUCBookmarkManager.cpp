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
	std::vector<boost::shared_ptr<MUCBookmark> > receivedBookmarks;
	foreach (Storage::Conference conference, payload->getConferences()) {
		String name = (!conference.name.isEmpty()) ? conference.name : conference.jid.getNode();
		boost::shared_ptr<MUCBookmark> bookmark(new MUCBookmark(conference.jid, name));
		bookmark->setAutojoin(conference.autoJoin);
		if (!conference.nick.isEmpty()) {
			bookmark->setNick(conference.nick);
		}
		if (!conference.password.isEmpty()) {
			bookmark->setPassword(conference.password);
		}
		receivedBookmarks.push_back(bookmark);
	}

	std::vector<boost::shared_ptr<MUCBookmark> > newBookmarks;
	foreach (boost::shared_ptr<MUCBookmark> oldBookmark, bookmarks_) {
		if (containsEquivalent(receivedBookmarks, oldBookmark)) {
			newBookmarks.push_back(oldBookmark);
		} else {
			onBookmarkRemoved(oldBookmark);
		} 
	}

	foreach (boost::shared_ptr<MUCBookmark> newBookmark, receivedBookmarks) {
		if (!containsEquivalent(bookmarks_, newBookmark)) {
			newBookmarks.push_back(newBookmark);
			onBookmarkAdded(newBookmark);
		}
	}
	bookmarks_ = newBookmarks;
}

bool MUCBookmarkManager::containsEquivalent(std::vector<boost::shared_ptr<MUCBookmark> > list, boost::shared_ptr<MUCBookmark> bookmark) {
	foreach (boost::shared_ptr<MUCBookmark> listBookmark, list) {
		if (*listBookmark == *bookmark) {
			return true;
		}
	}
	return false;
}

void MUCBookmarkManager::replaceBookmark(boost::shared_ptr<MUCBookmark> oldBookmark, boost::shared_ptr<MUCBookmark> newBookmark) {
	for (size_t i = 0; i < bookmarks_.size(); i++) {
		boost::shared_ptr<MUCBookmark> bookmark(bookmarks_[i]);
		if (bookmark.get() == oldBookmark.get()) {
			bookmarks_[i] = newBookmark;
			flush();
			onBookmarkRemoved(oldBookmark);
			onBookmarkAdded(newBookmark);
			return;
		}
	}
}

void MUCBookmarkManager::addBookmark(boost::shared_ptr<MUCBookmark> bookmark) {
	std::cout << "adding bookmark with pointer " << bookmark.get() << std::endl;
	bookmarks_.push_back(bookmark);
	onBookmarkAdded(bookmark);
	flush();
}


void MUCBookmarkManager::removeBookmark(boost::shared_ptr<MUCBookmark> bookmark) {
	std::vector<boost::shared_ptr<MUCBookmark> >::iterator it;
	for (it = bookmarks_.begin(); it != bookmarks_.end(); it++) {
		if ((*it).get() == bookmark.get()) {
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

const std::vector<boost::shared_ptr<MUCBookmark> >& MUCBookmarkManager::getBookmarks() {
	return bookmarks_;
}

}
