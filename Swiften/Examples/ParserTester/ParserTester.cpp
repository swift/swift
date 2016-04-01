/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/UnitTest/ParserTester.h>

#include <fstream>
#include <iostream>
#include <typeinfo>

#include <Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Parser/XMPPParser.h>
#include <Swiften/Parser/XMPPParserClient.h>

using namespace Swift;

class MyXMPPParserClient : public XMPPParserClient {
    public:
        virtual void handleStreamStart(const ProtocolHeader&) {
            std::cout << "-> Stream start" << std::endl;
        }
        virtual void handleElement(std::shared_ptr<ToplevelElement> element) {
            std::cout << "-> Element " << typeid(*element.get()).name() << std::endl;
        }
        virtual void handleStreamEnd() {
            std::cout << "-> Stream end" << std::endl;
        }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " file" << std::endl;
        return 0;
    }
    FullPayloadParserFactoryCollection factories;
    MyXMPPParserClient parserClient;
    PlatformXMLParserFactory xmlParserFactory;
    XMPPParser parser(&parserClient, &factories, &xmlParserFactory);
    ParserTester<XMLParserClient> tester(&parser);

    std::string line;
    std::ifstream myfile (argv[1]);
    if (myfile.is_open()) {
        while (!myfile.eof()) {
            getline (myfile,line);
            std::cout << "Parsing: " << line << std::endl;
            if (!tester.parse(line)) {
                std::cerr << "PARSE ERROR" << std::endl;
                return -1;
            }
        }
        myfile.close();
    }
    else {
        std::cerr << "Unable to open file " << argv[1] << std::endl;
    }
    return 0;
}
