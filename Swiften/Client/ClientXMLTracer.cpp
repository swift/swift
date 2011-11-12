/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Client/ClientXMLTracer.h>

#include <iostream>
#include <boost/bind.hpp>

namespace Swift {

ClientXMLTracer::ClientXMLTracer(CoreClient* client, bool bosh) : bosh(bosh) {
	beautifier = new XMLBeautifier(true, true);
	client->onDataRead.connect(boost::bind(&ClientXMLTracer::printData, this, '<', _1));
	client->onDataWritten.connect(boost::bind(&ClientXMLTracer::printData, this, '>', _1));
}

ClientXMLTracer::~ClientXMLTracer() {
	delete beautifier;
}

void ClientXMLTracer::printData(char direction, const SafeByteArray& data) {
	printLine(direction);
	if (bosh) {
		std::string line = byteArrayToString(ByteArray(data.begin(), data.end())); 
		size_t endOfHTTP = line.find("\r\n\r\n");
		if (false && endOfHTTP != std::string::npos) {
			/* Disabled because it swallows bits of XML (namespaces, if I recall) */
			std::cerr << line.substr(0, endOfHTTP) << std::endl << beautifier->beautify(line.substr(endOfHTTP)) << std::endl;
		}
		else {
			std::cerr << line << std::endl;
		}
	}
	else {
		std::cerr << beautifier->beautify(byteArrayToString(ByteArray(data.begin(), data.end()))) << std::endl;
	}
}

void ClientXMLTracer::printLine(char c) {
	for (unsigned int i = 0; i < 80; ++i) {
		std::cerr << c;
	}
	std::cerr << std::endl;
}

}
