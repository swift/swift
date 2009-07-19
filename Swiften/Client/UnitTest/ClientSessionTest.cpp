#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>

#include "Swiften/Parser/XMPPParser.h"
#include "Swiften/Parser/XMPPParserClient.h"
#include "Swiften/Serializer/XMPPSerializer.h"
#include "Swiften/StreamStack/TLSLayerFactory.h"
#include "Swiften/StreamStack/TLSLayer.h"
#include "Swiften/StreamStack/StreamStack.h"
#include "Swiften/StreamStack/WhitespacePingLayer.h"
#include "Swiften/Elements/ProtocolHeader.h"
#include "Swiften/Elements/StreamFeatures.h"
#include "Swiften/Elements/Element.h"
#include "Swiften/Elements/Error.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/AuthRequest.h"
#include "Swiften/Elements/AuthSuccess.h"
#include "Swiften/Elements/AuthFailure.h"
#include "Swiften/Elements/ResourceBind.h"
#include "Swiften/Elements/StartSession.h"
#include "Swiften/Elements/StartTLSRequest.h"
#include "Swiften/Elements/StartTLSFailure.h"
#include "Swiften/Elements/TLSProceed.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/EventLoop/DummyEventLoop.h"
#include "Swiften/Network/Connection.h"
#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Client/ClientSession.h"
#include "Swiften/TLS/PKCS12Certificate.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

using namespace Swift;

class ClientSessionTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ClientSessionTest);
		CPPUNIT_TEST(testConstructor);
		CPPUNIT_TEST(testStart_Error);
		CPPUNIT_TEST(testStart_XMLError);
		CPPUNIT_TEST(testStartTLS);
		CPPUNIT_TEST(testStartTLS_ServerError);
		CPPUNIT_TEST(testStartTLS_NoTLSSupport);
		CPPUNIT_TEST(testStartTLS_ConnectError);
		CPPUNIT_TEST(testStartTLS_ErrorAfterConnect);
		CPPUNIT_TEST(testAuthenticate);
		CPPUNIT_TEST(testAuthenticate_Unauthorized);
		CPPUNIT_TEST(testAuthenticate_NoValidAuthMechanisms);
		CPPUNIT_TEST(testResourceBind);
		CPPUNIT_TEST(testResourceBind_ChangeResource);
		CPPUNIT_TEST(testResourceBind_EmptyResource);
		CPPUNIT_TEST(testResourceBind_Error);
		CPPUNIT_TEST(testSessionStart);
		CPPUNIT_TEST(testSessionStart_Error);
		CPPUNIT_TEST(testSessionStart_AfterResourceBind);
		CPPUNIT_TEST(testWhitespacePing);
		CPPUNIT_TEST(testReceiveElementAfterSessionStarted);
		CPPUNIT_TEST(testSendElement);
		CPPUNIT_TEST_SUITE_END();

	public:
		ClientSessionTest() {}

		void setUp() {
			eventLoop_ = new DummyEventLoop();
			connection_ = boost::shared_ptr<MockConnection>(new MockConnection());
			tlsLayerFactory_ = new MockTLSLayerFactory();
			sessionStarted_ = false;
			needCredentials_ = false;
		}

		void tearDown() {
			delete tlsLayerFactory_;
			delete eventLoop_;
		}

		void testConstructor() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			CPPUNIT_ASSERT_EQUAL(ClientSession::Initial, session->getState());
		}

		void testStart_Error() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));

			getMockServer()->expectStreamStart();
			session->start();
			processEvents();
			CPPUNIT_ASSERT_EQUAL(ClientSession::WaitingForStreamStart, session->getState());

			getMockServer()->setError();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::ConnectionReadError, session->getError());
		}

		void testStart_XMLError() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));

			getMockServer()->expectStreamStart();
			session->start();
			processEvents();
			CPPUNIT_ASSERT_EQUAL(ClientSession::WaitingForStreamStart, session->getState());

			getMockServer()->sendInvalidXML();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::XMLError, session->getError());
		}

		void testStartTLS_NoTLSSupport() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			tlsLayerFactory_->setTLSSupported(false);
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithStartTLS();
			session->start();
			processEvents();
			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
		}

		void testStartTLS() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithStartTLS();
			getMockServer()->expectStartTLS();
			// FIXME: Test 'encrypting' state
			getMockServer()->sendTLSProceed();
			session->start();
			processEvents();
			CPPUNIT_ASSERT_EQUAL(ClientSession::Encrypting, session->getState());
			CPPUNIT_ASSERT(session->getTLSLayer());
			CPPUNIT_ASSERT(session->getTLSLayer()->isConnecting());

			getMockServer()->resetParser();
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			session->getTLSLayer()->setConnected();
			// FIXME: Test 'WatingForStreamStart' state
			processEvents();
			CPPUNIT_ASSERT_EQUAL(ClientSession::Negotiating, session->getState());
		}

		void testStartTLS_ServerError() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithStartTLS();
			getMockServer()->expectStartTLS();
			getMockServer()->sendTLSFailure();
			session->start();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::TLSError, session->getError());
		}

		void testStartTLS_ConnectError() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithStartTLS();
			getMockServer()->expectStartTLS();
			getMockServer()->sendTLSProceed();
			session->start();
			processEvents();
			session->getTLSLayer()->setError();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::TLSError, session->getError());
		}

		void testStartTLS_ErrorAfterConnect() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithStartTLS();
			getMockServer()->expectStartTLS();
			getMockServer()->sendTLSProceed();
			session->start();
			processEvents();
			getMockServer()->resetParser();
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			session->getTLSLayer()->setConnected();
			processEvents();

			session->getTLSLayer()->setError();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::TLSError, session->getError());
		}

		void testAuthenticate() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onNeedCredentials.connect(boost::bind(&ClientSessionTest::setNeedCredentials, this));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithAuthentication();
			session->start();
			processEvents();
			CPPUNIT_ASSERT_EQUAL(ClientSession::WaitingForCredentials, session->getState());
			CPPUNIT_ASSERT(needCredentials_);

			getMockServer()->expectAuth("me", "mypass");
			getMockServer()->sendAuthSuccess();
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			session->sendCredentials("mypass");
			CPPUNIT_ASSERT_EQUAL(ClientSession::Authenticating, session->getState());
			processEvents();
			CPPUNIT_ASSERT_EQUAL(ClientSession::Negotiating, session->getState());
		}

		void testAuthenticate_Unauthorized() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithAuthentication();
			session->start();
			processEvents();

			getMockServer()->expectAuth("me", "mypass");
			getMockServer()->sendAuthFailure();
			session->sendCredentials("mypass");
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::AuthenticationFailedError, session->getError());
		}

		void testAuthenticate_NoValidAuthMechanisms() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithUnsupportedAuthentication();
			session->start();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::NoSupportedAuthMechanismsError, session->getError());
		}

		void testResourceBind() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("Bar", "session-bind");
			// FIXME: Check CPPUNIT_ASSERT_EQUAL(ClientSession::BindingResource, session->getState());
			getMockServer()->sendResourceBindResponse("me@foo.com/Bar", "session-bind");
			session->start();

			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT_EQUAL(JID("me@foo.com/Bar"), session->getJID());
		}

		void testResourceBind_ChangeResource() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("Bar", "session-bind");
			getMockServer()->sendResourceBindResponse("me@foo.com/Bar123", "session-bind");
			session->start();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT_EQUAL(JID("me@foo.com/Bar123"), session->getJID());
		}

		void testResourceBind_EmptyResource() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("", "session-bind");
			getMockServer()->sendResourceBindResponse("me@foo.com/NewResource", "session-bind");
			session->start();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT_EQUAL(JID("me@foo.com/NewResource"), session->getJID());
		}

		void testResourceBind_Error() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("", "session-bind");
			getMockServer()->sendError("session-bind");
			session->start();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::ResourceBindError, session->getError());
		}

		void testSessionStart() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onSessionStarted.connect(boost::bind(&ClientSessionTest::setSessionStarted, this));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithSession();
			getMockServer()->expectSessionStart("session-start");
			// FIXME: Check CPPUNIT_ASSERT_EQUAL(ClientSession::StartingSession, session->getState());
			getMockServer()->sendSessionStartResponse("session-start");
			session->start();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT(sessionStarted_);
		}

		void testSessionStart_Error() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithSession();
			getMockServer()->expectSessionStart("session-start");
			getMockServer()->sendError("session-start");
			session->start();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStartError, session->getError());
		}

		void testSessionStart_AfterResourceBind() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onSessionStarted.connect(boost::bind(&ClientSessionTest::setSessionStarted, this));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBindAndSession();
			getMockServer()->expectResourceBind("Bar", "session-bind");
			getMockServer()->sendResourceBindResponse("me@foo.com/Bar", "session-bind");
			getMockServer()->expectSessionStart("session-start");
			getMockServer()->sendSessionStartResponse("session-start");
			session->start();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT(sessionStarted_);
		}

		void testWhitespacePing() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeatures();
			session->start();
			processEvents();
			CPPUNIT_ASSERT(session->getWhitespacePingLayer());
		}

		void testReceiveElementAfterSessionStarted() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeatures();
			session->start();
			processEvents();

			getMockServer()->expectMessage();
			session->sendElement(boost::shared_ptr<Message>(new Message()));
		}

		void testSendElement() {
			std::auto_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onElementReceived.connect(boost::bind(&ClientSessionTest::addReceivedElement, this, _1));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeatures();
			getMockServer()->sendMessage();
			session->start();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(receivedElements_.size()));
			CPPUNIT_ASSERT(boost::dynamic_pointer_cast<Message>(receivedElements_[0]));
		}

	private:
		struct MockConnection;

		boost::shared_ptr<MockConnection> getMockServer() const {
			return connection_;
		}

		void processEvents() {
			eventLoop_->processEvents();
			getMockServer()->assertNoMoreExpectations();
		}

		void setSessionStarted() {
			sessionStarted_ = true;
		}

		void setNeedCredentials() {
			needCredentials_ = true;
		}

		void addReceivedElement(boost::shared_ptr<Element> element) {
			receivedElements_.push_back(element);
		}
	
	private:
		struct MockConnection : public Connection, public XMPPParserClient {
			struct Event {
				enum Direction { In, Out };
				enum Type { StreamStartEvent, StreamEndEvent, ElementEvent };

				Event(
					Direction direction,
					Type type, 
					boost::shared_ptr<Element> element = boost::shared_ptr<Element>()) :
						direction(direction), type(type), element(element) {}

				Direction direction;
				Type	type;
				boost::shared_ptr<Element> element;
			};

			MockConnection() :
					resetParser_(false),
					domain_("foo.com"),
					parser_(0),
					serializer_(&payloadSerializers_) {
				parser_ = new XMPPParser(this, &payloadParserFactories_);
			}

			~MockConnection() {
				delete parser_;
			}

			void disconnect() { }

			void listen() {
				assert(false);
			}

			void connect(const HostAddressPort&) { assert(false); }
			void connect(const String&) { assert(false); }

			void setError() {
				MainEventLoop::postEvent(boost::bind(boost::ref(onDisconnected), Connection::ReadError));
			}

			void write(const ByteArray& data) {
				CPPUNIT_ASSERT(parser_->parse(data.toString()));
				if (resetParser_) {
					resetParser();
					resetParser_ = false;
				}
			}

			void resetParser() {
				delete parser_;
				parser_ = new XMPPParser(this, &payloadParserFactories_);
			}

			void handleStreamStart(const ProtocolHeader& header) {
				CPPUNIT_ASSERT_EQUAL(domain_, header.getTo());
				handleEvent(Event::StreamStartEvent);
			}

			void handleElement(boost::shared_ptr<Swift::Element> element) {
				handleEvent(Event::ElementEvent, element);
			}

			void handleStreamEnd() {
				handleEvent(Event::StreamEndEvent);
			}

			void handleEvent(Event::Type type, boost::shared_ptr<Element> element = boost::shared_ptr<Element>()) {
				CPPUNIT_ASSERT(!events_.empty());
				CPPUNIT_ASSERT_EQUAL(events_[0].direction, Event::In);
				CPPUNIT_ASSERT_EQUAL(events_[0].type, type);
				if (type == Event::ElementEvent) {
					CPPUNIT_ASSERT_EQUAL(serializer_.serializeElement(events_[0].element), serializer_.serializeElement(element));
				}
				events_.pop_front();

				while (!events_.empty() && events_[0].direction == Event::Out) {
					sendData(serializeEvent(events_[0]));
					events_.pop_front();
				}

				if (!events_.empty() && events_[0].type == Event::StreamStartEvent) {
					resetParser_ = true;
				}
			}

			String serializeEvent(const Event& event) {
				switch (event.type) {
					case Event::StreamStartEvent: 
						{
							ProtocolHeader header;
							header.setTo(domain_);
							return serializer_.serializeHeader(header);
						}
					case Event::ElementEvent:
						return serializer_.serializeElement(event.element);
					case Event::StreamEndEvent:
						return serializer_.serializeFooter();
				}
				assert(false);
				return "";
			}

			void assertNoMoreExpectations() {
				foreach (const Event& event, events_) {
					std::cout << "Unprocessed event: " << serializeEvent(event) << std::endl;
				}
				CPPUNIT_ASSERT(events_.empty());
			}

			void sendData(const ByteArray& data) {
				MainEventLoop::postEvent(boost::bind(boost::ref(onDataRead), data));
			}

			void expectStreamStart() {
				events_.push_back(Event(Event::In, Event::StreamStartEvent));
			}

			void expectStartTLS() {
				events_.push_back(Event(Event::In, Event::ElementEvent, boost::shared_ptr<StartTLSRequest>(new StartTLSRequest())));
			}

			void expectAuth(const String& user, const String& password) {
				String s	= String("") + '\0' + user + '\0' + password;
				events_.push_back(Event(Event::In, Event::ElementEvent, boost::shared_ptr<AuthRequest>(new AuthRequest("PLAIN", ByteArray(s.getUTF8Data(), s.getUTF8Size())))));
			}

			void expectResourceBind(const String& resource, const String& id) {
				boost::shared_ptr<ResourceBind> sessionStart(new ResourceBind());
				sessionStart->setResource(resource);
				events_.push_back(Event(Event::In, Event::ElementEvent, IQ::createRequest(IQ::Set, JID(), id, sessionStart)));
			}

			void expectSessionStart(const String& id) {
				events_.push_back(Event(Event::In, Event::ElementEvent, IQ::createRequest(IQ::Set, JID(), id, boost::shared_ptr<StartSession>(new StartSession()))));
			}
			
			void expectMessage() {
				events_.push_back(Event(Event::In, Event::ElementEvent, boost::shared_ptr<Message>(new Message())));
			}

			void sendInvalidXML() {
				sendData("<invalid xml/>");
			}

			void sendStreamStart() {
				events_.push_back(Event(Event::Out, Event::StreamStartEvent));
			}

			void sendStreamFeatures() {
				boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
				events_.push_back(Event(Event::Out, Event::ElementEvent, streamFeatures));
			}

			void sendStreamFeaturesWithStartTLS() {
				boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
				streamFeatures->setHasStartTLS();
				events_.push_back(Event(Event::Out, Event::ElementEvent, streamFeatures));
			}

			void sendStreamFeaturesWithAuthentication() {
				boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
				streamFeatures->addAuthenticationMechanism("PLAIN");
				events_.push_back(Event(Event::Out, Event::ElementEvent, streamFeatures));
			}

			void sendStreamFeaturesWithUnsupportedAuthentication() {
				boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
				streamFeatures->addAuthenticationMechanism("MY-UNSUPPORTED-MECH");
				events_.push_back(Event(Event::Out, Event::ElementEvent, streamFeatures));
			}

			void sendStreamFeaturesWithResourceBind() {
				boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
				streamFeatures->setHasResourceBind();
				events_.push_back(Event(Event::Out, Event::ElementEvent, streamFeatures));
			}

			void sendStreamFeaturesWithSession() {
				boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
				streamFeatures->setHasSession();
				events_.push_back(Event(Event::Out, Event::ElementEvent, streamFeatures));
			}

			void sendStreamFeaturesWithResourceBindAndSession() {
				boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
				streamFeatures->setHasResourceBind();
				streamFeatures->setHasSession();
				events_.push_back(Event(Event::Out, Event::ElementEvent, streamFeatures));
			}

			void sendMessage() {
				events_.push_back(Event(Event::Out, Event::ElementEvent, boost::shared_ptr<Message>(new Message())));
			}

			void sendTLSProceed() {
				events_.push_back(Event(Event::Out, Event::ElementEvent, boost::shared_ptr<TLSProceed>(new TLSProceed())));
			}

			void sendTLSFailure() {
				events_.push_back(Event(Event::Out, Event::ElementEvent, boost::shared_ptr<StartTLSFailure>(new StartTLSFailure())));
			}

			void sendAuthSuccess() {
				events_.push_back(Event(Event::Out, Event::ElementEvent, boost::shared_ptr<AuthSuccess>(new AuthSuccess())));
			}

			void sendAuthFailure() {
				events_.push_back(Event(Event::Out, Event::ElementEvent, boost::shared_ptr<AuthFailure>(new AuthFailure())));
			}

			void sendResourceBindResponse(const String& jid, const String& id) {
				boost::shared_ptr<ResourceBind> sessionStart(new ResourceBind());
				sessionStart->setJID(JID(jid));
				events_.push_back(Event(Event::Out, Event::ElementEvent, IQ::createResult(JID(), id, sessionStart)));
			}

			void sendError(const String& id) {
				events_.push_back(Event(Event::Out, Event::ElementEvent, IQ::createError(JID(), id, Swift::Error::NotAllowed, Swift::Error::Cancel)));
			}

			void sendSessionStartResponse(const String& id) {
				events_.push_back(Event(Event::Out, Event::ElementEvent, IQ::createResult(JID(), id, boost::shared_ptr<StartSession>(new StartSession()))));
			}

			bool resetParser_;
			String domain_;
			FullPayloadParserFactoryCollection payloadParserFactories_;
			FullPayloadSerializerCollection payloadSerializers_;
			XMPPParser* parser_;
			XMPPSerializer serializer_;
			std::deque<Event> events_;
		};

		struct MockTLSLayer : public TLSLayer {
			MockTLSLayer() : connecting_(false) {}
			bool setClientCertificate(const PKCS12Certificate&) { return true; }
			void writeData(const ByteArray& data) { onWriteData(data); }
			void handleDataRead(const ByteArray& data) { onDataRead(data); }
			void setConnected() { onConnected(); }
			void setError() { onError(); }
			void connect() { connecting_ = true; }
			bool isConnecting() { return connecting_; }

			bool connecting_;
		};

		struct MockTLSLayerFactory : public TLSLayerFactory {
			MockTLSLayerFactory() : haveTLS_(true) {}
			void setTLSSupported(bool b) { haveTLS_ = b; }
			virtual bool canCreate() const { return haveTLS_; }
			virtual boost::shared_ptr<TLSLayer> createTLSLayer() { 
				assert(haveTLS_);
				boost::shared_ptr<MockTLSLayer> result(new MockTLSLayer());
				layers_.push_back(result);
				return result;
			}
			std::vector< boost::shared_ptr<MockTLSLayer> > layers_;
			bool haveTLS_;
		};

		struct MockSession : public ClientSession {
			MockSession(const JID& jid, boost::shared_ptr<Connection> connection, TLSLayerFactory* tlsLayerFactory, PayloadParserFactoryCollection* payloadParserFactories, PayloadSerializerCollection* payloadSerializers) : ClientSession(jid, connection, tlsLayerFactory, payloadParserFactories, payloadSerializers) {}

			boost::shared_ptr<MockTLSLayer> getTLSLayer() const {
				return getStreamStack()->getLayer<MockTLSLayer>();
			}
			boost::shared_ptr<WhitespacePingLayer> getWhitespacePingLayer() const {
				return getStreamStack()->getLayer<WhitespacePingLayer>();
			}
		};

		MockSession* createSession(const String& jid) {
			return new MockSession(JID(jid), connection_, tlsLayerFactory_, &payloadParserFactories_, &payloadSerializers_);
		}


		DummyEventLoop* eventLoop_;
		boost::shared_ptr<MockConnection> connection_;
		MockTLSLayerFactory* tlsLayerFactory_;
		FullPayloadParserFactoryCollection payloadParserFactories_;
		FullPayloadSerializerCollection payloadSerializers_;
		bool sessionStarted_;
		bool needCredentials_;
		std::vector< boost::shared_ptr<Element> > receivedElements_;
		typedef std::vector< boost::function<void ()> > EventQueue;
		EventQueue events_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ClientSessionTest);
