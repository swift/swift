/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/Tree/NullParserElement.h>

namespace Swift {

std::shared_ptr<NullParserElement> NullParserElement::element = std::make_shared<NullParserElement>();

}
