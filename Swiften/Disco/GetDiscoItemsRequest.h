/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Elements/DiscoItems.h>

namespace Swift {
	class GetDiscoItemsRequest : public GenericRequest<DiscoItems> {
		public:
			typedef boost::shared_ptr<GetDiscoItemsRequest> ref;

			static ref create(const JID& jid, IQRouter* router) {
				return ref(new GetDiscoItemsRequest(jid, router));
			}

			static ref create(const JID& jid, const std::string& node, IQRouter* router) {
				return ref(new GetDiscoItemsRequest(jid, node, router));
			}

		private:
			GetDiscoItemsRequest(const JID& jid, IQRouter* router) :
					GenericRequest<DiscoItems>(IQ::Get, jid, boost::make_shared<DiscoItems>(), router) {
			}

			GetDiscoItemsRequest(const JID& jid, const std::string& node, IQRouter* router) :
				GenericRequest<DiscoItems>(IQ::Get, jid, boost::make_shared<DiscoItems>(), router) {
				getPayloadGeneric()->setNode(node);
			}
	};
}
