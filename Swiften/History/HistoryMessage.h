/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/JID/JID.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace Swift {
	class HistoryMessage {
		public:
			HistoryMessage(const std::string& message, const JID& from, const JID& to, const boost::posix_time::ptime time) : message_(message), from_(from), to_(to), time_(time) {
			}

			const std::string& getMessage() const {
				return message_;
			}

			const JID& getFrom() const {
				return from_;
			}

			const JID& getTo() const {
				return to_;
			}

			boost::posix_time::ptime getTime() const {
				return time_;
			}

			bool operator==(const HistoryMessage& o) const {
				return message_ == o.message_ && from_ == o.from_ && to_ == o.to_ && time_ == o.time_;
			}

		private:
			std::string message_;
			JID from_;
			JID to_;
			boost::posix_time::ptime time_;
	};
}
