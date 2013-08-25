/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/ErrorPayload.h>

struct lua_State;

namespace Swift {
	namespace Sluift {
		struct Response {
			Response(boost::shared_ptr<Payload> result, boost::shared_ptr<ErrorPayload> error) : result(result), error(error) {}
			~Response();

			static Response withResult(boost::shared_ptr<Payload> response) {
				return Response(response, boost::shared_ptr<ErrorPayload>());
			}

			static Response withError(boost::shared_ptr<ErrorPayload> error) {
				return Response(boost::shared_ptr<Payload>(), error);
			}

			int convertToLuaResult(lua_State* L);

			boost::shared_ptr<Payload> result;
			boost::shared_ptr<ErrorPayload> error;
		};
	}
}
