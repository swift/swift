/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/Tree/NullParserElement.h>

namespace Swift {

boost::shared_ptr<NullParserElement> NullParserElement::element = boost::make_shared<NullParserElement>();

}
