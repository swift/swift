/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include "Swiften/VCards/VCardStorageFactory.h"
#include "Swiften/VCards/VCardFileStorage.h"

namespace Swift {
	class VCardFileStorageFactory : public VCardStorageFactory {
		public:
			VCardFileStorageFactory(boost::filesystem::path base) : base(base) {
			}

			virtual VCardStorage* createVCardStorage(const String& profile) {
				return new VCardFileStorage(base / profile.getUTF8String() / "vcards");
			}

		private:
			boost::filesystem::path base;
	};
}
