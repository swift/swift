/*
 * Copyright (c) 2010-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>
#include <Swiften/Elements/StreamType.h>
#include <string>
#include <Swiften/Serializer/ElementSerializer.h>

namespace Swift {
	class PayloadSerializerCollection;
	class CompressRequestSerializer;
	class ProtocolHeader;

	class SWIFTEN_API XMPPSerializer {
		public:
			XMPPSerializer(PayloadSerializerCollection*, StreamType type, bool setExplictNSonTopLevelElements);

			std::string serializeHeader(const ProtocolHeader&) const;
			SafeByteArray serializeElement(boost::shared_ptr<ToplevelElement> stanza) const;
			std::string serializeFooter() const;
		
		private:
			std::string getDefaultNamespace() const;

		private:
			StreamType type_;
			std::vector< boost::shared_ptr<ElementSerializer> > serializers_;
	};
}
