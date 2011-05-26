/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/StreamInitiationParser.h>

#include <boost/lexical_cast.hpp>
#include <boost/cast.hpp>

#include <Swiften/Parser/PayloadParsers/FormParserFactory.h>
#include <Swiften/Parser/PayloadParsers/FormParser.h>
#include <Swiften/Base/foreach.h>


#define FILE_TRANSFER_NS "http://jabber.org/protocol/si/profile/file-transfer"
#define FEATURE_NEG_NS "http://jabber.org/protocol/feature-neg"

namespace Swift {

StreamInitiationParser::StreamInitiationParser() : level(TopLevel), formParser(0), inFile(false), inFeature(false)  {
	formParserFactory = new FormParserFactory();
}

StreamInitiationParser::~StreamInitiationParser() {
	delete formParserFactory;
}

void StreamInitiationParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (level == TopLevel) {
		getPayloadInternal()->setID(attributes.getAttribute("id"));
		if (!attributes.getAttribute("profile").empty()) {
			getPayloadInternal()->setIsFileTransfer(attributes.getAttribute("profile") == FILE_TRANSFER_NS);
		}
	}
	else if (level == PayloadLevel) {
		if (element == "file") {
			inFile = true;
			currentFile = StreamInitiationFileInfo();
			currentFile.setName(attributes.getAttribute("name"));
			try {
				currentFile.setSize(boost::lexical_cast<int>(attributes.getAttribute("size")));
			}
			catch (boost::bad_lexical_cast&) {
			}
		}
		else if (element == "feature" && ns == FEATURE_NEG_NS) {
			inFeature = true;
		}
	}
	else if (level == FileOrFeatureLevel) {
		if (inFile && element == "desc") {
			currentText.clear();
		}
		else if (inFeature && formParserFactory->canParse(element, ns, attributes)) {
			assert(!formParser);
			formParser = boost::polymorphic_downcast<FormParser*>(formParserFactory->createPayloadParser());
		}
	}

	if (formParser) {
		formParser->handleStartElement(element, ns, attributes);
	}
	++level;
}

void StreamInitiationParser::handleEndElement(const std::string& element, const std::string& ns) {
	--level;
	if (formParser) {
		formParser->handleEndElement(element, ns);
	}
	if (level == TopLevel) {
	}
	else if (level == PayloadLevel) {
		if (element == "file") {
			getPayloadInternal()->setFileInfo(currentFile);
			inFile = false;
		}
		else if (element == "feature" && ns == FEATURE_NEG_NS) {
			inFeature = false;
		}
	}
	else if (level == FileOrFeatureLevel) {
		if (inFile && element == "desc") {
			currentFile.setDescription(currentText);
		}
		else if (formParser) {
			Form::ref form = formParser->getPayloadInternal();
			if (form) {
				FormField::ref field = boost::dynamic_pointer_cast<FormField>(form->getField("stream-method"));
				if (field) {
					if (form->getType() == Form::FormType) {
						foreach (const FormField::Option& option, field->getOptions()) {
							getPayloadInternal()->addProvidedMethod(option.value);
						}
					}
					else if (form->getType() == Form::SubmitType) {
						if (field->getRawValues().size() > 0) {
							getPayloadInternal()->setRequestedMethod(field->getRawValues()[0]);
						}
					}
				}
			}
			delete formParser;
			formParser = NULL;
		}
	}
}

void StreamInitiationParser::handleCharacterData(const std::string& data) {
	if (formParser) {
		formParser->handleCharacterData(data);
	}
	else {
		currentText += data;
	}
}


}
