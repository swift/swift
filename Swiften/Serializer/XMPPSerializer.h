#ifndef SWIFTEN_XMPPSERIALIZER_H
#define SWIFTEN_XMPPSERIALIZER_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Elements/Element.h"
#include "Swiften/Base/String.h"
#include "Swiften/Serializer/ElementSerializer.h"

namespace Swift {
	class PayloadSerializerCollection;
	class CompressRequestSerializer;

	class XMPPSerializer {
		public:
			XMPPSerializer(PayloadSerializerCollection*);

			String serializeHeader(const String& from, const String& to, const String& id = "") const;
			String serializeElement(boost::shared_ptr<Element> stanza) const;
			String serializeFooter() const;
		
		private:
			std::vector< boost::shared_ptr<ElementSerializer> > serializers_;
	};
}

#endif
