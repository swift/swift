/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Queries/DummyIQChannel.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Queries/IQRouter.h>

using namespace Swift;

class RequestTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(RequestTest);
        CPPUNIT_TEST(testSendGet);
        CPPUNIT_TEST(testSendSet);
        CPPUNIT_TEST(testHandleIQ);
        CPPUNIT_TEST(testHandleIQ_InvalidID);
        CPPUNIT_TEST(testHandleIQ_Error);
        CPPUNIT_TEST(testHandleIQ_ErrorWithoutPayload);
        CPPUNIT_TEST(testHandleIQ_BeforeSend);
        CPPUNIT_TEST(testHandleIQ_DifferentPayload);
        CPPUNIT_TEST(testHandleIQ_RawXMLPayload);
        CPPUNIT_TEST(testHandleIQ_GetWithSameID);
        CPPUNIT_TEST(testHandleIQ_SetWithSameID);
        CPPUNIT_TEST(testHandleIQ_IncorrectSender);
        CPPUNIT_TEST(testHandleIQ_IncorrectSenderForServerQuery);
        CPPUNIT_TEST(testHandleIQ_IncorrectOtherResourceSenderForServerQuery);
        CPPUNIT_TEST(testHandleIQ_ServerRespondsWithDomain);
        CPPUNIT_TEST(testHandleIQ_ServerRespondsWithBareJID);
        CPPUNIT_TEST(testHandleIQ_ServerRespondsWithoutFrom);
        CPPUNIT_TEST(testHandleIQ_ServerRespondsWithFullJID);
        CPPUNIT_TEST_SUITE_END();

    public:
        class MyPayload : public Payload {
            public:
                MyPayload(const std::string& s = "") : text_(s) {}
                std::string text_;
        };

        struct MyOtherPayload : public Payload {
        };

        class MyRequest : public Request {
            public:
                MyRequest(
                        IQ::Type type,
                        const JID& receiver,
                        std::shared_ptr<Payload> payload,
                        IQRouter* router) :
                            Request(type, receiver, payload, router) {
                }

                virtual void handleResponse(std::shared_ptr<Payload> payload, ErrorPayload::ref error) {
                    onResponse(payload, error);
                }

            public:
                boost::signals2::signal<void (std::shared_ptr<Payload>, ErrorPayload::ref)> onResponse;
        };

    public:
        void setUp() {
            channel_ = new DummyIQChannel();
            router_ = new IQRouter(channel_);
            payload_ = std::make_shared<MyPayload>("foo");
            responsePayload_ = std::make_shared<MyPayload>("bar");
            responsesReceived_ = 0;
        }

        void tearDown() {
            delete router_;
            delete channel_;
        }

        void testSendSet() {
            MyRequest testling(IQ::Set, JID("foo@bar.com/baz"), payload_, router_);
            testling.send();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
            CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/baz"), channel_->iqs_[0]->getTo());
            CPPUNIT_ASSERT_EQUAL(IQ::Set, channel_->iqs_[0]->getType());
            CPPUNIT_ASSERT_EQUAL(std::string("test-id"), channel_->iqs_[0]->getID());
        }

        void testSendGet() {
            MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
            testling.send();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
            CPPUNIT_ASSERT_EQUAL(IQ::Get, channel_->iqs_[0]->getType());
        }

        void testHandleIQ() {
            MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            channel_->onIQReceived(createResponse(JID("foo@bar.com/baz"),"test-id"));

            CPPUNIT_ASSERT_EQUAL(1, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
        }

        // FIXME: Doesn't test that it didn't handle the payload
        void testHandleIQ_InvalidID() {
            MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            channel_->onIQReceived(createResponse(JID("foo@bar.com/baz"),"different-id"));

            CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
        }

        void testHandleIQ_Error() {
            MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            std::shared_ptr<IQ> error = createError(JID("foo@bar.com/baz"),"test-id");
            std::shared_ptr<Payload> errorPayload = std::make_shared<ErrorPayload>(ErrorPayload::InternalServerError);
            error->addPayload(errorPayload);
            channel_->onIQReceived(error);

            CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
            CPPUNIT_ASSERT_EQUAL(ErrorPayload::InternalServerError, receivedErrors[0].getCondition());
        }

        void testHandleIQ_ErrorWithoutPayload() {
            MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            channel_->onIQReceived(createError(JID("foo@bar.com/baz"),"test-id"));

            CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
            CPPUNIT_ASSERT_EQUAL(ErrorPayload::UndefinedCondition, receivedErrors[0].getCondition());
        }

        void testHandleIQ_BeforeSend() {
            MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            channel_->onIQReceived(createResponse(JID("foo@bar.com/baz"),"test-id"));

            CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(channel_->iqs_.size()));
        }

        void testHandleIQ_DifferentPayload() {
            MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
            testling.onResponse.connect(boost::bind(&RequestTest::handleDifferentResponse, this, _1, _2));
            testling.send();

            responsePayload_ = std::make_shared<MyOtherPayload>();
            channel_->onIQReceived(createResponse(JID("foo@bar.com/baz"),"test-id"));

            CPPUNIT_ASSERT_EQUAL(1, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
        }

        void testHandleIQ_RawXMLPayload() {
            payload_ = std::make_shared<RawXMLPayload>("<bla/>");
            MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
            testling.onResponse.connect(boost::bind(&RequestTest::handleRawXMLResponse, this, _1, _2));
            testling.send();

            responsePayload_ = std::make_shared<MyOtherPayload>();
            channel_->onIQReceived(createResponse(JID("foo@bar.com/baz"),"test-id"));

            CPPUNIT_ASSERT_EQUAL(1, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
        }

     void testHandleIQ_GetWithSameID() {
            MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            std::shared_ptr<IQ> response = createResponse(JID("foo@bar.com/baz"),"test-id");
            response->setType(IQ::Get);
            channel_->onIQReceived(response);

            CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(channel_->iqs_.size()));
        }

        void testHandleIQ_SetWithSameID() {
            MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            std::shared_ptr<IQ> response = createResponse(JID("foo@bar.com/baz"), "test-id");
            response->setType(IQ::Set);
            channel_->onIQReceived(response);

            CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(channel_->iqs_.size()));
        }

        void testHandleIQ_IncorrectSender() {
            MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
            router_->setJID("alice@wonderland.lit/TeaParty");
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            channel_->onIQReceived(createResponse(JID("anotherfoo@bar.com/baz"), "test-id"));

            CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
        }

        void testHandleIQ_IncorrectSenderForServerQuery() {
            MyRequest testling(IQ::Get, JID(), payload_, router_);
            router_->setJID("alice@wonderland.lit/TeaParty");
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            channel_->onIQReceived(createResponse(JID("foo@bar.com/baz"), "test-id"));

            CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
        }

        void testHandleIQ_IncorrectOtherResourceSenderForServerQuery() {
            MyRequest testling(IQ::Get, JID(), payload_, router_);
            router_->setJID("alice@wonderland.lit/TeaParty");
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            channel_->onIQReceived(createResponse(JID("alice@wonderland.lit/RabbitHole"), "test-id"));

            CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
        }

        void testHandleIQ_ServerRespondsWithDomain() {
            MyRequest testling(IQ::Get, JID(), payload_, router_);
            router_->setJID("alice@wonderland.lit/TeaParty");
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            channel_->onIQReceived(createResponse(JID("wonderland.lit"),"test-id"));

            CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
        }

        void testHandleIQ_ServerRespondsWithBareJID() {
            MyRequest testling(IQ::Get, JID(), payload_, router_);
            router_->setJID("alice@wonderland.lit/TeaParty");
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            channel_->onIQReceived(createResponse(JID("alice@wonderland.lit"),"test-id"));

            CPPUNIT_ASSERT_EQUAL(1, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
        }

        // This tests a bug in ejabberd servers (2.0.5)
        void testHandleIQ_ServerRespondsWithFullJID() {
            MyRequest testling(IQ::Get, JID(), payload_, router_);
            router_->setJID("alice@wonderland.lit/TeaParty");
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            channel_->onIQReceived(createResponse(JID("alice@wonderland.lit/TeaParty"),"test-id"));

            CPPUNIT_ASSERT_EQUAL(1, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
        }

        void testHandleIQ_ServerRespondsWithoutFrom() {
            MyRequest testling(IQ::Get, JID(), payload_, router_);
            router_->setJID("alice@wonderland.lit/TeaParty");
            testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
            testling.send();

            channel_->onIQReceived(createResponse(JID(),"test-id"));

            CPPUNIT_ASSERT_EQUAL(1, responsesReceived_);
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedErrors.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
        }



    private:
        void handleResponse(std::shared_ptr<Payload> p, ErrorPayload::ref e) {
            if (e) {
                receivedErrors.push_back(*e);
            }
            else {
                std::shared_ptr<MyPayload> payload(std::dynamic_pointer_cast<MyPayload>(p));
                CPPUNIT_ASSERT(payload);
                CPPUNIT_ASSERT_EQUAL(std::string("bar"), payload->text_);
                ++responsesReceived_;
            }
        }

        void handleDifferentResponse(std::shared_ptr<Payload> p, ErrorPayload::ref e) {
            CPPUNIT_ASSERT(!e);
            CPPUNIT_ASSERT(!p);
            ++responsesReceived_;
        }

        void handleRawXMLResponse(std::shared_ptr<Payload> p, ErrorPayload::ref e) {
            CPPUNIT_ASSERT(!e);
            CPPUNIT_ASSERT(p);
            CPPUNIT_ASSERT(std::dynamic_pointer_cast<MyOtherPayload>(p));
            ++responsesReceived_;
        }

        std::shared_ptr<IQ> createResponse(const JID& from, const std::string& id) {
            std::shared_ptr<IQ> iq(new IQ(IQ::Result));
            iq->setFrom(from);
            iq->addPayload(responsePayload_);
            iq->setID(id);
            return iq;
        }

        std::shared_ptr<IQ> createError(const JID& from, const std::string& id) {
            std::shared_ptr<IQ> iq(new IQ(IQ::Error));
            iq->setFrom(from);
            iq->setID(id);
            return iq;
        }

    private:
        IQRouter* router_;
        DummyIQChannel* channel_;
        std::shared_ptr<Payload> payload_;
        std::shared_ptr<Payload> responsePayload_;
        int responsesReceived_;
        std::vector<ErrorPayload> receivedErrors;
};

CPPUNIT_TEST_SUITE_REGISTRATION(RequestTest);
