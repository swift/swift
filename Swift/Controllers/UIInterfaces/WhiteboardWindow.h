/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"

#include <string>

namespace Swift {
	class WhiteboardSession;
	class WhiteboardElement;

	class WhiteboardWindow {
	public:
		virtual ~WhiteboardWindow() {}

		virtual void show() = 0;
		virtual void setSession(boost::shared_ptr<WhiteboardSession> session) = 0;
		virtual void activateWindow() = 0;
		virtual void setName(const std::string& name) = 0;
	};
}
