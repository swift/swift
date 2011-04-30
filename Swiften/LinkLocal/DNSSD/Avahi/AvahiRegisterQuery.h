/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <avahi-client/publish.h>

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class AvahiQuerier;

	class AvahiRegisterQuery : public DNSSDRegisterQuery, public AvahiQuery {
		public:	
			AvahiRegisterQuery(const std::string& name, int port, const ByteArray& txtRecord, boost::shared_ptr<AvahiQuerier> querier, EventLoop* eventLoop) : AvahiQuery(querier, eventLoop), name(name), port(port), txtRecord(txtRecord), group(0) {
			}

			void registerService();
			void unregisterService();
			void updateServiceInfo(const ByteArray& txtRecord);

		private:
			void doRegisterService();

			static void handleEntryGroupChange(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata) {
				static_cast<AvahiRegisterQuery*>(userdata)->handleEntryGroupChange(g, state);
			}

			void handleEntryGroupChange(AvahiEntryGroup* g, AvahiEntryGroupState state);

/*
			static void handleServiceRegisteredStatic(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context) {
				static_cast<AvahiRegisterQuery*>(context)->handleServiceRegistered(errorCode, name, regtype, domain);
			}

			void handleServiceRegistered(DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain) {
				if (errorCode != kDNSServiceErr_NoError) {
					eventLoop->postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
				}
				else {
				}
			}
			*/

		private:
			std::string name;
			int port;
			ByteArray txtRecord;
			AvahiEntryGroup* group;
	};
}
