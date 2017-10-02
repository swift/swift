/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

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

            virtual std::string serializePayload(std::shared_ptr<BLOCK_ELEMENT> payload)    const {
                XMLElement element(tag, "urn:xmpp:blocking");
                const std::vector<JID>& items = payload->getItems();
                for (const auto& i : items) {
                    std::shared_ptr<XMLElement> item = std::make_shared<XMLElement>("item");
                    item->setAttribute("jid", i);
                    element.addNode(item);
                }
                return element.serialize();
            }

        private:
            std::string tag;
    };
}
