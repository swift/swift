/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Disco/DiscoInfoResponder.h>

#include <memory>

#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Queries/IQRouter.h>

namespace Swift {

DiscoInfoResponder::DiscoInfoResponder(IQRouter* router) : GetResponder<DiscoInfo>(router) {
}

void DiscoInfoResponder::clearDiscoInfo() {
    info_ = DiscoInfo();
    nodeInfo_.clear();
}

void DiscoInfoResponder::setDiscoInfo(const DiscoInfo& info) {
    info_ = info;
}

void DiscoInfoResponder::setDiscoInfo(const std::string& node, const DiscoInfo& info) {
    DiscoInfo newInfo(info);
    newInfo.setNode(node);
    nodeInfo_[node] = newInfo;
}

bool DiscoInfoResponder::handleGetRequest(const JID& from, const JID&, const std::string& id, std::shared_ptr<DiscoInfo> info) {
    if (info->getNode().empty()) {
        sendResponse(from, id, std::make_shared<DiscoInfo>(info_));
    }
    else {
        std::map<std::string,DiscoInfo>::const_iterator i = nodeInfo_.find(info->getNode());
        if (i != nodeInfo_.end()) {
            sendResponse(from, id, std::make_shared<DiscoInfo>((*i).second));
        }
        else {
            sendError(from, id, ErrorPayload::ItemNotFound, ErrorPayload::Cancel);
        }
    }
    return true;
}

}
