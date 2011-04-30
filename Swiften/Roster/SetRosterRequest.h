/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>

#include <Swiften/Queries/Request.h>
#include <Swiften/Elements/RosterPayload.h>


namespace Swift {
	class SetRosterRequest : public Request {
		public:
			typedef boost::shared_ptr<SetRosterRequest> ref;

			static ref create(RosterPayload::ref payload, IQRouter* router) {
				return ref(new SetRosterRequest(JID(), payload, router));
			}

			static ref create(RosterPayload::ref payload, const JID& to, IQRouter* router) {
				return ref(new SetRosterRequest(to, payload, router));
			}

		private:
			SetRosterRequest(const JID& to, boost::shared_ptr<RosterPayload> payload, IQRouter* router) : Request(IQ::Set, to, boost::shared_ptr<RosterPayload>(payload), router) {
			}

			virtual void handleResponse(boost::shared_ptr<Payload> /*payload*/, ErrorPayload::ref error) {
				onResponse(error);
			}

		public:
			boost::signal<void (ErrorPayload::ref)> onResponse;
	};
}
