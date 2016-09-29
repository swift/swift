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
#include <Swiften/Queries/DummyIQChannel.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Queries/Requests/GetPrivateStorageRequest.h>

using namespace Swift;

class GetPrivateStorageRequestTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(GetPrivateStorageRequestTest);
        CPPUNIT_TEST(testSend);
        CPPUNIT_TEST(testHandleResponse);
        CPPUNIT_TEST(testHandleResponse_Error);
        CPPUNIT_TEST_SUITE_END();

    public:
        class MyPayload : public Payload {
            public:
                MyPayload(const std::string& text = "") : text(text) {}
                std::string text;
        };

    public:
        void setUp() {
            channel = std::unique_ptr<DummyIQChannel>(new DummyIQChannel());
            router = std::unique_ptr<IQRouter>(new IQRouter(channel.get()));
        }

        void tearDown() {
            router.reset();
        }

        void testSend() {
            GetPrivateStorageRequest<MyPayload>::ref request = GetPrivateStorageRequest<MyPayload>::create(router.get());
            request->send();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel->iqs_.size()));
            CPPUNIT_ASSERT_EQUAL(JID(), channel->iqs_[0]->getTo());
            CPPUNIT_ASSERT_EQUAL(IQ::Get, channel->iqs_[0]->getType());
            std::shared_ptr<PrivateStorage> storage = channel->iqs_[0]->getPayload<PrivateStorage>();
            CPPUNIT_ASSERT(storage);
            std::shared_ptr<MyPayload> payload = std::dynamic_pointer_cast<MyPayload>(storage->getPayload());
            CPPUNIT_ASSERT(payload);
        }

        void testHandleResponse() {
            GetPrivateStorageRequest<MyPayload>::ref testling = GetPrivateStorageRequest<MyPayload>::create(router.get());
            testling->onResponse.connect(boost::bind(&GetPrivateStorageRequestTest::handleResponse, this, _1, _2));
            testling->send();
            channel->onIQReceived(createResponse("test-id", "foo"));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(responses.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("foo"), std::dynamic_pointer_cast<MyPayload>(responses[0])->text);
        }

        void testHandleResponse_Error() {
            GetPrivateStorageRequest<MyPayload>::ref testling = GetPrivateStorageRequest<MyPayload>::create(router.get());
            testling->onResponse.connect(boost::bind(&GetPrivateStorageRequestTest::handleResponse, this, _1, _2));
            testling->send();
            channel->onIQReceived(createError("test-id"));

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(responses.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(errors.size()));
        }

    private:
        void handleResponse(std::shared_ptr<Payload> p, ErrorPayload::ref e) {
            if (e) {
                errors.push_back(*e);
            }
            else {
                responses.push_back(p);
            }
        }

        std::shared_ptr<IQ> createResponse(const std::string& id, const std::string& text) {
            std::shared_ptr<IQ> iq(new IQ(IQ::Result));
            std::shared_ptr<PrivateStorage> storage(new PrivateStorage());
            storage->setPayload(std::make_shared<MyPayload>(text));
            iq->addPayload(storage);
            iq->setID(id);
            return iq;
        }

        std::shared_ptr<IQ> createError(const std::string& id) {
            std::shared_ptr<IQ> iq(new IQ(IQ::Error));
            iq->setID(id);
            return iq;
        }

    private:
        std::unique_ptr<IQRouter> router;
        std::unique_ptr<DummyIQChannel> channel;
        std::vector< ErrorPayload > errors;
        std::vector< std::shared_ptr<Payload> > responses;
};

CPPUNIT_TEST_SUITE_REGISTRATION(GetPrivateStorageRequestTest);
