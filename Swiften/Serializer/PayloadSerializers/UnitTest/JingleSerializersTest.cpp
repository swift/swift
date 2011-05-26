/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferDescriptionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StreamInitiationFileInfoSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JinglePayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/StreamInitiationFileInfo.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Elements/JingleFileTransferHash.h>
#include <Swiften/Elements/JinglePayload.h>
#include <Swiften/Elements/JingleFileTransferReceived.h>
#include <Swiften/Base/DateTime.h>

using namespace Swift;

class JingleSerializersTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(JingleSerializersTest);
		CPPUNIT_TEST(testSerialize_StreamInitiationFileInfo);
		CPPUNIT_TEST(testSerialize_StreamInitiationFileInfoRange);
		
		CPPUNIT_TEST(testSerialize_Xep0261_Example1);
		CPPUNIT_TEST(testSerialize_Xep0261_Example9);
		CPPUNIT_TEST(testSerialize_Xep0261_Example13);
		
		CPPUNIT_TEST(testSerialize_Xep0234_Example1);
		CPPUNIT_TEST(testSerialize_Xep0234_Example3);
		CPPUNIT_TEST(testSerialize_Xep0234_Example5);
		CPPUNIT_TEST(testSerialize_Xep0234_Example8);
		CPPUNIT_TEST(testSerialize_Xep0234_Example10);
		CPPUNIT_TEST(testSerialize_Xep0234_Example13);

		CPPUNIT_TEST(testSerialize_Xep0260_Example1);

		CPPUNIT_TEST_SUITE_END();
		
		 boost::shared_ptr<JinglePayloadSerializer> createTestling() {
			 return boost::make_shared<JinglePayloadSerializer>(&collection);
		}

		
	public:
		void testSerialize_StreamInitiationFileInfo() {
			std::string expected =	"<file"
							" date=\"1969-07-21T02:56:15Z\""
							" hash=\"552da749930852c69ae5d2141d3766b1\""
							" name=\"test.txt\""
							" size=\"1022\""
							" xmlns=\"http://jabber.org/protocol/si/profile/file-transfer\">"
							"<desc>This is a test. If this were a real file...</desc>"
							"<range/>"
						"</file>";
			
			StreamInitiationFileInfo::ref fileInfo = boost::make_shared<StreamInitiationFileInfo>();
			fileInfo->setDate(stringToDateTime("1969-07-21T02:56:15Z"));
			fileInfo->setHash("552da749930852c69ae5d2141d3766b1");
			fileInfo->setSize(1022);
			fileInfo->setName("test.txt");
			fileInfo->setDescription("This is a test. If this were a real file...");
			fileInfo->setSupportsRangeRequests(true);
			
			boost::shared_ptr<StreamInitiationFileInfoSerializer> serializer = boost::make_shared<StreamInitiationFileInfoSerializer>();
			CPPUNIT_ASSERT_EQUAL(expected, serializer->serializePayload(fileInfo));
		}
		
		void testSerialize_StreamInitiationFileInfoRange() {
			std::string expected =	"<file hash=\"552da749930852c69ae5d2141d3766b1\""
							" xmlns=\"http://jabber.org/protocol/si/profile/file-transfer\">"
							"<range offset=\"270336\"/>"
						"</file>";
			
			StreamInitiationFileInfo::ref fileInfo = boost::make_shared<StreamInitiationFileInfo>();
			fileInfo->setHash("552da749930852c69ae5d2141d3766b1");
			fileInfo->setSupportsRangeRequests(true);
			fileInfo->setRangeOffset(270336);
			
			boost::shared_ptr<StreamInitiationFileInfoSerializer> serializer = boost::make_shared<StreamInitiationFileInfoSerializer>();
			CPPUNIT_ASSERT_EQUAL(expected, serializer->serializePayload(fileInfo));
		}
		
		
		// IBB Transport Method Examples
		
		// http://xmpp.org/extensions/xep-0261.html#example-1
		void testSerialize_Xep0261_Example1() {
			std::string expected = 
				"<jingle action=\"session-initiate\""
					" initiator=\"romeo@montague.lit/orchard\""
					" sid=\"a73sjjvkla37jfea\""
					" xmlns=\"urn:xmpp:jingle:1\">"
					"<content creator=\"initiator\" name=\"ex\">"
						"<transport block-size=\"4096\""
							" sid=\"ch3d9s71\""
							" xmlns=\"urn:xmpp:jingle:transports:ibb:1\"/>"
					"</content>"
				"</jingle>";
			
			JinglePayload::ref payload = boost::make_shared<JinglePayload>();
			payload->setAction(JinglePayload::SessionInitiate);
			payload->setSessionID("a73sjjvkla37jfea");
			payload->setInitiator(JID("romeo@montague.lit/orchard"));
			
			JingleIBBTransportPayload::ref transport = boost::make_shared<JingleIBBTransportPayload>();
			transport->setBlockSize(4096);
			transport->setSessionID("ch3d9s71");
			
			JingleContentPayload::ref content = boost::make_shared<JingleContentPayload>();
			content->setCreator(JingleContentPayload::InitiatorCreator);
			content->setName("ex");
			content->addTransport(transport);
			
			payload->addPayload(content);
			
			CPPUNIT_ASSERT_EQUAL(expected, createTestling()->serialize(payload));
		}
		
		// http://xmpp.org/extensions/xep-0261.html#example-9
		void testSerialize_Xep0261_Example9() {
			std::string expected =
				"<jingle action=\"transport-info\""
					" initiator=\"romeo@montague.lit/orchard\""
					" sid=\"a73sjjvkla37jfea\""
					" xmlns=\"urn:xmpp:jingle:1\">"
					"<content creator=\"initiator\" name=\"ex\">"
						"<transport block-size=\"2048\""
							" sid=\"bt8a71h6\""
							" xmlns=\"urn:xmpp:jingle:transports:ibb:1\"/>"
					"</content>"
				"</jingle>";
			
			JinglePayload::ref payload = boost::make_shared<JinglePayload>();
			payload->setAction(JinglePayload::TransportInfo);
			payload->setInitiator(JID("romeo@montague.lit/orchard"));
			payload->setSessionID("a73sjjvkla37jfea");
			
			JingleContentPayload::ref content = boost::make_shared<JingleContentPayload>();
			content->setCreator(JingleContentPayload::InitiatorCreator);
			content->setName("ex");
			
			JingleIBBTransportPayload::ref transport = boost::make_shared<JingleIBBTransportPayload>();
			transport->setBlockSize(2048);
			transport->setSessionID("bt8a71h6");
			
			content->addTransport(transport);
			payload->addPayload(content);
			
			CPPUNIT_ASSERT_EQUAL(expected, createTestling()->serialize(payload));
		}
		
		// http://xmpp.org/extensions/xep-0261.html#example-13
		void testSerialize_Xep0261_Example13() {
			std::string expected =
				"<jingle action=\"session-terminate\""
					" initiator=\"romeo@montague.lit/orchard\""
					" sid=\"a73sjjvkla37jfea\""
					" xmlns=\"urn:xmpp:jingle:1\">"
					"<reason><success/></reason>"
				"</jingle>";
			
			JinglePayload::ref payload = boost::make_shared<JinglePayload>();
			payload->setAction(JinglePayload::SessionTerminate);
			payload->setInitiator(JID("romeo@montague.lit/orchard"));
			payload->setSessionID("a73sjjvkla37jfea");
			payload->setReason(JinglePayload::Reason(JinglePayload::Reason::Success));
			
			CPPUNIT_ASSERT_EQUAL(expected, createTestling()->serialize(payload));
		}
		
		// http://xmpp.org/extensions/xep-0234.html#example-1
		void testSerialize_Xep0234_Example1() {
			std::string expected =	"<description xmlns=\"urn:xmpp:jingle:apps:file-transfer:3\">"
							"<offer>"
								"<file"
									" date=\"1969-07-21T02:56:15Z\""
									" hash=\"552da749930852c69ae5d2141d3766b1\""
									" name=\"test.txt\""
									" size=\"1022\""
									" xmlns=\"http://jabber.org/protocol/si/profile/file-transfer\">"
									"<desc>This is a test. If this were a real file...</desc>"
									"<range/>"
								"</file>"
							"</offer>"
						"</description>";
			JingleFileTransferDescription::ref desc = boost::make_shared<JingleFileTransferDescription>();
			StreamInitiationFileInfo fileInfo;
			
			fileInfo.setDate(stringToDateTime("1969-07-21T02:56:15Z"));
			fileInfo.setHash("552da749930852c69ae5d2141d3766b1");
			fileInfo.setSize(1022);
			fileInfo.setName("test.txt");
			fileInfo.setDescription("This is a test. If this were a real file...");
			fileInfo.setSupportsRangeRequests(true);
			
			desc->addOffer(fileInfo);
			
			CPPUNIT_ASSERT_EQUAL(expected, boost::make_shared<JingleFileTransferDescriptionSerializer>()->serialize(desc));
		}
		
		// http://xmpp.org/extensions/xep-0234.html#example-3
		void testSerialize_Xep0234_Example3() {
			std::string expected =
				"<jingle action=\"session-accept\""
					" initiator=\"romeo@montague.lit/orchard\""
					" sid=\"851ba2\""
					" xmlns=\"urn:xmpp:jingle:1\">"
					"<content creator=\"initiator\" name=\"a-file-offer\">"
						"<description xmlns=\"urn:xmpp:jingle:apps:file-transfer:3\">"
							"<offer>"
								"<file"
									" date=\"1969-07-21T02:56:15Z\""
									" hash=\"552da749930852c69ae5d2141d3766b1\""
									" name=\"test.txt\""
									" size=\"1022\""
									" xmlns=\"http://jabber.org/protocol/si/profile/file-transfer\">"
									"<desc>This is a test. If this were a real file...</desc>"
									"<range/>"
								"</file>"
							"</offer>"
						"</description>"
						/*"<transport xmlns=\"urn:xmpp:jingle:transports:s5b:1\""
							" mode=\"tcp\""
							" sid=\"vj3hs98y\">"
							"<candidate cid=\"ht567dq\""
								" host=\"192.169.1.10\""
								" jid=\"juliet@capulet.lit/balcony\""
								" port=\"6539\""
								" priority=\"8257636\""
								" type=\"direct\"/>"
							"<candidate cid=\"hr65dqyd\""
								" host=\"134.102.201.180\""
								" jid=\"juliet@capulet.lit/balcony\""
								" port=\"16453\""
								" priority=\"7929856\""
								" type=\"assisted\"/>"
							"<candidate cid=\"grt654q2\""
								" host=\"2001:638:708:30c9:219:d1ff:fea4:a17d\""
								" jid=\"juliet@capulet.lit/balcony\""
								" port=\"6539\""
								" priority=\"8257606\""
								" type=\"direct\"/>"
						"</transport>"*/
					"</content>"
				"</jingle>";
			
			JinglePayload::ref payload = boost::make_shared<JinglePayload>();
			payload->setAction(JinglePayload::SessionAccept);
			payload->setInitiator(JID("romeo@montague.lit/orchard"));
			payload->setSessionID("851ba2");
			
			JingleContentPayload::ref content = boost::make_shared<JingleContentPayload>();
			content->setCreator(JingleContentPayload::InitiatorCreator);
			content->setName("a-file-offer");
			
			JingleFileTransferDescription::ref description = boost::make_shared<JingleFileTransferDescription>();
			StreamInitiationFileInfo fileInfo;
			fileInfo.setName("test.txt");
			fileInfo.setSize(1022);
			fileInfo.setHash("552da749930852c69ae5d2141d3766b1");
			fileInfo.setDate(stringToDateTime("1969-07-21T02:56:15Z"));
			fileInfo.setDescription("This is a test. If this were a real file...");
			fileInfo.setSupportsRangeRequests(true);
			
			description->addOffer(fileInfo);
			content->addDescription(description);
			payload->addPayload(content);
			
			CPPUNIT_ASSERT_EQUAL(expected, createTestling()->serialize(payload));
		}
		
		// http://xmpp.org/extensions/xep-0234.html#example-5
		void testSerialize_Xep0234_Example5() {
			std::string expected = 
				"<jingle"
					" action=\"transport-info\""
					" initiator=\"romeo@montague.lit/orchard\""
					" sid=\"a73sjjvkla37jfea\""
					" xmlns=\"urn:xmpp:jingle:1\">"
					"<content creator=\"initiator\" name=\"ex\"/>"
						/*"<transport"
							" sid=\"vj3hs98y\""
							" xmlns=\"urn:xmpp:jingle:transports:s5b:1\">"
							"<candidate-used cid=\"hr65dqyd\"/>"
						"</transport>"*/
					//"</content>"
				"</jingle>";

			JinglePayload::ref payload = boost::make_shared<JinglePayload>();
			payload->setAction(JinglePayload::TransportInfo);
			payload->setInitiator(JID("romeo@montague.lit/orchard"));
			payload->setSessionID("a73sjjvkla37jfea");
			
			JingleContentPayload::ref content = boost::make_shared<JingleContentPayload>();
			content->setCreator(JingleContentPayload::InitiatorCreator);
			content->setName("ex");
			payload->addPayload(content);
			
			CPPUNIT_ASSERT_EQUAL(expected, createTestling()->serialize(payload));
		}
		
		// http://xmpp.org/extensions/xep-0234.html#example-8
		void testSerialize_Xep0234_Example8() {
			std::string expected = 
				"<jingle"
					" action=\"session-info\""
					" initiator=\"romeo@montague.lit/orchard\""
					" sid=\"a73sjjvkla37jfea\""
					" xmlns=\"urn:xmpp:jingle:1\">"
					"<checksum xmlns=\"urn:xmpp:jingle:apps:file-transfer:3\">"
						"<file>"
							"<hashes xmlns=\"urn:xmpp:hashes:0\">"
								"<hash algo=\"sha-1\">552da749930852c69ae5d2141d3766b1</hash>"
							"</hashes>"
						"</file>"
					"</checksum>"
				"</jingle>";
			
			JinglePayload::ref payload = boost::make_shared<JinglePayload>();
			payload->setAction(JinglePayload::SessionInfo);
			payload->setInitiator(JID("romeo@montague.lit/orchard"));
			payload->setSessionID("a73sjjvkla37jfea");
			
			JingleFileTransferHash::ref hash = boost::make_shared<JingleFileTransferHash>();
			hash->setHash("sha-1", "552da749930852c69ae5d2141d3766b1");
			
			payload->addPayload(hash);
			
			CPPUNIT_ASSERT_EQUAL(expected, createTestling()->serialize(payload));
		}
		
		// http://xmpp.org/extensions/xep-0234.html#example-10
		void testSerialize_Xep0234_Example10() {
			std::string expected = 
				"<jingle"
					" action=\"session-initiate\""
					" initiator=\"romeo@montague.lit/orchard\""
					" sid=\"uj3b2\""
					" xmlns=\"urn:xmpp:jingle:1\">"
					"<content creator=\"initiator\" name=\"a-file-request\">"
						"<description"
							" xmlns=\"urn:xmpp:jingle:apps:file-transfer:3\">"
							"<request>"
								"<file"
									" hash=\"552da749930852c69ae5d2141d3766b1\""
									" xmlns=\"http://jabber.org/protocol/si/profile/file-transfer\">"
									"<range offset=\"270336\"/>"
								"</file>"
							"</request>"
						"</description>"
						/*"<transport"
							" mode=\"tcp\""
							" sid=\"xig361fj\""
							" xmlns=\"urn:xmpp:jingle:transports:s5b:1\">"
							"<candidate"
								" cid=\"ht567dq\""
								" host=\"192.169.1.10\""
								" jid=\"juliet@capulet.lit/balcony\""
								" port=\"6539\""
								" priority=\"8257636\""
								" type=\"direct\"/>"
							"<candidate"
								" cid=\"hr65dqyd\""
								" host=\"134.102.201.180\""
								" jid=\"juliet@capulet.lit/balcony\""
								" port=\"16453\""
								" priority=\"7929856\""
								" type=\"assisted\"/>"
							"<candidate"
								" cid=\"grt654q2\""
								" host=\"2001:638:708:30c9:219:d1ff:fea4:a17d\""
								" jid=\"juliet@capulet.lit/balcony\""
								" port=\"6539\""
								" priority=\"8257606\""
								" type=\"direct\"/>"
						"</transport>"*/
					"</content>"
				"</jingle>";
			
			JinglePayload::ref payload = boost::make_shared<JinglePayload>();
			payload->setAction(JinglePayload::SessionInitiate);
			payload->setInitiator(JID("romeo@montague.lit/orchard"));
			payload->setSessionID("uj3b2");
			
			StreamInitiationFileInfo fileInfo;
			fileInfo.setHash("552da749930852c69ae5d2141d3766b1");
			fileInfo.setRangeOffset(270336);
			
			JingleFileTransferDescription::ref desc = boost::make_shared<JingleFileTransferDescription>();
			desc->addRequest(fileInfo);
			
			JingleContentPayload::ref content = boost::make_shared<JingleContentPayload>();
			content->setCreator(JingleContentPayload::InitiatorCreator);
			content->setName("a-file-request");
			content->addDescription(desc);
			
			payload->addPayload(content);
			
			CPPUNIT_ASSERT_EQUAL(expected, createTestling()->serialize(payload));
		}

		// http://xmpp.org/extensions/xep-0234.html#example-10
		void testSerialize_Xep0234_Example13() {
			std::string expected =
				"<jingle"
					" action=\"session-info\""
					" initiator=\"romeo@montague.lit/orchard\""
					" sid=\"a73sjjvkla37jfea\""
					" xmlns=\"urn:xmpp:jingle:1\">"
					"<received xmlns=\"urn:xmpp:jingle:apps:file-transfer:3\">"
						"<file"
							" hash=\"a749930852c69ae5d2141d3766b1552d\""
							" xmlns=\"http://jabber.org/protocol/si/profile/file-transfer\"/>"
					"</received>"
				"</jingle>";

			JinglePayload::ref payload = boost::make_shared<JinglePayload>();
			payload->setAction(JinglePayload::SessionInfo);
			payload->setInitiator(JID("romeo@montague.lit/orchard"));
			payload->setSessionID("a73sjjvkla37jfea");

			JingleFileTransferReceived::ref received = boost::make_shared<JingleFileTransferReceived>();

			StreamInitiationFileInfo fileInfo;
			fileInfo.setHash("a749930852c69ae5d2141d3766b1552d");

			received->setFileInfo(fileInfo);
			payload->addPayload(received);

			CPPUNIT_ASSERT_EQUAL(expected, createTestling()->serialize(payload));
		}

		// http://xmpp.org/extensions/xep-0260.html#example-1
		void testSerialize_Xep0260_Example1() {
			std::string expected =
				"<jingle"
					" action=\"session-initiate\""
					" initiator=\"romeo@montague.lit/orchard\""
					" sid=\"a73sjjvkla37jfea\""
					" xmlns=\"urn:xmpp:jingle:1\">"
					"<content creator=\"initiator\" name=\"ex\">"
						"<transport"
							" mode=\"tcp\""
							" sid=\"vj3hs98y\""
							" xmlns=\"urn:xmpp:jingle:transports:s5b:1\">"
							"<candidate cid=\"hft54dqy\""
								" host=\"192.168.4.1\""
								" jid=\"romeo@montague.lit/orchard\""
								" port=\"5086\""
								" priority=\"8257636\""
								" type=\"direct\"/>"
							"<candidate cid=\"hutr46fe\""
								" host=\"24.24.24.1\""
								" jid=\"romeo@montague.lit/orchard\""
								" port=\"5087\""
								" priority=\"8258636\""
								" type=\"direct\"/>"
						"</transport>"
					"</content>"
				"</jingle>";

			JinglePayload::ref payload = boost::make_shared<JinglePayload>();
			payload->setAction(JinglePayload::SessionInitiate);
			payload->setInitiator(JID("romeo@montague.lit/orchard"));
			payload->setSessionID("a73sjjvkla37jfea");

			JingleContentPayload::ref content = boost::make_shared<JingleContentPayload>();
			content->setCreator(JingleContentPayload::InitiatorCreator);
			content->setName("ex");

			JingleS5BTransportPayload::ref transport = boost::make_shared<JingleS5BTransportPayload>();
			transport->setMode(JingleS5BTransportPayload::TCPMode);
			transport->setSessionID("vj3hs98y");

			JingleS5BTransportPayload::Candidate candidate1;
			candidate1.cid = "hft54dqy";
			candidate1.hostPort = HostAddressPort(HostAddress("192.168.4.1"), 5086);
			candidate1.jid = JID("romeo@montague.lit/orchard");
			candidate1.priority = 8257636;
			candidate1.type = JingleS5BTransportPayload::Candidate::DirectType;
			transport->addCandidate(candidate1);

			JingleS5BTransportPayload::Candidate candidate2;
			candidate2.cid = "hutr46fe";
			candidate2.hostPort = HostAddressPort(HostAddress("24.24.24.1"), 5087);
			candidate2.jid = JID("romeo@montague.lit/orchard");
			candidate2.priority = 8258636;
			candidate2.type = JingleS5BTransportPayload::Candidate::DirectType;
			transport->addCandidate(candidate2);

			content->addTransport(transport);

			payload->addPayload(content);

			CPPUNIT_ASSERT_EQUAL(expected, createTestling()->serialize(payload));
		}

	private:
		FullPayloadSerializerCollection collection;
};

CPPUNIT_TEST_SUITE_REGISTRATION(JingleSerializersTest);

