/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

namespace Swift {
	class WhiteboardSession;
	class WhiteboardWindow;

	class WhiteboardWindowFactory {
	public :
		virtual ~WhiteboardWindowFactory() {}

		virtual WhiteboardWindow* createWhiteboardWindow(boost::shared_ptr<WhiteboardSession> whiteboardSession) = 0;
	};
}
