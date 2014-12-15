/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <iostream>
#include <string>

#include <Swiften/Base/String.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

namespace {
	template<typename T> void serializeValueAsString(boost::shared_ptr<FormField> field, boost::shared_ptr<XMLElement> parent) {
		std::string value = boost::dynamic_pointer_cast<T>(field)->getValue();
		if (!value.empty()) {
			boost::shared_ptr<XMLElement> valueElement(new XMLElement("value"));
			valueElement->addNode(XMLTextNode::create(value));
			parent->addNode(valueElement);
		}
	}
}


namespace Swift {

FormSerializer::FormSerializer() : GenericPayloadSerializer<Form>() {
}

std::string FormSerializer::serializePayload(boost::shared_ptr<Form> form)  const {
	if (!form) {
		return "";
	}

	boost::shared_ptr<XMLElement> formElement(new XMLElement("x", "jabber:x:data"));
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
	foreach(boost::shared_ptr<FormField> field, form->getFields()) {
		formElement->addNode(fieldToXML(field, true));
	}
	if (!form->getReportedFields().empty()) {
		boost::shared_ptr<XMLElement> reportedElement(new XMLElement("reported"));
		foreach(FormField::ref field, form->getReportedFields()) {
			reportedElement->addNode(fieldToXML(field, true));
		}
		formElement->addNode(reportedElement);
	}
	foreach(Form::FormItem item, form->getItems()) {
		boost::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
		foreach(FormField::ref field, item) {
			itemElement->addNode(fieldToXML(field, false));
		}
		formElement->addNode(itemElement);
	}

	return formElement->serialize();
}

boost::shared_ptr<XMLElement> FormSerializer::fieldToXML(boost::shared_ptr<FormField> field, bool withTypeAttribute) const {
	boost::shared_ptr<XMLElement> fieldElement(new XMLElement("field"));
	if (!field->getName().empty()) {
		fieldElement->setAttribute("var", field->getName());
	}
	if (!field->getLabel().empty()) {
		fieldElement->setAttribute("label", field->getLabel());
	}
	if (field->getRequired()) {
		fieldElement->addNode(boost::make_shared<XMLElement>("required"));
	}
	if (!field->getDescription().empty()) {
		boost::shared_ptr<XMLElement> descriptionElement(new XMLElement("desc"));
		descriptionElement->addNode(boost::make_shared<XMLTextNode>(field->getDescription()));
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
	foreach (const std::string& value, field->getValues()) {
		boost::shared_ptr<XMLElement> valueElement = boost::make_shared<XMLElement>("value");
		valueElement->addNode(boost::make_shared<XMLTextNode>(value));
		fieldElement->addNode(valueElement);
	}

	foreach (const FormField::Option& option, field->getOptions()) {
		boost::shared_ptr<XMLElement> optionElement(new XMLElement("option"));
		if (!option.label.empty()) {
			optionElement->setAttribute("label", option.label);
		}

		boost::shared_ptr<XMLElement> valueElement(new XMLElement("value"));
		valueElement->addNode(XMLTextNode::create(option.value));
		optionElement->addNode(valueElement);

		fieldElement->addNode(optionElement);
	}

	return fieldElement;
}

void FormSerializer::multiLineify(const std::string& text, const std::string& elementName, boost::shared_ptr<XMLElement> element) const {
	std::string unRdText(text);
	erase(unRdText, '\r');
	std::vector<std::string> lines = String::split(unRdText, '\n');
	foreach (std::string line, lines) {
		boost::shared_ptr<XMLElement> lineElement(new XMLElement(elementName));
		lineElement->addNode(boost::make_shared<XMLTextNode>(line));
		element->addNode(lineElement);
	}
}

}
