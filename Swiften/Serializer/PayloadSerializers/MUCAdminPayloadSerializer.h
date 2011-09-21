/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/MUCAdminPayload.h>

namespace Swift {
	class MUCAdminPayloadSerializer : public GenericPayloadSerializer<MUCAdminPayload> {
		public:
			MUCAdminPayloadSerializer();
			std::string affiliationToString(MUCOccupant::Affiliation affiliation) const;
			std::string roleToString(MUCOccupant::Role role) const;

			virtual std::string serializePayload(boost::shared_ptr<MUCAdminPayload> version)  const;
	};
}

