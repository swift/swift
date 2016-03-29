/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>

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
