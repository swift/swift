/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Queries/GenericRequest.h"
#include "Swiften/Elements/StreamInitiation.h"
#include "Swiften/Base/Shared.h"

namespace Swift {
	class StreamInitiationRequest : public GenericRequest<StreamInitiation>, public Shared<StreamInitiationRequest> {
		public:
			static ref create(const JID& jid, boost::shared_ptr<StreamInitiation> payload, IQRouter* router) {
				return ref(new StreamInitiationRequest(jid, payload, router));
			}

		private:
			StreamInitiationRequest(const JID& jid, boost::shared_ptr<StreamInitiation> payload, IQRouter* router) : GenericRequest<StreamInitiation>(IQ::Set, jid, payload, router) {
			}
	};
}
