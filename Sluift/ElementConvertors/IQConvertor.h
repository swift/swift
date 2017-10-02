/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/IQ.h>

#include <Sluift/ElementConvertors/StanzaConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class IQConvertor : public StanzaConvertor<IQ> {
        public:
            IQConvertor(LuaElementConvertors* convertors);
            virtual ~IQConvertor() override;

            virtual std::shared_ptr<IQ> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<IQ>) override;

            virtual boost::optional<Documentation> getDocumentation() const override;

            static std::string convertIQTypeToString(IQ::Type type);
            static IQ::Type convertIQTypeFromString(const std::string& type);

        private:
            LuaElementConvertors* convertors;
    };
}

