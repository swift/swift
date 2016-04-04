/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/UnboundDomainNameResolver.h>

#include <memory>
#include <vector>

#include <boost/bind.hpp>

#include <arpa/inet.h>
#include <ldns/ldns.h>
#include <unbound.h>
#include <unistd.h>

#include <Swiften/Base/Log.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Network/DomainNameResolveError.h>
#include <Swiften/Network/DomainNameServiceQuery.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/TimerFactory.h>

namespace Swift {

class UnboundQuery {
    public:
        UnboundQuery(UnboundDomainNameResolver* resolver, ub_ctx* context) : resolver(resolver), ubContext(context) {}
        virtual ~UnboundQuery() {}
        virtual void handleResult(int err, ub_result* result) = 0;
    protected:
        UnboundDomainNameResolver* resolver;
        ub_ctx* ubContext;
};

struct UnboundWrapperHelper {
    UnboundWrapperHelper(UnboundDomainNameResolver* resolver, std::shared_ptr<UnboundQuery> query) : resolver(resolver), query(query) {}
    UnboundDomainNameResolver* resolver;
    std::shared_ptr<UnboundQuery> query;
};

class UnboundDomainNameServiceQuery : public DomainNameServiceQuery, public UnboundQuery, public std::enable_shared_from_this<UnboundDomainNameServiceQuery>  {
    public:
        UnboundDomainNameServiceQuery(UnboundDomainNameResolver* resolver, ub_ctx* context, std::string name) : UnboundQuery(resolver, context), name(name) {
        }

        virtual ~UnboundDomainNameServiceQuery() { }

        virtual void run() {
            int retval;
            UnboundWrapperHelper* helper = new UnboundWrapperHelper(resolver, shared_from_this());

            retval = ub_resolve_async(ubContext, const_cast<char*>(name.c_str()), LDNS_RR_TYPE_SRV,
                          1 /* CLASS IN (internet) */,
                          helper, UnboundDomainNameResolver::unbound_callback_wrapper, NULL);
            if(retval != 0) {
                SWIFT_LOG(debug) << "resolve error: " << ub_strerror(retval) << std::endl;
                delete helper;
            }
        }

        void handleResult(int err, struct ub_result* result) {
            std::vector<DomainNameServiceQuery::Result> serviceRecords;

            if(err != 0) {
                SWIFT_LOG(debug) << "resolve error: " << ub_strerror(err) << std::endl;
            } else {
                if(result->havedata) {
                    ldns_pkt* replyPacket = 0;
                    ldns_buffer* buffer = ldns_buffer_new(1024);
                    if (buffer && ldns_wire2pkt(&replyPacket, static_cast<const uint8_t*>(result->answer_packet), result->answer_len) == LDNS_STATUS_OK) {
                        ldns_rr_list* rrList = ldns_pkt_answer(replyPacket);
                        for (size_t n = 0; n < ldns_rr_list_rr_count(rrList); n++) {
                            ldns_rr* rr = ldns_rr_list_rr(rrList, n);
                            if ((ldns_rr_get_type(rr) != LDNS_RR_TYPE_SRV) ||
                                (ldns_rr_get_class(rr) != LDNS_RR_CLASS_IN) ||
                                (ldns_rr_rd_count(rr) != 4)) {
                                continue;
                            }

                            DomainNameServiceQuery::Result serviceRecord;
                            serviceRecord.priority = ldns_rdf2native_int16(ldns_rr_rdf(rr, 0));
                            serviceRecord.weight = ldns_rdf2native_int16(ldns_rr_rdf(rr, 1));
                            serviceRecord.port = ldns_rdf2native_int16(ldns_rr_rdf(rr, 2));

                            ldns_buffer_rewind(buffer);
                            if ((ldns_rdf2buffer_str_dname(buffer, ldns_rr_rdf(rr, 3)) != LDNS_STATUS_OK) ||
                                (ldns_buffer_position(buffer) < 2) ||
                                !ldns_buffer_reserve(buffer, 1)) {
                                // either name invalid, empty or buffer to small
                                continue;
                            }
                            char terminator = 0;
                            ldns_buffer_write(buffer, &terminator, sizeof(terminator));

                            serviceRecord.hostname = std::string(reinterpret_cast<char*>(ldns_buffer_at(buffer, 0)));
                            serviceRecords.push_back(serviceRecord);
                            SWIFT_LOG(debug) << "hostname " << serviceRecord.hostname << " added" << std::endl;
                        }
                    }
                    if (replyPacket) ldns_pkt_free(replyPacket);
                    if (buffer) ldns_buffer_free(buffer);
                }
            }

            ub_resolve_free(result);
            onResult(serviceRecords);
        }

    private:
        std::string name;
};

class UnboundDomainNameAddressQuery : public DomainNameAddressQuery, public UnboundQuery, public std::enable_shared_from_this<UnboundDomainNameAddressQuery> {
    public:
        UnboundDomainNameAddressQuery(UnboundDomainNameResolver* resolver, ub_ctx* context, std::string name) : UnboundQuery(resolver, context), name(name) {
        }

        virtual ~UnboundDomainNameAddressQuery() { }

        virtual void run() {
            int retval;
            UnboundWrapperHelper* helper = new UnboundWrapperHelper(resolver, shared_from_this());

            //FIXME: support AAAA queries in some way
            retval = ub_resolve_async(ubContext, const_cast<char*>(name.c_str()), LDNS_RR_TYPE_A,
                          1 /* CLASS IN (internet) */,
                          helper, UnboundDomainNameResolver::unbound_callback_wrapper, NULL);
            if(retval != 0) {
                SWIFT_LOG(debug) << "resolve error: " << ub_strerror(retval) << std::endl;
                delete helper;
            }
        }

        void handleResult(int err, struct ub_result* result) {
            std::vector<HostAddress> addresses;
            boost::optional<DomainNameResolveError> error;
            SWIFT_LOG(debug) << "Result for: " << name << std::endl;

            if(err != 0) {
                SWIFT_LOG(debug) << "resolve error: " << ub_strerror(err) << std::endl;
                error = DomainNameResolveError();
            } else {
                if(result->havedata) {
                    for(int i=0; result->data[i]; i++) {
                        char address[100];
                        const char* addressStr = 0;
                        if ((addressStr = inet_ntop(AF_INET, result->data[i], address, 100))) {
                            SWIFT_LOG(debug) << "IPv4 address: " << addressStr << std::endl;
                            addresses.push_back(HostAddress(std::string(addressStr)));
                        } else if ((addressStr = inet_ntop(AF_INET6, result->data[i], address, 100))) {
                            SWIFT_LOG(debug) << "IPv6 address: " << addressStr << std::endl;
                            addresses.push_back(HostAddress(std::string(addressStr)));
                        } else {
                            SWIFT_LOG(debug) << "inet_ntop() failed" << std::endl;
                            error = DomainNameResolveError();
                        }
                    }
                } else {
                    error = DomainNameResolveError();
                }
            }

            ub_resolve_free(result);
            onResult(addresses, error);
        }

    private:
        std::string name;
};

UnboundDomainNameResolver::UnboundDomainNameResolver(IDNConverter* idnConverter, std::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) : idnConverter(idnConverter), ioService(ioService), ubDescriptior(*ioService), eventLoop(eventLoop) {
    ubContext = ub_ctx_create();
    if(!ubContext) {
        SWIFT_LOG(debug) << "could not create unbound context" << std::endl;
    }
    eventOwner = std::make_shared<EventOwner>();

    ub_ctx_async(ubContext, true);

    int ret;

    /* read /etc/resolv.conf for DNS proxy settings (from DHCP) */
    if( (ret=ub_ctx_resolvconf(ubContext, const_cast<char*>("/etc/resolv.conf"))) != 0) {
        SWIFT_LOG(error) << "error reading resolv.conf: " << ub_strerror(ret) << ". errno says: " << strerror(errno) << std::endl;
    }
    /* read /etc/hosts for locally supplied host addresses */
    if( (ret=ub_ctx_hosts(ubContext, const_cast<char*>("/etc/hosts"))) != 0) {
        SWIFT_LOG(error) << "error reading hosts: " << ub_strerror(ret) << ". errno says: " << strerror(errno) << std::endl;
    }

    ubDescriptior.assign(ub_fd(ubContext));

    ubDescriptior.async_read_some(boost::asio::null_buffers(), boost::bind(&UnboundDomainNameResolver::handleUBSocketReadable, this, boost::asio::placeholders::error));
}

UnboundDomainNameResolver::~UnboundDomainNameResolver() {
    eventLoop->removeEventsFromOwner(eventOwner);
    if (ubContext) {
        ub_ctx_delete(ubContext);
    }
}

void UnboundDomainNameResolver::unbound_callback(std::shared_ptr<UnboundQuery> query, int err, ub_result* result) {
    query->handleResult(err, result);
}

void UnboundDomainNameResolver::unbound_callback_wrapper(void* data, int err, ub_result* result) {
    UnboundWrapperHelper* helper = static_cast<UnboundWrapperHelper*>(data);
    UnboundDomainNameResolver* resolver = helper->resolver;
    resolver->unbound_callback(helper->query, err, result);
    delete helper;
}

void UnboundDomainNameResolver::handleUBSocketReadable(boost::system::error_code) {
    eventLoop->postEvent(boost::bind(&UnboundDomainNameResolver::processData, this), eventOwner);
    ubDescriptior.async_read_some(boost::asio::null_buffers(), boost::bind(&UnboundDomainNameResolver::handleUBSocketReadable, this, boost::asio::placeholders::error));
}

void UnboundDomainNameResolver::processData() {
    if (ub_poll(ubContext)) {
        int ret = ub_process(ubContext);
        if(ret != 0) {
            SWIFT_LOG(debug) << "resolve error: " << ub_strerror(ret) << std::endl;
        }
    }
}

std::shared_ptr<DomainNameServiceQuery> UnboundDomainNameResolver::createServiceQuery(const std::string& serviceLookupPrefix, const std::string& domain) {
    boost::optional<std::string> encodedDomain = idnConverter->getIDNAEncoded(domain);
    std::string result;
    if (encodedDomain) {
        result = serviceLookupPrefix + *encodedDomain;
    }
    return std::make_shared<UnboundDomainNameServiceQuery>(this, ubContext, result);
}

std::shared_ptr<DomainNameAddressQuery> UnboundDomainNameResolver::createAddressQuery(const std::string& name) {
    return std::make_shared<UnboundDomainNameAddressQuery>(this, ubContext, idnConverter->getIDNAEncoded(name).get_value_or(""));
}

}
