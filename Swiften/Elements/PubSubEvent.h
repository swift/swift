/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/ContainerPayload.h>
#include <Swiften/Elements/PubSubEventPayload.h>

namespace Swift {
	class SWIFTEN_API PubSubEvent : public ContainerPayload<PubSubEventPayload> {
		public:
			PubSubEvent();
			virtual ~PubSubEvent();
	};
}
