/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/Elements/Payload.h>

namespace Swift {
	class StartSession : public Payload {
		public:
			StartSession() {}
	};
}
