/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/IQ.h>

namespace Swift {
	class IQRouter;

	class SWIFTEN_API IQHandler {
		public:
			virtual ~IQHandler();

			virtual bool handleIQ(boost::shared_ptr<IQ>) = 0;
	};
}
