/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/InBandRegistrationPayloadParser.h>

#include <cassert>

#include <boost/cast.hpp>

#include <Swiften/Parser/PayloadParsers/FormParser.h>
#include <Swiften/Parser/PayloadParsers/FormParserFactory.h>

namespace Swift {

InBandRegistrationPayloadParser::InBandRegistrationPayloadParser() : level(TopLevel), formParser(nullptr)  {
    formParserFactory = new FormParserFactory();
}

InBandRegistrationPayloadParser::~InBandRegistrationPayloadParser() {
    delete formParserFactory;
}

void InBandRegistrationPayloadParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == TopLevel) {
    }
    else if (level == PayloadLevel) {
        if (element == "x" && ns == "jabber:x:data") {
            assert(!formParser);
            formParser = boost::polymorphic_downcast<FormParser*>(formParserFactory->createPayloadParser());
        }
        else {
            currentText.clear();
        }
    }

    if (formParser) {
        formParser->handleStartElement(element, ns, attributes);
    }

    ++level;
}

void InBandRegistrationPayloadParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;

    if (formParser) {
        formParser->handleEndElement(element, ns);
    }

    if (level == TopLevel) {
    }
    else if (level == PayloadLevel) {
        if (formParser) {
            getPayloadInternal()->setForm(formParser->getPayloadInternal());
            delete formParser;
            formParser = nullptr;
        }
        else if (element == "registered") {
            getPayloadInternal()->setRegistered(true);
        }
        else if (element == "remove") {
            getPayloadInternal()->setRemove(true);
        }
        else if (element == "instructions") {
            getPayloadInternal()->setInstructions(currentText);
        }
        else if (element == "username") {
            getPayloadInternal()->setUsername(currentText);
        }
        else if (element == "nick") {
            getPayloadInternal()->setNick(currentText);
        }
        else if (element == "password") {
            getPayloadInternal()->setPassword(currentText);
        }
        else if (element == "name") {
            getPayloadInternal()->setName(currentText);
        }
        else if (element == "first") {
            getPayloadInternal()->setFirst(currentText);
        }
        else if (element == "last") {
            getPayloadInternal()->setLast(currentText);
        }
        else if (element == "email") {
            getPayloadInternal()->setEMail(currentText);
        }
        else if (element == "address") {
            getPayloadInternal()->setAddress(currentText);
        }
        else if (element == "city") {
            getPayloadInternal()->setCity(currentText);
        }
        else if (element == "state") {
            getPayloadInternal()->setState(currentText);
        }
        else if (element == "zip") {
            getPayloadInternal()->setZip(currentText);
        }
        else if (element == "phone") {
            getPayloadInternal()->setPhone(currentText);
        }
        else if (element == "url") {
            getPayloadInternal()->setURL(currentText);
        }
        else if (element == "date") {
            getPayloadInternal()->setDate(currentText);
        }
        else if (element == "misc") {
            getPayloadInternal()->setMisc(currentText);
        }
        else if (element == "text") {
            getPayloadInternal()->setText(currentText);
        }
        else if (element == "key") {
            getPayloadInternal()->setKey(currentText);
        }
    }
}

void InBandRegistrationPayloadParser::handleCharacterData(const std::string& data) {
    if (formParser) {
        formParser->handleCharacterData(data);
    }
    else {
        currentText += data;
    }
}

}
