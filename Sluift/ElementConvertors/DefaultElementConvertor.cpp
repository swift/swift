/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/DefaultElementConvertor.h>

#include <iostream>
#include <typeinfo>
#include <string>

using namespace Swift;

DefaultElementConvertor::DefaultElementConvertor() {
}

DefaultElementConvertor::~DefaultElementConvertor() {
}

boost::shared_ptr<Payload> DefaultElementConvertor::convertFromLua(lua_State*, int, const std::string& type) {
	std::cerr << "Warning: Unable to convert type '" << type << "'" << std::endl;
	return boost::shared_ptr<Payload>();
}

boost::optional<std::string> DefaultElementConvertor::convertToLua(lua_State*, boost::shared_ptr<Payload>) {
	// Should have been handled by the raw XML convertor
	assert(false);
	return NO_RESULT;
}
