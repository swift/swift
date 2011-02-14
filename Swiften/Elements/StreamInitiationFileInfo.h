/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

namespace Swift {
	struct StreamInitiationFileInfo {
		StreamInitiationFileInfo(const std::string& name = "", const std::string& description = "", int size = -1) : name(name), description(description), size(size) {}

		std::string name;
		std::string description;
		int size;
	};
}
