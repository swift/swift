/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class SOCKS5BytestreamServerManager;

	class SWIFTEN_API SOCKS5BytestreamServerInitializeRequest {
		public:
			SOCKS5BytestreamServerInitializeRequest(SOCKS5BytestreamServerManager* manager);
			~SOCKS5BytestreamServerInitializeRequest();

			void start();
			void stop();

		public:
			boost::signal<void (bool /* success */)> onFinished;

		private:
			void handleInitialized(bool success);

		private:
			SOCKS5BytestreamServerManager* manager;
	};
}

