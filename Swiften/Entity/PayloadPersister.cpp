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
	if (!boost::filesystem::exists(path.parent_path())) {
		try {
			boost::filesystem::create_directories(path.parent_path());
		}
		catch (const boost::filesystem::filesystem_error& e) {
			std::cerr << "ERROR: " << e.what() << std::endl;
		}
	}
	boost::filesystem::ofstream file(path);
	file << getSerializer()->serialize(payload);
	file.close();
}

boost::shared_ptr<Payload> PayloadPersister::loadPayload(const boost::filesystem::path& path) {
	if (boost::filesystem::exists(path)) {
		ByteArray data;
		data.readFromFile(path.string());
		boost::shared_ptr<PayloadParser> parser(createParser());
		PayloadParserTester tester(parser.get());
		tester.parse(data.toString());
		return parser->getPayload();
	}
	else {
		return boost::shared_ptr<Payload>();
	}
}
