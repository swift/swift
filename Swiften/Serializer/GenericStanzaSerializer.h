/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/StanzaSerializer.h>

namespace Swift {
	template<typename STANZA_TYPE>
	class GenericStanzaSerializer : public StanzaSerializer {
		public:
			GenericStanzaSerializer(const std::string& tag, PayloadSerializerCollection* payloadSerializers, const boost::optional<std::string>& explicitNS = boost::optional<std::string>()) : StanzaSerializer(tag, payloadSerializers, explicitNS) {}

			virtual bool canSerialize(boost::shared_ptr<ToplevelElement> element) const {
				return dynamic_cast<STANZA_TYPE*>(element.get()) != 0;
			}

			virtual void setStanzaSpecificAttributes(
					boost::shared_ptr<ToplevelElement> stanza, 
					XMLElement& element) const {
				setStanzaSpecificAttributesGeneric(
						boost::dynamic_pointer_cast<STANZA_TYPE>(stanza), element);
			}

			virtual void setStanzaSpecificAttributesGeneric(
					boost::shared_ptr<STANZA_TYPE>, 
					XMLElement&) const = 0;
	};
}
