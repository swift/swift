/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/JID/JID.h>
#include <Swiften/FileTransfer/FileTransfer.h>
#include <Swiften/FileTransfer/WriteBytestream.h>

namespace Swift {
	class IncomingFileTransfer : public FileTransfer {
		public:
			typedef boost::shared_ptr<IncomingFileTransfer> ref;

			virtual ~IncomingFileTransfer();
			
			virtual void accept(WriteBytestream::ref) = 0;

			virtual const JID& getSender() const = 0;
			virtual const JID& getRecipient() const = 0;
	};
}
