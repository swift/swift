/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	template<typename BLOCK_ELEMENT>
	class BlockSerializer : public GenericPayloadSerializer<BLOCK_ELEMENT> {
		public:
			BlockSerializer(std::string tag) : GenericPayloadSerializer<BLOCK_ELEMENT>(), tag(tag) {
			}

			virtual std::string serializePayload(boost::shared_ptr<BLOCK_ELEMENT> payload)	const {
				XMLElement element(tag, "urn:xmpp:blocking");
				const std::vector<JID>& items = payload->getItems();
				for (std::vector<JID>::const_iterator i = items.begin(); i != items.end(); ++i) {
					boost::shared_ptr<XMLElement> item = boost::make_shared<XMLElement>("item");
					item->setAttribute("jid", *i);
				}
				return element.serialize();
			}

		private:
			std::string tag;
	};
}
