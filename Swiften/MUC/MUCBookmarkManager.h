/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/Storage.h>
#include <Swiften/MUC/MUCBookmark.h>

namespace Swift {
    class IQRouter;

    class SWIFTEN_API MUCBookmarkManager {
        public:
            MUCBookmarkManager(IQRouter* iqRouter);

            void addBookmark(const MUCBookmark& bookmark);
            void removeBookmark(const MUCBookmark& bookmark);
            void replaceBookmark(const MUCBookmark& oldBookmark, const MUCBookmark& newBookmark);
            const std::vector<MUCBookmark>& getBookmarks() const;
            boost::optional<MUCBookmark> lookupBookmark(const JID& bookmarkJID) const;

        public:
            boost::signals2::signal<void (const MUCBookmark&)> onBookmarkAdded;
            boost::signals2::signal<void (const MUCBookmark&)> onBookmarkRemoved;
            /**
             * When server bookmarks are ready to be used (request response has been received).
             */
            boost::signals2::signal<void ()> onBookmarksReady;

        private:
            bool containsEquivalent(const std::vector<MUCBookmark>& list, const MUCBookmark& bookmark);
            void handleBookmarksReceived(std::shared_ptr<Storage> payload, ErrorPayload::ref error);
            void flush();

        private:
            bool ready_;
            bool handlingReceivedBookmarks_;
            std::vector<MUCBookmark> bookmarks_;
            IQRouter* iqRouter_;
            std::shared_ptr<Storage> storage_;
    };
}
