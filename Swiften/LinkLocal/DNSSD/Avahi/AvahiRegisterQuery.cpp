/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiRegisterQuery.h>

#include <iostream>
#include <boost/bind.hpp>

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuerier.h>

namespace Swift {

void AvahiRegisterQuery::registerService() {
	std::cout << "Registering service " << name << ":" << port << std::endl;
	avahi_threaded_poll_lock(querier->getThreadedPoll());
	if (!group) {
		std::cout << "Creating entry group" << std::endl;
		group = avahi_entry_group_new(querier->getClient(), handleEntryGroupChange, this);
		if (!group) {
			std::cout << "Error ceating entry group" << std::endl;
			eventLoop->postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
		}
	}

	doRegisterService();
	avahi_threaded_poll_unlock(querier->getThreadedPoll());
}

void AvahiRegisterQuery::unregisterService() {
	if (group) {
		avahi_entry_group_free(group);
		group = NULL;
	}
}

void AvahiRegisterQuery::updateServiceInfo(const ByteArray& txtRecord) {
	this->txtRecord = txtRecord;
	avahi_threaded_poll_lock(querier->getThreadedPoll());
	assert(group);
	avahi_entry_group_reset(group);
	doRegisterService();
	avahi_threaded_poll_unlock(querier->getThreadedPoll());
}

void AvahiRegisterQuery::doRegisterService() {
	AvahiStringList* txtList;
	avahi_string_list_parse(vecptr(txtRecord), txtRecord.size(), &txtList);

	int result = avahi_entry_group_add_service_strlst(group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, static_cast<AvahiPublishFlags>(0), name.c_str(), "_presence._tcp", NULL, NULL, port, txtList);
	if (result < 0) {
		std::cout << "Error registering service: " << avahi_strerror(result) << std::endl;
		eventLoop->postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
	}
	result = avahi_entry_group_commit(group);
	if (result < 0) {
		std::cout << "Error registering service: " << avahi_strerror(result) << std::endl;
	}
}

void AvahiRegisterQuery::handleEntryGroupChange(AvahiEntryGroup* g, AvahiEntryGroupState state) {
	std::cout << "ENtry group callback: " << state << std::endl;
	switch (state) {
		case AVAHI_ENTRY_GROUP_ESTABLISHED :
			// Domain is a hack!
			eventLoop->postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>(DNSSDServiceID(name, "local", "_presence._tcp", 0))), shared_from_this());
			std::cout << "Entry group established" << std::endl;
			break;
	case AVAHI_ENTRY_GROUP_COLLISION : {
			std::cout << "Entry group collision" << std::endl;
			/*char *n;
			n = avahi_alternative_service_name(name);
			avahi_free(name);
			name = n;*/
			break;
	}

	case AVAHI_ENTRY_GROUP_FAILURE :
			std::cout << "Entry group failure " << avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g))) << std::endl;
			break;

	case AVAHI_ENTRY_GROUP_UNCOMMITED:
	case AVAHI_ENTRY_GROUP_REGISTERING:
			;

	/*
	DNSServiceErrorType result = DNSServiceRegister(
			&sdRef, 0, 0, name.c_str(), "_presence._tcp", NULL, NULL, port, 
			txtRecord.getSize(), txtRecord.getData(), 
			&AvahiRegisterQuery::handleServiceRegisteredStatic, this);
	if (result != kDNSServiceErr_NoError) {
		sdRef = NULL;
	}*/
	//eventLoop->postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
	}
}


}
