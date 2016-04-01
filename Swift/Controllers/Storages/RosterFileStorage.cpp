/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Storages/RosterFileStorage.h>

#include <Swiften/Entity/GenericPayloadPersister.h>
#include <Swiften/Parser/PayloadParsers/RosterParser.h>
#include <Swiften/Serializer/PayloadSerializers/RosterSerializer.h>

using namespace Swift;

typedef GenericPayloadPersister<RosterPayload, RosterParser, RosterSerializer> RosterPersister;

RosterFileStorage::RosterFileStorage(const boost::filesystem::path& path) : path(path) {
}

std::shared_ptr<RosterPayload> RosterFileStorage::getRoster() const {
    return RosterPersister().loadPayloadGeneric(path);
}

void RosterFileStorage::setRoster(std::shared_ptr<RosterPayload> roster) {
    RosterPersister().savePayload(roster, path);
}
