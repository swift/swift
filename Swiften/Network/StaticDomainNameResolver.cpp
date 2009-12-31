#include "Swiften/Network/StaticDomainNameResolver.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "Swiften/Network/DomainNameResolveError.h"
#include "Swiften/Base/String.h"

using namespace Swift;

namespace {
	struct ServiceQuery : public DomainNameServiceQuery, public EventOwner {
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
			MainEventLoop::postEvent(boost::bind(boost::ref(onResult), results)); 
		}

		String service;
		StaticDomainNameResolver* resolver;
	};

	struct AddressQuery : public DomainNameAddressQuery, public EventOwner {
		AddressQuery(const String& host, StaticDomainNameResolver* resolver) : host(host), resolver(resolver) {}

		virtual void run() {
			if (!resolver->getIsResponsive()) {
				return;
			}
			StaticDomainNameResolver::AddressesMap::const_iterator i = resolver->getAddresses().find(host);
			if (i != resolver->getAddresses().end()) {
				MainEventLoop::postEvent(
						boost::bind(boost::ref(onResult), i->second, boost::optional<DomainNameResolveError>()));
			}
			else {
				MainEventLoop::postEvent(boost::bind(boost::ref(onResult), HostAddress(), boost::optional<DomainNameResolveError>(DomainNameResolveError())));
			}

		}

		String host;
		StaticDomainNameResolver* resolver;
	};
}

namespace Swift {

StaticDomainNameResolver::StaticDomainNameResolver() : isResponsive(true) {
}

void StaticDomainNameResolver::addAddress(const String& domain, const HostAddress& address) {
	addresses[domain] = address;
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

boost::shared_ptr<DomainNameServiceQuery> StaticDomainNameResolver::createServiceQuery(const String& name) {
	return boost::shared_ptr<DomainNameServiceQuery>(new ServiceQuery(name, this));
}

boost::shared_ptr<DomainNameAddressQuery> StaticDomainNameResolver::createAddressQuery(const String& name) {
	return boost::shared_ptr<DomainNameAddressQuery>(new AddressQuery(name, this));
}

}
