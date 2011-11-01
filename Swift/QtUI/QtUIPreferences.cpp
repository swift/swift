/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/QtUI/QtUIPreferences.h>

namespace Swift {
QtUIPreferences::QtUIPreferences() : compactRosters_(false) {
	
}

QtUIPreferences::~QtUIPreferences() {
	
}

void QtUIPreferences::setCompactRosters(bool compact) {
	compactRosters_ = compact;
	emit onCompactRostersChanged(compact);
}

bool QtUIPreferences::getCompactRosters() {
	return compactRosters_;
}

}
