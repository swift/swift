/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_XMLNode_H
#define SWIFTEN_XMLNode_H

#include "Swiften/Base/String.h"

namespace Swift {
	class XMLNode {
		public:
			virtual ~XMLNode();

			virtual String serialize() = 0;
	};
}

#endif
