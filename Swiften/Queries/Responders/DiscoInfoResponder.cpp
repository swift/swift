#include "Swiften/Queries/Responders/DiscoInfoResponder.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Elements/DiscoInfo.h"

namespace Swift {

DiscoInfoResponder::DiscoInfoResponder(IQRouter* router) : GetResponder<DiscoInfo>(router) {
}

void DiscoInfoResponder::setDiscoInfo(const DiscoInfo& info) {
	info_ = info;
}

void DiscoInfoResponder::setDiscoInfo(const String& node, const DiscoInfo& info) {
	DiscoInfo newInfo(info);
	newInfo.setNode(node);
	nodeInfo_[node] = newInfo;
}

bool DiscoInfoResponder::handleGetRequest(const JID& from, const String& id, boost::shared_ptr<DiscoInfo> info) {
	if (info->getNode().isEmpty()) {
		sendResponse(from, id, boost::shared_ptr<DiscoInfo>(new DiscoInfo(info_)));
	}
	else {
		std::map<String,DiscoInfo>::const_iterator i = nodeInfo_.find(info->getNode());
		if (i != nodeInfo_.end()) {
			sendResponse(from, id, boost::shared_ptr<DiscoInfo>(new DiscoInfo((*i).second)));
		}
		else {
			sendError(from, id, Error::ItemNotFound, Error::Cancel);
		}
	}
	return true;
}

}
