/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Whiteboard/WhiteboardInsertOperation.h>

#include <list>

namespace Swift {
	class SWIFTEN_API WhiteboardServer {
	public:
		void handleLocalOperationReceived(WhiteboardOperation::ref operation);
		WhiteboardOperation::ref handleClientOperationReceived(WhiteboardOperation::ref operation);
		void print();

	private:
		std::list<WhiteboardOperation::ref> operations_;
	};
}
