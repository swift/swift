/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Storages/CapsFileStorage.h"

#include <Swiften/Entity/GenericPayloadPersister.h>
#include "Swiften/Serializer/PayloadSerializers/DiscoInfoSerializer.h"
#include "Swiften/Parser/PayloadParsers/DiscoInfoParser.h"
#include "Swiften/StringCodecs/Hexify.h"
#include "Swiften/StringCodecs/Base64.h"

using namespace Swift;

typedef GenericPayloadPersister<DiscoInfo, DiscoInfoParser, DiscoInfoSerializer> DiscoInfoPersister;

CapsFileStorage::CapsFileStorage(const boost::filesystem::path& path) : path(path) {
}

DiscoInfo::ref CapsFileStorage::getDiscoInfo(const std::string& hash) const {
	return DiscoInfoPersister().loadPayloadGeneric(getCapsPath(hash));
}

void CapsFileStorage::setDiscoInfo(const std::string& hash, DiscoInfo::ref discoInfo) {
	DiscoInfo::ref bareDiscoInfo(new DiscoInfo(*discoInfo.get()));
	bareDiscoInfo->setNode("");
	DiscoInfoPersister().savePayload(bareDiscoInfo, getCapsPath(hash));
}

boost::filesystem::path CapsFileStorage::getCapsPath(const std::string& hash) const {
	return path / (Hexify::hexify(Base64::decode(hash)) + ".xml");
}
