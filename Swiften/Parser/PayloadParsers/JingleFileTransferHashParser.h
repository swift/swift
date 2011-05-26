/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/JingleFileTransferHash.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {

class JingleFileTransferHashParser : public GenericPayloadParser<JingleFileTransferHash> {
public:
	JingleFileTransferHashParser();
	
	virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
	virtual void handleEndElement(const std::string& element, const std::string&);
	virtual void handleCharacterData(const std::string& data);
	
private:
	std::string algo;
	std::string hash;
};

}
