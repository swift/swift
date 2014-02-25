/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
	class Delay;
	class Stanza;

	class SWIFTEN_API Forwarded : public Payload {
		public:
			virtual ~Forwarded();

			void setDelay(boost::shared_ptr<Delay> delay) { delay_ = delay; }
			const boost::shared_ptr<Delay>& getDelay() const { return delay_; }

			void setStanza(boost::shared_ptr<Stanza> stanza) { stanza_ = stanza; }
			const boost::shared_ptr<Stanza>& getStanza() const { return stanza_; }

		private:
			boost::shared_ptr<Delay> delay_;
			boost::shared_ptr<Stanza> stanza_;
	};
}
