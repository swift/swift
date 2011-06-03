/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/StreamResumed.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
	class StreamResumedSerializer : public GenericElementSerializer<StreamResumed> {
		public:
			StreamResumedSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<Element>) const;
	};
}
