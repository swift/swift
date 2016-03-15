/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/ResultSet.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
	class LuaElementConvertors;

	class ResultSetConvertor : public GenericLuaElementConvertor<ResultSet> {
		public:
			ResultSetConvertor();
			virtual ~ResultSetConvertor();

			virtual boost::shared_ptr<ResultSet> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<ResultSet>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;
	};
}

