/*
 * Copyright (c) 2010-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API Error {
		public:
			Error() {}
			SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(Error)
			virtual ~Error();
	};
}
