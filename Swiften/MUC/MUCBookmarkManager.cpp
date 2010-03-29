#include "MUCBookmarkManager.h"

#include <boost/bind.hpp>

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
	std::vector<boost::shared_ptr<MUCBookmark> > newBookmarks;
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
		newBookmarks.push_back(bookmark);
	}

	//FIXME: This needs to be fixed before we start doing anything supporting updates
	foreach (boost::shared_ptr<MUCBookmark> oldBookmark, bookmarks_) {
		onBookmarkRemoved(oldBookmark);
	}

	foreach (boost::shared_ptr<MUCBookmark> newBookmark, newBookmarks) {
		onBookmarkAdded(newBookmark);
	}


}

void MUCBookmarkManager::addBookmark(boost::shared_ptr<MUCBookmark> bookmark) {
	bookmarks_.push_back(bookmark);
	flush();
	onBookmarkAdded(bookmark);
}


void MUCBookmarkManager::removeBookmark(boost::shared_ptr<MUCBookmark> bookmark) {
	std::vector<boost::shared_ptr<MUCBookmark> >::iterator it;
	for (it = bookmarks_.begin(); it != bookmarks_.end(); it++) {
		if ((*it).get() == bookmark.get()) {
			bookmarks_.erase(it);
			onBookmarkRemoved(bookmark);
			return;
		}
	}
	assert(false);
	flush();
}

void MUCBookmarkManager::flush() {
	//FIXME: some code may be useful
}

const std::vector<boost::shared_ptr<MUCBookmark> >& MUCBookmarkManager::getBookmarks() {
	return bookmarks_;
}

}
