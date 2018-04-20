/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/FormField.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSub.h>
#include <Swiften/Elements/PubSubItems.h>
#include <Swiften/Elements/PubSubPublish.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Queries/IQRouter.h>

#include <Swift/Controllers/FdpFormSubmitController.h>
#include <Swift/Controllers/UIEvents/FdpFormSubmitWindowOpenUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UnitTest/MockFdpFormSubmitWindowFactory.h>
#include <Swift/Controllers/UnitTest/MockFdpFormSubmitWindow.h>

namespace Swift {

class FdpFormSubmitControllerTest : public ::testing::Test {

    protected:
        void SetUp() {
            clientJid_ = JID(clientJIDString_);
            stanzaChannel_ = std::make_unique<DummyStanzaChannel>();
            iqRouter_ = std::make_unique<IQRouter>(stanzaChannel_.get());
            uiEventStream_ = std::make_unique<UIEventStream>();
            fdpWindowFactory_ = std::make_unique<MockFdpFormSubmitWindowFactory>();
            fdpController_ = std::make_unique<FdpFormSubmitController>(clientJid_, iqRouter_.get(), uiEventStream_.get(), fdpWindowFactory_.get());
            auto fdpWindowOpenUIEvent = std::make_shared<FdpFormSubmitWindowOpenUIEvent>();
            uiEventStream_->send(fdpWindowOpenUIEvent);
            fdpWindow_ = fdpWindowFactory_->getMockFdpFormSubmitWindow();
        }

        void TearDown() {
        }

        std::shared_ptr<DiscoItems> createDiscoItemsResult();
        std::shared_ptr<PubSub> createTemplatePubSubResult(const std::shared_ptr<Form>& form);
        std::shared_ptr<PubSub> createSubmittedPubSubResult();

        std::string clientJIDString_ = "testjid@test.im/swift";
        JID clientJid_;
        std::unique_ptr<DummyStanzaChannel> stanzaChannel_;
        std::unique_ptr<IQRouter> iqRouter_;
        std::unique_ptr<UIEventStream> uiEventStream_;
        std::unique_ptr<MockFdpFormSubmitWindowFactory> fdpWindowFactory_;
        std::unique_ptr<FdpFormSubmitController> fdpController_;
        MockFdpFormSubmitWindow* fdpWindow_;
};

std::shared_ptr<DiscoItems> FdpFormSubmitControllerTest::createDiscoItemsResult() {
    auto discoItems = std::make_shared<DiscoItems>();
    discoItems->addItem(DiscoItems::Item("node0", JID(), "fdp/template/testNode0"));
    discoItems->addItem(DiscoItems::Item("node1", JID(), "fdp/template/testNode1"));
    discoItems->addItem(DiscoItems::Item("node2", JID(), "fdp/template/testNode2"));
    return discoItems;
}

std::shared_ptr<PubSub> FdpFormSubmitControllerTest::createTemplatePubSubResult(const std::shared_ptr<Form>& form) {
    auto pubSubItem = std::make_shared<PubSubItem>();
    pubSubItem->addData(form);
    auto pubSubItems = std::make_shared<PubSubItems>();
    pubSubItems->addItem(pubSubItem);
    auto pubSub = std::make_shared<PubSub>();
    pubSub->setPayload(pubSubItems);
    return pubSub;
}

std::shared_ptr<PubSub> FdpFormSubmitControllerTest::createSubmittedPubSubResult() {
    auto pubSubItem = std::make_shared<PubSubItem>();
    pubSubItem->setID("testID");
    auto pubSubPublish = std::make_shared<PubSubPublish>();
    pubSubPublish->addItem(pubSubItem);
    pubSubPublish->setNode("fdp/submitted/test");
    auto pubSub = std::make_shared<PubSub>();
    pubSub->setPayload(pubSubPublish);
    return pubSub;
}

TEST_F(FdpFormSubmitControllerTest, testRequestPubSubNodeData) {
    std::string domainName = "fdp.example.test";
    fdpWindow_->onRequestPubSubNodeData(domainName);
    ASSERT_EQ(1, stanzaChannel_->sentStanzas.size());
    auto requestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[0]);
    ASSERT_EQ(domainName, requestIq->getTo());
    auto discoItemsPayloads = requestIq->getPayloads<DiscoItems>();
    ASSERT_EQ(1, discoItemsPayloads.size());

    auto discoItemsResult = createDiscoItemsResult();
    auto resultIq = IQ::createResult(clientJid_, domainName, requestIq->getID(), discoItemsResult);
    stanzaChannel_->onIQReceived(resultIq);

    auto discoItemsList = discoItemsResult->getItems();
    ASSERT_EQ(discoItemsList.size(), fdpWindow_->nodeData.size());
    for (unsigned int i = 0; i < fdpWindow_->nodeData.size(); i++) {
        auto nodeItem = fdpWindow_->nodeData[i];
        auto discoItem = discoItemsList[i];
        ASSERT_EQ(discoItem.getNode(), nodeItem.first);
        ASSERT_EQ(discoItem.getName(), nodeItem.second);
    }
    ASSERT_EQ(FdpFormSubmitWindow::NodeError::NoError, fdpWindow_->nodeError_);
}

TEST_F(FdpFormSubmitControllerTest, testRequestPubSubNodeDataError) {
    std::string domainName = "fdp.example.test";
    fdpWindow_->onRequestPubSubNodeData(domainName);
    ASSERT_EQ(1, stanzaChannel_->sentStanzas.size());
    auto requestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[0]);
    ASSERT_EQ(domainName, requestIq->getTo());

    auto resultIq = IQ::createError(clientJid_, domainName, requestIq->getID());
    stanzaChannel_->onIQReceived(resultIq);

    ASSERT_EQ(true, fdpWindow_->nodeData.empty());
    ASSERT_EQ(FdpFormSubmitWindow::NodeError::DomainNotFound, fdpWindow_->nodeError_);
}

TEST_F(FdpFormSubmitControllerTest, testRequestTemplateForm) {
    std::string domainName = "fdp.example.test";
    fdpWindow_->onRequestPubSubNodeData(domainName);
    auto nodeDataRequestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[0]);
    auto discoItemsResult = createDiscoItemsResult();
    auto discoItemsResultIq = IQ::createResult(clientJid_, domainName, nodeDataRequestIq->getID(), discoItemsResult);
    stanzaChannel_->onIQReceived(discoItemsResultIq);

    std::string templateNodeName = "fdp/template/test";
    fdpWindow_->onRequestTemplateForm(templateNodeName);
    ASSERT_EQ(2, stanzaChannel_->sentStanzas.size());
    auto requestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[1]);
    ASSERT_EQ(domainName, requestIq->getTo());
    auto pubSubPayloads = requestIq->getPayloads<PubSub>();
    ASSERT_EQ(1, pubSubPayloads.size());

    std::string value0("value0");
    std::string value1("value1@example.test");
    auto field0 = std::make_shared<FormField>(FormField::TextSingleType, value0);
    auto field1 = std::make_shared<FormField>(FormField::JIDSingleType, value1);
    auto form = std::make_shared<Form>();
    form->addField(field0);
    form->addField(field1);
    auto pubSubResult = createTemplatePubSubResult(form);
    auto resultIq = IQ::createResult(clientJid_, domainName, requestIq->getID(), pubSubResult);
    stanzaChannel_->onIQReceived(resultIq);

    ASSERT_EQ(form, fdpWindow_->templateForm_);
    auto fields = fdpWindow_->templateForm_->getFields();
    ASSERT_EQ(2, fields.size());
    ASSERT_EQ(field0, fields[0]);
    ASSERT_EQ(field1, fields[1]);
    ASSERT_EQ(value0, fields[0]->getTextSingleValue());
    ASSERT_EQ(value1, fields[1]->getJIDSingleValue());
    ASSERT_EQ(FdpFormSubmitWindow::TemplateError::NoError, fdpWindow_->templateError_);
}

TEST_F(FdpFormSubmitControllerTest, testRequestTemplateFormError) {
    std::string domainName = "fdp.example.test";
    fdpWindow_->onRequestPubSubNodeData(domainName);
    auto nodeDataRequestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[0]);
    auto discoItemsResult = createDiscoItemsResult();
    auto discoItemsResultIq = IQ::createResult(clientJid_, domainName, nodeDataRequestIq->getID(), discoItemsResult);
    stanzaChannel_->onIQReceived(discoItemsResultIq);

    std::string templateNodeName = "fdp/template/test";
    fdpWindow_->onRequestTemplateForm(templateNodeName);
    ASSERT_EQ(2, stanzaChannel_->sentStanzas.size());
    auto requestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[1]);
    ASSERT_EQ(domainName, requestIq->getTo());

    auto resultIq = IQ::createError(clientJid_, domainName, requestIq->getID());
    stanzaChannel_->onIQReceived(resultIq);

    ASSERT_EQ(nullptr, fdpWindow_->templateForm_);
    ASSERT_EQ(FdpFormSubmitWindow::TemplateError::RequestFailed, fdpWindow_->templateError_);
}

TEST_F(FdpFormSubmitControllerTest, testSubmitForm) {
    std::string domainName = "fdp.example.test";
    std::string templateNodeName = "fdp/template/test";
    fdpWindow_->onRequestPubSubNodeData(domainName);
    auto nodeDataRequestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[0]);
    auto discoItemsResult = createDiscoItemsResult();
    auto discoItemsResultIq = IQ::createResult(clientJid_, domainName, nodeDataRequestIq->getID(), discoItemsResult);
    stanzaChannel_->onIQReceived(discoItemsResultIq);
    fdpWindow_->onRequestTemplateForm(templateNodeName);
    auto templateFormRequestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[1]);
    auto templateForm = std::make_shared<Form>();
    auto templatePubSubResult = createTemplatePubSubResult(templateForm);
    auto templatePubSubResultIq = IQ::createResult(clientJid_, domainName, templateFormRequestIq->getID(), templatePubSubResult);
    stanzaChannel_->onIQReceived(templatePubSubResultIq);

    std::string value0("value0");
    std::string value1("value1@example.test");
    auto field0 = std::make_shared<FormField>(FormField::TextSingleType, value0);
    auto field1 = std::make_shared<FormField>(FormField::JIDSingleType, value1);
    auto form = std::make_shared<Form>();
    form->addField(field0);
    form->addField(field1);
    fdpWindow_->onSubmitForm(form);

    ASSERT_EQ(3, stanzaChannel_->sentStanzas.size());
    auto requestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[2]);
    ASSERT_EQ(domainName, requestIq->getTo());
    auto pubSubPayloads = requestIq->getPayloads<PubSub>();
    ASSERT_EQ(1, pubSubPayloads.size());
    auto pubSubPayload = pubSubPayloads[0];
    auto pubSubPublishPayload = std::dynamic_pointer_cast<PubSubPublish>(pubSubPayload->getPayload());
    ASSERT_TRUE(pubSubPublishPayload);
    auto pubSubItems = pubSubPublishPayload->getItems();
    ASSERT_EQ(1, pubSubItems.size());
    auto dataList = pubSubItems[0]->getData();
    ASSERT_EQ(1, dataList.size());
    auto submittedForm = std::dynamic_pointer_cast<Form>(dataList[0]);
    ASSERT_TRUE(submittedForm);
    ASSERT_EQ(form, submittedForm);
    auto fields = submittedForm->getFields();
    ASSERT_EQ(2, fields.size());
    ASSERT_EQ(field0, fields[0]);
    ASSERT_EQ(field1, fields[1]);
    ASSERT_EQ(value0, fields[0]->getTextSingleValue());
    ASSERT_EQ(value1, fields[1]->getJIDSingleValue());

    auto pubSubResult = createSubmittedPubSubResult();
    auto resultIq = IQ::createResult(clientJid_, domainName, requestIq->getID(), pubSubResult);
    stanzaChannel_->onIQReceived(resultIq);

    ASSERT_EQ(true, fdpWindow_->submissionSuccess_);
}

TEST_F(FdpFormSubmitControllerTest, testSubmitFormError) {
    std::string domainName = "fdp.example.test";
    std::string templateNodeName = "fdp/template/test";
    fdpWindow_->onRequestPubSubNodeData(domainName);
    auto nodeDataRequestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[0]);
    auto discoItemsResult = createDiscoItemsResult();
    auto discoItemsResultIq = IQ::createResult(clientJid_, domainName, nodeDataRequestIq->getID(), discoItemsResult);
    stanzaChannel_->onIQReceived(discoItemsResultIq);
    fdpWindow_->onRequestTemplateForm(templateNodeName);
    auto templateFormRequestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[1]);
    auto templateForm = std::make_shared<Form>();
    auto templatePubSubResult = createTemplatePubSubResult(templateForm);
    auto templatePubSubResultIq = IQ::createResult(clientJid_, domainName, templateFormRequestIq->getID(), templatePubSubResult);
    stanzaChannel_->onIQReceived(templatePubSubResultIq);

    auto form = std::make_shared<Form>();
    fdpWindow_->onSubmitForm(form);
    ASSERT_EQ(3, stanzaChannel_->sentStanzas.size());
    auto requestIq = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[2]);
    ASSERT_EQ(domainName, requestIq->getTo());

    auto resultIq = IQ::createError(clientJid_, domainName, requestIq->getID());
    stanzaChannel_->onIQReceived(resultIq);

    ASSERT_EQ(false, fdpWindow_->submissionSuccess_);
}

}
