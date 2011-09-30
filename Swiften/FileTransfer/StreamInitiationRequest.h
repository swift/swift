/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Elements/StreamInitiation.h>


namespace Swift {
	class StreamInitiationRequest : public GenericRequest<StreamInitiation> {
		public:
			typedef boost::shared_ptr<StreamInitiationRequest> ref;

			static ref create(const JID& jid, boost::shared_ptr<StreamInitiation> payload, IQRouter* router) {
				return ref(new StreamInitiationRequest(jid, payload, router));
			}

			static ref create(const JID& from, const JID& to, boost::shared_ptr<StreamInitiation> payload, IQRouter* router) {
				return ref(new StreamInitiationRequest(from, to, payload, router));
			}

		private:
			StreamInitiationRequest(const JID& jid, boost::shared_ptr<StreamInitiation> payload, IQRouter* router) : GenericRequest<StreamInitiation>(IQ::Set, jid, payload, router) {
			}

			StreamInitiationRequest(const JID& from, const JID& to, boost::shared_ptr<StreamInitiation> payload, IQRouter* router) : GenericRequest<StreamInitiation>(IQ::Set, from, to, payload, router) {
			}
	};
}
