/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/StreamFeaturesSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

StreamFeaturesSerializer::StreamFeaturesSerializer() {
}

SafeByteArray StreamFeaturesSerializer::serialize(std::shared_ptr<ToplevelElement> element)  const {
    std::shared_ptr<StreamFeatures> streamFeatures(std::dynamic_pointer_cast<StreamFeatures>(element));

    XMLElement streamFeaturesElement("stream:features");
    if (streamFeatures->hasStartTLS()) {
        streamFeaturesElement.addNode(std::make_shared<XMLElement>("starttls", "urn:ietf:params:xml:ns:xmpp-tls"));
    }
    if (!streamFeatures->getCompressionMethods().empty()) {
        std::shared_ptr<XMLElement> compressionElement(new XMLElement("compression", "http://jabber.org/features/compress"));
        for (const auto& method : streamFeatures->getCompressionMethods()) {
            std::shared_ptr<XMLElement> methodElement(new XMLElement("method"));
            methodElement->addNode(std::make_shared<XMLTextNode>(method));
            compressionElement->addNode(methodElement);
        }
        streamFeaturesElement.addNode(compressionElement);
    }
    if (!streamFeatures->getAuthenticationMechanisms().empty()) {
        std::shared_ptr<XMLElement> mechanismsElement(new XMLElement("mechanisms", "urn:ietf:params:xml:ns:xmpp-sasl"));
        for (const auto& mechanism : streamFeatures->getAuthenticationMechanisms()) {
            std::shared_ptr<XMLElement> mechanismElement(new XMLElement("mechanism"));
            mechanismElement->addNode(std::make_shared<XMLTextNode>(mechanism));
            mechanismsElement->addNode(mechanismElement);
        }
        streamFeaturesElement.addNode(mechanismsElement);
    }
    if (streamFeatures->hasResourceBind()) {
        streamFeaturesElement.addNode(std::make_shared<XMLElement>("bind", "urn:ietf:params:xml:ns:xmpp-bind"));
    }
    if (streamFeatures->hasSession()) {
        streamFeaturesElement.addNode(std::make_shared<XMLElement>("session", "urn:ietf:params:xml:ns:xmpp-session"));
    }
    if (streamFeatures->hasStreamManagement()) {
        streamFeaturesElement.addNode(std::make_shared<XMLElement>("sm", "urn:xmpp:sm:2"));
    }
    if (streamFeatures->hasRosterVersioning()) {
        streamFeaturesElement.addNode(std::make_shared<XMLElement>("ver", "urn:xmpp:features:rosterver"));
    }
    return createSafeByteArray(streamFeaturesElement.serialize());
}

}
