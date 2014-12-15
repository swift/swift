/*
 * Copyright (c) 2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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

