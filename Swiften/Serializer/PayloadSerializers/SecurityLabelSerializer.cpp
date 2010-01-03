#include "Swiften/Serializer/PayloadSerializers/SecurityLabelSerializer.h"
#include "Swiften/Base/foreach.h"
#include "Swiften/Serializer/XML/XMLRawTextNode.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"
#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {

SecurityLabelSerializer::SecurityLabelSerializer() : GenericPayloadSerializer<SecurityLabel>() {
}

String SecurityLabelSerializer::serializePayload(boost::shared_ptr<SecurityLabel> label)  const {
	XMLElement element("securitylabel", "urn:xmpp:sec-label:0");
	if (!label->getDisplayMarking().isEmpty()) {
		boost::shared_ptr<XMLElement> displayMarking(new XMLElement("displaymarking"));
		if (!label->getForegroundColor().isEmpty()) {
			displayMarking->setAttribute("fgcolor", label->getForegroundColor());
		}
		if (!label->getBackgroundColor().isEmpty()) {
			displayMarking->setAttribute("bgcolor", label->getBackgroundColor());
		}
		displayMarking->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(label->getDisplayMarking())));
		element.addNode(displayMarking);
	}

	boost::shared_ptr<XMLElement> labelElement(new XMLElement("label"));
	labelElement->addNode(boost::shared_ptr<XMLRawTextNode>(new XMLRawTextNode(label->getLabel())));
	element.addNode(labelElement);
	
	foreach(const String& equivalentLabel, label->getEquivalentLabels()) {
		boost::shared_ptr<XMLElement> equivalentLabelElement(new XMLElement("equivalentlabel"));
		equivalentLabelElement->addNode(boost::shared_ptr<XMLRawTextNode>(new XMLRawTextNode(equivalentLabel)));
		element.addNode(equivalentLabelElement);
	}
	return element.serialize();
}

}
