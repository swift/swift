/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <typeinfo>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Disco/JIDDiscoInfoResponder.h>
#include <Swiften/Queries/DummyIQChannel.h>
#include <Swiften/Queries/IQRouter.h>

using namespace Swift;

class JIDDiscoInfoResponderTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(JIDDiscoInfoResponderTest);
        CPPUNIT_TEST(testHandleRequest_GetToplevelInfo);
        CPPUNIT_TEST(testHandleRequest_GetNodeInfo);
        CPPUNIT_TEST(testHandleRequest_GetInvalidNodeInfo);
        CPPUNIT_TEST(testHandleRequest_GetUnknownJID);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            channel_ = new DummyIQChannel();
            router_ = new IQRouter(channel_);
        }

        void tearDown() {
            delete router_;
            delete channel_;
        }

        void testHandleRequest_GetToplevelInfo() {
            JIDDiscoInfoResponder testling(router_);
            testling.start();
            DiscoInfo discoInfo;
            discoInfo.addFeature("foo");
            testling.setDiscoInfo(JID("foo@bar.com/baz"), discoInfo);

            std::shared_ptr<DiscoInfo> query(new DiscoInfo());
            channel_->onIQReceived(IQ::createRequest(IQ::Get, JID("foo@bar.com/baz"), "id-1", query));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
            std::shared_ptr<DiscoInfo> payload(channel_->iqs_[0]->getPayload<DiscoInfo>());
            CPPUNIT_ASSERT(payload);
            CPPUNIT_ASSERT_EQUAL(std::string(""), payload->getNode());
            CPPUNIT_ASSERT(payload->hasFeature("foo"));

            testling.stop();
        }

        void testHandleRequest_GetNodeInfo() {
            JIDDiscoInfoResponder testling(router_);
            testling.start();
            DiscoInfo discoInfo;
            discoInfo.addFeature("foo");
            testling.setDiscoInfo(JID("foo@bar.com/baz"), discoInfo);
            DiscoInfo discoInfoBar;
            discoInfoBar.addFeature("bar");
            testling.setDiscoInfo(JID("foo@bar.com/baz"), "bar-node", discoInfoBar);

            std::shared_ptr<DiscoInfo> query(new DiscoInfo());
            query->setNode("bar-node");
            channel_->onIQReceived(IQ::createRequest(IQ::Get, JID("foo@bar.com/baz"), "id-1", query));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
            std::shared_ptr<DiscoInfo> payload(channel_->iqs_[0]->getPayload<DiscoInfo>());
            CPPUNIT_ASSERT(payload);
            CPPUNIT_ASSERT_EQUAL(std::string("bar-node"), payload->getNode());
            CPPUNIT_ASSERT(payload->hasFeature("bar"));

            testling.stop();
        }

        void testHandleRequest_GetInvalidNodeInfo() {
            JIDDiscoInfoResponder testling(router_);
            DiscoInfo discoInfo;
            discoInfo.addFeature("foo");
            testling.setDiscoInfo(JID("foo@bar.com/baz"), discoInfo);
            testling.start();

            std::shared_ptr<DiscoInfo> query(new DiscoInfo());
            query->setNode("bar-node");
            channel_->onIQReceived(IQ::createRequest(IQ::Get, JID("foo@bar.com/baz"), "id-1", query));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
            std::shared_ptr<ErrorPayload> payload(channel_->iqs_[0]->getPayload<ErrorPayload>());
            CPPUNIT_ASSERT(payload);

            testling.stop();
        }

        void testHandleRequest_GetUnknownJID() {
            JIDDiscoInfoResponder testling(router_);
            DiscoInfo discoInfo;
            discoInfo.addFeature("foo");
            testling.setDiscoInfo(JID("foo@bar.com/baz"), discoInfo);
            testling.start();

            std::shared_ptr<DiscoInfo> query(new DiscoInfo());
            channel_->onIQReceived(IQ::createRequest(IQ::Get, JID("foo@bar.com/fum"), "id-1", query));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
            std::shared_ptr<ErrorPayload> payload(channel_->iqs_[0]->getPayload<ErrorPayload>());
            CPPUNIT_ASSERT(payload);

            testling.stop();
        }

    private:
        IQRouter* router_;
        DummyIQChannel* channel_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(JIDDiscoInfoResponderTest);
