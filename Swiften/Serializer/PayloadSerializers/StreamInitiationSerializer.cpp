/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/StreamInitiationSerializer.h>

#include <memory>
#include <memory>
#include <boost/lexical_cast.hpp>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>


#define FILE_TRANSFER_NS "http://jabber.org/protocol/si/profile/file-transfer"
#define FEATURE_NEG_NS "http://jabber.org/protocol/feature-neg"

namespace Swift {

StreamInitiationSerializer::StreamInitiationSerializer() {
}

std::string StreamInitiationSerializer::serializePayload(std::shared_ptr<StreamInitiation> streamInitiation)    const {
    assert(streamInitiation->getIsFileTransfer());

    XMLElement siElement("si", "http://jabber.org/protocol/si");
    if (!streamInitiation->getID().empty()) {
        siElement.setAttribute("id", streamInitiation->getID());
    }
    siElement.setAttribute("profile", FILE_TRANSFER_NS);

    if (streamInitiation->getFileInfo()) {
        StreamInitiationFileInfo file = *streamInitiation->getFileInfo();
        std::shared_ptr<XMLElement> fileElement(new XMLElement("file", "http://jabber.org/protocol/si/profile/file-transfer"));
        fileElement->setAttribute("name", file.getName());
        if (file.getSize() != 0) {
            fileElement->setAttribute("size", boost::lexical_cast<std::string>(file.getSize()));
        }
        if (!file.getDescription().empty()) {
            std::shared_ptr<XMLElement> descElement(new XMLElement("desc"));
            descElement->addNode(std::make_shared<XMLTextNode>(file.getDescription()));
            fileElement->addNode(descElement);
        }
        siElement.addNode(fileElement);
    }

    std::shared_ptr<XMLElement> featureElement(new XMLElement("feature", FEATURE_NEG_NS));
    if (streamInitiation->getProvidedMethods().size() > 0) {
        Form::ref form(new Form(Form::FormType));
        FormField::ref field = std::make_shared<FormField>(FormField::ListSingleType);
        field->setName("stream-method");
        for (const auto& method : streamInitiation->getProvidedMethods()) {
            field->addOption(FormField::Option("", method));
        }
        form->addField(field);
        featureElement->addNode(std::make_shared<XMLRawTextNode>(FormSerializer().serialize(form)));
    }
    else if (!streamInitiation->getRequestedMethod().empty()) {
        Form::ref form(new Form(Form::SubmitType));
        FormField::ref field = std::make_shared<FormField>(FormField::ListSingleType);
        field->addValue(streamInitiation->getRequestedMethod());
        field->setName("stream-method");
        form->addField(field);
        featureElement->addNode(std::make_shared<XMLRawTextNode>(FormSerializer().serialize(form)));
    }
    siElement.addNode(featureElement);
    return siElement.serialize();
}

}
