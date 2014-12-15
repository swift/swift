/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
	class StreamManagementEnabledSerializer : public GenericElementSerializer<StreamManagementEnabled> {
		public:
			StreamManagementEnabledSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement>) const;
	};
}
