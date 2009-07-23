#pragma once

#include <vector>
#include <map>
#include <boost/bind.hpp>

#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/LinkLocal/DNSSDService.h"
#include "Swiften/LinkLocal/DNSSDServiceFactory.h"

namespace Swift {
	class MockDNSSDService : public DNSSDService {
		public:
			MockDNSSDService() {
			}
			
			void start() {
			}

			void stop() {
			}

			virtual void registerService(const String&, int, const LinkLocalServiceInfo&) {
				assert(false);
			}

			virtual void updateService(const LinkLocalServiceInfo&) {
				assert(false);
			}

			virtual void unregisterService() {
				assert(false);
			}

			virtual void startResolvingService(const LinkLocalServiceID& id) {
				resolvingServices.push_back(id);
				broadcastServiceInfo(id);
			}

			virtual void stopResolvingService(const LinkLocalServiceID& id) {
				resolvingServices.erase(std::remove(resolvingServices.begin(), resolvingServices.end(), id), resolvingServices.end());
			}
			
			virtual void resolveHostname(const String&, int) {
				assert(false);
			}

			void addService(const LinkLocalServiceID& id) {
				MainEventLoop::postEvent(boost::bind(boost::ref(onServiceAdded), id));
			}

			void removeService(const LinkLocalServiceID& id) {
				serviceInfo.erase(id);
				MainEventLoop::postEvent(boost::bind(boost::ref(onServiceRemoved), id));
			}

			void setServiceInfo(const LinkLocalServiceID& id, const DNSSDService::ResolveResult& info) {
				std::pair<ServiceInfoMap::iterator, bool> r = serviceInfo.insert(std::make_pair(id, info));
				if (!r.second) {
					r.first->second = info;
				}
				broadcastServiceInfo(id);
			}

		private:
			void broadcastServiceInfo(const LinkLocalServiceID& id) {
				if (std::find(resolvingServices.begin(), resolvingServices.end(), id) != resolvingServices.end()) {
					ServiceInfoMap::const_iterator i = serviceInfo.find(id);
					if (i != serviceInfo.end()) {
						MainEventLoop::postEvent(
								boost::bind(boost::ref(onServiceResolved), id, i->second));
					}
				}
			}

		private:
			typedef std::map<LinkLocalServiceID,DNSSDService::ResolveResult> ServiceInfoMap;
			ServiceInfoMap serviceInfo;
			std::vector<LinkLocalServiceID> resolvingServices;
	};

  class MockDNSSDServiceFactory : public DNSSDServiceFactory {
    public:
      boost::shared_ptr<DNSSDService> createDNSSDService() {
        boost::shared_ptr<MockDNSSDService> result(new MockDNSSDService());   
        services.push_back(result);
        return result;
      }

      bool canCreate() const {
        return true;
      }

      std::vector<boost::shared_ptr<MockDNSSDService> > services;
  };

}
