/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Client/ClientXMLTracer.h>

#include <iostream>

#include <boost/bind.hpp>

#include <Swiften/Base/Platform.h>

namespace Swift {

ClientXMLTracer::ClientXMLTracer(CoreClient* client, bool bosh) : bosh_(bosh) {
#ifdef SWIFTEN_PLATFORM_WIN32
    beautifier_ = std::unique_ptr<XMLBeautifier>(new XMLBeautifier(true, false));
#else
    beautifier_ = std::unique_ptr<XMLBeautifier>(new XMLBeautifier(true, true));
#endif
    onDataReadConnection_ = client->onDataRead.connect(boost::bind(&ClientXMLTracer::printData, this, '<', _1));
    onDataWrittenConnection_ = client->onDataWritten.connect(boost::bind(&ClientXMLTracer::printData, this, '>', _1));
}

void ClientXMLTracer::printData(char direction, const SafeByteArray& data) {
    if (bosh_) {
        printLine(direction);
        std::string line = byteArrayToString(ByteArray(data.begin(), data.end()));
// Disabled because it swallows bits of XML (namespaces, if I recall)
//        size_t endOfHTTP = line.find("\r\n\r\n");
//        if (false && endOfHTTP != std::string::npos) {
//            std::cerr << line.substr(0, endOfHTTP) << std::endl << beautifier->beautify(line.substr(endOfHTTP)) << std::endl;
//        }
//        else {
            std::cerr << line << std::endl;
//        }
    }
    else {
        const auto& str = beautifier_->beautify(byteArrayToString(ByteArray(data.begin(), data.end())));

        if (beautifier_->wasReset()) {
            printLine(direction);
        }
        std::cerr << str;
        if (beautifier_->getLevel() <= 1) {
            std::cerr << std::endl;
        }

    }
}

void ClientXMLTracer::printLine(char c) {
    for (unsigned int i = 0; i < 80; ++i) {
        std::cerr << c;
    }
    std::cerr << std::endl;
}

}
