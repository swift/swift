/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>

#include <Swift/Controllers/HighlightRule.h>

namespace Swift {

	class HighlightManager;

	class Highlighter {
		public:
			Highlighter(HighlightManager* manager);

			enum Mode { ChatMode, MUCMode };
			void setMode(Mode mode);

			void setNick(const std::string& nick) { nick_ = nick; }

			HighlightAction findAction(const std::string& body, const std::string& sender) const;

			void handleHighlightAction(const HighlightAction& action);

		private:
			HighlightManager* manager_;
			Mode mode_;
			HighlightRule::MessageType messageType_;
			std::string nick_;
	};

}
