/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/ContainerPayload.h>

#include <Swiften/Elements/PubSubOwnerPayload.h>

namespace Swift {
	class SWIFTEN_API PubSubOwnerPubSub : public ContainerPayload<PubSubOwnerPayload> {
		public:
			PubSubOwnerPubSub();
			virtual ~PubSubOwnerPubSub();
	};
}
