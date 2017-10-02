/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/Payload.h>

struct lua_State;

namespace Swift {
    namespace Sluift {
        struct Response {
            Response(std::shared_ptr<Payload> result, std::shared_ptr<ErrorPayload> error) : result(result), error(error) {}
            SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(Response)
            ~Response();

            static Response withResult(std::shared_ptr<Payload> response) {
                return Response(response, std::shared_ptr<ErrorPayload>());
            }

            static Response withError(std::shared_ptr<ErrorPayload> error) {
                return Response(std::shared_ptr<Payload>(), error);
            }

            int convertToLuaResult(lua_State* L);

            std::shared_ptr<Payload> result;
            std::shared_ptr<ErrorPayload> error;
        };
    }
}
