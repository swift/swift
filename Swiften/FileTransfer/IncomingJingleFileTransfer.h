/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Jingle/IncomingJingleSession.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>

namespace Swift {
	class IncomingJingleFileTransfer : public IncomingFileTransfer {
		public:
			typedef boost::shared_ptr<IncomingJingleFileTransfer> ref;

			IncomingJingleFileTransfer(IncomingJingleSession::ref session);

			virtual void accept(WriteBytestream::ref);

		private:
			IncomingJingleSession::ref session;
			WriteBytestream::ref stream;
	};
}
