#include "Swiften/Serializer/StanzaSerializer.h"

#include <sstream>
#include <typeinfo>
#include <iostream>

#include "Swiften/Serializer/XML/XMLElement.h"
#include "Swiften/Serializer/XML/XMLRawTextNode.h"
#include "Swiften/Serializer/PayloadSerializer.h"
#include "Swiften/Serializer/PayloadSerializerCollection.h"
#include "Swiften/Elements/Stanza.h"

namespace Swift {

StanzaSerializer::StanzaSerializer(const String& tag, PayloadSerializerCollection* payloadSerializers) : tag_(tag), payloadSerializers_(payloadSerializers) {
}

String StanzaSerializer::serialize(boost::shared_ptr<Element> element) const {
	boost::shared_ptr<Stanza> stanza(boost::dynamic_pointer_cast<Stanza>(element));

	XMLElement stanzaElement(tag_);
	if (stanza->getFrom().isValid()) {
		stanzaElement.setAttribute("from", stanza->getFrom());
	}
	if (stanza->getTo().isValid()) {
		stanzaElement.setAttribute("to", stanza->getTo());
	}
	if (!stanza->getID().isEmpty()) {
		stanzaElement.setAttribute("id", stanza->getID());
	}
	setStanzaSpecificAttributes(stanza, stanzaElement);

	String serializedPayloads;
	foreach (const boost::shared_ptr<Payload>& payload, stanza->getPayloads()) {
		PayloadSerializer* serializer = payloadSerializers_->getPayloadSerializer(payload);
		if (serializer) {
			serializedPayloads += serializer->serialize(payload);
		}
		else {
			std::cerr << "Could not find serializer for " << typeid(*(payload.get())).name() << std::endl;
		}
	}
	if (!serializedPayloads.isEmpty()) {
		stanzaElement.addNode(boost::shared_ptr<XMLNode>(new XMLRawTextNode(serializedPayloads)));
	}

	return stanzaElement.serialize();
}

}
