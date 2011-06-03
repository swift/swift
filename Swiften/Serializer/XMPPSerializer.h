/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <Swiften/Elements/Element.h>
#include <Swiften/Elements/StreamType.h>
#include <string>
#include <Swiften/Serializer/ElementSerializer.h>

namespace Swift {
	class PayloadSerializerCollection;
	class CompressRequestSerializer;
	class ProtocolHeader;

	class XMPPSerializer {
		public:
			XMPPSerializer(PayloadSerializerCollection*, StreamType type);

			std::string serializeHeader(const ProtocolHeader&) const;
			SafeByteArray serializeElement(boost::shared_ptr<Element> stanza) const;
			std::string serializeFooter() const;
		
		private:
			std::string getDefaultNamespace() const;

		private:
			StreamType type_;
			std::vector< boost::shared_ptr<ElementSerializer> > serializers_;
	};
}
