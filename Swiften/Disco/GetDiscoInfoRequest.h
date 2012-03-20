/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Elements/DiscoInfo.h>

namespace Swift {
	class GetDiscoInfoRequest : public GenericRequest<DiscoInfo> {
		public:
			typedef boost::shared_ptr<GetDiscoInfoRequest> ref;

			static ref create(const JID& jid, IQRouter* router) {
				return ref(new GetDiscoInfoRequest(jid, router));
			}

			static ref create(const JID& jid, const std::string& node, IQRouter* router) {
				return ref(new GetDiscoInfoRequest(jid, node, router));
			}

		private:
			GetDiscoInfoRequest(const JID& jid, IQRouter* router) :
					GenericRequest<DiscoInfo>(IQ::Get, jid, boost::make_shared<DiscoInfo>(), router) {
			}

			GetDiscoInfoRequest(const JID& jid, const std::string& node, IQRouter* router) :
					GenericRequest<DiscoInfo>(IQ::Get, jid, boost::make_shared<DiscoInfo>(), router) {
				getPayloadGeneric()->setNode(node);
			}
	};
}
