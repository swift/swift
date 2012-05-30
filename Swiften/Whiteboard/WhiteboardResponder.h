/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Queries/SetResponder.h>
#include <Swiften/Elements/WhiteboardPayload.h>

namespace Swift {
	class IQRouter;
	class WhiteboardSessionManager;

	class WhiteboardResponder : public SetResponder<WhiteboardPayload> {
	public:
		WhiteboardResponder(WhiteboardSessionManager* sessionManager, IQRouter* router);
		bool handleSetRequest(const JID& from, const JID& /*to*/, const std::string& id, boost::shared_ptr<WhiteboardPayload> payload);

	private:
		WhiteboardSessionManager* sessionManager_;
		IQRouter* router_;
	};
}
