/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */


#pragma once

#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Elements/S5BProxyRequest.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	class PayloadSerializerCollection;

	class S5BProxyRequestSerializer : public GenericPayloadSerializer<S5BProxyRequest> {
		public:
			virtual std::string serializePayload(boost::shared_ptr<S5BProxyRequest> s5bProxyRequest) const {
				XMLElement queryElement("query", "http://jabber.org/protocol/bytestreams");
				if (s5bProxyRequest && s5bProxyRequest->getStreamHost()) {
					boost::shared_ptr<XMLElement> streamHost = boost::make_shared<XMLElement>("streamhost");
					streamHost->setAttribute("host", s5bProxyRequest->getStreamHost().get().addressPort.getAddress().toString());
					streamHost->setAttribute("port", boost::lexical_cast<std::string>(s5bProxyRequest->getStreamHost().get().addressPort.getPort()));
					streamHost->setAttribute("jid", s5bProxyRequest->getStreamHost().get().jid.toString());
					queryElement.addNode(streamHost);
				} else if (s5bProxyRequest && s5bProxyRequest->getActivate()) {
					queryElement.setAttribute("sid", s5bProxyRequest->getSID());
					boost::shared_ptr<XMLElement> activate = boost::make_shared<XMLElement>("activate", "", s5bProxyRequest->getActivate().get().toString());
					queryElement.addNode(activate);
				}
				return queryElement.serialize();
			}
	};
}
