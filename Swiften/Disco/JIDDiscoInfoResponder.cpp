/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Disco/JIDDiscoInfoResponder.h>

#include <memory>

#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Queries/IQRouter.h>

namespace Swift {

JIDDiscoInfoResponder::JIDDiscoInfoResponder(IQRouter* router) : GetResponder<DiscoInfo>(router) {
}

void JIDDiscoInfoResponder::clearDiscoInfo(const JID& jid) {
    info.erase(jid);
}

void JIDDiscoInfoResponder::setDiscoInfo(const JID& jid, const DiscoInfo& discoInfo) {
    JIDDiscoInfoMap::iterator i = info.insert(std::make_pair(jid, JIDDiscoInfo())).first;
    i->second.discoInfo = discoInfo;
}

void JIDDiscoInfoResponder::setDiscoInfo(const JID& jid, const std::string& node, const DiscoInfo& discoInfo) {
    JIDDiscoInfoMap::iterator i = info.insert(std::make_pair(jid, JIDDiscoInfo())).first;
    DiscoInfo newInfo(discoInfo);
    newInfo.setNode(node);
    i->second.nodeDiscoInfo[node] = newInfo;
}

bool JIDDiscoInfoResponder::handleGetRequest(const JID& from, const JID& to, const std::string& id, std::shared_ptr<DiscoInfo> discoInfo) {
    JIDDiscoInfoMap::const_iterator i = info.find(to);
    if (i != info.end()) {
        if (discoInfo->getNode().empty()) {
            sendResponse(from, to, id, std::make_shared<DiscoInfo>(i->second.discoInfo));
        }
        else {
            std::map<std::string,DiscoInfo>::const_iterator j = i->second.nodeDiscoInfo.find(discoInfo->getNode());
            if (j != i->second.nodeDiscoInfo.end()) {
                sendResponse(from, to, id, std::make_shared<DiscoInfo>(j->second));
            }
            else {
                sendError(from, to, id, ErrorPayload::ItemNotFound, ErrorPayload::Cancel);
            }
        }
    }
    else {
        sendError(from, to, id, ErrorPayload::ItemNotFound, ErrorPayload::Cancel);
    }
    return true;
}

}
