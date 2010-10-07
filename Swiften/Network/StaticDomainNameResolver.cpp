/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Network/StaticDomainNameResolver.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "Swiften/Network/DomainNameResolveError.h"
#include "Swiften/Base/String.h"

using namespace Swift;

namespace {
	struct ServiceQuery : public DomainNameServiceQuery, public boost::enable_shared_from_this<ServiceQuery> {
		ServiceQuery(const String& service, Swift::StaticDomainNameResolver* resolver) : service(service), resolver(resolver) {}

		virtual void run() {
			if (!resolver->getIsResponsive()) {
				return;
			}
			std::vector<DomainNameServiceQuery::Result> results;
			for(StaticDomainNameResolver::ServicesCollection::const_iterator i = resolver->getServices().begin(); i != resolver->getServices().end(); ++i) {
				if (i->first == service) {
					results.push_back(i->second);
				}
			}
			MainEventLoop::postEvent(boost::bind(&ServiceQuery::emitOnResult, shared_from_this(), results));
		}

		void emitOnResult(std::vector<DomainNameServiceQuery::Result> results) {
			onResult(results);
		}

		String service;
		StaticDomainNameResolver* resolver;
	};

	struct AddressQuery : public DomainNameAddressQuery, public boost::enable_shared_from_this<AddressQuery> {
		AddressQuery(const String& host, StaticDomainNameResolver* resolver) : host(host), resolver(resolver) {}

		virtual void run() {
			if (!resolver->getIsResponsive()) {
				return;
			}
			StaticDomainNameResolver::AddressesMap::const_iterator i = resolver->getAddresses().find(host);
			if (i != resolver->getAddresses().end()) {
				MainEventLoop::postEvent(
						boost::bind(&AddressQuery::emitOnResult, shared_from_this(), i->second, boost::optional<DomainNameResolveError>()));
			}
			else {
				MainEventLoop::postEvent(boost::bind(&AddressQuery::emitOnResult, shared_from_this(), std::vector<HostAddress>(), boost::optional<DomainNameResolveError>(DomainNameResolveError())));
			}
		}

		void emitOnResult(std::vector<HostAddress> results, boost::optional<DomainNameResolveError> error) {
			onResult(results, error);
		}

		String host;
		StaticDomainNameResolver* resolver;
	};
}

namespace Swift {

StaticDomainNameResolver::StaticDomainNameResolver() : isResponsive(true) {
}

void StaticDomainNameResolver::addAddress(const String& domain, const HostAddress& address) {
	addresses[domain].push_back(address);
}

void StaticDomainNameResolver::addService(const String& service, const DomainNameServiceQuery::Result& result) {
	services.push_back(std::make_pair(service, result));
}

void StaticDomainNameResolver::addXMPPClientService(const String& domain, const HostAddressPort& address) {
	static int hostid = 0;
	String hostname(std::string("host-") + boost::lexical_cast<std::string>(hostid));
	hostid++;

	addService("_xmpp-client._tcp." + domain, ServiceQuery::Result(hostname, address.getPort(), 0, 0));
	addAddress(hostname, address.getAddress());
}

void StaticDomainNameResolver::addXMPPClientService(const String& domain, const String& hostname, int port) {
	addService("_xmpp-client._tcp." + domain, ServiceQuery::Result(hostname, port, 0, 0));
}

boost::shared_ptr<DomainNameServiceQuery> StaticDomainNameResolver::createServiceQuery(const String& name) {
	return boost::shared_ptr<DomainNameServiceQuery>(new ServiceQuery(name, this));
}

boost::shared_ptr<DomainNameAddressQuery> StaticDomainNameResolver::createAddressQuery(const String& name) {
	return boost::shared_ptr<DomainNameAddressQuery>(new AddressQuery(name, this));
}

}
