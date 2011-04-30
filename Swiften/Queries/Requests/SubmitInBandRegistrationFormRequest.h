/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>

#include <Swiften/Queries/Request.h>
#include <Swiften/Elements/InBandRegistrationPayload.h>


namespace Swift {
	class SetInBandRegistrationRequest : public Request {
		public:
			typedef boost::shared_ptr<SetInBandRegistrationRequest> ref;

			static ref create(const JID& to, InBandRegistrationPayload::ref payload, IQRouter* router) {
				return ref(new SetInBandRegistrationRequest(to, payload, router));
			}

		private:
			SetInBandRegistrationRequest(const JID& to, InBandRegistrationPayload::ref payload, IQRouter* router) : Request(IQ::Set, to, InBandRegistrationPayload::ref(payload), router) {
			}

			virtual void handleResponse(boost::shared_ptr<Payload> payload, ErrorPayload::ref error) {
				onResponse(payload, error);
			}

		public:
			boost::signal<void (boost::shared_ptr<Payload>, ErrorPayload::ref)> onResponse;
	};
}
