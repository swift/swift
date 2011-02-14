/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Base/ByteArray.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include "Swiften/FileTransfer/SOCKS5BytestreamServerSession.h"
#include "Swiften/FileTransfer/ByteArrayReadBytestream.h"
#include "Swiften/FileTransfer/SOCKS5BytestreamRegistry.h"
#include "Swiften/Network/DummyConnection.h"
#include "Swiften/EventLoop/DummyEventLoop.h"
#include "Swiften/Base/StartStopper.h"

using namespace Swift;

class SOCKS5BytestreamServerSessionTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SOCKS5BytestreamServerSessionTest);
		CPPUNIT_TEST(testAuthenticate);
		CPPUNIT_TEST(testAuthenticate_Chunked);
		CPPUNIT_TEST(testRequest);
		CPPUNIT_TEST(testRequest_UnknownBytestream);
		CPPUNIT_TEST(testReceiveData);
		CPPUNIT_TEST(testReceiveData_Chunked);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			receivedDataChunks = 0;
			eventLoop = new DummyEventLoop();
			connection = boost::shared_ptr<DummyConnection>(new DummyConnection(eventLoop));
			connection->onDataSent.connect(boost::bind(&SOCKS5BytestreamServerSessionTest::handleDataWritten, this, _1));
			stream1 = boost::shared_ptr<ByteArrayReadBytestream>(new ByteArrayReadBytestream(ByteArray("abcdefg")));
		}

		void tearDown() {
			connection.reset();
			delete eventLoop;
		}

		void testAuthenticate() {
			std::auto_ptr<SOCKS5BytestreamServerSession> testling(createSession());
			StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());

			receive(ByteArray("\x05\x02\x01\x02"));

			CPPUNIT_ASSERT_EQUAL(ByteArray("\x05\x00", 2), receivedData);
		}

		void testAuthenticate_Chunked() {
			std::auto_ptr<SOCKS5BytestreamServerSession> testling(createSession());
			StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());

			receive(ByteArray("\x05\x02\x01"));

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedData.getSize()));
			receive(ByteArray("\x01"));
			CPPUNIT_ASSERT_EQUAL(ByteArray("\x05\x00", 2), receivedData);
		}

		void testRequest() {
			std::auto_ptr<SOCKS5BytestreamServerSession> testling(createSession());
			StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());
			bytestreams.addBytestream("abcdef", stream1);
			authenticate();

			ByteArray hostname("abcdef");
			receive(ByteArray("\x05\x01\x00\x03", 4) + hostname.getSize() + hostname + ByteArray("\x00\x00", 2));
			CPPUNIT_ASSERT_EQUAL(ByteArray("\x05\x00\x00\x03\x06\x61\x62\x63\x64\x65\x66\x00\x00", 13), ByteArray(receivedData.getData(), 13));
		}

		void testRequest_UnknownBytestream() {
			std::auto_ptr<SOCKS5BytestreamServerSession> testling(createSession());
			StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());
			authenticate();

			ByteArray hostname("abcdef");
			receive(ByteArray("\x05\x01\x00\x03", 4) + hostname.getSize() + hostname + ByteArray("\x00\x00", 2));
			CPPUNIT_ASSERT_EQUAL(ByteArray("\x05\x04\x00\x03\x06\x61\x62\x63\x64\x65\x66\x00\x00", 13), receivedData);
		}

		void testReceiveData() {
			std::auto_ptr<SOCKS5BytestreamServerSession> testling(createSession());
			StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());
			bytestreams.addBytestream("abcdef", stream1);
			authenticate();
			request("abcdef");
			eventLoop->processEvents();
			skipHeader("abcdef");

			CPPUNIT_ASSERT_EQUAL(ByteArray("abcdefg"), receivedData);
			CPPUNIT_ASSERT_EQUAL(2, receivedDataChunks);
		}

		void testReceiveData_Chunked() {
			std::auto_ptr<SOCKS5BytestreamServerSession> testling(createSession());
			testling->setChunkSize(3);
			StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());
			bytestreams.addBytestream("abcdef", stream1);
			authenticate();
			request("abcdef");
			eventLoop->processEvents();

			skipHeader("abcdef");
			CPPUNIT_ASSERT_EQUAL(ByteArray("abcdefg"), receivedData);
			CPPUNIT_ASSERT_EQUAL(4, receivedDataChunks);
		}

	private:
		void receive(const ByteArray& data) {
			connection->receive(data);
			eventLoop->processEvents();
		}

		void authenticate() {
			receive(ByteArray("\x05\x02\x01\x02"));
			receivedData.clear();
			receivedDataChunks = 0;
		}

		void request(const std::string& hostname) {
			receive(ByteArray("\x05\x01\x00\x03", 4) + hostname.size() + hostname + ByteArray("\x00\x00", 2));
		}

		void skipHeader(const std::string& hostname) {
			int headerSize = 7 + hostname.size();
			receivedData = ByteArray(receivedData.getData() + headerSize, receivedData.getSize() - headerSize);
		}


		void handleDataWritten(const ByteArray& data) {
			receivedData += data;
			receivedDataChunks++;
		}

	private:
		SOCKS5BytestreamServerSession* createSession() {
			SOCKS5BytestreamServerSession* session = new SOCKS5BytestreamServerSession(connection, &bytestreams);
			return session;
		}

	private:
		DummyEventLoop* eventLoop;
		SOCKS5BytestreamRegistry bytestreams;
		boost::shared_ptr<DummyConnection> connection;
		ByteArray receivedData;
		int receivedDataChunks;
		boost::shared_ptr<ByteArrayReadBytestream> stream1;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SOCKS5BytestreamServerSessionTest);
