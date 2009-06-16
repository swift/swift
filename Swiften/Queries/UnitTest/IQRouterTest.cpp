#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include "Swiften/Queries/IQHandler.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Queries/DummyIQChannel.h"

using namespace Swift;

class IQRouterTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(IQRouterTest);
		CPPUNIT_TEST(testRemoveHandler);
		CPPUNIT_TEST(testRemoveHandler_AfterHandleIQ);
		CPPUNIT_TEST(testHandleIQ_SuccesfulHandlerFirst);
		CPPUNIT_TEST(testHandleIQ_SuccesfulHandlerLast);
		CPPUNIT_TEST(testHandleIQ_NoSuccesfulHandler);
		CPPUNIT_TEST(testHandleIQ_HandlerRemovedDuringHandle);
		CPPUNIT_TEST_SUITE_END();

	public:
		IQRouterTest() {}

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

			channel_->onIQReceived(boost::shared_ptr<IQ>(new IQ()));

			CPPUNIT_ASSERT_EQUAL(0, handler1.called);
			CPPUNIT_ASSERT_EQUAL(1, handler2.called);
		}

		void testRemoveHandler_AfterHandleIQ() {
			IQRouter testling(channel_);
			DummyIQHandler handler1(true, &testling);
			DummyIQHandler handler2(true, &testling);

			channel_->onIQReceived(boost::shared_ptr<IQ>(new IQ()));
			testling.removeHandler(&handler1);
			channel_->onIQReceived(boost::shared_ptr<IQ>(new IQ()));

			CPPUNIT_ASSERT_EQUAL(1, handler1.called);
			CPPUNIT_ASSERT_EQUAL(1, handler2.called);
		}

		void testHandleIQ_SuccesfulHandlerFirst() {
			IQRouter testling(channel_);
			DummyIQHandler handler1(true, &testling);
			DummyIQHandler handler2(false, &testling);

			channel_->onIQReceived(boost::shared_ptr<IQ>(new IQ()));

			CPPUNIT_ASSERT_EQUAL(1, handler1.called);
			CPPUNIT_ASSERT_EQUAL(0, handler2.called);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(channel_->iqs_.size()));
		}

		void testHandleIQ_SuccesfulHandlerLast() {
			IQRouter testling(channel_);
			DummyIQHandler handler1(false, &testling);
			DummyIQHandler handler2(true, &testling);

			channel_->onIQReceived(boost::shared_ptr<IQ>(new IQ()));

			CPPUNIT_ASSERT_EQUAL(1, handler1.called);
			CPPUNIT_ASSERT_EQUAL(1, handler2.called);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(channel_->iqs_.size()));
		}

		void testHandleIQ_NoSuccesfulHandler() {
			IQRouter testling(channel_);
			DummyIQHandler handler(false, &testling);

			channel_->onIQReceived(boost::shared_ptr<IQ>(new IQ()));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
			CPPUNIT_ASSERT(channel_->iqs_[0]->getPayload<Error>());
		}


		void testHandleIQ_HandlerRemovedDuringHandle() {
			IQRouter testling(channel_);
			RemovingIQHandler handler1(&testling);
			DummyIQHandler handler2(true, &testling);

			channel_->onIQReceived(boost::shared_ptr<IQ>(new IQ()));
			channel_->onIQReceived(boost::shared_ptr<IQ>(new IQ()));

			CPPUNIT_ASSERT_EQUAL(1, handler1.called);
			CPPUNIT_ASSERT_EQUAL(2, handler2.called);
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
