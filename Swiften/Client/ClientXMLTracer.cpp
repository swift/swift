/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Client/ClientXMLTracer.h>

#include <iostream>
#include <boost/bind.hpp>

namespace Swift {

ClientXMLTracer::ClientXMLTracer(CoreClient* client) {
	client->onDataRead.connect(boost::bind(&ClientXMLTracer::printData, '<', _1));
	client->onDataWritten.connect(boost::bind(&ClientXMLTracer::printData, '>', _1));
}

void ClientXMLTracer::printData(char direction, const SafeByteArray& data) {
	printLine(direction);
	std::cerr << byteArrayToString(ByteArray(data.begin(), data.end())) << std::endl;
}

void ClientXMLTracer::printLine(char c) {
	for (unsigned int i = 0; i < 80; ++i) {
		std::cerr << c;
	}
	std::cerr << std::endl;
}

}
