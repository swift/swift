#include "MUCBookmarkManager.h"

#include "Swiften/Queries/IQRouter.h"

namespace Swift {

MUCBookmarkManager::MUCBookmarkManager(IQRouter* iqRouter) {
	iqRouter_ = iqRouter;
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
