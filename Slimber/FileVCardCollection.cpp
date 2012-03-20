/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Slimber/FileVCardCollection.h"

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/filesystem/fstream.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/Serializer/PayloadSerializers/VCardSerializer.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h>
#include <Swiften/Parser/PayloadParsers/VCardParser.h>

namespace Swift {

FileVCardCollection::FileVCardCollection(boost::filesystem::path dir) : vcardsPath(dir) {
}

boost::shared_ptr<VCard> FileVCardCollection::getOwnVCard() const {
	if (boost::filesystem::exists(vcardsPath / std::string("vcard.xml"))) {
		ByteArray data;
		readByteArrayFromFile(data, boost::filesystem::path(vcardsPath / std::string("vcard.xml")).string());

		VCardParser parser;
		PayloadParserTester tester(&parser);
		tester.parse(byteArrayToString(data));
		return boost::dynamic_pointer_cast<VCard>(parser.getPayload());
	}
	else {
		return boost::make_shared<VCard>();
	}
}

void FileVCardCollection::setOwnVCard(boost::shared_ptr<VCard> v) {
	boost::filesystem::ofstream file(vcardsPath / std::string("vcard.xml"));
	file << VCardSerializer().serializePayload(v);
	file.close();
}

}
