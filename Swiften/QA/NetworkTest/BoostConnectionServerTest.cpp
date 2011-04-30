/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>

#include <string>
#include <Swiften/Network/BoostConnectionServer.h>
#include <Swiften/Network/BoostIOServiceThread.h>
#include <Swiften/EventLoop/DummyEventLoop.h>

using namespace Swift;

class BoostConnectionServerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(BoostConnectionServerTest);
		CPPUNIT_TEST(testConstructor_TwoServersOnSamePort);
		CPPUNIT_TEST(testStart_Conflict);
		CPPUNIT_TEST(testStop);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			boostIOServiceThread_ = new BoostIOServiceThread();
			eventLoop_ = new DummyEventLoop();
			stopped = false;
			stoppedError.reset();
		}

		void tearDown() {
			while (eventLoop_->hasEvents()) {
				eventLoop_->processEvents();
			}
			delete eventLoop_;
			delete boostIOServiceThread_;
		}

		void testConstructor_TwoServersOnSamePort() {
			BoostConnectionServer::ref testling(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
			BoostConnectionServer::ref testling2(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
		}

		void testStart_Conflict() {
			BoostConnectionServer::ref testling(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
			testling->start();

			BoostConnectionServer::ref testling2(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
			testling2->onStopped.connect(
					boost::bind(&BoostConnectionServerTest::handleStopped, this, _1));

			testling->stop();
		}

		void testStop() {
			BoostConnectionServer::ref testling(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
			testling->start();

			testling->stop();

			BoostConnectionServer::ref testling2(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
			testling2->start();

			testling2->stop();
		}

		void handleStopped(boost::optional<BoostConnectionServer::Error> e) {
			stopped = true;
			stoppedError = e;
		}

	private:
		BoostIOServiceThread* boostIOServiceThread_;
		DummyEventLoop* eventLoop_;
		bool stopped;
		boost::optional<BoostConnectionServer::Error> stoppedError;
};

CPPUNIT_TEST_SUITE_REGISTRATION(BoostConnectionServerTest);
