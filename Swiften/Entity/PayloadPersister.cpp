/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Entity/PayloadPersister.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Parser/PayloadParser.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h>
#include <Swiften/Serializer/PayloadSerializer.h>

using namespace Swift;

PayloadPersister::PayloadPersister() {
}

PayloadPersister::~PayloadPersister() {
}

void PayloadPersister::savePayload(std::shared_ptr<Payload> payload, const boost::filesystem::path& path) {
    try {
        if (!boost::filesystem::exists(path.parent_path())) {
            boost::filesystem::create_directories(path.parent_path());
        }
        boost::filesystem::ofstream file(path);
        file << getSerializer()->serialize(payload);
        file.close();
    }
    catch (const boost::filesystem::filesystem_error& e) {
        SWIFT_LOG(error) << e.what() << std::endl;
    }
}

std::shared_ptr<Payload> PayloadPersister::loadPayload(const boost::filesystem::path& path) {
    try {
        if (boost::filesystem::exists(path)) {
            ByteArray data;
            readByteArrayFromFile(data, path);
            std::shared_ptr<PayloadParser> parser(createParser());
            PayloadParserTester tester(parser.get());
            tester.parse(byteArrayToString(data));
            return parser->getPayload();
        }
    }
    catch (const boost::filesystem::filesystem_error& e) {
        SWIFT_LOG(error) << e.what() << std::endl;
    }
    return std::shared_ptr<Payload>();
}
