/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Queries/GenericRequest.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Base/Shared.h"

namespace Swift {
	class GetVCardRequest : public GenericRequest<VCard>, public Shared<GetVCardRequest> {
		public:
			GetVCardRequest(const JID& jid, IQRouter* router) : GenericRequest<VCard>(IQ::Get, jid, boost::shared_ptr<Payload>(new VCard()), router) {
			}
	};
}
