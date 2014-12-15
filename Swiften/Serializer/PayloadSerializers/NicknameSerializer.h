/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/Nickname.h>

namespace Swift {
	class NicknameSerializer : public GenericPayloadSerializer<Nickname> {
		public:
			NicknameSerializer();

			virtual std::string serializePayload(boost::shared_ptr<Nickname>)  const;
	};
}


