/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/HighlightAction.h>

namespace Swift {

void HighlightAction::setHighlightWholeMessage(bool highlightText)
{
	highlightWholeMessage_ = highlightText;
	if (!highlightWholeMessage_) {
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

bool operator ==(HighlightAction const& a, HighlightAction const& b) {
	if (a.highlightWholeMessage() != b.highlightWholeMessage()) {
		return false;
	}

	if (a.getTextColor() != b.getTextColor()) {
		return false;
	}

	if (a.getTextBackground() != b.getTextBackground()) {
		return false;
	}

	if (a.playSound() != b.playSound()) {
		return false;
	}

	if (a.getSoundFile() != b.getSoundFile()) {
		return false;
	}

	return true;
}

bool operator !=(HighlightAction const& a, HighlightAction const& b) {
	return !(a == b);
}

}
