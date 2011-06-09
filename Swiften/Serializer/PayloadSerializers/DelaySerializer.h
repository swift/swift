/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/Delay.h>

namespace Swift {
	class DelaySerializer : public GenericPayloadSerializer<Delay> {
		public:
			DelaySerializer();

			virtual std::string serializePayload(boost::shared_ptr<Delay>)  const;
	};
}


