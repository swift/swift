#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuerier.h"

#include <boost/bind.hpp>

#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDBrowseQuery.h"
#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDRegisterQuery.h"
#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDResolveServiceQuery.h"
#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDResolveHostnameQuery.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {

FakeDNSSDQuerier::FakeDNSSDQuerier(const String& domain) : domain(domain) {
}

FakeDNSSDQuerier::~FakeDNSSDQuerier() {
	if (!runningQueries.empty()) {
		std::cerr << "FakeDNSSDQuerier: Running queries not empty at destruction time" << std::endl;
	}
}

boost::shared_ptr<DNSSDBrowseQuery> FakeDNSSDQuerier::createBrowseQuery() {
	return boost::shared_ptr<DNSSDBrowseQuery>(new FakeDNSSDBrowseQuery(shared_from_this()));
}

boost::shared_ptr<DNSSDRegisterQuery> FakeDNSSDQuerier::createRegisterQuery(const String& name, int port, const ByteArray& info) {
	return boost::shared_ptr<DNSSDRegisterQuery>(new FakeDNSSDRegisterQuery(name, port, info, shared_from_this()));
}

boost::shared_ptr<DNSSDResolveServiceQuery> FakeDNSSDQuerier::createResolveServiceQuery(const DNSSDServiceID& service) {
	return boost::shared_ptr<DNSSDResolveServiceQuery>(new FakeDNSSDResolveServiceQuery(service, shared_from_this()));
}

boost::shared_ptr<DNSSDResolveHostnameQuery> FakeDNSSDQuerier::createResolveHostnameQuery(const String& hostname, int interfaceIndex) {
	return boost::shared_ptr<DNSSDResolveHostnameQuery>(new FakeDNSSDResolveHostnameQuery(hostname, interfaceIndex, shared_from_this()));
}

void FakeDNSSDQuerier::addRunningQuery(boost::shared_ptr<FakeDNSSDQuery> query) {
	runningQueries.push_back(query);
	if (boost::shared_ptr<FakeDNSSDBrowseQuery> browseQuery = boost::dynamic_pointer_cast<FakeDNSSDBrowseQuery>(query)) {
		foreach(const DNSSDServiceID& service, services) {
			MainEventLoop::postEvent(boost::bind(boost::ref(browseQuery->onServiceAdded), service), shared_from_this());
		}
	}
	else if (boost::shared_ptr<FakeDNSSDResolveServiceQuery> resolveQuery = boost::dynamic_pointer_cast<FakeDNSSDResolveServiceQuery>(query)) {
		for(ServiceInfoMap::const_iterator i = serviceInfo.begin(); i != serviceInfo.end(); ++i) {
			if (i->first == resolveQuery->service) {
				MainEventLoop::postEvent(boost::bind(boost::ref(resolveQuery->onServiceResolved), i->second), shared_from_this());
			}
		}
	}
	else if (boost::shared_ptr<FakeDNSSDRegisterQuery> registerQuery = boost::dynamic_pointer_cast<FakeDNSSDRegisterQuery>(query)) {
		DNSSDServiceID service(registerQuery->name, domain);
		MainEventLoop::postEvent(boost::bind(boost::ref(registerQuery->onRegisterFinished), service), shared_from_this());
	}
	else if (boost::shared_ptr<FakeDNSSDResolveHostnameQuery> resolveHostnameQuery = boost::dynamic_pointer_cast<FakeDNSSDResolveHostnameQuery>(query)) {
		std::map<String,boost::optional<HostAddress> >::const_iterator i = addresses.find(resolveHostnameQuery->hostname);
		if (i != addresses.end()) {
			MainEventLoop::postEvent(
					boost::bind(
						boost::ref(resolveHostnameQuery->onHostnameResolved), i->second), 
					shared_from_this());
		}
	}
}

void FakeDNSSDQuerier::removeRunningQuery(boost::shared_ptr<FakeDNSSDQuery> query) {
	runningQueries.erase(std::remove(
		runningQueries.begin(), runningQueries.end(), query), runningQueries.end());
}

void FakeDNSSDQuerier::addService(const DNSSDServiceID& id) {
	services.insert(id);
	foreach(const boost::shared_ptr<FakeDNSSDBrowseQuery>& query, getQueries<FakeDNSSDBrowseQuery>()) {
		MainEventLoop::postEvent(boost::bind(boost::ref(query->onServiceAdded), id), shared_from_this());
	}
}

void FakeDNSSDQuerier::removeService(const DNSSDServiceID& id) {
	services.erase(id);
	serviceInfo.erase(id);
	foreach(const boost::shared_ptr<FakeDNSSDBrowseQuery>& query, getQueries<FakeDNSSDBrowseQuery>()) {
		MainEventLoop::postEvent(boost::bind(boost::ref(query->onServiceRemoved), id), shared_from_this());
	}
}

void FakeDNSSDQuerier::setServiceInfo(const DNSSDServiceID& id, const DNSSDResolveServiceQuery::Result& info) {
	std::pair<ServiceInfoMap::iterator, bool> r = serviceInfo.insert(std::make_pair(id, info));
	if (!r.second) {
		r.first->second = info;
	}
	foreach(const boost::shared_ptr<FakeDNSSDResolveServiceQuery>& query, getQueries<FakeDNSSDResolveServiceQuery>()) {
		if (query->service == id) {
			MainEventLoop::postEvent(boost::bind(boost::ref(query->onServiceResolved), info), shared_from_this());
		}
	}
}

bool FakeDNSSDQuerier::isServiceRegistered(const String& name, int port, const ByteArray& info) {
	foreach(const boost::shared_ptr<FakeDNSSDRegisterQuery>& query, getQueries<FakeDNSSDRegisterQuery>()) {
		if (query->name == name && query->port == port && query->info == info) {
			return true;
		}
	}
	return false;
}

void FakeDNSSDQuerier::setBrowseError() {
	foreach(const boost::shared_ptr<FakeDNSSDBrowseQuery>& query, getQueries<FakeDNSSDBrowseQuery>()) {
		MainEventLoop::postEvent(boost::ref(query->onError), shared_from_this());
	}
}

void FakeDNSSDQuerier::setRegisterError() {
	foreach(const boost::shared_ptr<FakeDNSSDRegisterQuery>& query, getQueries<FakeDNSSDRegisterQuery>()) {
		MainEventLoop::postEvent(boost::bind(boost::ref(query->onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
	}
}

void FakeDNSSDQuerier::setAddress(const String& hostname, boost::optional<HostAddress> address) {
	addresses[hostname] = address;
	foreach(const boost::shared_ptr<FakeDNSSDResolveHostnameQuery>& query, getQueries<FakeDNSSDResolveHostnameQuery>()) {
		if (query->hostname == hostname) {
			MainEventLoop::postEvent(boost::bind(
					boost::ref(query->onHostnameResolved), address), shared_from_this());
		}
	}
}

}
