/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/VCards/VCardFileStorage.h"

#include <boost/filesystem/fstream.hpp>

#include <Swiften/Base/String.h>
#include "Swiften/JID/JID.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Serializer/PayloadSerializers/VCardSerializer.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"
#include "Swiften/Parser/PayloadParsers/VCardParser.h"

namespace Swift {

VCardFileStorage::VCardFileStorage(boost::filesystem::path dir) : vcardsPath(dir) {
}

boost::shared_ptr<VCard> VCardFileStorage::getVCard(const JID& jid) const {
	boost::filesystem::path vcardPath(getVCardPath(jid));
	if (boost::filesystem::exists(vcardPath)) {
		ByteArray data;
		data.readFromFile(vcardPath.string());

		VCardParser parser;
		PayloadParserTester tester(&parser);
		tester.parse(std::string(data.getData(), data.getSize()));
		return boost::dynamic_pointer_cast<VCard>(parser.getPayload());
	}
	else {
		return boost::shared_ptr<VCard>();
	}
}

void VCardFileStorage::setVCard(const JID& jid, VCard::ref v) {
	boost::filesystem::path vcardPath(getVCardPath(jid));
	if (!boost::filesystem::exists(vcardPath.parent_path())) {
		try {
			boost::filesystem::create_directories(vcardPath.parent_path());
		}
		catch (const boost::filesystem::filesystem_error& e) {
			std::cerr << "ERROR: " << e.what() << std::endl;
		}
	}
	boost::filesystem::ofstream file(getVCardPath(jid));
	file << VCardSerializer().serializePayload(v);
	file.close();
}

boost::filesystem::path VCardFileStorage::getVCardPath(const JID& jid) const {
	std::string file(jid.toString());
	String::replaceAll(file, '/', "%2f");
	return boost::filesystem::path(vcardsPath / (file + ".xml"));
}

}
