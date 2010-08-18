/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
	template<typename T>
	class Shared {
		public:
			typedef boost::shared_ptr<T> ref;

			template<typename X>
			static boost::shared_ptr<T> cast(const X& from) {
				return boost::dynamic_pointer_cast<T>(from);
			}
	};
}
