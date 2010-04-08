/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Elements/Element.h"
#include "Swiften/Base/String.h"
#include "Swiften/Serializer/ElementSerializer.h"

namespace Swift {
	class PayloadSerializerCollection;
	class CompressRequestSerializer;
	class ProtocolHeader;

	class XMPPSerializer {
		public:
			XMPPSerializer(PayloadSerializerCollection*);

			String serializeHeader(const ProtocolHeader&) const;
			String serializeElement(boost::shared_ptr<Element> stanza) const;
			String serializeFooter() const;
		
		private:
			std::vector< boost::shared_ptr<ElementSerializer> > serializers_;
	};
}
