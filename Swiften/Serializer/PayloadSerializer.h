#ifndef SWIFTEN_PAYLOADSERIALIZER_H
#define SWIFTEN_PAYLOADSERIALIZER_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class PayloadSerializer {
		public:
			virtual ~PayloadSerializer();

			virtual bool canSerialize(boost::shared_ptr<Payload>) const = 0;
			virtual String serialize(boost::shared_ptr<Payload>) const = 0;
	};
}

#endif
