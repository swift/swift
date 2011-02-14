/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_STANZASERIALIZER_H
#define SWIFTEN_STANZASERIALIZER_H

#include "Swiften/Elements/Stanza.h"
#include "Swiften/Serializer/ElementSerializer.h"
#include <string>

namespace Swift {
	class PayloadSerializerCollection;
	class XMLElement;

	class StanzaSerializer : public ElementSerializer {
		public:
			StanzaSerializer(const std::string& tag, PayloadSerializerCollection* payloadSerializers);

			virtual std::string serialize(boost::shared_ptr<Element>) const;
			virtual void setStanzaSpecificAttributes(boost::shared_ptr<Element>, XMLElement&) const = 0;

		private:
			std::string tag_;
			PayloadSerializerCollection* payloadSerializers_;
	};
}

#endif
