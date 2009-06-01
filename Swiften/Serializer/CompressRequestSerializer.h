#ifndef SWIFTEN_COMPRESSREQUESTSERIALIZER_H
#define SWIFTEN_COMPRESSREQUESTSERIALIZER_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Serializer/ElementSerializer.h"

namespace Swift {
	class CompressRequestSerializer : public ElementSerializer {
		public:
			CompressRequestSerializer();

			virtual String serialize(boost::shared_ptr<Element> element)  const;
			virtual bool canSerialize(boost::shared_ptr<Element> element) const;
	};
}

#endif
