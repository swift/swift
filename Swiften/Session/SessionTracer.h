/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Session/Session.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class SessionTracer {
		public:
			SessionTracer(boost::shared_ptr<Session> session);

		private:
			void printData(char direction, const SafeByteArray& data);

			boost::shared_ptr<Session> session;
	};
}
