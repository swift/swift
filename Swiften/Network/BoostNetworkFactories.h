/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Network/NetworkFactories.h"
#include "Swiften/Network/BoostIOServiceThread.h"

namespace Swift {
	class EventLoop;

	class BoostNetworkFactories : public NetworkFactories {
		public:
			BoostNetworkFactories(EventLoop* eventLoop);
			~BoostNetworkFactories();

			virtual TimerFactory* getTimerFactory() const {
				return timerFactory;
			}

			virtual ConnectionFactory* getConnectionFactory() const {
				return connectionFactory;
			}

			BoostIOServiceThread* getIOServiceThread() {
				return &ioServiceThread;
			}

			DomainNameResolver* getDomainNameResolver() const {
				return domainNameResolver;
			}

		private:
			BoostIOServiceThread ioServiceThread;
			TimerFactory* timerFactory;
			ConnectionFactory* connectionFactory;
			DomainNameResolver* domainNameResolver;
	};
}
