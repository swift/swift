/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
	class SWIFTEN_API CarbonsReceived : public Payload {
		public:
			typedef boost::shared_ptr<CarbonsReceived> ref;

		public:
			virtual ~CarbonsReceived();
			void setForwarded(boost::shared_ptr<Forwarded> forwarded);
			boost::shared_ptr<Forwarded> getForwarded() const;

		private:
			boost::shared_ptr<Forwarded> forwarded_;
	};
}
