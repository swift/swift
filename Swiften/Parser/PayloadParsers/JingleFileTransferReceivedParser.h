/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/JingleFileTransferReceived.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {

class JingleFileTransferReceivedParser : public GenericPayloadParser<JingleFileTransferReceived> {
public:
	JingleFileTransferReceivedParser();
	
	virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
	virtual void handleEndElement(const std::string& element, const std::string&);
	virtual void handleCharacterData(const std::string& data);
	
private:
	boost::shared_ptr<PayloadParser> currentPayloadParser;
	int level;
};

}