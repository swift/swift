/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#define SWIFTEN_CACHE_JID_PREP

#include <sstream>
#include <string>
#include <vector>

#ifdef SWIFTEN_CACHE_JID_PREP
#include <mutex>
#include <unordered_map>
#endif

#include <boost/optional.hpp>

#include <Swiften/Base/String.h>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/JID/JID.h>

#ifndef SWIFTEN_JID_NO_DEFAULT_IDN_CONVERTER
#include <memory>
#include <Swiften/IDN/PlatformIDNConverter.h>
#endif

using namespace Swift;

#ifdef SWIFTEN_CACHE_JID_PREP
typedef std::unordered_map<std::string, std::string> PrepCache;

static std::mutex namePrepCacheMutex;
static PrepCache nodePrepCache;
static PrepCache domainPrepCache;
static PrepCache resourcePrepCache;
#endif

static const std::vector<char> escapedChars = {' ', '"', '&', '\'', '/', '<', '>', '@', ':'};

static IDNConverter* idnConverter = nullptr;

#ifndef SWIFTEN_JID_NO_DEFAULT_IDN_CONVERTER
namespace {
    struct IDNInitializer {
        IDNInitializer() : defaultIDNConverter(PlatformIDNConverter::create()) {
            idnConverter = defaultIDNConverter.get();
        }
        std::shared_ptr<IDNConverter> defaultIDNConverter;
    } initializer;
}
#endif

static std::string getEscaped(char c) {
    return makeString() << '\\' << std::hex << static_cast<int>(c);
}

static bool getEscapeSequenceValue(const std::string& sequence, unsigned char& value) {
    std::stringstream s;
    unsigned int v;
    s << std::hex << sequence;
    s >> v;
    value = static_cast<unsigned char>(v);
    return (!s.fail() && !s.bad() && (value == 0x5C || std::find(escapedChars.begin(), escapedChars.end(), value) != escapedChars.end()));
}

namespace Swift {

JID::JID(const char* jid) : valid_(true) {
    assert(jid);
    initializeFromString(std::string(jid));
}

JID::JID(const std::string& jid) : valid_(true) {
    initializeFromString(jid);
}

JID::JID(const std::string& node, const std::string& domain) : valid_(true), hasResource_(false) {
    nameprepAndSetComponents(node, domain, "");
}

JID::JID(const std::string& node, const std::string& domain, const std::string& resource) : valid_(true), hasResource_(true) {
    if (resource.empty()) {
        valid_ = false;
    }
    nameprepAndSetComponents(node, domain, resource);
}

void JID::initializeFromString(const std::string& jid) {
    if (String::beginsWith(jid, '@')) {
        valid_ = false;
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
    if (domain.empty() || !idnConverter->getIDNAEncoded(domain)) {
        valid_ = false;
        return;
    }

    if (hasResource_ && resource.empty()) {
        valid_ = false;
        return;
    }
#ifndef SWIFTEN_CACHE_JID_PREP
    node_ = idnConverter->getStringPrepared(node, IDNConverter::XMPPNodePrep);
    domain_ = idnConverter->getStringPrepared(domain, IDNConverter::NamePrep);
    resource_ = idnConverter->getStringPrepared(resource, IDNConverter::XMPPResourcePrep);
#else
    std::unique_lock<std::mutex> lock(namePrepCacheMutex);

    std::pair<PrepCache::iterator, bool> r;

    r = nodePrepCache.insert(std::make_pair(node, std::string()));
    if (r.second) {
        try {
            r.first->second = idnConverter->getStringPrepared(node, IDNConverter::XMPPNodePrep);
        }
        catch (...) {
            nodePrepCache.erase(r.first);
            valid_ = false;
            return;
        }
    }
    node_ = r.first->second;

    r = domainPrepCache.insert(std::make_pair(domain, std::string()));
    if (r.second) {
        try {
            r.first->second = idnConverter->getStringPrepared(domain, IDNConverter::NamePrep);
        }
        catch (...) {
            domainPrepCache.erase(r.first);
            valid_ = false;
            return;
        }
    }
    domain_ = r.first->second;

    r = resourcePrepCache.insert(std::make_pair(resource, std::string()));
    if (r.second) {
        try {
            r.first->second = idnConverter->getStringPrepared(resource, IDNConverter::XMPPResourcePrep);
        }
        catch (...) {
            resourcePrepCache.erase(r.first);
            valid_ = false;
            return;
        }
    }
    resource_ = r.first->second;
#endif

    if (domain_.empty()) {
        valid_ = false;
        return;
    }
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

std::string JID::getEscapedNode(const std::string& node) {
    std::string result;
    for (std::string::const_iterator i = node.begin(); i != node.end(); ++i) {
        if (std::find(escapedChars.begin(), escapedChars.end(), *i) != escapedChars.end()) {
            result += getEscaped(*i);
            continue;
        }
        else if (*i == '\\') {
            // Check if we have an escaped dissalowed character sequence
            std::string::const_iterator innerBegin = i + 1;
            if (innerBegin != node.end() && innerBegin + 1 != node.end()) {
                std::string::const_iterator innerEnd = innerBegin + 2;
                unsigned char value;
                if (getEscapeSequenceValue(std::string(innerBegin, innerEnd), value)) {
                    result += getEscaped(*i);
                    continue;
                }
            }
        }
        result += *i;
    }
    return result;
}

std::string JID::getUnescapedNode() const {
    std::string result;
    for (std::string::const_iterator j = node_.begin(); j != node_.end();) {
        if (*j == '\\') {
            std::string::const_iterator innerEnd = j + 1;
            for (size_t i = 0; i < 2 && innerEnd != node_.end(); ++i, ++innerEnd) {
            }
            unsigned char value;
            if (getEscapeSequenceValue(std::string(j + 1, innerEnd), value)) {
                result += std::string(reinterpret_cast<const char*>(&value), 1);
                j = innerEnd;
                continue;
            }
        }
        result += *j;
        ++j;
    }
    return result;
}

void JID::setIDNConverter(IDNConverter* converter) {
    idnConverter = converter;
}

std::ostream& operator<<(std::ostream& os, const JID& j) {
    os << j.toString();
    return os;
}

boost::optional<JID> JID::parse(const std::string& s) {
    JID jid(s);
    return jid.isValid() ? jid : boost::optional<JID>();
}

}
