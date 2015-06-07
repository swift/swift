/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Whiteboard/WhiteboardElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardColor.h>

#include <vector>
#include <utility>

namespace Swift {
	class SWIFTEN_API WhiteboardFreehandPathElement : public WhiteboardElement {
		typedef std::pair<int, int> Point;
	public:
		typedef boost::shared_ptr<WhiteboardFreehandPathElement> ref;
	public:
		WhiteboardFreehandPathElement() {
		}

		void setPoints(const std::vector<Point>& points) {
			points_ = points;
		}

		const std::vector<Point>& getPoints() const {
			return points_;
		}

		const WhiteboardColor& getColor() const {
			return color_;
		}

		void setColor(const WhiteboardColor& color) {
			color_ = color;
		}

		int getPenWidth() const {
			return penWidth_;
		}

		void setPenWidth(const int penWidth) {
			penWidth_ = penWidth;
		}

		void accept(WhiteboardElementVisitor& visitor) {
			visitor.visit(*this);
		}

	private:
		std::vector<Point> points_;
		WhiteboardColor color_;
		int penWidth_;
	};
}
