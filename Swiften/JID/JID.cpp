/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <stringprep.h>
#include <vector>
#include <iostream>

#include "Swiften/JID/JID.h"
#include "Swiften/StringPrep/StringPrep.h"

namespace Swift {

JID::JID(const char* jid) {
	initializeFromString(String(jid));
}

JID::JID(const String& jid) {
	initializeFromString(jid);
}

JID::JID(const String& node, const String& domain) : hasResource_(false) {
	nameprepAndSetComponents(node, domain, "");
}

JID::JID(const String& node, const String& domain, const String& resource) : hasResource_(true) {
	nameprepAndSetComponents(node, domain, resource);
}

void JID::initializeFromString(const String& jid) {
	if (jid.beginsWith('@')) {
		return;
	}

	String bare, resource;
	size_t slashIndex = jid.find('/');
	if (slashIndex != jid.npos()) {
		hasResource_ = true;
		bare = jid.getSubstring(0, slashIndex);
		resource = jid.getSubstring(slashIndex + 1, jid.npos());
	}
	else {
		hasResource_ = false;
		bare = jid;
	}
	std::pair<String,String> nodeAndDomain = bare.getSplittedAtFirst('@');
	if (nodeAndDomain.second.isEmpty()) {
		nameprepAndSetComponents("", nodeAndDomain.first, resource);
	}
	else {
		nameprepAndSetComponents(nodeAndDomain.first, nodeAndDomain.second, resource);
	}
}


void JID::nameprepAndSetComponents(const String& node, const String& domain, const String& resource) {
	node_ = StringPrep::getPrepared(node, StringPrep::NamePrep);
	domain_ = StringPrep::getPrepared(domain, StringPrep::XMPPNodePrep);
	resource_ = StringPrep::getPrepared(resource, StringPrep::XMPPResourcePrep);
}

String JID::toString() const {
	String string;
	if (!node_.isEmpty()) {
		string += node_ + "@";
	}
	string += domain_;
	if (!isBare()) {
		string += "/" + resource_;
	}
	return string;
}

int JID::compare(const Swift::JID& o, CompareType compareType) const {
	if (node_ < o.node_) { return -1; }
	if (node_ > o.node_) { return 1; }
	if (domain_ < o.domain_) { return -1; }
	if (domain_ > o.domain_) { return 1; }
	if (compareType == WithResource) {
		if (hasResource_ != o.hasResource_) {
			return hasResource_ ? 1 : -1;
		}
		if (resource_ < o.resource_) { return -1; }
		if (resource_ > o.resource_) { return 1; }
	}
	return 0;
}

} // namespace Swift

