/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Queries/GenericRequest.h"
#include "Swiften/Elements/IBB.h"
#include "Swiften/Base/Shared.h"

namespace Swift {
	class IBBRequest : public GenericRequest<IBB>, public Shared<IBBRequest> {
		public:
			static ref create(const JID& jid, boost::shared_ptr<IBB> payload, IQRouter* router) {
				return ref(new IBBRequest(jid, payload, router));
			}

		private:
			IBBRequest(const JID& jid, boost::shared_ptr<IBB> payload, IQRouter* router) : GenericRequest<IBB>(IQ::Set, jid, payload, router) {
			}
	};
}
