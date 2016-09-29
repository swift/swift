/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/SoftwareVersion.h>
#include <Swiften/Queries/DummyIQChannel.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Queries/Responder.h>

using namespace Swift;

class ResponderTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(ResponderTest);
        CPPUNIT_TEST(testConstructor);
        CPPUNIT_TEST(testStart);
        CPPUNIT_TEST(testStop);
        CPPUNIT_TEST(testHandleIQ_Set);
        CPPUNIT_TEST(testHandleIQ_Get);
        CPPUNIT_TEST(testHandleIQ_Error);
        CPPUNIT_TEST(testHandleIQ_Result);
        CPPUNIT_TEST(testHandleIQ_NoPayload);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            channel_ = std::unique_ptr<DummyIQChannel>(new DummyIQChannel());
            router_ = std::unique_ptr<IQRouter>(new IQRouter(channel_.get()));
            payload_ = std::make_shared<SoftwareVersion>("foo");
        }

        void tearDown() {
            router_.reset();
        }

        void testConstructor() {
            MyResponder testling(router_.get());

            channel_->onIQReceived(createRequest(IQ::Set));

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling.setPayloads_.size()));
        }

        void testStart() {
            MyResponder testling(router_.get());

            testling.start();
            channel_->onIQReceived(createRequest(IQ::Set));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(testling.setPayloads_.size()));
        }

        void testStop() {
            MyResponder testling(router_.get());

            testling.start();
            testling.stop();
            channel_->onIQReceived(createRequest(IQ::Set));

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling.setPayloads_.size()));
        }

        void testHandleIQ_Set() {
            MyResponder testling(router_.get());

            CPPUNIT_ASSERT(dynamic_cast<IQHandler*>(&testling)->handleIQ(createRequest(IQ::Set)));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(testling.setPayloads_.size()));
            CPPUNIT_ASSERT(payload_ == testling.setPayloads_[0]);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling.getPayloads_.size()));
        }

        void testHandleIQ_Get() {
            MyResponder testling(router_.get());

            CPPUNIT_ASSERT(dynamic_cast<IQHandler*>(&testling)->handleIQ(createRequest(IQ::Get)));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(testling.getPayloads_.size()));
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling.setPayloads_.size()));
            CPPUNIT_ASSERT(payload_ == testling.getPayloads_[0]);
        }

        void testHandleIQ_Error() {
            MyResponder testling(router_.get());

            CPPUNIT_ASSERT(!dynamic_cast<IQHandler*>(&testling)->handleIQ(createRequest(IQ::Error)));

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling.getPayloads_.size()));
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling.setPayloads_.size()));
        }

        void testHandleIQ_Result() {
            MyResponder testling(router_.get());

            CPPUNIT_ASSERT(!dynamic_cast<IQHandler*>(&testling)->handleIQ(createRequest(IQ::Result)));

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling.getPayloads_.size()));
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling.setPayloads_.size()));
        }

        void testHandleIQ_NoPayload() {
            MyResponder testling(router_.get());

            CPPUNIT_ASSERT(!dynamic_cast<IQHandler*>(&testling)->handleIQ(std::make_shared<IQ>(IQ::Get)));

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling.getPayloads_.size()));
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling.setPayloads_.size()));
        }

    private:
        std::shared_ptr<IQ> createRequest(IQ::Type type) {
            std::shared_ptr<IQ> iq(new IQ(type));
            iq->addPayload(payload_);
            iq->setID("myid");
            iq->setFrom(JID("foo@bar.com/baz"));
            return iq;
        }

    private:
        class MyResponder : public Responder<SoftwareVersion> {
            public:
                MyResponder(IQRouter* router) : Responder<SoftwareVersion>(router), getRequestResponse_(true), setRequestResponse_(true) {}

                virtual bool handleGetRequest(const JID& from, const JID&, const std::string& id, std::shared_ptr<SoftwareVersion> payload) {
                    CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/baz"), from);
                    CPPUNIT_ASSERT_EQUAL(std::string("myid"), id);
                    getPayloads_.push_back(payload);
                    return getRequestResponse_;
                }
                virtual bool handleSetRequest(const JID& from, const JID&, const std::string& id, std::shared_ptr<SoftwareVersion> payload) {
                    CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/baz"), from);
                    CPPUNIT_ASSERT_EQUAL(std::string("myid"), id);
                    setPayloads_.push_back(payload);
                    return setRequestResponse_;
                }

                bool getRequestResponse_;
                bool setRequestResponse_;
                std::vector<std::shared_ptr<SoftwareVersion> > getPayloads_;
                std::vector<std::shared_ptr<SoftwareVersion> > setPayloads_;
        };

    private:
        std::unique_ptr<IQRouter> router_;
        std::unique_ptr<DummyIQChannel> channel_;
        std::shared_ptr<SoftwareVersion> payload_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ResponderTest);
