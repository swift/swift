#ifndef SWIFTEN_AuthRequestSerializer_H
#define SWIFTEN_AuthRequestSerializer_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/AuthRequest.h"
#include "Swiften/Serializer/GenericElementSerializer.h"

namespace Swift {
	class AuthRequestSerializer : public GenericElementSerializer<AuthRequest> {
		public:
			AuthRequestSerializer();

			virtual String serialize(boost::shared_ptr<Element> element)  const;
	};
}

#endif
