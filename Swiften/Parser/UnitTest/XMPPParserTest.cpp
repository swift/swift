/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <vector>

#include <Swiften/Elements/ProtocolHeader.h>
#include <string>
#include <Swiften/Parser/XMPPParser.h>
#include <Swiften/Parser/ElementParser.h>
#include <Swiften/Parser/XMPPParserClient.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/StreamFeatures.h>
#include <Swiften/Elements/UnknownElement.h>

using namespace Swift;

class XMPPParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(XMPPParserTest);
		CPPUNIT_TEST(testParse_SimpleSession);
		CPPUNIT_TEST(testParse_SimpleClientFromServerSession);
		CPPUNIT_TEST(testParse_Presence);
		CPPUNIT_TEST(testParse_IQ);
		CPPUNIT_TEST(testParse_Message);
		CPPUNIT_TEST(testParse_StreamFeatures);
		CPPUNIT_TEST(testParse_UnknownElement);
		CPPUNIT_TEST(testParse_StrayCharacterData);
		CPPUNIT_TEST(testParse_InvalidStreamStart);
		CPPUNIT_TEST(testParse_ElementEndAfterInvalidStreamStart);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse_SimpleSession() {
			XMPPParser testling(&client_, &factories_, &xmlParserFactory_);

			CPPUNIT_ASSERT(testling.parse("<?xml version='1.0'?>"));
			CPPUNIT_ASSERT(testling.parse("<stream:stream to='example.com' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' >"));
			CPPUNIT_ASSERT(testling.parse("<presence/>"));
			CPPUNIT_ASSERT(testling.parse("<presence/>"));
			CPPUNIT_ASSERT(testling.parse("<iq/>"));
			CPPUNIT_ASSERT(testling.parse("</stream:stream>"));

			CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(client_.events.size()));
			CPPUNIT_ASSERT_EQUAL(Client::StreamStart, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(std::string("example.com"), client_.events[0].header->getTo());
			CPPUNIT_ASSERT_EQUAL(Client::ElementEvent, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(Client::ElementEvent, client_.events[2].type);
			CPPUNIT_ASSERT_EQUAL(Client::ElementEvent, client_.events[3].type);
			CPPUNIT_ASSERT_EQUAL(Client::StreamEnd, client_.events[4].type);
		}

		void testParse_SimpleClientFromServerSession() {
			XMPPParser testling(&client_, &factories_, &xmlParserFactory_);

			CPPUNIT_ASSERT(testling.parse("<?xml version='1.0'?>"));
			CPPUNIT_ASSERT(testling.parse("<stream:stream from='example.com' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' id='aeab'>"));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(client_.events.size()));
			CPPUNIT_ASSERT_EQUAL(Client::StreamStart, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(std::string("example.com"), client_.events[0].header->getFrom());
			CPPUNIT_ASSERT_EQUAL(std::string("aeab"), client_.events[0].header->getID());
		}


		void testParse_Presence() {
			XMPPParser testling(&client_, &factories_, &xmlParserFactory_);

			CPPUNIT_ASSERT(testling.parse("<stream:stream xmlns:stream='http://etherx.jabber.org/streams'>"));
			CPPUNIT_ASSERT(testling.parse("<presence/>"));

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(client_.events.size()));
			CPPUNIT_ASSERT_EQUAL(Client::ElementEvent, client_.events[1].type);
			CPPUNIT_ASSERT(dynamic_cast<Presence*>(client_.events[1].element.get()));
		}

		void testParse_IQ() {
			XMPPParser testling(&client_, &factories_, &xmlParserFactory_);

			CPPUNIT_ASSERT(testling.parse("<stream:stream xmlns:stream='http://etherx.jabber.org/streams'>"));
			CPPUNIT_ASSERT(testling.parse("<iq/>"));

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(client_.events.size()));
			CPPUNIT_ASSERT_EQUAL(Client::ElementEvent, client_.events[1].type);
			CPPUNIT_ASSERT(dynamic_cast<IQ*>(client_.events[1].element.get()));
		}

		void testParse_Message() {
			XMPPParser testling(&client_, &factories_, &xmlParserFactory_);

			CPPUNIT_ASSERT(testling.parse("<stream:stream xmlns:stream='http://etherx.jabber.org/streams'>"));
			CPPUNIT_ASSERT(testling.parse("<message/>"));

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(client_.events.size()));
			CPPUNIT_ASSERT_EQUAL(Client::ElementEvent, client_.events[1].type);
			CPPUNIT_ASSERT(dynamic_cast<Message*>(client_.events[1].element.get()));
		}

		void testParse_StreamFeatures() {
			XMPPParser testling(&client_, &factories_, &xmlParserFactory_);

			CPPUNIT_ASSERT(testling.parse("<stream:stream xmlns:stream='http://etherx.jabber.org/streams'>"));
			CPPUNIT_ASSERT(testling.parse("<stream:features/>"));

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(client_.events.size()));
			CPPUNIT_ASSERT_EQUAL(Client::ElementEvent, client_.events[1].type);
			CPPUNIT_ASSERT(dynamic_cast<StreamFeatures*>(client_.events[1].element.get()));
		}

		void testParse_UnknownElement() {
			XMPPParser testling(&client_, &factories_, &xmlParserFactory_);

			CPPUNIT_ASSERT(testling.parse("<stream:stream xmlns:stream='http://etherx.jabber.org/streams'>"));
			CPPUNIT_ASSERT(testling.parse("<presence/>"));
			CPPUNIT_ASSERT(testling.parse("<foo/>"));
			CPPUNIT_ASSERT(testling.parse("<bar/>"));
			CPPUNIT_ASSERT(testling.parse("<presence/>"));

			CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(client_.events.size()));
			CPPUNIT_ASSERT_EQUAL(Client::ElementEvent, client_.events[2].type);
			CPPUNIT_ASSERT(dynamic_cast<UnknownElement*>(client_.events[2].element.get()));
			CPPUNIT_ASSERT_EQUAL(Client::ElementEvent, client_.events[3].type);
			CPPUNIT_ASSERT(dynamic_cast<UnknownElement*>(client_.events[3].element.get()));
			CPPUNIT_ASSERT_EQUAL(Client::ElementEvent, client_.events[4].type);
			CPPUNIT_ASSERT(dynamic_cast<Presence*>(client_.events[4].element.get()));
		}

		void testParse_StrayCharacterData() {
			XMPPParser testling(&client_, &factories_, &xmlParserFactory_);

			CPPUNIT_ASSERT(testling.parse("<stream:stream xmlns:stream='http://etherx.jabber.org/streams'>"));
			CPPUNIT_ASSERT(testling.parse("<presence/>"));
			CPPUNIT_ASSERT(testling.parse("bla"));
			CPPUNIT_ASSERT(testling.parse("<iq/>"));

			CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(client_.events.size()));
			CPPUNIT_ASSERT_EQUAL(Client::ElementEvent, client_.events[2].type);
			CPPUNIT_ASSERT(dynamic_cast<IQ*>(client_.events[2].element.get()));
		}

		void testParse_InvalidStreamStart() {
			XMPPParser testling(&client_, &factories_, &xmlParserFactory_);

			CPPUNIT_ASSERT(!testling.parse("<tream>"));
		}

		void testParse_ElementEndAfterInvalidStreamStart() {
			XMPPParser testling(&client_, &factories_, &xmlParserFactory_);

			CPPUNIT_ASSERT(!testling.parse("<tream/>"));
		}

	private:
		class Client : public XMPPParserClient {
			public:
				enum Type { StreamStart, ElementEvent, StreamEnd };
				struct Event {
					Event(Type type, boost::shared_ptr<Element> element) 
						: type(type), element(element) {}
					Event(Type type, const ProtocolHeader& header) : type(type), header(header) {}

					Event(Type type) : type(type) {}

					Type type;
					boost::optional<ProtocolHeader> header;
					boost::shared_ptr<Element> element;
				};

				Client() {}

				void handleStreamStart(const ProtocolHeader& header) {
					events.push_back(Event(StreamStart, header));
				}

				void handleElement(boost::shared_ptr<Element> element) {
					events.push_back(Event(ElementEvent, element));
				}

				void handleStreamEnd() {
					events.push_back(Event(StreamEnd));
				}

				std::vector<Event> events;
		} client_;
		PayloadParserFactoryCollection factories_;
		PlatformXMLParserFactory xmlParserFactory_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(XMPPParserTest);
