/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/StreamInitiationFileInfo.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {

class StreamInitiationFileInfoParser : public GenericPayloadParser<StreamInitiationFileInfo> {
	public:
		StreamInitiationFileInfoParser();

		virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
		virtual void handleEndElement(const std::string& element, const std::string&);
		virtual void handleCharacterData(const std::string& data);
		
	private:
		int level;
		bool parseDescription;
		std::string desc;
};

}
