/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class TimerFactory;
	class ConnectionFactory;
	class DomainNameResolver;

	/**
	 * An interface collecting network factories.
	 */
	class NetworkFactories {
		public:
			virtual ~NetworkFactories();

			virtual TimerFactory* getTimerFactory() const = 0;
			virtual ConnectionFactory* getConnectionFactory() const = 0;
			virtual DomainNameResolver* getDomainNameResolver() const = 0;
	};
}
