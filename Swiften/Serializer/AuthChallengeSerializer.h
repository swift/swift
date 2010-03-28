#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/AuthChallenge.h"
#include "Swiften/Serializer/GenericElementSerializer.h"

namespace Swift {
	class AuthChallengeSerializer : public GenericElementSerializer<AuthChallenge> {
		public:
			AuthChallengeSerializer();

			virtual String serialize(boost::shared_ptr<Element> element)  const;
	};
}
