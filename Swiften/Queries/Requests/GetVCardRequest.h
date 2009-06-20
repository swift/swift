#pragma once

#include <cassert>

#include "Swiften/Queries/GenericRequest.h"
#include "Swiften/Elements/VCard.h"

namespace Swift {
	class GetVCardRequest : public GenericRequest<VCard> {
		public:
			GetVCardRequest(const JID& jid, IQRouter* router) :
					GenericRequest<VCard>(IQ::Get, jid, boost::shared_ptr<Payload>(new VCard()), router) {
				assert(jid.isBare());
			}
	};
}
