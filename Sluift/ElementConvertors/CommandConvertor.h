/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Command.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class CommandConvertor : public GenericLuaElementConvertor<Command> {
        public:
            CommandConvertor(LuaElementConvertors* convertors);
            virtual ~CommandConvertor() override;

            virtual std::shared_ptr<Command> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Command>) override;

        private:
            LuaElementConvertors* convertors;
    };
}
