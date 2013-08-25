/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Sluift/LuaElementConvertors.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/BoostNetworkFactories.h>

namespace Swift {
	struct SluiftGlobals {
		SluiftGlobals() : networkFactories(&eventLoop) {}

		int timeout;
		bool debug;
		LuaElementConvertors elementConvertor;
		SimpleEventLoop eventLoop;
		BoostNetworkFactories networkFactories;
		int bootIndex;
	};
}
