/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_ApplicationMessageDisplay_H
#define SWIFTEN_ApplicationMessageDisplay_H

namespace Swift {
	class String;

	class ApplicationMessageDisplay {
		public:
			virtual ~ApplicationMessageDisplay();

			virtual void setMessage(const String& message) = 0;
	};
}

#endif
