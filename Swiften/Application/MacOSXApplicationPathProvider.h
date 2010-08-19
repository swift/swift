/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Application/ApplicationPathProvider.h"

namespace Swift {
	class MacOSXApplicationPathProvider : public ApplicationPathProvider {
		public:
			MacOSXApplicationPathProvider(const String& name);

			virtual boost::filesystem::path getHomeDir() const;
			boost::filesystem::path getSettingsDir() const;
	};
}
