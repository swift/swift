/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/VCards/VCardFileStorage.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <Swiften/Base/String.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/Base/foreach.h>
#include "Swiften/JID/JID.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Serializer/PayloadSerializers/VCardSerializer.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"
#include "Swiften/Parser/PayloadParsers/VCardParser.h"

namespace Swift {

VCardFileStorage::VCardFileStorage(boost::filesystem::path dir) : vcardsPath(dir) {
	cacheFile = vcardsPath / "vcards.xml";
	if (boost::filesystem::exists(cacheFile)) {
		boost::property_tree::ptree tree;
		try {
			boost::property_tree::xml_parser::read_xml(cacheFile.string(), tree);
		}
		catch (const boost::property_tree::xml_parser::xml_parser_error& e) {
			std::cerr << "Error reading vcards file: " << e.filename() << ":" << e.line() << ": " << e.message() << std::endl;
		}
		foreach(const boost::property_tree::ptree::value_type &v, tree.get_child("vcards")) {
			try {
				JID jid(v.second.get<std::string>("jid"));
				std::string hash(v.second.get<std::string>("phash"));
				if (jid.isValid()) {
					photoHashes.insert(std::make_pair(jid, hash));
				}
			}
			catch (const boost::property_tree::ptree_error& e) {
				std::cerr << "Invalid vcard value: " << e.what() << std::endl;
			}
		}
	}
}

boost::shared_ptr<VCard> VCardFileStorage::getVCard(const JID& jid) const {
	boost::filesystem::path vcardPath(getVCardPath(jid));
	if (boost::filesystem::exists(vcardPath)) {
		ByteArray data;
		data.readFromFile(vcardPath.string());

		VCardParser parser;
		PayloadParserTester tester(&parser);
		tester.parse(data.toString());
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
	getAndUpdatePhotoHash(jid, v);
}

boost::filesystem::path VCardFileStorage::getVCardPath(const JID& jid) const {
	std::string file(jid.toString());
	String::replaceAll(file, '/', "%2f");
	return boost::filesystem::path(vcardsPath / (file + ".xml"));
}

std::string VCardFileStorage::getPhotoHash(const JID& jid) const {
	PhotoHashMap::const_iterator i = photoHashes.find(jid);
	if (i != photoHashes.end()) {
		return i->second;
	}
	else {
		VCard::ref vCard = getVCard(jid);
		return getAndUpdatePhotoHash(jid, vCard);
	}
}

std::string VCardFileStorage::getAndUpdatePhotoHash(const JID& jid, VCard::ref vCard) const {
	std::string hash;
	if (vCard && !vCard->getPhoto().isEmpty()) {
		hash = Hexify::hexify(SHA1::getHash(vCard->getPhoto()));
	}
	std::pair<PhotoHashMap::iterator, bool> r = photoHashes.insert(std::make_pair(jid, hash));
	if (r.second) {
		savePhotoHashes();
	}
	else if (r.first->second != hash) {
		r.first->second = hash;
		savePhotoHashes();
	}
	return hash;
}

void VCardFileStorage::savePhotoHashes() const {
	boost::property_tree::ptree tree;
	for (PhotoHashMap::const_iterator i = photoHashes.begin(); i != photoHashes.end(); ++i) {
		boost::property_tree::ptree entry;
		entry.put("jid", i->first.toString());
		entry.put("phash", i->second);
		tree.add_child("vcards.vcard", entry);
	}
	try {
		boost::property_tree::xml_parser::write_xml(cacheFile.string(), tree);
	}
	catch (const boost::property_tree::xml_parser::xml_parser_error& e) {
		std::cerr << "Error writing vcards file: " << e.filename() << ": " << e.message() << std::endl;
	}
}


}
