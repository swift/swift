/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/StanzaSerializer.h>

#include <sstream>
#include <typeinfo>

#include <Swiften/Base/String.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/Serializer/PayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {

StanzaSerializer::StanzaSerializer(const std::string& tag, PayloadSerializerCollection* payloadSerializers, const boost::optional<std::string>& explicitNS) : tag_(tag), payloadSerializers_(payloadSerializers), explicitDefaultNS_(explicitNS) {
}

SafeByteArray StanzaSerializer::serialize(std::shared_ptr<ToplevelElement> element) const {
    if (explicitDefaultNS_) {
        return serialize(element, explicitDefaultNS_.get());
    }
    else {
        return serialize(element, "");
    }
}

SafeByteArray StanzaSerializer::serialize(std::shared_ptr<ToplevelElement> element, const std::string& xmlns) const {
    std::shared_ptr<Stanza> stanza(std::dynamic_pointer_cast<Stanza>(element));

    XMLElement stanzaElement(tag_, explicitDefaultNS_ ? explicitDefaultNS_.get() : xmlns);
    if (stanza->getFrom().isValid()) {
        stanzaElement.setAttribute("from", stanza->getFrom());
    }
    if (stanza->getTo().isValid()) {
        stanzaElement.setAttribute("to", stanza->getTo());
    }
    if (!stanza->getID().empty()) {
        stanzaElement.setAttribute("id", stanza->getID());
    }
    setStanzaSpecificAttributes(stanza, stanzaElement);

    std::string serializedPayloads;
    for (const auto& payload : stanza->getPayloads()) {
        PayloadSerializer* serializer = payloadSerializers_->getPayloadSerializer(payload);
        if (serializer) {
            serializedPayloads += serializer->serialize(payload);
        }
        else {
            SWIFT_LOG(warning) << "Could not find serializer for " << typeid(*(payload.get())).name() << std::endl;
        }
    }
    serializedPayloads = String::sanitizeXMPPString(serializedPayloads);
    if (!serializedPayloads.empty()) {
        stanzaElement.addNode(std::make_shared<XMLRawTextNode>(serializedPayloads));
    }

    return createSafeByteArray(stanzaElement.serialize());
}

}
