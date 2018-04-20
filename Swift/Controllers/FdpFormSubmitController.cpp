/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/FdpFormSubmitController.h>

#include <Swiften/Disco/GetDiscoItemsRequest.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Queries/PubSubRequest.h>

#include <Swift/Controllers/UIEvents/FdpFormSubmitWindowOpenUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/FdpFormSubmitWindow.h>
#include <Swift/Controllers/UIInterfaces/FdpFormSubmitWindowFactory.h>

namespace Swift {

FdpFormSubmitController::FdpFormSubmitController(const JID& self, IQRouter* iqRouter, UIEventStream* uiEventStream, FdpFormSubmitWindowFactory* factory) : selfJID_(self), iqRouter_(iqRouter), uiEventStream_(uiEventStream), factory_(factory), formSubmitWindow_(nullptr) {
    fdpFormSubmitWindowOpenUIEventConnection_= uiEventStream_->onUIEvent.connect( [this](const std::shared_ptr<UIEvent>& uiEvent){ handleUIEvent(uiEvent); });
}

FdpFormSubmitController::~FdpFormSubmitController() {
}

void FdpFormSubmitController::handleUIEvent(const std::shared_ptr<UIEvent>& uiEvent) {
    if (auto openEvent = std::dynamic_pointer_cast<FdpFormSubmitWindowOpenUIEvent>(uiEvent)) {
        if (formSubmitWindow_) {
            formSubmitWindow_->raise();
        }
        else {
            createFormSubmitWindow();
        }
    }
}

void FdpFormSubmitController::createFormSubmitWindow() {
    formSubmitWindow_ = factory_->createFdpFormSubmitWindow();
    formSubmitWindow_->onCloseEvent.connect([this](){ closeFormSubmitWindow(); });
    formSubmitWindow_->onRequestPubSubNodeData.connect([this](const std::string& string){ requestPubSubNodeData(string); });
    formSubmitWindow_->onRequestTemplateForm.connect([this](const std::string& fdpTemplateNodeName){ requestTemplateForm(fdpTemplateNodeName); });
    formSubmitWindow_->onSubmitForm.connect([this](const std::shared_ptr<Form>& form){ submitForm(form); });
    formSubmitWindow_->show();
}

void FdpFormSubmitController::closeFormSubmitWindow() {
    formSubmitWindow_.reset();
}

void FdpFormSubmitController::requestPubSubNodeData(const std::string& domainName) {
    JID domainJID(domainName);
    auto discoItemsRequest = GetDiscoItemsRequest::create(domainJID, iqRouter_);
    discoItemsRequest->onResponse.connect(
        [this, domainName](std::shared_ptr<DiscoItems> discoItems, ErrorPayload::ref errorPayloadRef) {
            if (!discoItems || errorPayloadRef) {
                formSubmitWindow_->showNodePlaceholder(FdpFormSubmitWindow::NodeError::DomainNotFound);
                return;
            }
            currentDomain_ = domainName;

            bool templateNodeAdded = false;
            formSubmitWindow_->clearNodeData();
            auto discoItemList = discoItems->getItems();
            for (auto discoItem : discoItemList) {
                auto node = discoItem.getNode();
                if (node.substr(0, 13) != "fdp/template/") {
                    continue;
                }

                std::string nodeName = discoItem.getName().empty() ? node : discoItem.getName();

                formSubmitWindow_->addNode(node, nodeName);
                templateNodeAdded = true;
            }
            if (!templateNodeAdded) {
                formSubmitWindow_->showNodePlaceholder(FdpFormSubmitWindow::NodeError::NoFdpNodesInDomain);
            }
        }
    );
    discoItemsRequest->send();
}

void FdpFormSubmitController::requestTemplateForm(const std::string& nodeName) {
    auto pubSubItems = std::make_shared<PubSubItems>(nodeName);
    pubSubItems->setMaximumItems(1);
    auto formRequest = std::make_shared<PubSubRequest<PubSubItems>>(IQ::Type::Get, selfJID_, currentDomain_, pubSubItems, iqRouter_);

    formRequest->onResponse.connect(
        [this, nodeName](std::shared_ptr<PubSubItems> response, ErrorPayload::ref errorPayload) {
            if (!response || errorPayload) {
                formSubmitWindow_->showFormPlaceholder(FdpFormSubmitWindow::TemplateError::RequestFailed);
                return;
            }
            auto pubSubItemList = response->getItems();
            if (pubSubItemList.empty()) {
                formSubmitWindow_->showFormPlaceholder(FdpFormSubmitWindow::TemplateError::CannotLocateForm);
                return;
            }
            auto payloadList = pubSubItemList[0]->getData();
            if (payloadList.empty()) {
                formSubmitWindow_->showFormPlaceholder(FdpFormSubmitWindow::TemplateError::CannotLocateForm);
                return;
            }
            if (auto form = std::dynamic_pointer_cast<Form>(payloadList[0])) {
                currentTemplateNode_ = nodeName;
                formSubmitWindow_->setFormData(form);
            }
            else {
                formSubmitWindow_->showFormPlaceholder(FdpFormSubmitWindow::TemplateError::InvalidPayload);
                return;
            }
        }
    );

    formRequest->send();
}

void FdpFormSubmitController::submitForm(const std::shared_ptr<Form>& form) {
    std::string submittedNode = currentTemplateNode_;
    submittedNode.replace(submittedNode.find("/template/", 0), 10, "/submitted/");
    auto pubSubItem = std::make_shared<PubSubItem>();
    auto pubSubItems = std::make_shared<PubSubItems>(submittedNode);
    auto pubSubPublish = std::make_shared<PubSubPublish>();
    pubSubPublish->setNode(submittedNode);
    pubSubPublish->addItem(pubSubItem);
    pubSubItem->addData(form);
    pubSubItems->addItem(pubSubItem);
    auto formRequest = std::make_shared<PubSubRequest<PubSubPublish>>(IQ::Type::Set, selfJID_, currentDomain_, pubSubPublish, iqRouter_);

    formRequest->onResponse.connect(
        [this](std::shared_ptr<PubSubPublish> response, ErrorPayload::ref errorPayload) {
            if (!response || errorPayload) {
                formSubmitWindow_->handleSubmitServerResponse(false);
                return;
            }
            formSubmitWindow_->handleSubmitServerResponse(true);
        }
    );

    formRequest->send();
}

}
