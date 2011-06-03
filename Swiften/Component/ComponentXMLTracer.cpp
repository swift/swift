/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Component/ComponentXMLTracer.h>

#include <iostream>
#include <boost/bind.hpp>

namespace Swift {

ComponentXMLTracer::ComponentXMLTracer(CoreComponent* client) {
	client->onDataRead.connect(boost::bind(&ComponentXMLTracer::printData, '<', _1));
	client->onDataWritten.connect(boost::bind(&ComponentXMLTracer::printData, '>', _1));
}

void ComponentXMLTracer::printData(char direction, const SafeByteArray& data) {
	printLine(direction);
	std::cerr << byteArrayToString(ByteArray(data.begin(), data.end())) << std::endl;
}

void ComponentXMLTracer::printLine(char c) {
	for (unsigned int i = 0; i < 80; ++i) {
		std::cerr << c;
	}
	std::cerr << std::endl;
}

}
