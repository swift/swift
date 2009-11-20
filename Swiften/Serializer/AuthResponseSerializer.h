#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/AuthResponse.h"
#include "Swiften/Serializer/GenericElementSerializer.h"

namespace Swift {
	class AuthResponseSerializer : public GenericElementSerializer<AuthResponse> {
		public:
			AuthResponseSerializer();

			virtual String serialize(boost::shared_ptr<Element> element)  const;
	};
}
