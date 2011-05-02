/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/IQ.h>

namespace Swift {
	class IQRouter;

	class IQHandler {
		public:
			virtual ~IQHandler();

			virtual bool handleIQ(boost::shared_ptr<IQ>) = 0;
	};
}
