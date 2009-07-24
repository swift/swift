#pragma once

#include "Swiften/LinkLocal/BonjourQuery.h"
#include "Swiften/LinkLocal/DNSSDPublishQuery.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class BonjourQuerier;

	class BonjourPublishQuery : public DNSSDPublishQuery, public BonjourQuery {
		public:	
			BonjourPublishQuery(const String& name, int port, const LinkLocalServiceInfo& info, boost::shared_ptr<BonjourQuerier> querier) : BonjourQuery(querier) {
				ByteArray txtRecord = info.toTXTRecord();
				DNSServiceErrorType result = DNSServiceRegister(
						&sdRef, 0, 0, name.getUTF8Data(), "_presence._tcp", NULL, NULL, port, 
						txtRecord.getSize(), txtRecord.getData(), 
						&BonjourPublishQuery::handleServiceRegistered, this);
				if (result != kDNSServiceErr_NoError) {
					// TODO
					std::cerr << "Error creating service registration" << std::endl;
				}
			}

			void publish() {
				run();
			}

		private:
			static void handleServiceRegistered(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context) {
				std::cout << "Publish finished " << name << std::endl;
				BonjourPublishQuery* query = static_cast<BonjourPublishQuery*>(context);
				if (errorCode != kDNSServiceErr_NoError) {
					query->onPublishFinished(boost::optional<LinkLocalServiceID>());
				}
				else {
					query->onPublishFinished(boost::optional<LinkLocalServiceID>(LinkLocalServiceID(name, regtype, domain, 0)));
				}
			}
	};
}
