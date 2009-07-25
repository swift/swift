#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
	class String;
	class LinkLocalServiceInfo;
	class DNSSDServiceID;
	class DNSSDBrowseQuery;
	class DNSSDRegisterQuery;
	class DNSSDResolveServiceQuery;
	class DNSSDResolveHostnameQuery;

	class DNSSDQuerier {
		public:
			virtual ~DNSSDQuerier();

			virtual boost::shared_ptr<DNSSDBrowseQuery> createBrowseQuery() = 0;
			virtual boost::shared_ptr<DNSSDRegisterQuery> createRegisterQuery(
					const String& name, int port, const LinkLocalServiceInfo& info) = 0;
			virtual boost::shared_ptr<DNSSDResolveServiceQuery> createResolveServiceQuery(
					const DNSSDServiceID&) = 0;
			virtual boost::shared_ptr<DNSSDResolveHostnameQuery> createResolveHostnameQuery(
					const String& hostname, int interfaceIndex) = 0;
	};
}
