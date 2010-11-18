/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/DiscoServiceWalker.h>

#include <boost/bind.hpp>

#include "Swiften/Disco/GetDiscoInfoRequest.h"
#include "Swiften/Disco/GetDiscoItemsRequest.h"

namespace Swift {

DiscoServiceWalker::DiscoServiceWalker(const JID& service, IQRouter* iqRouter, size_t maxSteps) : service_(service), iqRouter_(iqRouter), maxSteps_(maxSteps) {

}

void DiscoServiceWalker::beginWalk() {
	assert(servicesBeingSearched_.size() == 0);
	walkNode(service_);
}

void DiscoServiceWalker::walkNode(const JID& jid) {
	servicesBeingSearched_.push_back(jid);
	searchedServices_.push_back(jid);
	GetDiscoInfoRequest::ref discoInfoRequest = GetDiscoInfoRequest::create(jid, iqRouter_);
	discoInfoRequest->onResponse.connect(boost::bind(&DiscoServiceWalker::handleDiscoInfoResponse, this, _1, _2, jid));
	discoInfoRequest->send();
}

void DiscoServiceWalker::handleReceivedDiscoItem(const JID& item) {
	if (std::find(searchedServices_.begin(), searchedServices_.end(), item) != searchedServices_.end()) {
		/* Don't recurse infinitely */
		return;
	}
	walkNode(item);
}

void DiscoServiceWalker::handleDiscoInfoResponse(boost::shared_ptr<DiscoInfo> info, ErrorPayload::ref error, const JID& jid) {
	if (error) {
		handleDiscoError(jid, error);
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
		GetDiscoItemsRequest::ref discoItemsRequest = GetDiscoItemsRequest::create(jid, iqRouter_);
		discoItemsRequest->onResponse.connect(boost::bind(&DiscoServiceWalker::handleDiscoItemsResponse, this, _1, _2, jid));
		discoItemsRequest->send();
	} else {
		completed = true;
	}
	onServiceFound(jid, info);
	if (completed) {
		markNodeCompleted(jid);
	}
}

void DiscoServiceWalker::handleDiscoItemsResponse(boost::shared_ptr<DiscoItems> items, ErrorPayload::ref error, const JID& jid) {
	if (error) {
		handleDiscoError(jid, error);
		return;
	}
	foreach (DiscoItems::Item item, items->getItems()) {
		if (item.getNode().isEmpty()) {
			/* Don't look at noded items. It's possible that this will exclude some services,
			 * but I've never seen one in the wild, and it's an easy fix for not looping.
			 */
			handleReceivedDiscoItem(item.getJID());
		}
	}
	markNodeCompleted(jid);
}

void DiscoServiceWalker::handleDiscoError(const JID& jid, ErrorPayload::ref /*error*/) {
	markNodeCompleted(jid);
}

void DiscoServiceWalker::markNodeCompleted(const JID& jid) {
	servicesBeingSearched_.erase(std::remove(servicesBeingSearched_.begin(), servicesBeingSearched_.end(), jid), servicesBeingSearched_.end());
	/* All results are in */
	if (servicesBeingSearched_.size() == 0) {
		onWalkComplete();
	}
	/* Check if we're on a rampage */
	if (searchedServices_.size() >= maxSteps_) {
		onWalkComplete();
	}
}

}
