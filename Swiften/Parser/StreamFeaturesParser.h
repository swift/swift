/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_STREAMFEATURESPARSER_H
#define SWIFTEN_STREAMFEATURESPARSER_H

#include "Swiften/Base/String.h"
#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/StreamFeatures.h"

namespace Swift {
	class StreamFeaturesParser : public GenericElementParser<StreamFeatures> {
		public:
			StreamFeaturesParser();

		private:
			void handleStartElement(const String& element, const String& ns, const AttributeMap& attributes);
			void handleEndElement(const String& element, const String& ns);
			void handleCharacterData(const String& data);

		private:
			int currentDepth_;
			String currentText_;
			bool inMechanisms_;
			bool inMechanism_;
			bool inCompression_;
			bool inCompressionMethod_;
	};
}

#endif
