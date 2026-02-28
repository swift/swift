/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/BlockListController.h>

#include <boost/bind.hpp>

#include <Swiften/Base/format.h>
#include <Swiften/Client/ClientBlockListManager.h>

#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/UIEvents/RequestBlockListDialogUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChangeBlockStateUIEvent.h>
#include <Swift/Controllers/UIInterfaces/BlockListEditorWidget.h>
#include <Swift/Controllers/XMPPEvents/ErrorEvent.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

namespace Swift {

BlockListController::BlockListController(ClientBlockListManager* blockListManager, UIEventStream* uiEventStream, BlockListEditorWidgetFactory* blockListEditorWidgetFactory, EventController* eventController) : blockListManager_(blockListManager), blockListEditorWidgetFactory_(blockListEditorWidgetFactory), blockListEditorWidget_(nullptr), eventController_(eventController), remainingRequests_(0), uiEventStream_(uiEventStream) {
    uiEventStream->onUIEvent.connect(boost::bind(&BlockListController::handleUIEvent, this, _1));
    blockListManager_->getBlockList()->onItemAdded.connect(boost::bind(&BlockListController::handleBlockListChanged, this));
    blockListManager_->getBlockList()->onItemRemoved.connect(boost::bind(&BlockListController::handleBlockListChanged, this));
}

BlockListController::~BlockListController() {
    uiEventStream_->onUIEvent.disconnect(boost::bind(&BlockListController::handleUIEvent, this, _1));
    blockListManager_->getBlockList()->onItemAdded.disconnect(boost::bind(&BlockListController::handleBlockListChanged, this));
    blockListManager_->getBlockList()->onItemRemoved.disconnect(boost::bind(&BlockListController::handleBlockListChanged, this));
}

void BlockListController::blockListDifferences(const std::vector<JID> &newBlockList, std::vector<JID> &jidsToUnblock, std::vector<JID> &jidsToBlock) const {
    for (const auto& jid : blockListBeforeEdit) {
        if (std::find(newBlockList.begin(), newBlockList.end(), jid) == newBlockList.end()) {
            jidsToUnblock.push_back(jid);
        }
    }

    for (const auto& jid : newBlockList) {
        if (std::find(blockListBeforeEdit.begin(), blockListBeforeEdit.end(), jid) == blockListBeforeEdit.end()) {
            jidsToBlock.push_back(jid);
        }
    }
}

void BlockListController::handleUIEvent(std::shared_ptr<UIEvent> rawEvent) {
    // handle UI dialog
    std::shared_ptr<RequestBlockListDialogUIEvent> requestDialogEvent = std::dynamic_pointer_cast<RequestBlockListDialogUIEvent>(rawEvent);
    if (requestDialogEvent != nullptr) {
        if (blockListEditorWidget_ == nullptr) {
            blockListEditorWidget_ = blockListEditorWidgetFactory_->createBlockListEditorWidget();
            blockListEditorWidget_->onSetNewBlockList.connect(boost::bind(&BlockListController::handleSetNewBlockList, this, _1));
        }
        blockListBeforeEdit = blockListManager_->getBlockList()->getItems();
        blockListEditorWidget_->setCurrentBlockList(blockListBeforeEdit);
        blockListEditorWidget_->setError("");
        blockListEditorWidget_->show();
        return;
    }

    // handle block state change
    std::shared_ptr<RequestChangeBlockStateUIEvent> changeStateEvent = std::dynamic_pointer_cast<RequestChangeBlockStateUIEvent>(rawEvent);
    if (changeStateEvent != nullptr) {
        if (changeStateEvent->getBlockState() == RequestChangeBlockStateUIEvent::Blocked) {
            GenericRequest<BlockPayload>::ref blockRequest = blockListManager_->createBlockJIDRequest(changeStateEvent->getContact());
            blockRequest->onResponse.connect(boost::bind(&BlockListController::handleBlockResponse, this, blockRequest, _1, _2, std::vector<JID>(1, changeStateEvent->getContact()), false));
            blockRequest->send();
        } else if (changeStateEvent->getBlockState() == RequestChangeBlockStateUIEvent::Unblocked) {
            GenericRequest<UnblockPayload>::ref unblockRequest = blockListManager_->createUnblockJIDRequest(changeStateEvent->getContact());
            unblockRequest->onResponse.connect(boost::bind(&BlockListController::handleUnblockResponse, this, unblockRequest, _1, _2, std::vector<JID>(1, changeStateEvent->getContact()), false));
            unblockRequest->send();
        }
        return;
    }
}

void BlockListController::handleBlockResponse(GenericRequest<BlockPayload>::ref request, std::shared_ptr<BlockPayload>, ErrorPayload::ref error, const std::vector<JID>& jids, bool originEditor) {
    if (error) {
        std::string errorMessage;
        // FIXME: Handle reporting of list of JIDs in a translatable way.
        errorMessage = str(format(QT_TRANSLATE_NOOP("", "Failed to block %1%.")) % jids.at(0).toString());
        if (!error->getText().empty()) {
            errorMessage = str(format(QT_TRANSLATE_NOOP("", "%1%: %2%.")) % errorMessage % error->getText());
        }
        if (blockListEditorWidget_ && originEditor) {
            blockListEditorWidget_->setError(errorMessage);
            blockListEditorWidget_->setBusy(false);
        }
        else {
            eventController_->handleIncomingEvent(std::make_shared<ErrorEvent>(request->getReceiver(), errorMessage));
        }
    }
    if (originEditor) {
        remainingRequests_--;
        if (blockListEditorWidget_ && (remainingRequests_ == 0) && !error) {
            blockListEditorWidget_->setBusy(false);
            blockListEditorWidget_->hide();
        }
    }
}

void BlockListController::handleUnblockResponse(GenericRequest<UnblockPayload>::ref request, std::shared_ptr<UnblockPayload>, ErrorPayload::ref error, const std::vector<JID>& jids, bool originEditor) {
    if (error) {
        std::string errorMessage;
        // FIXME: Handle reporting of list of JIDs in a translatable way.
        errorMessage = str(format(QT_TRANSLATE_NOOP("", "Failed to unblock %1%.")) % jids.at(0).toString());
        if (!error->getText().empty()) {
            errorMessage = str(format(QT_TRANSLATE_NOOP("", "%1%: %2%.")) % errorMessage % error->getText());
        }
        if (blockListEditorWidget_ && originEditor) {
            blockListEditorWidget_->setError(errorMessage);
            blockListEditorWidget_->setBusy(false);
        }
        else {
            eventController_->handleIncomingEvent(std::make_shared<ErrorEvent>(request->getReceiver(), errorMessage));
        }
    }
    if (originEditor) {
        remainingRequests_--;
        if (blockListEditorWidget_ && (remainingRequests_ == 0) && !error) {
            blockListEditorWidget_->setBusy(false);
            blockListEditorWidget_->hide();
        }
    }
}

void BlockListController::handleSetNewBlockList(const std::vector<JID> &newBlockList) {
    std::vector<JID> jidsToBlock;
    std::vector<JID> jidsToUnblock;

    blockListDifferences(newBlockList, jidsToUnblock, jidsToBlock);

    if (!jidsToBlock.empty()) {
        remainingRequests_++;
        GenericRequest<BlockPayload>::ref blockRequest = blockListManager_->createBlockJIDsRequest(jidsToBlock);
        blockRequest->onResponse.connect(boost::bind(&BlockListController::handleBlockResponse, this, blockRequest, _1, _2, jidsToBlock, true));
        blockRequest->send();
    }
    if (!jidsToUnblock.empty()) {
        remainingRequests_++;
        GenericRequest<UnblockPayload>::ref unblockRequest = blockListManager_->createUnblockJIDsRequest(jidsToUnblock);
        unblockRequest->onResponse.connect(boost::bind(&BlockListController::handleUnblockResponse, this, unblockRequest, _1, _2, jidsToUnblock, true));
        unblockRequest->send();
    }
    if (!jidsToBlock.empty() || !jidsToUnblock.empty()) {
        assert(blockListEditorWidget_);
        blockListEditorWidget_->setBusy(true);
        blockListEditorWidget_->setError("");
    } else {
        blockListEditorWidget_->hide();
    }
}

void BlockListController::handleBlockListChanged() {
    if (blockListEditorWidget_) {
        std::vector<JID> jidsToBlock;
        std::vector<JID> jidsToUnblock;

        blockListDifferences(blockListEditorWidget_->getCurrentBlockList(), jidsToUnblock, jidsToBlock);
        blockListBeforeEdit = blockListManager_->getBlockList()->getItems();

        for (const auto& jid : jidsToBlock) {
            if (std::find(blockListBeforeEdit.begin(), blockListBeforeEdit.end(), jid) == blockListBeforeEdit.end()) {
                blockListBeforeEdit.push_back(jid);
            }
        }

        for (const auto& jid : jidsToUnblock) {
            blockListBeforeEdit.erase(std::remove(blockListBeforeEdit.begin(), blockListBeforeEdit.end(), jid), blockListBeforeEdit.end());
        }

        blockListEditorWidget_->setCurrentBlockList(blockListBeforeEdit);
    }
}

}
