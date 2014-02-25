/*
 * Copyright (c) 2013-2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Stanza.h>
#include <Swiften/Serializer/ElementSerializer.h>
#include <string>

namespace Swift {
	class PayloadSerializerCollection;
	class XMLElement;

	class StanzaSerializer : public ElementSerializer {
		public:
			StanzaSerializer(const std::string& tag, PayloadSerializerCollection* payloadSerializers);

			virtual SafeByteArray serialize(boost::shared_ptr<Element> element) const;
			virtual SafeByteArray serialize(boost::shared_ptr<Element> element, const std::string& xmlns) const;
			virtual void setStanzaSpecificAttributes(boost::shared_ptr<Element>, XMLElement&) const = 0;

		private:
			std::string tag_;
			PayloadSerializerCollection* payloadSerializers_;
	};
}
