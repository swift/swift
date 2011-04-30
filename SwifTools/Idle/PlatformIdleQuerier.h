/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <SwifTools/Idle/IdleQuerier.h>

namespace Swift {
	class PlatformIdleQuerier : public IdleQuerier {
		public:
			PlatformIdleQuerier();
			~PlatformIdleQuerier();

			virtual int getIdleTimeSeconds() {
				return querier->getIdleTimeSeconds();
			}

		private:
			IdleQuerier* querier;
	};
}
