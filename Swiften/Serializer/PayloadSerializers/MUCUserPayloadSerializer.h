/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/MUCUserPayload.h"

namespace Swift {
	class MUCUserPayloadSerializer : public GenericPayloadSerializer<MUCUserPayload> {
		public:
			MUCUserPayloadSerializer();
			std::string affiliationToString(MUCOccupant::Affiliation affiliation) const;
			std::string roleToString(MUCOccupant::Role role) const;

			virtual std::string serializePayload(boost::shared_ptr<MUCUserPayload> version)  const;
	};
}

