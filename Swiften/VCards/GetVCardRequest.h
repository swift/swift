/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Elements/VCard.h>


namespace Swift {
	class GetVCardRequest : public GenericRequest<VCard> {
		public:
			typedef boost::shared_ptr<GetVCardRequest> ref;

			static ref create(const JID& jid, IQRouter* router) {
				return ref(new GetVCardRequest(jid, router));
			}

		private:
			GetVCardRequest(const JID& jid, IQRouter* router) : GenericRequest<VCard>(IQ::Get, jid, boost::shared_ptr<Payload>(new VCard()), router) {
			}
	};
}
