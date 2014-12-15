/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

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
		typedef std::vector<Value> Array;
		
		inline boost::shared_ptr<Value> nilRef() {
			return boost::make_shared<Value>(Nil());
		}

		inline boost::shared_ptr<Value> valueRef(const std::string& value) {
			return boost::make_shared<Value>(value);
		}

		inline boost::shared_ptr<Value> intRef(int value) {
			return boost::make_shared<Value>(value);
		}

		inline boost::shared_ptr<Value> boolRef(bool value) {
			return boost::make_shared<Value>(value);
		}

		inline boost::shared_ptr<Value> valueRef(const Table& table) {
			return boost::make_shared<Value>(table);
		}

		inline boost::shared_ptr<Value> valueRef(const Array& array) {
		 	return boost::make_shared<Value>(array);
		}

		void pushValue(lua_State* state, const Value& value);
	}
}
