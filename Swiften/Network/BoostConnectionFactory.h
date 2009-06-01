#ifndef SWIFTEN_BoostConnectionFactory_H
#define SWIFTEN_BoostConnectionFactory_H

#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Network/BoostConnection.h"

namespace Swift {
	class BoostConnection;

	class BoostConnectionFactory : public ConnectionFactory {
		public:
			BoostConnectionFactory();

			virtual BoostConnection* createConnection(const String& domain);
	};
}

#endif
