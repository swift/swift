/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/IQ.h>

#include <Sluift/ElementConvertors/StanzaConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class IQConvertor : public StanzaConvertor<IQ> {
        public:
            IQConvertor(LuaElementConvertors* convertors);
            virtual ~IQConvertor();

            virtual boost::shared_ptr<IQ> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, boost::shared_ptr<IQ>) SWIFTEN_OVERRIDE;

            virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

            static std::string convertIQTypeToString(IQ::Type type);
            static IQ::Type convertIQTypeFromString(const std::string& type);

        private:
            LuaElementConvertors* convertors;
    };
}

