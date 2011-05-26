/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/FileTransfer/FileTransfer.h>

namespace Swift {
	class OutgoingFileTransfer : public FileTransfer {
		public:
			typedef boost::shared_ptr<OutgoingFileTransfer> ref;
		public:
			virtual ~OutgoingFileTransfer();

			virtual void start() = 0;
			virtual void stop() = 0;
	};
}
