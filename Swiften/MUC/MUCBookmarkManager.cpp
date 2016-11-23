/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/MUC/MUCBookmarkManager.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Queries/Requests/GetPrivateStorageRequest.h>
#include <Swiften/Queries/Requests/SetPrivateStorageRequest.h>

namespace Swift {

MUCBookmarkManager::MUCBookmarkManager(IQRouter* iqRouter) {
    iqRouter_ = iqRouter;
    ready_ = false;
    GetPrivateStorageRequest<Storage>::ref request = GetPrivateStorageRequest<Storage>::create(iqRouter_);
    request->onResponse.connect(boost::bind(&MUCBookmarkManager::handleBookmarksReceived, this, _1, _2));
    request->send();
}

void MUCBookmarkManager::handleBookmarksReceived(std::shared_ptr<Storage> payload, ErrorPayload::ref error) {
    if (error) {
        return;
    }

    ready_ = true;
    onBookmarksReady();

    storage = payload;

    std::vector<MUCBookmark> receivedBookmarks;
    for (const auto& room : payload->getRooms()) {
        receivedBookmarks.push_back(MUCBookmark(room));
    }

    std::vector<MUCBookmark> newBookmarks;
    for (const auto& oldBookmark : bookmarks_) {
        if (containsEquivalent(receivedBookmarks, oldBookmark)) {
            newBookmarks.push_back(oldBookmark);
        } else {
            onBookmarkRemoved(oldBookmark);
        }
    }

    for (const auto& newBookmark : receivedBookmarks) {
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
    if (!ready_) return;
    for (auto& bookmark : bookmarks_) {
        if (bookmark == oldBookmark) {
            bookmark = newBookmark;
            flush();
            onBookmarkRemoved(oldBookmark);
            onBookmarkAdded(newBookmark);
            return;
        }
    }
}

void MUCBookmarkManager::addBookmark(const MUCBookmark& bookmark) {
    if (!ready_) return;
    bookmarks_.push_back(bookmark);
    onBookmarkAdded(bookmark);
    flush();
}


void MUCBookmarkManager::removeBookmark(const MUCBookmark& bookmark) {
    if (!ready_) return;
    std::vector<MUCBookmark>::iterator it;
    for (it = bookmarks_.begin(); it != bookmarks_.end(); ++it) {
        if ((*it) == bookmark) {
            bookmarks_.erase(it);
            onBookmarkRemoved(bookmark);
            break;
        }
    }
    flush();
}

void MUCBookmarkManager::flush() {
    if (!storage) {
        storage = std::make_shared<Storage>();
    }
    // Update the storage element
    storage->clearRooms();
    for (const auto& bookmark : bookmarks_) {
        storage->addRoom(bookmark.toStorage());
    }

    // Send an iq to save the storage element
    SetPrivateStorageRequest<Storage>::ref request = SetPrivateStorageRequest<Storage>::create(storage, iqRouter_);
    // FIXME: We should care about the result
    //request->onResponse.connect(boost::bind(&MUCBookmarkManager::handleBookmarksSet, this, _1, _2));
    request->send();
}

const std::vector<MUCBookmark>& MUCBookmarkManager::getBookmarks() const {
    return bookmarks_;
}

}
