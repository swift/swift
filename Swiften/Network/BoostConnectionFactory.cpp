#include "Swiften/Network/BoostConnectionFactory.h"

namespace Swift {

BoostConnectionFactory::BoostConnectionFactory() {
}

BoostConnection* BoostConnectionFactory::createConnection(const String& domain) {
	return new BoostConnection(domain);
}

}
