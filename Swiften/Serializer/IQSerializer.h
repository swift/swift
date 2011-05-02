/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericStanzaSerializer.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	class IQSerializer : public GenericStanzaSerializer<IQ> {
		public:
			IQSerializer(PayloadSerializerCollection* payloadSerializers) : 
					GenericStanzaSerializer<IQ>("iq", payloadSerializers) {}

		private:
			virtual void setStanzaSpecificAttributesGeneric(
					boost::shared_ptr<IQ> iq, 
					XMLElement& element) const {
				switch (iq->getType()) {
					case IQ::Get: element.setAttribute("type","get"); break;
					case IQ::Set: element.setAttribute("type","set"); break;
					case IQ::Result: element.setAttribute("type","result"); break;
					case IQ::Error: element.setAttribute("type","error"); break;
				}
			}
	};
}
