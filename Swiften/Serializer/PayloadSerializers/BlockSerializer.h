/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Base/foreach.h>

namespace Swift {
	template<typename BLOCK_ELEMENT>
	class BlockSerializer : public GenericPayloadSerializer<BLOCK_ELEMENT> {
		public:
			BlockSerializer(std::string tag) : GenericPayloadSerializer<BLOCK_ELEMENT>(), tag(tag) {
			}

			virtual std::string serializePayload(boost::shared_ptr<BLOCK_ELEMENT> payload)  const {
				XMLElement element(tag, "urn:xmpp:blocking");
				foreach (const JID& jid, payload->getItems()) {
					boost::shared_ptr<XMLElement> item = boost::make_shared<XMLElement>("item");
					item->setAttribute("jid", jid);
				}
				return element.serialize();
			}

		private:
			std::string tag;
	};
}
