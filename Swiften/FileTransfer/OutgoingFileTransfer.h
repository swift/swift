/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class OutgoingFileTransfer {
		public:
			virtual ~OutgoingFileTransfer();

			virtual void start() = 0;
			virtual void stop() = 0;
	};
}
