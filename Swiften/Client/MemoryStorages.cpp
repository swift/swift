/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Client/MemoryStorages.h>
#include <Swiften/VCards/VCardMemoryStorage.h>
#include <Swiften/Avatars/AvatarMemoryStorage.h>
#include <Swiften/Disco/CapsMemoryStorage.h>
#include <Swiften/Roster/RosterMemoryStorage.h>
#ifdef SWIFT_EXPERIMENTAL_HISTORY
#include <Swiften/History/SQLiteHistoryStorage.h>
#endif

namespace Swift {

MemoryStorages::MemoryStorages(CryptoProvider* crypto) {
    vcardStorage = new VCardMemoryStorage(crypto);
    capsStorage = new CapsMemoryStorage();
    avatarStorage = new AvatarMemoryStorage();
    rosterStorage = new RosterMemoryStorage();
#ifdef SWIFT_EXPERIMENTAL_HISTORY
    historyStorage = new SQLiteHistoryStorage(":memory:");
#else
    historyStorage = nullptr;
#endif
}

MemoryStorages::~MemoryStorages() {
    delete rosterStorage;
    delete avatarStorage;
    delete capsStorage;
    delete vcardStorage;
#ifdef SWIFT_EXPERIMENTAL_HISTORY
    delete historyStorage;
#endif
}

VCardStorage* MemoryStorages::getVCardStorage() const {
    return vcardStorage;
}

CapsStorage* MemoryStorages::getCapsStorage() const {
    return capsStorage;
}

AvatarStorage* MemoryStorages::getAvatarStorage() const {
    return avatarStorage;
}

RosterStorage* MemoryStorages::getRosterStorage() const {
    return rosterStorage;
}

HistoryStorage* MemoryStorages::getHistoryStorage() const {
#ifdef SWIFT_EXPERIMENTAL_HISTORY
    return historyStorage;
#else
    return nullptr;
#endif
}


}
