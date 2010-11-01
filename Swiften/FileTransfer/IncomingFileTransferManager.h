/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/boost_bsignals.h"

namespace Swift {
	class IncomingFileTransferManager {
		public:
			boost::signal<void (IncomingFileTransfer::ref)> onIncomingFileTransfer;
	};
}
