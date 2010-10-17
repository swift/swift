/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Queries/GenericRequest.h"
#include "Swiften/Elements/Bytestreams.h"
#include "Swiften/Base/Shared.h"

namespace Swift {
	class BytestreamsRequest : public GenericRequest<Bytestreams>, public Shared<BytestreamsRequest> {
		public:
			static ref create(const JID& jid, boost::shared_ptr<Bytestreams> payload, IQRouter* router) {
				return ref(new BytestreamsRequest(jid, payload, router));
			}

		private:
			BytestreamsRequest(const JID& jid, boost::shared_ptr<Bytestreams> payload, IQRouter* router) : GenericRequest<Bytestreams>(IQ::Set, jid, payload, router) {
			}
	};
}
