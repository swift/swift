#include <stringprep.h>
#include <vector>
#include <iostream>

#include "Swiften/JID/JID.h"

namespace Swift {


class StringPrepper {
	private:
		static const int MAX_STRINGPREP_SIZE = 1024;

	public:
		static String getNamePrepped(const String& name) {
			return getStringPrepped(name, stringprep_nameprep);
		}

		static String getNodePrepped(const String& node) {
			return getStringPrepped(node, stringprep_xmpp_nodeprep);
		}

		static String getResourcePrepped(const String& resource) {
			return getStringPrepped(resource, stringprep_xmpp_resourceprep);
		}

		static String getStringPrepped(const String& s, const Stringprep_profile profile[]) {
			std::vector<char> input(s.getUTF8String().begin(), s.getUTF8String().end());
			input.resize(MAX_STRINGPREP_SIZE);
			if (stringprep(&input[0], MAX_STRINGPREP_SIZE, static_cast<Stringprep_profile_flags>(0), profile) == 0) {
				return String(&input[0]);
			}
			else {
				return "";
			}
		}
};


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
	node_ = StringPrepper::getNamePrepped(node);
	domain_ = StringPrepper::getNodePrepped(domain);
	resource_ = StringPrepper::getResourcePrepped(resource);
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

