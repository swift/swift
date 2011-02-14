/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#define SWIFTEN_CACHE_JID_PREP

#include <vector>
#include <iostream>

#include <string>
#ifdef SWIFTEN_CACHE_JID_PREP
#include <boost/unordered_map.hpp>
#endif
#include <stringprep.h>

#include <Swiften/Base/String.h>
#include "Swiften/JID/JID.h"
#include "Swiften/IDN/StringPrep.h"

#ifdef SWIFTEN_CACHE_JID_PREP
typedef boost::unordered_map<std::string, std::string> PrepCache;

static PrepCache nodePrepCache;
static PrepCache domainPrepCache;
static PrepCache resourcePrepCache;
#endif

namespace Swift {

JID::JID(const char* jid) {
	initializeFromString(std::string(jid));
}

JID::JID(const std::string& jid) {
	initializeFromString(jid);
}

JID::JID(const std::string& node, const std::string& domain) : hasResource_(false) {
	nameprepAndSetComponents(node, domain, "");
}

JID::JID(const std::string& node, const std::string& domain, const std::string& resource) : hasResource_(true) {
	nameprepAndSetComponents(node, domain, resource);
}

void JID::initializeFromString(const std::string& jid) {
	if (String::beginsWith(jid, '@')) {
		return;
	}

	std::string bare, resource;
	size_t slashIndex = jid.find('/');
	if (slashIndex != jid.npos) {
		hasResource_ = true;
		bare = jid.substr(0, slashIndex);
		resource = jid.substr(slashIndex + 1, jid.npos);
	}
	else {
		hasResource_ = false;
		bare = jid;
	}
	std::pair<std::string,std::string> nodeAndDomain = String::getSplittedAtFirst(bare, '@');
	if (nodeAndDomain.second.empty()) {
		nameprepAndSetComponents("", nodeAndDomain.first, resource);
	}
	else {
		nameprepAndSetComponents(nodeAndDomain.first, nodeAndDomain.second, resource);
	}
}


void JID::nameprepAndSetComponents(const std::string& node, const std::string& domain, const std::string& resource) {
#ifndef SWIFTEN_CACHE_JID_PREP
	node_ = StringPrep::getPrepared(node, StringPrep::NamePrep);
	domain_ = StringPrep::getPrepared(domain, StringPrep::XMPPNodePrep);
	resource_ = StringPrep::getPrepared(resource, StringPrep::XMPPResourcePrep);
#else
	std::pair<PrepCache::iterator, bool> r;

	r = nodePrepCache.insert(std::make_pair(node, std::string()));
	if (r.second) {
		r.first->second = StringPrep::getPrepared(node, StringPrep::NamePrep);
	}
	node_ = r.first->second;

	r = domainPrepCache.insert(std::make_pair(domain, std::string()));
	if (r.second) {
		r.first->second = StringPrep::getPrepared(domain, StringPrep::XMPPNodePrep);
	}
	domain_ = r.first->second;

	r = resourcePrepCache.insert(std::make_pair(resource, std::string()));
	if (r.second) {
		r.first->second = StringPrep::getPrepared(resource, StringPrep::XMPPResourcePrep);
	}
	resource_ = r.first->second;
#endif
}

std::string JID::toString() const {
	std::string string;
	if (!node_.empty()) {
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

