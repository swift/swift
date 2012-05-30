/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Whiteboard/WhiteboardOperation.h>

#include <Swiften/Elements/Whiteboard/WhiteboardElement.h>

namespace Swift {
	class WhiteboardDeleteOperation : public WhiteboardOperation {
	public:
		typedef boost::shared_ptr<WhiteboardDeleteOperation> ref;
	public:
		~WhiteboardDeleteOperation() {
		}

		std::string getElementID() const {
			return elementID_;
		}

		void setElementID(const std::string& elementID) {
			elementID_ = elementID;
		}

	private:
		std::string elementID_;
	};
}
