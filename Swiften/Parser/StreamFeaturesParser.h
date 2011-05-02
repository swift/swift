/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/StreamFeatures.h>

namespace Swift {
	class StreamFeaturesParser : public GenericElementParser<StreamFeatures> {
		public:
			StreamFeaturesParser();

		private:
			void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes);
			void handleEndElement(const std::string& element, const std::string& ns);
			void handleCharacterData(const std::string& data);

		private:
			int currentDepth_;
			std::string currentText_;
			bool inMechanisms_;
			bool inMechanism_;
			bool inCompression_;
			bool inCompressionMethod_;
	};
}
