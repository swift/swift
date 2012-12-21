/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>

namespace Swift {

	class HighlightRule;

	class HighlightAction {
		public:
			HighlightAction() : highlightText_(false), playSound_(false) {}

			bool highlightText() const { return highlightText_; }
			void setHighlightText(bool highlightText);

			const std::string& getTextColor() const { return textColor_; }
			void setTextColor(const std::string& textColor) { textColor_ = textColor; }

			const std::string& getTextBackground() const { return textBackground_; }
			void setTextBackground(const std::string& textBackground) { textBackground_ = textBackground; }

			bool playSound() const { return playSound_; }
			void setPlaySound(bool playSound);

			const std::string& getSoundFile() const { return soundFile_; }
			void setSoundFile(const std::string& soundFile) { soundFile_ = soundFile; }

			bool isEmpty() const { return !highlightText_ && !playSound_; }

		private:
			bool highlightText_;
			std::string textColor_;
			std::string textBackground_;

			bool playSound_;
			std::string soundFile_;
	};

}
