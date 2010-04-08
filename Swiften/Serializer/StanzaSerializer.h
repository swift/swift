/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_STANZASERIALIZER_H
#define SWIFTEN_STANZASERIALIZER_H

#include "Swiften/Elements/Stanza.h"
#include "Swiften/Serializer/ElementSerializer.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class PayloadSerializerCollection;
	class XMLElement;

	class StanzaSerializer : public ElementSerializer {
		public:
			StanzaSerializer(const String& tag, PayloadSerializerCollection* payloadSerializers);

			virtual String serialize(boost::shared_ptr<Element>) const;
			virtual void setStanzaSpecificAttributes(boost::shared_ptr<Element>, XMLElement&) const = 0;

		private:
			String tag_;
			PayloadSerializerCollection* payloadSerializers_;
	};
}

#endif
