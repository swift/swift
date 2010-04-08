/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Client/Client.h"

namespace Swift {
	class ClientXMLTracer {
		public:
			ClientXMLTracer(Client* client) {
				client->onDataRead.connect(boost::bind(&ClientXMLTracer::printData, '<', _1));
				client->onDataWritten.connect(boost::bind(&ClientXMLTracer::printData, '>', _1));
			}

		private:
			static void printData(char direction, const String& data) {
				printLine(direction);
				std::cerr << data << std::endl;
			}

			static void printLine(char c) {
				for (unsigned int i = 0; i < 80; ++i) {
					std::cerr << c;
				}
				std::cerr << std::endl;
			}
	};
}
