/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Elements/Bytestreams.h>

namespace Swift {
	class BytestreamsRequest : public GenericRequest<Bytestreams> {
		public:
			typedef boost::shared_ptr<BytestreamsRequest> ref;

			static ref create(const JID& jid, boost::shared_ptr<Bytestreams> payload, IQRouter* router) {
				return ref(new BytestreamsRequest(jid, payload, router));
			}

			static ref create(const JID& from, const JID& to, boost::shared_ptr<Bytestreams> payload, IQRouter* router) {
				return ref(new BytestreamsRequest(from, to, payload, router));
			}

		private:
			BytestreamsRequest(const JID& jid, boost::shared_ptr<Bytestreams> payload, IQRouter* router) : GenericRequest<Bytestreams>(IQ::Set, jid, payload, router) {
			}

			BytestreamsRequest(const JID& from, const JID& to, boost::shared_ptr<Bytestreams> payload, IQRouter* router) : GenericRequest<Bytestreams>(IQ::Set, from, to, payload, router) {
			}
	};
}
