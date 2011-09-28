/*
 * Copyright (c) 2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cassert>

#include <Swiften/Parser/BOSHParser.h>
#include <Swiften/Parser/XMLParser.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>

namespace Swift {

BOSHParser::BOSHParser() :
	xmlParser_(0),
	level_(-1),
	parseErrorOccurred_(false)
{
	xmlParser_ = PlatformXMLParserFactory().createXMLParser(this);
}

BOSHParser::~BOSHParser() {
	delete xmlParser_;
}

bool BOSHParser::parse(const std::string& data) {
	bool xmlParseResult = xmlParser_->parse(data);
	return xmlParseResult && !parseErrorOccurred_;
}

void BOSHParser::handleStartElement(const std::string& /*element*/, const std::string& /*ns*/, const AttributeMap& attributes) {
	if (!parseErrorOccurred_) {
		if (level_ == BoshTopLevel) {
			boshBodyAttributes_ = attributes;
		}
	}
	++level_;
}

void BOSHParser::handleEndElement(const std::string& /*element*/, const std::string& /*ns*/) {
	assert(level_ > BoshTopLevel);
	--level_;
	if (!parseErrorOccurred_) {

	}
}

void BOSHParser::handleCharacterData(const std::string& /*data*/) {
	if (!parseErrorOccurred_) {

	}
}

}
