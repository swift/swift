/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/ServerIdentityVerifier.h>

#include <boost/algorithm/string.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/IDN/IDNConverter.h>

namespace Swift {

ServerIdentityVerifier::ServerIdentityVerifier(const JID& jid, IDNConverter* idnConverter) : domainValid(false) {
	domain = jid.getDomain();
	boost::optional<std::string> domainResult = idnConverter->getIDNAEncoded(domain);
	if (!!domainResult) {
		encodedDomain = *domainResult;
		domainValid = true;
	}
}

bool ServerIdentityVerifier::certificateVerifies(Certificate::ref certificate) {
	bool hasSAN = false;

	if (certificate == NULL) {
	    return false;
	}
	// DNS names
	std::vector<std::string> dnsNames = certificate->getDNSNames();
	foreach (const std::string& dnsName, dnsNames) {
		if (matchesDomain(dnsName)) {
			return true;
		}
	}
	hasSAN |= !dnsNames.empty();

	// SRV names
	std::vector<std::string> srvNames = certificate->getSRVNames();
	foreach (const std::string& srvName, srvNames) {
		// Only match SRV names that begin with the service; this isn't required per
		// spec, but we're being purist about this.
		if (boost::starts_with(srvName, "_xmpp-client.") && matchesDomain(srvName.substr(std::string("_xmpp-client.").size(), srvName.npos))) {
			return true;
		}
	}
	hasSAN |= !srvNames.empty();

	// XmppAddr
	std::vector<std::string> xmppAddresses = certificate->getXMPPAddresses();
	foreach (const std::string& xmppAddress, xmppAddresses) {
		if (matchesAddress(xmppAddress)) {
			return true;
		}
	}
	hasSAN |= !xmppAddresses.empty();

	// CommonNames. Only check this if there was no SAN (according to spec).
	if (!hasSAN) {
		std::vector<std::string> commonNames = certificate->getCommonNames();
		foreach (const std::string& commonName, commonNames) {
			if (matchesDomain(commonName)) {
				return true;
			}
		}
	}

	return false;
}

bool ServerIdentityVerifier::matchesDomain(const std::string& s) const {
	if (!domainValid) {
		return false;
	}
	if (boost::starts_with(s, "*.")) {
		std::string matchString(s.substr(2, s.npos));
		std::string matchDomain = encodedDomain;
		size_t dotIndex = matchDomain.find('.');
		if (dotIndex != matchDomain.npos) {
			matchDomain = matchDomain.substr(dotIndex + 1, matchDomain.npos);
		}
		return matchString == matchDomain;
	}
	else {
		return s == encodedDomain;
	}
}

bool ServerIdentityVerifier::matchesAddress(const std::string& s) const {
	return s == domain;
}

}
