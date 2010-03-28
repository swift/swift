#pragma once

#include <iostream>

#include "Swiften/Session/Session.h"
#include "Swiften/Base/String.h"
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
				for (unsigned int i = 0; i < 72 - session->getLocalJID().toString().getLength() - session->getRemoteJID().toString().getLength(); ++i) {
					std::cerr << direction;
				}
				std::cerr << " " << session->getRemoteJID()<< " " << direction << direction << std::endl;
				std::cerr << String(data.getData(), data.getSize()) << std::endl;
			}

			boost::shared_ptr<Session> session;
	};
}
