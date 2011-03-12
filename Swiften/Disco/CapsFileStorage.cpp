/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Disco/CapsFileStorage.h"

#include <iostream>
#include <boost/filesystem/fstream.hpp>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Serializer/PayloadSerializers/DiscoInfoSerializer.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"
#include "Swiften/Parser/PayloadParsers/DiscoInfoParser.h"
#include "Swiften/StringCodecs/Hexify.h"
#include "Swiften/StringCodecs/Base64.h"

namespace Swift {

CapsFileStorage::CapsFileStorage(const boost::filesystem::path& path) : path(path) {
}

DiscoInfo::ref CapsFileStorage::getDiscoInfo(const std::string& hash) const {
	boost::filesystem::path capsPath(getCapsPath(hash));
	if (boost::filesystem::exists(capsPath)) {
		ByteArray data;
		data.readFromFile(capsPath.string());

		DiscoInfoParser parser;
		PayloadParserTester tester(&parser);
		tester.parse(data.toString());
		return boost::dynamic_pointer_cast<DiscoInfo>(parser.getPayload());
	}
	else {
		return DiscoInfo::ref();
	}
}

void CapsFileStorage::setDiscoInfo(const std::string& hash, DiscoInfo::ref discoInfo) {
	boost::filesystem::path capsPath(getCapsPath(hash));
	if (!boost::filesystem::exists(capsPath.parent_path())) {
		try {
			boost::filesystem::create_directories(capsPath.parent_path());
		}
		catch (const boost::filesystem::filesystem_error& e) {
			std::cerr << "ERROR: " << e.what() << std::endl;
		}
	}
	DiscoInfo::ref bareDiscoInfo(new DiscoInfo(*discoInfo.get()));
	bareDiscoInfo->setNode("");
	boost::filesystem::ofstream file(capsPath);
	file << DiscoInfoSerializer().serializePayload(bareDiscoInfo);
	file.close();
}

boost::filesystem::path CapsFileStorage::getCapsPath(const std::string& hash) const {
	return path / (Hexify::hexify(Base64::decode(hash)) + ".xml");
}

}
