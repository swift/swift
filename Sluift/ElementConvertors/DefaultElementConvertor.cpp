/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/DefaultElementConvertor.h>

#include <string>
#include <typeinfo>

#include <Swiften/Base/Log.h>

using namespace Swift;

DefaultElementConvertor::DefaultElementConvertor() {
}

DefaultElementConvertor::~DefaultElementConvertor() {
}

std::shared_ptr<Element> DefaultElementConvertor::convertFromLua(lua_State*, int, const std::string& type) {
    SWIFT_LOG(warning) << "Unable to convert type '" << type << "'" << std::endl;
    return std::shared_ptr<Element>();
}

boost::optional<std::string> DefaultElementConvertor::convertToLua(lua_State*, std::shared_ptr<Element>) {
    // Should have been handled by the raw XML convertor
    assert(false);
    return NO_RESULT;
}
