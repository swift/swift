#pragma once

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/signals.hpp>

#include "Swiften/MUC/MUCBookmark.h"

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
			boost::signal<void ()> onBookmarksChanged;
		private:
			
			std::vector<boost::shared_ptr<MUCBookmark> > bookmarks_;
			IQRouter* iqRouter_;
	};
}
