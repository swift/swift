#ifndef SWIFTEN_CompressFailureSerializer_H
#define SWIFTEN_CompressFailureSerializer_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/CompressFailure.h"
#include "Swiften/Serializer/GenericElementSerializer.h"
#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {
	class CompressFailureSerializer : public GenericElementSerializer<CompressFailure> {
		public:
			CompressFailureSerializer() : GenericElementSerializer<CompressFailure>() {
			}

			virtual String serialize(boost::shared_ptr<Element>) const {
				return XMLElement("failure", "http://jabber.org/protocol/compress").serialize();
			}
	};
}

#endif
