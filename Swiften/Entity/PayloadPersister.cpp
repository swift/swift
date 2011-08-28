/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Entity/PayloadPersister.h>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Parser/PayloadParser.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Serializer/PayloadSerializer.h>

using namespace Swift;

PayloadPersister::PayloadPersister() {
}

PayloadPersister::~PayloadPersister() {
}

void PayloadPersister::savePayload(boost::shared_ptr<Payload> payload, const boost::filesystem::path& path) {
	try {
		if (!boost::filesystem::exists(path.parent_path())) {
			boost::filesystem::create_directories(path.parent_path());
		}
		boost::filesystem::ofstream file(path);
		file << getSerializer()->serialize(payload);
		file.close();
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

boost::shared_ptr<Payload> PayloadPersister::loadPayload(const boost::filesystem::path& path) {
	try {
		if (boost::filesystem::exists(path)) {
			ByteArray data;
			readByteArrayFromFile(data, path.string());
			boost::shared_ptr<PayloadParser> parser(createParser());
			PayloadParserTester tester(parser.get());
			tester.parse(byteArrayToString(data));
			return parser->getPayload();
		}
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	return boost::shared_ptr<Payload>();
}
