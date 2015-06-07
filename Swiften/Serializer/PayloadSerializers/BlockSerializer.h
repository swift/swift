/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	template<typename BLOCK_ELEMENT>
	class SWIFTEN_API BlockSerializer : public GenericPayloadSerializer<BLOCK_ELEMENT> {
		public:
			BlockSerializer(std::string tag) : GenericPayloadSerializer<BLOCK_ELEMENT>(), tag(tag) {
			}

			virtual std::string serializePayload(boost::shared_ptr<BLOCK_ELEMENT> payload)	const {
				XMLElement element(tag, "urn:xmpp:blocking");
				const std::vector<JID>& items = payload->getItems();
				for (std::vector<JID>::const_iterator i = items.begin(); i != items.end(); ++i) {
					boost::shared_ptr<XMLElement> item = boost::make_shared<XMLElement>("item");
					item->setAttribute("jid", *i);
					element.addNode(item);
				}
				return element.serialize();
			}

		private:
			std::string tag;
	};
}
