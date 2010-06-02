/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "Swiften/Base/boost_bsignals.h"

#include "Swiften/Network/Connection.h"

namespace Swift {
	class ConnectionServer {
		public:
			virtual ~ConnectionServer();

			boost::signal<void (boost::shared_ptr<Connection>)> onNewConnection;
	};
}
