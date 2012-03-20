/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/StreamFeaturesSerializer.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Base/foreach.h>

namespace Swift {

StreamFeaturesSerializer::StreamFeaturesSerializer() {
}

SafeByteArray StreamFeaturesSerializer::serialize(boost::shared_ptr<Element> element)  const {
	boost::shared_ptr<StreamFeatures> streamFeatures(boost::dynamic_pointer_cast<StreamFeatures>(element));

	XMLElement streamFeaturesElement("stream:features");
	if (streamFeatures->hasStartTLS()) {
		streamFeaturesElement.addNode(boost::make_shared<XMLElement>("starttls", "urn:ietf:params:xml:ns:xmpp-tls"));
	}
	if (!streamFeatures->getCompressionMethods().empty()) {
		boost::shared_ptr<XMLElement> compressionElement(new XMLElement("compression", "http://jabber.org/features/compress"));
		foreach(const std::string& method, streamFeatures->getCompressionMethods()) {
			boost::shared_ptr<XMLElement> methodElement(new XMLElement("method"));
			methodElement->addNode(boost::make_shared<XMLTextNode>(method));
			compressionElement->addNode(methodElement);
		}
		streamFeaturesElement.addNode(compressionElement);
	}
	if (!streamFeatures->getAuthenticationMechanisms().empty()) {
		boost::shared_ptr<XMLElement> mechanismsElement(new XMLElement("mechanisms", "urn:ietf:params:xml:ns:xmpp-sasl"));
		foreach(const std::string& mechanism, streamFeatures->getAuthenticationMechanisms()) {
			boost::shared_ptr<XMLElement> mechanismElement(new XMLElement("mechanism"));
			mechanismElement->addNode(boost::make_shared<XMLTextNode>(mechanism));
			mechanismsElement->addNode(mechanismElement);
		}
		streamFeaturesElement.addNode(mechanismsElement);
	}
	if (streamFeatures->hasResourceBind()) {
		streamFeaturesElement.addNode(boost::make_shared<XMLElement>("bind", "urn:ietf:params:xml:ns:xmpp-bind"));
	}
	if (streamFeatures->hasSession()) {
		streamFeaturesElement.addNode(boost::make_shared<XMLElement>("session", "urn:ietf:params:xml:ns:xmpp-session"));
	}
	if (streamFeatures->hasStreamManagement()) {
		streamFeaturesElement.addNode(boost::make_shared<XMLElement>("sm", "urn:xmpp:sm:2"));
	}
	if (streamFeatures->hasRosterVersioning()) {
		streamFeaturesElement.addNode(boost::make_shared<XMLElement>("ver", "urn:xmpp:features:rosterver"));
	}
	return createSafeByteArray(streamFeaturesElement.serialize());
}

}
