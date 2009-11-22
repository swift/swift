#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/AuthSuccess.h"
#include "Swiften/Serializer/GenericElementSerializer.h"

namespace Swift {
	class AuthSuccessSerializer : public GenericElementSerializer<AuthSuccess> {
		public:
			AuthSuccessSerializer();

			virtual String serialize(boost::shared_ptr<Element> element)  const;
	};
}
