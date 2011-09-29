/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/FileTransfer/JingleTransport.h>
#include <Swiften/FileTransfer/IBBReceiveSession.h>

namespace Swift {
	class JingleIncomingIBBTransport : public JingleTransport {
		public:
			typedef boost::shared_ptr<JingleIncomingIBBTransport> ref;

			JingleIncomingIBBTransport(const JID& from, const JID& to, const std::string& id, size_t size, IQRouter* router);

			virtual void start();
			virtual void stop();

		private:
			IBBReceiveSession ibbSession;
	};
}
