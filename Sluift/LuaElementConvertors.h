/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

struct lua_State;

namespace Swift {
	class LuaElementConvertor;
	class Payload;

	class LuaElementConvertors {
		public:
			LuaElementConvertors();
			virtual ~LuaElementConvertors();

			boost::shared_ptr<Payload> convertFromLua(lua_State*, int index);
			int convertToLua(lua_State*, boost::shared_ptr<Payload>);

			/**
			 * Adds a toplevel type+data table with the given type.
			 */
			boost::shared_ptr<Payload> convertFromLuaUntyped(lua_State*, int index, const std::string& type);
			
			/**
			 * Strips the toplevel type+data table, and only return the
			 * data.
			 */
			int convertToLuaUntyped(lua_State*, boost::shared_ptr<Payload>);

			const std::vector< boost::shared_ptr<LuaElementConvertor> >& getConvertors() const {
				return convertors;
			}

		private:
			boost::optional<std::string> doConvertToLuaUntyped(lua_State*, boost::shared_ptr<Payload>);
			void registerConvertors();

		private:
			std::vector< boost::shared_ptr<LuaElementConvertor> > convertors;
	};
}
