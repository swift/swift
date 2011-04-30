/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <typeinfo>

#include <Swiften/Disco/DiscoInfoResponder.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Queries/DummyIQChannel.h>

using namespace Swift;

class DiscoInfoResponderTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(DiscoInfoResponderTest);
		CPPUNIT_TEST(testHandleRequest_GetToplevelInfo);
		CPPUNIT_TEST(testHandleRequest_GetNodeInfo);
		CPPUNIT_TEST(testHandleRequest_GetInvalidNodeInfo);
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
			DiscoInfoResponder testling(router_);
			testling.start();
			DiscoInfo discoInfo;
			discoInfo.addFeature("foo");
			testling.setDiscoInfo(discoInfo);

			boost::shared_ptr<DiscoInfo> query(new DiscoInfo());
			channel_->onIQReceived(IQ::createRequest(IQ::Get, JID("foo@bar.com"), "id-1", query));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
			boost::shared_ptr<DiscoInfo> payload(channel_->iqs_[0]->getPayload<DiscoInfo>());
			CPPUNIT_ASSERT(payload);
			CPPUNIT_ASSERT_EQUAL(std::string(""), payload->getNode());
			CPPUNIT_ASSERT(payload->hasFeature("foo"));

			testling.stop();
		}

		void testHandleRequest_GetNodeInfo() {
			DiscoInfoResponder testling(router_); 
			testling.start();
			DiscoInfo discoInfo;
			discoInfo.addFeature("foo");
			testling.setDiscoInfo(discoInfo);
			DiscoInfo discoInfoBar;
			discoInfoBar.addFeature("bar");
			testling.setDiscoInfo("bar-node", discoInfoBar);

			boost::shared_ptr<DiscoInfo> query(new DiscoInfo());
			query->setNode("bar-node");
			channel_->onIQReceived(IQ::createRequest(IQ::Get, JID("foo@bar.com"), "id-1", query));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
			boost::shared_ptr<DiscoInfo> payload(channel_->iqs_[0]->getPayload<DiscoInfo>());
			CPPUNIT_ASSERT(payload);
			CPPUNIT_ASSERT_EQUAL(std::string("bar-node"), payload->getNode());
			CPPUNIT_ASSERT(payload->hasFeature("bar"));

			testling.stop();
		}

		void testHandleRequest_GetInvalidNodeInfo() {
			DiscoInfoResponder testling(router_); 
			boost::shared_ptr<DiscoInfo> query(new DiscoInfo());
			query->setNode("bar-node");
			channel_->onIQReceived(IQ::createRequest(IQ::Get, JID("foo@bar.com"), "id-1", query));
			testling.start();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
			boost::shared_ptr<ErrorPayload> payload(channel_->iqs_[0]->getPayload<ErrorPayload>());
			CPPUNIT_ASSERT(payload);

			testling.stop();
		}

	private:
		IQRouter* router_;
		DummyIQChannel* channel_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(DiscoInfoResponderTest);
