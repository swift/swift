/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Queries/GenericRequest.h"
#include "Swiften/Elements/RosterPayload.h"


namespace Swift {
	class GetRosterRequest : public GenericRequest<RosterPayload> {
		public:
			typedef boost::shared_ptr<GetRosterRequest> ref;

			static ref create(IQRouter* router) {
				return ref(new GetRosterRequest(router));
			}

		private:
			GetRosterRequest(IQRouter* router) :
					GenericRequest<RosterPayload>(IQ::Get, JID(), boost::shared_ptr<Payload>(new RosterPayload()), router) {
			}
	};
}
