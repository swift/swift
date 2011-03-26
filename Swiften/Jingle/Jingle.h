/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Elements/JingleContentPayload.h>

namespace Swift {
	namespace Jingle {
		template<typename T>
		JingleContentPayload::ref getContentWithDescription(const std::vector<JingleContentPayload::ref>& contents) {
			for (size_t i = 0; i < contents.size(); ++i) {
				if (contents[i]->getDescription<T>()) {
					return contents[i];
				}
			}
			return JingleContentPayload::ref();
		}
	}
}
