/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "LastLineTracker.h"

using namespace Swift;

LastLineTracker::LastLineTracker() {
	lastFocus = true;
	shouldMove = false;
}

void LastLineTracker::setHasFocus(bool focus) {
	if (!focus && lastFocus) {
			shouldMove = true;
			lastFocus = focus;
			return;
	}
	shouldMove = false;
	lastFocus = focus;
}

bool LastLineTracker::getShouldMoveLastLine() {
	bool ret = shouldMove;
	shouldMove = false;
	return ret;
}
