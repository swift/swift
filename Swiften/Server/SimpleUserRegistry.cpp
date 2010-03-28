#include "Swiften/Server/SimpleUserRegistry.h"

namespace Swift {

SimpleUserRegistry::SimpleUserRegistry() {
}

bool SimpleUserRegistry::isValidUserPassword(const JID& user, const String& password) const {
	std::map<JID,String>::const_iterator i = users.find(user);
	return i != users.end() ? i->second == password : false;
}

void SimpleUserRegistry::addUser(const JID& user, const String& password) {
	users.insert(std::make_pair(user, password));
}

}
