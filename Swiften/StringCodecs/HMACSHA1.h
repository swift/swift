/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class ByteArray;

	class HMACSHA1 {
		public:
			static ByteArray getResult(const ByteArray& key, const ByteArray& data);
	};
}
