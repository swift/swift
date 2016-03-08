/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <Swift/Controllers/HighlightRule.h>

namespace Swift {

	class HighlightManager;

	class Highlighter {
		public:
			Highlighter(HighlightManager* manager);

			enum Mode { ChatMode, MUCMode };
			void setMode(Mode mode);

			void setNick(const std::string& nick) { nick_ = nick; }
			std::string getNick() const { return nick_; }

			HighlightAction findFirstFullMessageMatchAction(const std::string& body, const std::string& sender) const;

			void handleHighlightAction(const HighlightAction& action);

		private:
			HighlightManager* manager_;
			Mode mode_;
			HighlightRule::MessageType messageType_;
			std::string nick_;
	};

}
