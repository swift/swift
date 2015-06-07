/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamResumed.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
	class SWIFTEN_API StreamResumedSerializer : public GenericElementSerializer<StreamResumed> {
		public:
			StreamResumedSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement>) const;
	};
}
