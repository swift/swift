/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/Tree/NullParserElement.h>

namespace Swift {

boost::shared_ptr<NullParserElement> NullParserElement::element = boost::make_shared<NullParserElement>();

}
