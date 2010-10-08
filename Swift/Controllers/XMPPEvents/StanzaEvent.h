/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Swiften/Base/boost_bsignals.h"

namespace Swift {
	class StanzaEvent {
		public:
			StanzaEvent() : time_(boost::posix_time::microsec_clock::universal_time()) {concluded_ = false;};
			virtual ~StanzaEvent() {};
			void conclude() {concluded_ = true; onConclusion();};
			/** Do not call this directly from outside the class.
			 * If you connect to this signal, you *must* disconnect from it manually. */
			boost::signal<void()> onConclusion;
			bool getConcluded() {return concluded_;};
			boost::posix_time::ptime getTime() {return time_;}
		private:
			bool concluded_;
			boost::posix_time::ptime time_;
	};
}
