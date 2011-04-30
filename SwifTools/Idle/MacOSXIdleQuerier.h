/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <IOKit/IOKitLib.h>

#include <SwifTools/Idle/IdleQuerier.h>

namespace Swift {
	class MacOSXIdleQuerier : public IdleQuerier {
		public:
			MacOSXIdleQuerier();

			virtual int getIdleTimeSeconds();

		private:
			io_service_t ioService;
	};
}
