/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
	class SWIFTEN_API PubSubEventPayload : public Payload {
		public:
			virtual ~PubSubEventPayload();
	};
}
