/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/TLS/ServerIdentityVerifier.h"

#include "Swiften/Base/foreach.h"
#include "Swiften/IDN/IDNA.h"

namespace Swift {

ServerIdentityVerifier::ServerIdentityVerifier(const JID& jid) {
	domain = jid.getDomain();
	encodedDomain = IDNA::getEncoded(domain);
}

bool ServerIdentityVerifier::certificateVerifies(Certificate::ref certificate) {
	bool hasSAN = false;

	// DNS names
	std::vector<String> dnsNames = certificate->getDNSNames();
	foreach (const String& dnsName, dnsNames) {
		if (matchesDomain(dnsName)) {
			return true;
		}
	}
	hasSAN |= !dnsNames.empty();

	// SRV names
	std::vector<String> srvNames = certificate->getSRVNames();
	foreach (const String& srvName, srvNames) {
		// Only match SRV names that begin with the service; this isn't required per
		// spec, but we're being purist about this.
		if (srvName.beginsWith("_xmpp-client.") && matchesDomain(srvName.getSubstring(String("_xmpp-client.").getUTF8Size(), srvName.npos()))) {
			return true;
		}
	}
	hasSAN |= !srvNames.empty();

	// XmppAddr
	std::vector<String> xmppAddresses = certificate->getXMPPAddresses();
	foreach (const String& xmppAddress, xmppAddresses) {
		if (matchesAddress(xmppAddress)) {
			return true;
		}
	}
	hasSAN |= !xmppAddresses.empty();

	// CommonNames. Only check this if there was no SAN (according to spec).
	if (!hasSAN) {
		std::vector<String> commonNames = certificate->getCommonNames();
		foreach (const String& commonName, commonNames) {
			if (matchesDomain(commonName)) {
				return true;
			}
		}
	}

	return false;
}

bool ServerIdentityVerifier::matchesDomain(const String& s) {
	if (s.beginsWith("*.")) {
		String matchString(s.getSubstring(2, s.npos()));
		String matchDomain = encodedDomain;
		int dotIndex = matchDomain.find('.');
		if (dotIndex >= 0) {
			matchDomain = matchDomain.getSubstring(dotIndex + 1, matchDomain.npos());
		}
		return matchString == matchDomain;
	}
	else {
		return s == encodedDomain;
	}
}

bool ServerIdentityVerifier::matchesAddress(const String& s) {
	return s == domain;
}

}
