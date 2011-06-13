/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/SecurityLabel.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class SerializingParser;

	class SecurityLabelParser : public GenericPayloadParser<SecurityLabel> {
		public:
			SecurityLabelParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);
			boost::shared_ptr<SecurityLabel> getLabelPayload() const;
		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1,
				DisplayMarkingOrLabelLevel = 2,
				SecurityLabelLevel = 3
			};
			int level_;
			SerializingParser* labelParser_;
			std::string currentText_;
	};
}
