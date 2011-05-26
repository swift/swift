/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/FileTransfer/FileTransfer.h>

namespace Swift {
	class JingleTransport {
		public:
			typedef boost::shared_ptr<JingleTransport> ref;

			virtual ~JingleTransport();

			virtual void start() = 0;
			virtual void stop() = 0;

			boost::signal<void (const std::vector<unsigned char>&)> onDataReceived;
			boost::signal<void (boost::optional<FileTransferError>)> onFinished;
	};
}
