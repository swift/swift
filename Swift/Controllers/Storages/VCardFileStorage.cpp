/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Storages/VCardFileStorage.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

#include <Swiften/Entity/GenericPayloadPersister.h>
#include <Swiften/Base/String.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/Base/foreach.h>
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Serializer/PayloadSerializers/VCardSerializer.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"
#include "Swiften/Parser/PayloadParsers/VCardParser.h"

using namespace Swift;

typedef GenericPayloadPersister<VCard, VCardParser, VCardSerializer> VCardPersister;

VCardFileStorage::VCardFileStorage(boost::filesystem::path dir) : vcardsPath(dir) {
	cacheFile = vcardsPath / "phashes";
	if (boost::filesystem::exists(cacheFile)) {
		try {
			boost::filesystem::ifstream file(cacheFile);
			std::string line;
			if (file.is_open()) {
				while (!file.eof()) {
					getline(file, line);
					std::pair<std::string, std::string> r = String::getSplittedAtFirst(line, ' ');
					JID jid(r.second);
					if (jid.isValid()) {
						photoHashes.insert(std::make_pair(jid, r.first));
					}
					else if (!r.first.empty() || !r.second.empty()) {
						std::cerr << "Invalid entry in phashes file" << std::endl;
					}
				}
			}
		}
		catch (...) {
			std::cerr << "Error reading phashes file" << std::endl;
		}
	}
}

boost::shared_ptr<VCard> VCardFileStorage::getVCard(const JID& jid) const {
	boost::shared_ptr<VCard> result = VCardPersister().loadPayloadGeneric(getVCardPath(jid));
	getAndUpdatePhotoHash(jid, result);
	return result;
}

void VCardFileStorage::setVCard(const JID& jid, VCard::ref v) {
	VCardPersister().savePayload(v, getVCardPath(jid));
	getAndUpdatePhotoHash(jid, v);
}

boost::filesystem::path VCardFileStorage::getVCardPath(const JID& jid) const {
	try {
		std::string file(jid.toString());
		String::replaceAll(file, '/', "%2f");
		return boost::filesystem::path(vcardsPath / (file + ".xml"));
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return boost::filesystem::path();
	}
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
	if (vCard && !vCard->getPhoto().empty()) {
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
	try {
		boost::filesystem::ofstream file(cacheFile);
		for (PhotoHashMap::const_iterator i = photoHashes.begin(); i != photoHashes.end(); ++i) {
			file << i->second << " " << i->first.toString() << std::endl;
		}
		file.close();
	}
	catch (...) {
		std::cerr << "Error writing vcards file" << std::endl;
	}
}
