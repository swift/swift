/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Whiteboard/WhiteboardOperation.h>
#include <list>
#include <utility>

namespace Swift {
	class WhiteboardClient {
	public:
		struct Result {
			WhiteboardOperation::ref client;
			WhiteboardOperation::ref server;
		};
		/*!
		 * @return Operation to send
		 */   	
		WhiteboardOperation::ref handleLocalOperationReceived(WhiteboardOperation::ref operation);
		/*!
		 * @return pair.first-element to handle locally, pair.second-element to send to server
		 */
		Result handleServerOperationReceived(WhiteboardOperation::ref operation);
		void print();

	private:
		std::list<WhiteboardOperation::ref> localOperations_;
		std::list<WhiteboardOperation::ref> serverOperations_;
		std::list<WhiteboardOperation::ref> bridge_;
		std::string lastSentOperationID_;
	};
}
