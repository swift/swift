/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

#include "Swiften/Queries/Request.h"
#include "Swiften/Elements/RosterPayload.h"

namespace Swift {
	class SetRosterRequest : public Request {
		public:
			SetRosterRequest(boost::shared_ptr<RosterPayload> payload, IQRouter* router) : Request(IQ::Set, JID(), boost::shared_ptr<RosterPayload>(payload), router) {
			}

			virtual void handleResponse(boost::shared_ptr<Payload> /*payload*/, boost::optional<ErrorPayload> error) {
				onResponse(error);
			}

		public:
			boost::signal<void (const boost::optional<ErrorPayload>&)> onResponse;
	};
}
