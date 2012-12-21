/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swift/Controllers/HighlightAction.h>

namespace Swift {

void HighlightAction::setHighlightText(bool highlightText)
{
	highlightText_ = highlightText;
	if (!highlightText_) {
		textColor_.clear();
		textBackground_.clear();
	}
}

void HighlightAction::setPlaySound(bool playSound)
{
	playSound_ = playSound;
	if (!playSound_) {
		soundFile_.clear();
	}
}

}
