/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/VCards/VCardFileStorage.h"

#include <boost/filesystem/fstream.hpp>

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
		tester.parse(String(data.getData(), data.getSize()));
		return boost::dynamic_pointer_cast<VCard>(parser.getPayload());
	}
	else {
		return boost::shared_ptr<VCard>();
	}
}

void VCardFileStorage::setVCard(const JID& jid, boost::shared_ptr<VCard> v) {
	boost::filesystem::path vcardPath(getVCardPath(jid));
	if (!boost::filesystem::exists(vcardPath.parent_path())) {
		boost::filesystem::create_directories(vcardPath.parent_path());
	}
	boost::filesystem::ofstream file(getVCardPath(jid));
	file << VCardSerializer().serializePayload(v);
	file.close();
}

boost::filesystem::path VCardFileStorage::getVCardPath(const JID& jid) const {
	return boost::filesystem::path(vcardsPath / (jid.toBare().toString().getUTF8String() + ".xml"));
}

}
