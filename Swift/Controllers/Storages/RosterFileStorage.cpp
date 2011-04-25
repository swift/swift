/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Storages/RosterFileStorage.h>

#include <Swiften/Entity/GenericPayloadPersister.h>
#include <Swiften/Serializer/PayloadSerializers/RosterSerializer.h>
#include <Swiften/Parser/PayloadParsers/RosterParser.h>

using namespace Swift;

typedef GenericPayloadPersister<RosterPayload, RosterParser, RosterSerializer> RosterPersister;

RosterFileStorage::RosterFileStorage(const boost::filesystem::path& path) : path(path) {
}

boost::shared_ptr<RosterPayload> RosterFileStorage::getRoster() const {
	return RosterPersister().loadPayloadGeneric(path);
}

void RosterFileStorage::setRoster(boost::shared_ptr<RosterPayload> roster) {
	RosterPersister().savePayload(roster, path);
}
