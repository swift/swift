/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
	class JID;
	class FileTransferTransporter;

	class SWIFTEN_API FileTransferTransporterFactory {
		public:
			virtual ~FileTransferTransporterFactory();

			virtual FileTransferTransporter* createInitiatorTransporter(
					const JID& initiator, 
					const JID& responder) = 0;
			virtual FileTransferTransporter* createResponderTransporter(
					const JID& initiator, 
					const JID& responder, 
					const std::string& s5bSessionID) = 0;
	};
}
