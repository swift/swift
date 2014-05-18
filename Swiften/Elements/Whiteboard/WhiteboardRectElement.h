/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Whiteboard/WhiteboardElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardColor.h>

namespace Swift {
	class WhiteboardRectElement : public WhiteboardElement {
	public:
		typedef boost::shared_ptr<WhiteboardRectElement> ref;
	public:
		WhiteboardRectElement(int x, int y, int width, int height) : penWidth_(1) {
			x_ = x;
			y_ = y;
			width_ = width;
			height_ = height;
		}

		int getX() const {
			return x_;
		}

		int getY() const {
			return y_;
		}

		int getWidth() const {
			return width_;
		}

		int getHeight() const {
			return height_;
		}

		const WhiteboardColor& getPenColor() const {
			return penColor_;
		}

		void setPenColor(const WhiteboardColor& color) {
			penColor_ = color;
		}

		const WhiteboardColor& getBrushColor() const {
			return brushColor_;
		}

		void setBrushColor(const WhiteboardColor& color) {
			brushColor_ = color;
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
		int x_, y_, width_, height_;
		WhiteboardColor penColor_;
		WhiteboardColor brushColor_;
		int penWidth_;
	};
}
