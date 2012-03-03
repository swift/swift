/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Network/ChainedConnector.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/StaticDomainNameResolver.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/Network/DomainNameResolveError.h>

using namespace Swift;

class ChainedConnectorTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ChainedConnectorTest);
		CPPUNIT_TEST(testConnect_FirstConnectorSucceeds);
		CPPUNIT_TEST(testConnect_SecondConnectorSucceeds);
		CPPUNIT_TEST(testConnect_NoConnectorSucceeds);
		CPPUNIT_TEST(testConnect_NoDNS);
		CPPUNIT_TEST(testStop);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			error.reset();
			host = HostAddressPort(HostAddress("1.1.1.1"), 1234);
			eventLoop = new DummyEventLoop();
			resolver = new StaticDomainNameResolver(eventLoop);
			resolver->addXMPPClientService("foo.com", host);
			connectionFactory1 = new MockConnectionFactory(eventLoop, 1);
			connectionFactory2 = new MockConnectionFactory(eventLoop, 2);
			timerFactory = new DummyTimerFactory();
		}

		void tearDown() {
			delete timerFactory;
			delete connectionFactory2;
			delete connectionFactory1;
			delete resolver;
			delete eventLoop;
		}

		void testConnect_FirstConnectorSucceeds() {
			boost::shared_ptr<ChainedConnector> testling(createConnector());
			connectionFactory1->connects = true;
			connectionFactory2->connects = false;

			testling->start();
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
			CPPUNIT_ASSERT(connections[0]);
			CPPUNIT_ASSERT_EQUAL(1, boost::dynamic_pointer_cast<MockConnection>(connections[0])->id);
			CPPUNIT_ASSERT(!boost::dynamic_pointer_cast<DomainNameResolveError>(error));
		}

		void testConnect_SecondConnectorSucceeds() {
			boost::shared_ptr<ChainedConnector> testling(createConnector());
			connectionFactory1->connects = false;
			connectionFactory2->connects = true;

			testling->start();
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
			CPPUNIT_ASSERT(connections[0]);
			CPPUNIT_ASSERT_EQUAL(2, boost::dynamic_pointer_cast<MockConnection>(connections[0])->id);
			CPPUNIT_ASSERT(!boost::dynamic_pointer_cast<DomainNameResolveError>(error));
		}

		void testConnect_NoConnectorSucceeds() {
			boost::shared_ptr<ChainedConnector> testling(createConnector());
			connectionFactory1->connects = false;
			connectionFactory2->connects = false;

			testling->start();
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
			CPPUNIT_ASSERT(!connections[0]);
			CPPUNIT_ASSERT(!boost::dynamic_pointer_cast<DomainNameResolveError>(error));
		}

		void testConnect_NoDNS() {
			/* Reset resolver so there's no record */
			delete resolver;
			resolver = new StaticDomainNameResolver(eventLoop);
			boost::shared_ptr<ChainedConnector> testling(createConnector());
			connectionFactory1->connects = false;
			connectionFactory2->connects = false;

			testling->start();
			//testling->stop();
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
			CPPUNIT_ASSERT(!connections[0]);
			CPPUNIT_ASSERT(boost::dynamic_pointer_cast<DomainNameResolveError>(error));
		}

		void testStop() {
			boost::shared_ptr<ChainedConnector> testling(createConnector());
			connectionFactory1->connects = true;
			connectionFactory2->connects = false;

			testling->start();
			testling->stop();
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
			CPPUNIT_ASSERT(!connections[0]);
		}

	private:
		boost::shared_ptr<ChainedConnector> createConnector() {
			std::vector<ConnectionFactory*> factories;
			factories.push_back(connectionFactory1);
			factories.push_back(connectionFactory2);
			boost::shared_ptr<ChainedConnector> connector = boost::make_shared<ChainedConnector>("foo.com", resolver, factories, timerFactory);
			connector->onConnectFinished.connect(boost::bind(&ChainedConnectorTest::handleConnectorFinished, this, _1, _2));
			return connector;
		}

		void handleConnectorFinished(boost::shared_ptr<Connection> connection, boost::shared_ptr<Error> resultError) {
			error = resultError;
			boost::shared_ptr<MockConnection> c(boost::dynamic_pointer_cast<MockConnection>(connection));
			if (connection) {
				assert(c);
			}
			connections.push_back(c);
		}

		struct MockConnection : public Connection {
			public:
				MockConnection(bool connects, int id, EventLoop* eventLoop) : connects(connects), id(id), eventLoop(eventLoop) {
				}

				void listen() { assert(false); }
				void connect(const HostAddressPort&) {
					eventLoop->postEvent(boost::bind(boost::ref(onConnectFinished), !connects));
				}

				HostAddressPort getLocalAddress() const { return HostAddressPort(); }
				void disconnect() { assert(false); }
				void write(const SafeByteArray&) { assert(false); }

				bool connects;
				int id;
				EventLoop* eventLoop;
		};

		struct MockConnectionFactory : public ConnectionFactory {
			MockConnectionFactory(EventLoop* eventLoop, int id) : eventLoop(eventLoop), connects(true), id(id) {
			}

			boost::shared_ptr<Connection> createConnection() {
				return boost::make_shared<MockConnection>(connects, id, eventLoop);
			}

			EventLoop* eventLoop;
			bool connects;
			int id;
		};

	private:
		HostAddressPort host;
		DummyEventLoop* eventLoop;
		StaticDomainNameResolver* resolver;
		MockConnectionFactory* connectionFactory1;
		MockConnectionFactory* connectionFactory2;
		DummyTimerFactory* timerFactory;
		std::vector< boost::shared_ptr<MockConnection> > connections;
		boost::shared_ptr<Error> error;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChainedConnectorTest);
