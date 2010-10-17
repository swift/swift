/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/FormSerializer.h"

#include <boost/shared_ptr.hpp>
#include <iostream>

#include "Swiften/Base/foreach.h"
#include "Swiften/Base/String.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"
#include "Swiften/Serializer/XML/XMLRawTextNode.h"

using namespace Swift;

namespace {
	template<typename T> void serializeValueAsString(boost::shared_ptr<FormField> field, boost::shared_ptr<XMLElement> parent) {
		String value = boost::dynamic_pointer_cast<T>(field)->getValue();
		if (!value.isEmpty()) {
			boost::shared_ptr<XMLElement> valueElement(new XMLElement("value"));
			valueElement->addNode(XMLTextNode::create(value));
			parent->addNode(valueElement);
		}
	}
}


namespace Swift {

FormSerializer::FormSerializer() : GenericPayloadSerializer<Form>() {
}

String FormSerializer::serializePayload(boost::shared_ptr<Form> form)  const {
	boost::shared_ptr<XMLElement> formElement(new XMLElement("x", "jabber:x:data"));
	String type;
	switch (form->getType()) {
		case Form::FormType: type = "form"; break;
		case Form::SubmitType: type = "submit"; break;
		case Form::CancelType: type = "cancel"; break;
		case Form::ResultType: type = "result"; break;
	}
	formElement->setAttribute("type", type);
	if (!form->getTitle().isEmpty()) {
		multiLineify(form->getTitle(), "title", formElement);
	}
	if (!form->getInstructions().isEmpty()) {
		multiLineify(form->getInstructions(), "instructions", formElement);
	}
	foreach(boost::shared_ptr<FormField> field, form->getFields()) {
		formElement->addNode(fieldToXML(field));
	}
	return formElement->serialize();
}

boost::shared_ptr<XMLElement> FormSerializer::fieldToXML(boost::shared_ptr<FormField> field) const {
	boost::shared_ptr<XMLElement> fieldElement(new XMLElement("field"));
	if (!field->getName().isEmpty()) {
		fieldElement->setAttribute("var", field->getName());
	}
	if (!field->getLabel().isEmpty()) {
		fieldElement->setAttribute("label", field->getLabel());
	}
	if (field->getRequired()) {
		fieldElement->addNode(boost::shared_ptr<XMLElement>(new XMLElement("required")));
	}
	if (!field->getDescription().isEmpty()) {
		boost::shared_ptr<XMLElement> descriptionElement(new XMLElement("desc"));
		descriptionElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(field->getDescription())));
		fieldElement->addNode(descriptionElement);
	}

	// Set the value and type
	String fieldType;
	if (boost::dynamic_pointer_cast<BooleanFormField>(field)) {
		fieldType = "boolean";
		boost::shared_ptr<XMLElement> valueElement(new XMLElement("value"));
		valueElement->addNode(XMLTextNode::create(boost::dynamic_pointer_cast<BooleanFormField>(field)->getValue() ? "1" : "0"));
		fieldElement->addNode(valueElement);
	}
	else if (boost::dynamic_pointer_cast<FixedFormField>(field)) {
		fieldType = "fixed";
		serializeValueAsString<FixedFormField>(field, fieldElement);
	}
	else if (boost::dynamic_pointer_cast<HiddenFormField>(field)) {
		fieldType = "hidden";
		serializeValueAsString<HiddenFormField>(field, fieldElement);
	}
	else if (boost::dynamic_pointer_cast<ListSingleFormField>(field)) {
		fieldType = "list-single";
		serializeValueAsString<ListSingleFormField>(field, fieldElement);
	}
	else if (boost::dynamic_pointer_cast<TextPrivateFormField>(field)) {
		fieldType = "text-private";
		serializeValueAsString<TextPrivateFormField>(field, fieldElement);
	}
	else if (boost::dynamic_pointer_cast<TextSingleFormField>(field)) {
		fieldType = "text-single";
		serializeValueAsString<TextSingleFormField>(field, fieldElement);
	}
	else if (boost::dynamic_pointer_cast<JIDMultiFormField>(field)) {
		fieldType = "jid-multi";
		std::vector<JID> jids = boost::dynamic_pointer_cast<JIDMultiFormField>(field)->getValue();
		foreach(const JID& jid, jids) {
			boost::shared_ptr<XMLElement> valueElement(new XMLElement("value"));
			valueElement->addNode(XMLTextNode::create(jid.toString()));
			fieldElement->addNode(valueElement);
		}
	}
	else if (boost::dynamic_pointer_cast<JIDSingleFormField>(field)) {
		fieldType = "jid-single";
		boost::shared_ptr<XMLElement> valueElement(new XMLElement("value"));
		valueElement->addNode(XMLTextNode::create(boost::dynamic_pointer_cast<JIDSingleFormField>(field)->getValue().toString()));
		fieldElement->addNode(valueElement);
	}
	else if (boost::dynamic_pointer_cast<ListMultiFormField>(field)) {
		fieldType = "list-multi";
		std::vector<String> lines = boost::dynamic_pointer_cast<ListMultiFormField>(field)->getValue();
		foreach(const String& line, lines) {
			boost::shared_ptr<XMLElement> valueElement(new XMLElement("value"));
			valueElement->addNode(XMLTextNode::create(line));
			fieldElement->addNode(valueElement);
		}
	}
	else if (boost::dynamic_pointer_cast<TextMultiFormField>(field)) {
		fieldType = "text-multi";
		multiLineify(boost::dynamic_pointer_cast<TextMultiFormField>(field)->getValue(), "value", fieldElement);
	}
	else if (boost::dynamic_pointer_cast<UntypedFormField>(field)) {
		std::vector<String> lines = boost::dynamic_pointer_cast<UntypedFormField>(field)->getValue();
		foreach(const String& line, lines) {
			boost::shared_ptr<XMLElement> valueElement(new XMLElement("value"));
			valueElement->addNode(XMLTextNode::create(line));
			fieldElement->addNode(valueElement);
		}
	}
	else {
		assert(false);
	}
	if (!fieldType.isEmpty()) {
		fieldElement->setAttribute("type", fieldType);
	}

	foreach (const FormField::Option& option, field->getOptions()) {
		boost::shared_ptr<XMLElement> optionElement(new XMLElement("option"));
		if (!option.label.isEmpty()) {
			optionElement->setAttribute("label", option.label);
		}

		boost::shared_ptr<XMLElement> valueElement(new XMLElement("value"));
		valueElement->addNode(XMLTextNode::create(option.value));
		optionElement->addNode(valueElement);

		fieldElement->addNode(optionElement);
	}

	return fieldElement;
}

void FormSerializer::multiLineify(const String& text, const String& elementName, boost::shared_ptr<XMLElement> element) const {
	String unRdText(text);
	unRdText.removeAll('\r');
	std::vector<String> lines = unRdText.split('\n');
	foreach (String line, lines) {
		boost::shared_ptr<XMLElement> lineElement(new XMLElement(elementName));
		lineElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(line)));
		element->addNode(lineElement);
	}
}

}
