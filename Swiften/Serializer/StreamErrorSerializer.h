/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Elements/StreamError.h>

namespace Swift {
	class StreamErrorSerializer : public GenericElementSerializer<StreamError> {
		public:
			StreamErrorSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<Element> error) const;
	};
}
