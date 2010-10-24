/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

namespace Swift {
	class QtCachedImageScaler {
		public:
			QtCachedImageScaler();

			boost::filesystem::path getScaledImage(const boost::filesystem::path& image, int size);
	};
}

