/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>

#include <string>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/IQ.h>

namespace Swift {
	class SWIFTEN_API IQChannel {
		public:
			virtual ~IQChannel();

			virtual void sendIQ(boost::shared_ptr<IQ>) = 0;
			virtual std::string getNewIQID() = 0;
			
			virtual bool isAvailable() const = 0;

			boost::signal<void (boost::shared_ptr<IQ>)> onIQReceived;
	};
}
