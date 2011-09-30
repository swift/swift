/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Disco/DiscoServiceWalker.h>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>

#include <boost/bind.hpp>

namespace Swift {

DiscoServiceWalker::DiscoServiceWalker(const JID& service, IQRouter* iqRouter, size_t maxSteps) : service_(service), iqRouter_(iqRouter), maxSteps_(maxSteps), active_(false) {

}

void DiscoServiceWalker::beginWalk() {
	SWIFT_LOG(debug) << "Starting walk to " << service_ << std::endl;
	assert(!active_);
	assert(servicesBeingSearched_.empty());
	active_ = true;
	walkNode(service_);
}

void DiscoServiceWalker::endWalk() {
	if (active_) {
		SWIFT_LOG(debug) << "Ending walk to " << service_ << std::endl;
		foreach (GetDiscoInfoRequest::ref request, pendingDiscoInfoRequests_) {
			request->onResponse.disconnect(boost::bind(&DiscoServiceWalker::handleDiscoInfoResponse, this, _1, _2, request));
		}
		foreach (GetDiscoItemsRequest::ref request, pendingDiscoItemsRequests_) {
			request->onResponse.disconnect(boost::bind(&DiscoServiceWalker::handleDiscoItemsResponse, this, _1, _2, request));
		}
		active_ = false;
	}
}

void DiscoServiceWalker::walkNode(const JID& jid) {
	SWIFT_LOG(debug) << "Walking node " << jid << std::endl;
	servicesBeingSearched_.insert(jid);
	searchedServices_.insert(jid);
	GetDiscoInfoRequest::ref discoInfoRequest = GetDiscoInfoRequest::create(jid, iqRouter_);
	discoInfoRequest->onResponse.connect(boost::bind(&DiscoServiceWalker::handleDiscoInfoResponse, this, _1, _2, discoInfoRequest));
	pendingDiscoInfoRequests_.insert(discoInfoRequest);
	discoInfoRequest->send();
}

void DiscoServiceWalker::handleDiscoInfoResponse(boost::shared_ptr<DiscoInfo> info, ErrorPayload::ref error, GetDiscoInfoRequest::ref request) {
	/* If we got canceled, don't do anything */
	if (!active_) {
		return;
	}

	SWIFT_LOG(debug) << "Disco info response from " << request->getReceiver() << std::endl;

	pendingDiscoInfoRequests_.erase(request);
	if (error) {
		handleDiscoError(request->getReceiver(), error);
		return;
	}

	bool couldContainServices = false;
	foreach (DiscoInfo::Identity identity, info->getIdentities()) {
		if (identity.getCategory() == "server") {
			couldContainServices = true;
		}
	}
	bool completed = false;
	if (couldContainServices) {
		GetDiscoItemsRequest::ref discoItemsRequest = GetDiscoItemsRequest::create(request->getReceiver(), iqRouter_);
		discoItemsRequest->onResponse.connect(boost::bind(&DiscoServiceWalker::handleDiscoItemsResponse, this, _1, _2, discoItemsRequest));
		pendingDiscoItemsRequests_.insert(discoItemsRequest);
		discoItemsRequest->send();
	} else {
		completed = true;
	}
	onServiceFound(request->getReceiver(), info);
	if (completed) {
		markNodeCompleted(request->getReceiver());
	}
}

void DiscoServiceWalker::handleDiscoItemsResponse(boost::shared_ptr<DiscoItems> items, ErrorPayload::ref error, GetDiscoItemsRequest::ref request) {
	/* If we got canceled, don't do anything */
	if (!active_) {
		return;
	}

	SWIFT_LOG(debug) << "Received disco items from " << request->getReceiver() << std::endl;
	pendingDiscoItemsRequests_.erase(request);
	if (error) {
		handleDiscoError(request->getReceiver(), error);
		return;
	}
	foreach (DiscoItems::Item item, items->getItems()) {
		if (item.getNode().empty()) {
			/* Don't look at noded items. It's possible that this will exclude some services,
			 * but I've never seen one in the wild, and it's an easy fix for not looping.
			 */
			if (std::find(searchedServices_.begin(), searchedServices_.end(), item.getJID()) == searchedServices_.end()) { /* Don't recurse infinitely */
				SWIFT_LOG(debug) << "Received disco item " << item.getJID() << std::endl;
				walkNode(item.getJID());
			}
		}
	}
	markNodeCompleted(request->getReceiver());
}

void DiscoServiceWalker::handleDiscoError(const JID& jid, ErrorPayload::ref /*error*/) {
	SWIFT_LOG(debug) << "Disco error from " << jid << std::endl;
	markNodeCompleted(jid);
}

void DiscoServiceWalker::markNodeCompleted(const JID& jid) {
	SWIFT_LOG(debug) << "Node completed " << jid << std::endl;
	servicesBeingSearched_.erase(jid);
	/* All results are in */
	if (servicesBeingSearched_.empty()) {
		active_ = false;
		onWalkComplete();
	}
	/* Check if we're on a rampage */
	else if (searchedServices_.size() >= maxSteps_) {
		active_ = false;
		onWalkComplete();
	}
}

}
