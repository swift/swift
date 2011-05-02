/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/UnknownElement.h>

namespace Swift {
	class UnknownElementParser : public GenericElementParser<UnknownElement> {
		public:
			UnknownElementParser() : GenericElementParser<UnknownElement>() {}
	};
}
