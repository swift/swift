/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/MUC/MUCBookmarkManager.h>

#include <algorithm>
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
    if (error || !payload) {
        return;
    }

    ready_ = true;
    handlingReceivedBookmarks_ = true;
    onBookmarksReady();

    storage_ = payload;

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
    std::vector<MUCBookmark> newAddedBookmarksToBeSignaled;
    for (const auto& newBookmark : receivedBookmarks) {
        if (!containsEquivalent(bookmarks_, newBookmark)) {
            newBookmarks.push_back(newBookmark);
            //If the bookmark does not exist in bookmark manager, after emmiting the signal, chatsmanager will try to join the room, if the bookmark has autojoin to true.
            //The bookmark is not yet available in bookmark manager, therefore a new bookmark will be created which will be lost when newBookmarks replace bookmarks.
            newAddedBookmarksToBeSignaled.push_back(newBookmark);
        }
    }
    bookmarks_ = newBookmarks;
    for (auto bookmark : newAddedBookmarksToBeSignaled) {
        onBookmarkAdded(bookmark);
   }

    handlingReceivedBookmarks_ = false;
}

bool MUCBookmarkManager::containsEquivalent(const std::vector<MUCBookmark>& list, const MUCBookmark& bookmark) {
    return std::find_if(list.begin(), list.end(), [&](const MUCBookmark& val) { return bookmark.getRoom() == val.getRoom(); }) != list.end();
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
    if (auto found = lookupBookmark(bookmark.getRoom())) {
        if (found != bookmark) {
            replaceBookmark(found.get(), bookmark);
        }
    }
    else {
        bookmarks_.push_back(bookmark);
        onBookmarkAdded(bookmark);
    }
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
    if (handlingReceivedBookmarks_) {
        return;
    }
    if (!storage_) {
        storage_ = std::make_shared<Storage>();
    }
    // Update the storage element
    storage_->clearRooms();
    for (const auto& bookmark : bookmarks_) {
        storage_->addRoom(bookmark.toStorage());
    }

    // Send an iq to save the storage element
    SetPrivateStorageRequest<Storage>::ref request = SetPrivateStorageRequest<Storage>::create(storage_, iqRouter_);
    // FIXME: We should care about the result
    //request->onResponse.connect(boost::bind(&MUCBookmarkManager::handleBookmarksSet, this, _1, _2));
    request->send();
}

const std::vector<MUCBookmark>& MUCBookmarkManager::getBookmarks() const {
    return bookmarks_;
}

boost::optional<MUCBookmark> MUCBookmarkManager::lookupBookmark(const JID& bookmarkJID) const {
    auto bookmarkIterator = std::find_if(bookmarks_.begin(), bookmarks_.end(), [&](const MUCBookmark& val) { return bookmarkJID == val.getRoom(); });
    if (bookmarkIterator != bookmarks_.end()) {
        return *bookmarkIterator;
    }
    return boost::none;
}

}
