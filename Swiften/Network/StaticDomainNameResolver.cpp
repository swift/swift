#include "Swiften/Network/StaticDomainNameResolver.h"
#include "Swiften/Network/DomainNameResolveError.h"
#include "Swiften/Base/String.h"

using namespace Swift;

namespace {
	struct ServiceQuery : public DomainNameServiceQuery {
		ServiceQuery(const String& service, Swift::StaticDomainNameResolver* resolver) : service(service), resolver(resolver) {}

		virtual void run() {
		}

		String service;
		StaticDomainNameResolver* resolver;
	};

	struct AddressQuery : public DomainNameAddressQuery {
		AddressQuery(const String& host, StaticDomainNameResolver* resolver) : host(host), resolver(resolver) {}

		virtual void run() {
		}

		String host;
		StaticDomainNameResolver* resolver;
	};
}

namespace Swift {

void StaticDomainNameResolver::addAddress(const String& domain, const HostAddress& address) {
	addresses[domain] = address;
}

void StaticDomainNameResolver::addService(const String& service, const DomainNameServiceQuery::Result& result) {
	services.push_back(std::make_pair(service, result));
}

void StaticDomainNameResolver::addXMPPClientService(const String& domain, const HostAddressPort& address) {
	addService("_xmpp-client._tcp." + domain, ServiceQuery::Result(domain, 5222, 0, 0));
	addAddress(domain, address.getAddress());
}

boost::shared_ptr<DomainNameServiceQuery> StaticDomainNameResolver::createServiceQuery(const String& name) {
	return boost::shared_ptr<DomainNameServiceQuery>(new ServiceQuery(name, this));
}

boost::shared_ptr<DomainNameAddressQuery> StaticDomainNameResolver::createAddressQuery(const String& name) {
	return boost::shared_ptr<DomainNameAddressQuery>(new AddressQuery(name, this));
}

}
