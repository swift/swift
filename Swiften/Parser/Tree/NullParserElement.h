/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/Parser/Tree/ParserElement.h>

namespace Swift {
	class NullParserElement : public ParserElement {
		public:
			NullParserElement() : ParserElement("", "", AttributeMap()) {}

			virtual operator bool() { return false; }

			static boost::shared_ptr<NullParserElement> element;
	};
}
