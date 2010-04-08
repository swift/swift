/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_MacOSXApplicationInitializer_H
#define SWIFTEN_MacOSXApplicationInitializer_H

namespace Swift {
	class MacOSXApplicationInitializer {
		public:
			MacOSXApplicationInitializer();
			~MacOSXApplicationInitializer();

		private:
			class Private;
			Private* d;
	};
}

#endif
