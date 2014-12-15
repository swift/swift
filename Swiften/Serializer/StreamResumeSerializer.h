/*
 * Copyright (c) 2011-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/StreamResume.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
	class StreamResumeSerializer : public GenericElementSerializer<StreamResume> {
		public:
			StreamResumeSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement>) const;
	};
}
