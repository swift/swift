/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Application/MacOSX/MacOSXApplication.h"

#include <iostream>

namespace Swift {

MacOSXApplication::MacOSXApplication(const String& name) : Application(name) {
}

ApplicationMessageDisplay* MacOSXApplication::getApplicationMessageDisplay() {
	return &messageDisplay_;
}

}
