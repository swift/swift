/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>

#include <memory>
#include <string>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/String.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

using namespace Swift;

namespace {
    template<typename T> void serializeValueAsString(std::shared_ptr<FormField> field, std::shared_ptr<XMLElement> parent) {
        std::string value = std::dynamic_pointer_cast<T>(field)->getValue();
        if (!value.empty()) {
            std::shared_ptr<XMLElement> valueElement(new XMLElement("value"));
            valueElement->addNode(XMLTextNode::create(value));
            parent->addNode(valueElement);
        }
    }
}

namespace Swift {

FormSerializer::FormSerializer() : GenericPayloadSerializer<Form>() {}

std::string FormSerializer::serializePayload(std::shared_ptr<Form> form)  const {
    if (!form) {
        return "";
    }

    std::shared_ptr<XMLElement> formElement(new XMLElement("x", "jabber:x:data"));
    std::string type;
    switch (form->getType()) {
        case Form::FormType: type = "form"; break;
        case Form::SubmitType: type = "submit"; break;
        case Form::CancelType: type = "cancel"; break;
        case Form::ResultType: type = "result"; break;
    }
    formElement->setAttribute("type", type);
    if (!form->getTitle().empty()) {
        multiLineify(form->getTitle(), "title", formElement);
    }
    if (!form->getInstructions().empty()) {
        multiLineify(form->getInstructions(), "instructions", formElement);
    }
    for (const auto& page : form->getPages()) {
        formElement->addNode(pageToXML(page));
    }
    for (const auto& field : form->getFields()) {
        formElement->addNode(fieldToXML(field, true));
    }
    if (!form->getReportedFields().empty()) {
        std::shared_ptr<XMLElement> reportedElement(new XMLElement("reported"));
        for (const auto& field : form->getReportedFields()) {
            reportedElement->addNode(fieldToXML(field, true));
        }
        formElement->addNode(reportedElement);
    }

    for (const auto& item : form->getItems()) {
        std::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
        for (const auto& field : item) {
            itemElement->addNode(fieldToXML(field, false));
        }
        formElement->addNode(itemElement);
    }

    for (const auto& text : form->getTextElements()) {
        formElement->addNode(textToXML(text));
    }

    for (const auto& field : fields_) {
        formElement->addNode(fieldToXML(field,true));
    }

    return formElement->serialize();
}

std::shared_ptr<XMLElement> FormSerializer::textToXML(std::shared_ptr<FormText> text) const {
    std::shared_ptr<XMLElement> textElement (new XMLElement("text"));
    textElement->addNode(std::make_shared<XMLTextNode>(text->getTextString()));
    return textElement;
}

std::shared_ptr<XMLElement> FormSerializer::fieldRefToXML(const std::string& ref) const {
    std::shared_ptr<XMLElement> fieldRefElement(new XMLElement("fieldref"));
    fieldRefElement->setAttribute("var", ref);
    return fieldRefElement;
}

std::shared_ptr<XMLElement> FormSerializer::pageToXML(std::shared_ptr<FormPage> page) const {
    std::shared_ptr<XMLElement> pageElement(new XMLElement("page"));
    pageElement->setAttribute("xmlns", "http://jabber.org/protocol/xdata-layout");
    if (!page->getLabel().empty()) {
        pageElement->setAttribute("label", page->getLabel());
    }
    for (const auto& text : page->getTextElements()) {
        pageElement->addNode(textToXML(text));
    }
    for (const auto& field : page->getFields()) {
        pageElement->addNode(fieldRefToXML(field->getName()));
        fields_.push_back(field);
    }
    for (const auto& reportedRef: page->getReportedRefs()) {
        (void)reportedRef;
        pageElement->addNode(std::make_shared<XMLElement>("reportedref"));
    }
    for (const auto& section : page->getChildSections()) {
        pageElement->addNode(sectionToXML(section));
    }
    return pageElement;
}

std::shared_ptr<XMLElement> FormSerializer::sectionToXML(std::shared_ptr<FormSection> section) const {
    std::shared_ptr<XMLElement> sectionElement(new XMLElement("section"));
    if (!section->getLabel().empty()) {
        sectionElement->setAttribute("label", section->getLabel());
    }
    for (const auto& text : section->getTextElements()) {
        sectionElement->addNode(textToXML(text));
    }
    for (const auto& field : section->getFields()) {
        sectionElement->addNode(fieldRefToXML(field->getName()));
        fields_.push_back(field);
    }
    for (const auto& reportedRef : section->getReportedRefs()) {
        (void)reportedRef;
        sectionElement->addNode(std::make_shared<XMLElement>("reportedref"));
    }
    for (const auto& childSection : section->getChildSections()) {
        sectionElement->addNode(sectionToXML(childSection));
    }
    return sectionElement;
}

std::shared_ptr<XMLElement> FormSerializer::fieldToXML(std::shared_ptr<FormField> field, bool withTypeAttribute) const {
    std::shared_ptr<XMLElement> fieldElement(new XMLElement("field"));
    if (!field->getName().empty()) {
        fieldElement->setAttribute("var", field->getName());
    }
    if (!field->getLabel().empty()) {
        fieldElement->setAttribute("label", field->getLabel());
    }
    if (field->getRequired()) {
        fieldElement->addNode(std::make_shared<XMLElement>("required"));
    }
    if (!field->getDescription().empty()) {
        std::shared_ptr<XMLElement> descriptionElement(new XMLElement("desc"));
        descriptionElement->addNode(std::make_shared<XMLTextNode>(field->getDescription()));
        fieldElement->addNode(descriptionElement);
    }

    // Set the value and type
    std::string fieldType;
    switch (field->getType()) {
        case FormField::UnknownType: fieldType = ""; break;
        case FormField::BooleanType: fieldType = "boolean"; break;
        case FormField::FixedType: fieldType = "fixed"; break;
        case FormField::HiddenType: fieldType = "hidden"; break;
        case FormField::ListSingleType: fieldType = "list-single"; break;
        case FormField::TextMultiType: fieldType = "text-multi"; break;
        case FormField::TextPrivateType: fieldType = "text-private"; break;
        case FormField::TextSingleType: fieldType = "text-single"; break;
        case FormField::JIDSingleType: fieldType = "jid-single"; break;
        case FormField::JIDMultiType: fieldType = "jid-multi"; break;
        case FormField::ListMultiType: fieldType = "list-multi"; break;
    }
    if (!fieldType.empty() && withTypeAttribute) {
        fieldElement->setAttribute("type", fieldType);
    }
    for (const auto& value : field->getValues()) {
        std::shared_ptr<XMLElement> valueElement = std::make_shared<XMLElement>("value");
        valueElement->addNode(std::make_shared<XMLTextNode>(value));
        fieldElement->addNode(valueElement);
    }

    for (const auto& option : field->getOptions()) {
        std::shared_ptr<XMLElement> optionElement(new XMLElement("option"));
        if (!option.label.empty()) {
            optionElement->setAttribute("label", option.label);
        }

        std::shared_ptr<XMLElement> valueElement(new XMLElement("value"));
        valueElement->addNode(XMLTextNode::create(option.value));
        optionElement->addNode(valueElement);
        fieldElement->addNode(optionElement);
    }
    return fieldElement;
}

void FormSerializer::multiLineify(const std::string& text, const std::string& elementName, std::shared_ptr<XMLElement> element) const {
    std::string unRdText(text);
    erase(unRdText, '\r');
    std::vector<std::string> lines = String::split(unRdText, '\n');
    for (const auto& line : lines) {
        std::shared_ptr<XMLElement> lineElement(new XMLElement(elementName));
        lineElement->addNode(std::make_shared<XMLTextNode>(line));
        element->addNode(lineElement);
    }
}
}
