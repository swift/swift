/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <QString>

namespace Swift {
	class QtScaledAvatarCache {
		public:
			QtScaledAvatarCache(int size);

			QString getScaledAvatarPath(const QString& path);

		private:
			int size;
	};
}
