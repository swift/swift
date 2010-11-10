/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

#include "Swiften/Queries/Request.h"
#include "Swiften/Elements/InBandRegistrationPayload.h"


namespace Swift {
	class SetInBandRegistrationRequest : public Request {
		public:
			typedef boost::shared_ptr<SetInBandRegistrationRequest> ref;

			static ref create(InBandRegistrationPayload::ref payload, IQRouter* router) {
				return ref(new SetInBandRegistrationRequest(payload, router));
			}

		private:
			SetInBandRegistrationRequest(InBandRegistrationPayload::ref payload, IQRouter* router) : Request(IQ::Set, JID(), InBandRegistrationPayload::ref(payload), router) {
			}

			virtual void handleResponse(Payload::ref payload, boost::optional<ErrorPayload> error) {
				onResponse(payload, error);
			}

		public:
			boost::signal<void (Payload::ref, const boost::optional<ErrorPayload>&)> onResponse;
	};
}
