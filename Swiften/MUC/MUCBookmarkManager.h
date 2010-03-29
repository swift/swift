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
			/** Call flush after editing an existing bookmark. */
			void flush();
			/** Returns pointers to the bookmarks. These can be edited, and then flush()ed.*/
			const std::vector<boost::shared_ptr<MUCBookmark> >& getBookmarks(); 
			boost::signal<void (boost::shared_ptr<MUCBookmark>)> onBookmarkAdded;
			boost::signal<void (boost::shared_ptr<MUCBookmark>)> onBookmarkRemoved;
		private:
			void handleBookmarksReceived(boost::shared_ptr<Storage> payload, const boost::optional<ErrorPayload>& error);
			std::vector<boost::shared_ptr<MUCBookmark> > bookmarks_;
			IQRouter* iqRouter_;
	};
}
