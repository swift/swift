/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Whiteboard/WhiteboardElementVisitor.h>

namespace Swift {
	class WhiteboardElement {
	public:
		typedef boost::shared_ptr<WhiteboardElement> ref;

	public:
		virtual ~WhiteboardElement() {}
		virtual void accept(WhiteboardElementVisitor& visitor) = 0;

		const std::string& getID() const {
			return id_;
		}

		void setID(const std::string& id) {
			id_ = id;
		}

	private:
		std::string id_;
	};
}
