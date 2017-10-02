/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Storages/VCardFileStorage.h>

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <Swiften/Base/Path.h>
#include <Swiften/Base/String.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/Entity/GenericPayloadPersister.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h>
#include <Swiften/Parser/PayloadParsers/VCardParser.h>
#include <Swiften/Serializer/PayloadSerializers/VCardSerializer.h>
#include <Swiften/StringCodecs/Hexify.h>

using namespace Swift;

typedef GenericPayloadPersister<VCard, VCardParser, VCardSerializer> VCardPersister;

VCardFileStorage::VCardFileStorage(boost::filesystem::path dir, CryptoProvider* crypto) : VCardStorage(crypto), vcardsPath(dir), crypto(crypto) {
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

std::shared_ptr<VCard> VCardFileStorage::getVCard(const JID& jid) const {
    std::shared_ptr<VCard> result = VCardPersister().loadPayloadGeneric(getVCardPath(jid));
    getAndUpdatePhotoHash(jid, result);
    return result;
}

boost::posix_time::ptime VCardFileStorage::getVCardWriteTime(const JID& jid) const {
    if (vcardWriteTimes.find(jid) == vcardWriteTimes.end()) {
        return boost::posix_time::ptime();
    }
    else {
        return vcardWriteTimes.at(jid);
    }
}

void VCardFileStorage::setVCard(const JID& jid, VCard::ref v) {
    vcardWriteTimes[jid] = boost::posix_time::second_clock::universal_time();
    VCardPersister().savePayload(v, getVCardPath(jid));
    getAndUpdatePhotoHash(jid, v);
}

boost::filesystem::path VCardFileStorage::getVCardPath(const JID& jid) const {
    try {
        std::string file(jid.toString());
        String::replaceAll(file, '/', "%2f");
        return boost::filesystem::path(vcardsPath / stringToPath(file + ".xml"));
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
        hash = Hexify::hexify(crypto->getSHA1Hash(vCard->getPhoto()));
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
