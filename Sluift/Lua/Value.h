/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>

struct lua_State;

namespace Swift {
	namespace Lua {
		struct Nil {};

		typedef boost::make_recursive_variant<
				Nil,
				bool,
				int, 
				std::string,
				std::vector< boost::recursive_variant_ >,
				std::map<std::string, boost::shared_ptr<boost::recursive_variant_> >
			>::type Value;

		typedef std::map<std::string, boost::shared_ptr<Value> > Table;
		
		void pushValue(lua_State* state, const Value& value);
	}
}
