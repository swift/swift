/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/LuaElementConvertor.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

namespace Swift {
	class RawXMLElementConvertor : public LuaElementConvertor {
		public:
			RawXMLElementConvertor();
			virtual ~RawXMLElementConvertor();

			virtual boost::shared_ptr<Element> convertFromLua(lua_State*, int index, const std::string& type) SWIFTEN_OVERRIDE;
			virtual boost::optional<std::string> convertToLua(lua_State*, boost::shared_ptr<Element>) SWIFTEN_OVERRIDE;

		private:
			FullPayloadSerializerCollection serializers;
	};
}
