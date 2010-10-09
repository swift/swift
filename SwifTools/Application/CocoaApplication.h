/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class CocoaApplication {
		public:
			CocoaApplication();
			~CocoaApplication();

		private:
			class Private;
			Private* d;
	};
}
