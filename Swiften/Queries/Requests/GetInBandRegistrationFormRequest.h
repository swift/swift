/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Elements/InBandRegistrationPayload.h>


namespace Swift {
	class GetInBandRegistrationFormRequest : public GenericRequest<InBandRegistrationPayload> {
		public:
			typedef boost::shared_ptr<GetInBandRegistrationFormRequest> ref;

			static ref create(const JID& to, IQRouter* router) {
				return ref(new GetInBandRegistrationFormRequest(to, router));
			}

		private:
			GetInBandRegistrationFormRequest(const JID& to, IQRouter* router) : GenericRequest<InBandRegistrationPayload>(IQ::Get, to, InBandRegistrationPayload::ref(new InBandRegistrationPayload()), router) {
			}
	};
}
