/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
	class StreamManagementEnabledSerializer : public GenericElementSerializer<StreamManagementEnabled> {
		public:
			StreamManagementEnabledSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<Element>) const;
	};
}
