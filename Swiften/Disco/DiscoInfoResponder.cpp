/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Disco/DiscoInfoResponder.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Elements/DiscoInfo.h>

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

bool DiscoInfoResponder::handleGetRequest(const JID& from, const JID&, const std::string& id, boost::shared_ptr<DiscoInfo> info) {
	if (info->getNode().empty()) {
		sendResponse(from, id, boost::make_shared<DiscoInfo>(info_));
	}
	else {
		std::map<std::string,DiscoInfo>::const_iterator i = nodeInfo_.find(info->getNode());
		if (i != nodeInfo_.end()) {
			sendResponse(from, id, boost::make_shared<DiscoInfo>((*i).second));
		}
		else {
			sendError(from, id, ErrorPayload::ItemNotFound, ErrorPayload::Cancel);
		}
	}
	return true;
}

}
