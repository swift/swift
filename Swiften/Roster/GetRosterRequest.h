/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
	class SWIFTEN_API GetRosterRequest : public GenericRequest<RosterPayload> {
		public:
			typedef boost::shared_ptr<GetRosterRequest> ref;

			static ref create(IQRouter* router) {
				return ref(new GetRosterRequest(router));
			}

			static ref create(IQRouter* router, const std::string& version) {
				ref result(new GetRosterRequest(router));
				result->getPayloadGeneric()->setVersion(version);
				return result;
			}

		private:
			GetRosterRequest(IQRouter* router) :
					GenericRequest<RosterPayload>(IQ::Get, JID(), boost::shared_ptr<Payload>(new RosterPayload()), router) {
			}
	};
}
