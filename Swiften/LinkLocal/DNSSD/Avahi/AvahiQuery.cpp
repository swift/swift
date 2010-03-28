#include "Swiften/LinkLocal/DNSSD/Avahi/AvahiQuery.h"
#include "Swiften/LinkLocal/DNSSD/Avahi/AvahiQuerier.h"

namespace Swift {

AvahiQuery::AvahiQuery(boost::shared_ptr<AvahiQuerier> q) : querier(q) {
}

AvahiQuery::~AvahiQuery() {
}

}

