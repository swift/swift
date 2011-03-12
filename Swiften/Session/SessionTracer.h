/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <iostream>

#include "Swiften/Session/Session.h"
#include <string>
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class SessionTracer {
		public:
			SessionTracer(boost::shared_ptr<Session> session) : session(session) {
				session->onDataRead.connect(boost::bind(&SessionTracer::printData, this, '<', _1));
				session->onDataWritten.connect(boost::bind(&SessionTracer::printData, this, '>', _1));
			}

		private:
			void printData(char direction, const ByteArray& data) {
				std::cerr << direction << direction << " " << session->getLocalJID() << " ";
				for (unsigned int i = 0; i < 72 - session->getLocalJID().toString().size() - session->getRemoteJID().toString().size(); ++i) {
					std::cerr << direction;
				}
				std::cerr << " " << session->getRemoteJID()<< " " << direction << direction << std::endl;
				std::cerr << data.toString() << std::endl;
			}

			boost::shared_ptr<Session> session;
	};
}
