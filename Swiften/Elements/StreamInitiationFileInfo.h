/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/String.h>

namespace Swift {
	struct StreamInitiationFileInfo {
		StreamInitiationFileInfo(const String& name = "", const String& description = "", int size = -1) : name(name), description(description), size(size) {}

		String name;
		String description;
		int size;
	};
}
