/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/DefaultElementConvertor.h>

#include <iostream>
#include <string>
#include <typeinfo>

using namespace Swift;

DefaultElementConvertor::DefaultElementConvertor() {
}

DefaultElementConvertor::~DefaultElementConvertor() {
}

boost::shared_ptr<Element> DefaultElementConvertor::convertFromLua(lua_State*, int, const std::string& type) {
	std::cerr << "Warning: Unable to convert type '" << type << "'" << std::endl;
	return boost::shared_ptr<Element>();
}

boost::optional<std::string> DefaultElementConvertor::convertToLua(lua_State*, boost::shared_ptr<Element>) {
	// Should have been handled by the raw XML convertor
	assert(false);
	return NO_RESULT;
}
