/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Queries/GenericRequest.h"
#include "Swiften/Elements/InBandRegistrationPayload.h"


namespace Swift {
	class GetInBandRegistrationFormRequest : public GenericRequest<InBandRegistrationPayload> {
		public:
			typedef boost::shared_ptr<GetInBandRegistrationFormRequest> ref;

			static ref create(IQRouter* router) {
				return ref(new GetInBandRegistrationFormRequest(router));
			}

		private:
			GetInBandRegistrationFormRequest(IQRouter* router) : GenericRequest<InBandRegistrationPayload>(IQ::Get, JID(), InBandRegistrationPayload::ref(new InBandRegistrationPayload()), router) {
			}
	};
}
