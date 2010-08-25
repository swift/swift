/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include "Swiften/VCards/VCardStorageFactory.h"
#include "Swiften/VCards/VCardMemoryStorage.h"

namespace Swift {
	class VCardMemoryStorageFactory : public VCardStorageFactory {
	public:
		VCardMemoryStorageFactory() {
		}
		
		virtual VCardStorage* createVCardStorage(const String& profile) {
			return new VCardMemoryStorage();
		}
	};
}