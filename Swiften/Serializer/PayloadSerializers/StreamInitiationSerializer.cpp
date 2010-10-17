/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/StreamInitiationSerializer.h"

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "Swiften/Base/foreach.h"
#include "Swiften/Serializer/XML/XMLElement.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"
#include "Swiften/Serializer/XML/XMLRawTextNode.h"
#include "Swiften/Serializer/PayloadSerializerCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FormSerializer.h"


#define FILE_TRANSFER_NS "http://jabber.org/protocol/si/profile/file-transfer"
#define FEATURE_NEG_NS "http://jabber.org/protocol/feature-neg"

namespace Swift {

StreamInitiationSerializer::StreamInitiationSerializer() {
}

String StreamInitiationSerializer::serializePayload(boost::shared_ptr<StreamInitiation> streamInitiation)	const {
	assert(streamInitiation->getIsFileTransfer());

	XMLElement siElement("si", "http://jabber.org/protocol/si");
	if (!streamInitiation->getID().isEmpty()) {
		siElement.setAttribute("id", streamInitiation->getID());
	}
	siElement.setAttribute("profile", FILE_TRANSFER_NS);

	if (streamInitiation->getFileInfo()) {
		StreamInitiation::FileInfo file = *streamInitiation->getFileInfo();
		boost::shared_ptr<XMLElement> fileElement(new XMLElement("file", "http://jabber.org/protocol/si/profile/file-transfer"));
		fileElement->setAttribute("name", file.name);
		if (file.size != -1) {
			fileElement->setAttribute("size", boost::lexical_cast<std::string>(file.size));
		}
		if (!file.description.isEmpty()) {
			boost::shared_ptr<XMLElement> descElement(new XMLElement("desc"));
			descElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(file.description)));
			fileElement->addNode(descElement);
		}
		siElement.addNode(fileElement);
	}

	boost::shared_ptr<XMLElement> featureElement(new XMLElement("feature", "http://jabber.org/protocol/feature-neg"));
	if (streamInitiation->getProvidedMethods().size() > 0) {
		Form::ref form(new Form(Form::FormType));
		ListSingleFormField::ref field = ListSingleFormField::create();
		field->setName("stream-method");
		foreach(const String& method, streamInitiation->getProvidedMethods()) {
			field->addOption(FormField::Option("", method));
		}
		form->addField(field);
		featureElement->addNode(boost::shared_ptr<XMLRawTextNode>(new XMLRawTextNode(FormSerializer().serialize(form))));
	}
	else if (!streamInitiation->getRequestedMethod().isEmpty()) {
		Form::ref form(new Form(Form::SubmitType));
		ListSingleFormField::ref field = ListSingleFormField::create(streamInitiation->getRequestedMethod());
		field->setName("stream-method");
		form->addField(field);
		featureElement->addNode(boost::shared_ptr<XMLRawTextNode>(new XMLRawTextNode(FormSerializer().serialize(form))));
	}
	siElement.addNode(featureElement);
	return siElement.serialize();
}

}
