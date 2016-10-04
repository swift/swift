/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

#include <boost/shared_ptr.hpp>
#include <boost/optional/optional.hpp>

namespace Swift {
	class SWIFTEN_API ITunesInterface {
		public:
			struct Track {
				std::string name;
				std::string artist;
				std::string album;
				long trackNumber;
				double duration;
				long rating;
			};

			ITunesInterface();
			virtual ~ITunesInterface();

			boost::optional<Track> getCurrentTrack() const;

		private:
			bool haveApplication() const;

		private:
			struct Private;
			boost::shared_ptr<Private> p;
	};
}
