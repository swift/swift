/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
