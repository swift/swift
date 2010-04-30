/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/signals.hpp>
#include <boost/optional.hpp>

#include "Swiften/MUC/MUCBookmark.h"
#include "Swiften/Elements/Storage.h"
#include "Swiften/Queries/Requests/GetPrivateStorageRequest.h"

namespace Swift {
	class IQRouter;
	class MUCBookmarkManager {
		public:
			MUCBookmarkManager(IQRouter* iqRouter);
			void addBookmark(boost::shared_ptr<MUCBookmark> bookmark);
			void removeBookmark(boost::shared_ptr<MUCBookmark> bookmark);
			void replaceBookmark(boost::shared_ptr<MUCBookmark> oldBookmark, boost::shared_ptr<MUCBookmark> newBookmark);
			const std::vector<boost::shared_ptr<MUCBookmark> >& getBookmarks(); 
			boost::signal<void (boost::shared_ptr<MUCBookmark>)> onBookmarkAdded;
			boost::signal<void (boost::shared_ptr<MUCBookmark>)> onBookmarkRemoved;
		private:
			bool containsEquivalent(std::vector<boost::shared_ptr<MUCBookmark> > list, boost::shared_ptr<MUCBookmark> bookmark);
			void handleBookmarksReceived(boost::shared_ptr<Storage> payload, const boost::optional<ErrorPayload>& error);
			void flush();
			std::vector<boost::shared_ptr<MUCBookmark> > bookmarks_;
			IQRouter* iqRouter_;
	};
}
