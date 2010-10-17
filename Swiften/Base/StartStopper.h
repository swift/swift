/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	template<typename T> class StartStopper {
		public:
			StartStopper(T* target) : target(target) {
				target->start();
			}

			~StartStopper() {
				target->stop();
			}
		
		private:
			T* target;
	};
}
