/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Storages/FileStorages.h>

#include <Swiften/Base/Path.h>
#include <Swiften/History/SQLiteHistoryStorage.h>

#include <Swift/Controllers/Storages/AvatarFileStorage.h>
#include <Swift/Controllers/Storages/CapsFileStorage.h>
#include <Swift/Controllers/Storages/RosterFileStorage.h>
#include <Swift/Controllers/Storages/VCardFileStorage.h>

namespace Swift {

FileStorages::FileStorages(const boost::filesystem::path& baseDir, const JID& jid, CryptoProvider* crypto) {
    boost::filesystem::path profile = stringToPath(jid.toBare());
    vcardStorage = new VCardFileStorage(baseDir / profile / "vcards", crypto);
    capsStorage = new CapsFileStorage(baseDir / "caps");
    avatarStorage = new AvatarFileStorage(baseDir / "avatars", baseDir / profile / "avatars", crypto);
    rosterStorage = new RosterFileStorage(baseDir / profile / "roster.xml");
#ifdef SWIFT_EXPERIMENTAL_HISTORY
    historyStorage = new SQLiteHistoryStorage(baseDir / "history.db");
#else
    historyStorage = nullptr;
#endif
}

FileStorages::~FileStorages() {
    delete rosterStorage;
    delete avatarStorage;
    delete capsStorage;
    delete vcardStorage;
    delete historyStorage;
}

VCardStorage* FileStorages::getVCardStorage() const {
    return vcardStorage;
}

CapsStorage* FileStorages::getCapsStorage() const {
    return capsStorage;
}

AvatarStorage* FileStorages::getAvatarStorage() const {
    return avatarStorage;
}

RosterStorage* FileStorages::getRosterStorage() const {
    return rosterStorage;
}

HistoryStorage* FileStorages::getHistoryStorage() const {
#ifdef SWIFT_EXPERIMENTAL_HISTORY
    return historyStorage;
#else
    return nullptr;
#endif
}

}
