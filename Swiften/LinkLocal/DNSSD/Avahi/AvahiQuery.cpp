/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/LinkLocal/DNSSD/Avahi/AvahiQuery.h"
#include "Swiften/LinkLocal/DNSSD/Avahi/AvahiQuerier.h"

namespace Swift {

AvahiQuery::AvahiQuery(boost::shared_ptr<AvahiQuerier> q) : querier(q) {
}

AvahiQuery::~AvahiQuery() {
}

}

