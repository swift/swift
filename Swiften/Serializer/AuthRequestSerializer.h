/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/AuthRequest.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
	class SWIFTEN_API AuthRequestSerializer : public GenericElementSerializer<AuthRequest> {
		public:
			AuthRequestSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement> element)  const;
	};
}
