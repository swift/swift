/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Swiften/Elements/Payload.h>

namespace Swift {

	class Idle : public Payload {
	public:
		typedef boost::shared_ptr<Idle> ref;

	public:
		Idle() {}
		Idle(boost::posix_time::ptime since) : since_(since) {
		}

		void setSince(boost::posix_time::ptime since) {
			since_ = since;
		}

		boost::posix_time::ptime getSince() const {
			return since_;
		}

	private:
		boost::posix_time::ptime since_;
	};

}
