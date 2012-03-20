/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/bind.hpp>

#include <Swiften/Queries/IQHandler.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Queries/DummyIQChannel.h>

using namespace Swift;

class IQRouterTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(IQRouterTest);
		CPPUNIT_TEST(testRemoveHandler);
		CPPUNIT_TEST(testRemoveHandler_AfterHandleIQ);
		CPPUNIT_TEST(testHandleIQ_SuccesfulHandlerFirst);
		CPPUNIT_TEST(testHandleIQ_SuccesfulHandlerLast);
		CPPUNIT_TEST(testHandleIQ_NoSuccesfulHandler);
		CPPUNIT_TEST(testHandleIQ_HandlerRemovedDuringHandle);
		CPPUNIT_TEST(testSendIQ_WithFrom);
		CPPUNIT_TEST(testSendIQ_WithoutFrom);
		CPPUNIT_TEST(testHandleIQ_WithFrom);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			channel_ = new DummyIQChannel();
		}

		void tearDown() {
			delete channel_;
		}

		void testRemoveHandler() {
			IQRouter testling(channel_);
			DummyIQHandler handler1(true, &testling);
			DummyIQHandler handler2(true, &testling);
			testling.removeHandler(&handler1);

			channel_->onIQReceived(boost::make_shared<IQ>());

			CPPUNIT_ASSERT_EQUAL(0, handler1.called);
			CPPUNIT_ASSERT_EQUAL(1, handler2.called);
		}

		void testRemoveHandler_AfterHandleIQ() {
			IQRouter testling(channel_);
			DummyIQHandler handler2(true, &testling);
			DummyIQHandler handler1(true, &testling);

			channel_->onIQReceived(boost::make_shared<IQ>());
			testling.removeHandler(&handler1);
			channel_->onIQReceived(boost::make_shared<IQ>());

			CPPUNIT_ASSERT_EQUAL(1, handler1.called);
			CPPUNIT_ASSERT_EQUAL(1, handler2.called);
		}

		void testHandleIQ_SuccesfulHandlerFirst() {
			IQRouter testling(channel_);
			DummyIQHandler handler2(false, &testling);
			DummyIQHandler handler1(true, &testling);

			channel_->onIQReceived(boost::make_shared<IQ>());

			CPPUNIT_ASSERT_EQUAL(1, handler1.called);
			CPPUNIT_ASSERT_EQUAL(0, handler2.called);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(channel_->iqs_.size()));
		}

		void testHandleIQ_SuccesfulHandlerLast() {
			IQRouter testling(channel_);
			DummyIQHandler handler2(true, &testling);
			DummyIQHandler handler1(false, &testling);

			channel_->onIQReceived(boost::make_shared<IQ>());

			CPPUNIT_ASSERT_EQUAL(1, handler1.called);
			CPPUNIT_ASSERT_EQUAL(1, handler2.called);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(channel_->iqs_.size()));
		}

		void testHandleIQ_NoSuccesfulHandler() {
			IQRouter testling(channel_);
			DummyIQHandler handler(false, &testling);

			channel_->onIQReceived(boost::make_shared<IQ>());

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
			CPPUNIT_ASSERT(channel_->iqs_[0]->getPayload<ErrorPayload>());
		}


		void testHandleIQ_HandlerRemovedDuringHandle() {
			IQRouter testling(channel_);
			DummyIQHandler handler2(true, &testling);
			RemovingIQHandler handler1(&testling);

			channel_->onIQReceived(boost::make_shared<IQ>());
			channel_->onIQReceived(boost::make_shared<IQ>());

			CPPUNIT_ASSERT_EQUAL(1, handler1.called);
			CPPUNIT_ASSERT_EQUAL(2, handler2.called);
		}

		void testSendIQ_WithFrom() {
			IQRouter testling(channel_);
			testling.setFrom(JID("foo@bar.com/baz"));

			testling.sendIQ(boost::make_shared<IQ>());

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/baz"), channel_->iqs_[0]->getFrom());
		}

		void testSendIQ_WithoutFrom() {
			IQRouter testling(channel_);

			testling.sendIQ(boost::make_shared<IQ>());

			CPPUNIT_ASSERT_EQUAL(JID(), channel_->iqs_[0]->getFrom());
		}

		void testHandleIQ_WithFrom() {
			IQRouter testling(channel_);
			testling.setFrom(JID("foo@bar.com/baz"));
			DummyIQHandler handler(false, &testling);

			channel_->onIQReceived(boost::make_shared<IQ>());

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/baz"), channel_->iqs_[0]->getFrom());
		}

	private:
		struct DummyIQHandler : public IQHandler {
			DummyIQHandler(bool handle, IQRouter* router) : handle(handle), router(router), called(0) {
				router->addHandler(this);
			}

			~DummyIQHandler() {
				router->removeHandler(this);
			}

			virtual bool handleIQ(boost::shared_ptr<IQ>) {
				called++;
				return handle;
			}
			bool handle;
			IQRouter* router;
			int called;
		};

		struct RemovingIQHandler : public IQHandler {
			RemovingIQHandler(IQRouter* router) : router(router), called(0) {
				router->addHandler(this);
			}

			virtual bool handleIQ(boost::shared_ptr<IQ>) {
				called++;
				router->removeHandler(this);
				return false;
			}
			IQRouter* router;
			int called;
		};


		DummyIQChannel* channel_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IQRouterTest);
