/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/JingleFileTransferHash.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JinglePayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Elements/StreamInitiationFileInfo.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/StringCodecs/Base64.h>

using namespace Swift;

class JingleParserTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(JingleParserTest);
        CPPUNIT_TEST(testParse_Xep0166_Example3);
        CPPUNIT_TEST(testParse_Xep0166_Example8);

        CPPUNIT_TEST(testParse_Xep0261_Example1);
        CPPUNIT_TEST(testParse_Xep0261_Example3);
        CPPUNIT_TEST(testParse_Xep0261_Example9);
        CPPUNIT_TEST(testParse_Xep0261_Example13);

        CPPUNIT_TEST(testParse_Xep0234_Example1);
        CPPUNIT_TEST(testParse_Xep0234_Example3);
        CPPUNIT_TEST(testParse_Xep0234_Example5);
        CPPUNIT_TEST(testParse_Xep0234_Example8);
        CPPUNIT_TEST(testParse_Xep0234_Example10);

        CPPUNIT_TEST(testParse_Xep0260_Example1);
        CPPUNIT_TEST(testParse_Xep0260_Example3);
        CPPUNIT_TEST_SUITE_END();

    public:
        //http://xmpp.org/extensions/xep-0166.html#example-3
        void testParse_Xep0166_Example3() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                "     action='session-terminate'\n"
                "     sid='a73sjjvkla37jfea'>\n"
                "    <reason>\n"
                "        <success/>\n"
                "    </reason>\n"
                "</jingle>\n"
            ));

            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::SessionTerminate, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(std::string("a73sjjvkla37jfea"), jingle->getSessionID());
            CPPUNIT_ASSERT_EQUAL(JinglePayload::Reason::Success,
                jingle->getReason().get_value_or(JinglePayload::Reason(JinglePayload::Reason::UnknownType, "")).type);
        }

        //http://xmpp.org/extensions/xep-0166.html#example-8
        void testParse_Xep0166_Example8() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                "        action='session-terminate'\n"
                "        sid='a73sjjvkla37jfea'>\n"
                "    <reason>\n"
                "        <success/>\n"
                "        <text>Sorry, gotta go!</text>\n"
                "    </reason>\n"
                "</jingle>\n"
            ));
            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::SessionTerminate, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(std::string("a73sjjvkla37jfea"), jingle->getSessionID());
            CPPUNIT_ASSERT_EQUAL(JinglePayload::Reason::Success,
                jingle->getReason().get_value_or(JinglePayload::Reason(JinglePayload::Reason::UnknownType, "")).type);
            CPPUNIT_ASSERT_EQUAL(std::string("Sorry, gotta go!"),
                jingle->getReason().get_value_or(JinglePayload::Reason(JinglePayload::Reason::UnknownType, "")).text);
        }

        // IBB Transport Method Examples

        // http://xmpp.org/extensions/xep-0261.html#example-1
        void testParse_Xep0261_Example1() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                "          action='session-initiate'\n"
                "          initiator='romeo@montague.lit/orchard'\n"
                "          sid='a73sjjvkla37jfea'>\n"
                "    <content creator='initiator' name='ex'>\n"
                "      <description xmlns='urn:xmpp:example'/>\n"
                "      <transport xmlns='urn:xmpp:jingle:transports:ibb:1'\n"
                "                 block-size='4096'\n"
                "                 sid='ch3d9s71'/>\n"
                "    </content>\n"
                "</jingle>\n"
            ));
            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::SessionInitiate, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), jingle->getInitiator());
            CPPUNIT_ASSERT_EQUAL(std::string("a73sjjvkla37jfea"), jingle->getSessionID());

            std::vector<JingleContentPayload::ref> payloads = jingle->getContents();
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), payloads.size());
            JingleContentPayload::ref payload = payloads[0];
            CPPUNIT_ASSERT_EQUAL(JingleContentPayload::InitiatorCreator, payload->getCreator());
            CPPUNIT_ASSERT_EQUAL(std::string("ex"), payload->getName());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), payload->getTransports().size());

            JingleIBBTransportPayload::ref transportPaylod = payload->getTransport<JingleIBBTransportPayload>();
            CPPUNIT_ASSERT(transportPaylod);
            CPPUNIT_ASSERT_EQUAL(4096U, *transportPaylod->getBlockSize());
            CPPUNIT_ASSERT_EQUAL(std::string("ch3d9s71"), transportPaylod->getSessionID());
        }

        // http://xmpp.org/extensions/xep-0261.html#example-1
        void testParse_Xep0261_Example3() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                "          action='session-accept'\n"
                "          initiator='romeo@montague.lit/orchard'\n"
                "          responder='juliet@capulet.lit/balcony'\n"
                "          sid='a73sjjvkla37jfea'>\n"
                "    <content creator='initiator' name='ex'>\n"
                "      <description xmlns='urn:xmpp:example'/>\n"
                "      <transport xmlns='urn:xmpp:jingle:transports:ibb:1'\n"
                "                 block-size='2048'\n"
                "                 sid='ch3d9s71'/>\n"
                "    </content>\n"
                "  </jingle>\n"
            ));
            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::SessionAccept, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), jingle->getInitiator());
            CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.lit/balcony"), jingle->getResponder());
            CPPUNIT_ASSERT_EQUAL(std::string("a73sjjvkla37jfea"), jingle->getSessionID());

            std::vector<JingleContentPayload::ref> payloads = jingle->getContents();
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), payloads.size());
            JingleContentPayload::ref payload = payloads[0];
            CPPUNIT_ASSERT_EQUAL(JingleContentPayload::InitiatorCreator, payload->getCreator());
            CPPUNIT_ASSERT_EQUAL(std::string("ex"), payload->getName());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), payload->getTransports().size());

            JingleIBBTransportPayload::ref transportPaylod = payload->getTransport<JingleIBBTransportPayload>();
            CPPUNIT_ASSERT(transportPaylod);
            CPPUNIT_ASSERT_EQUAL(2048U, *transportPaylod->getBlockSize());
            CPPUNIT_ASSERT_EQUAL(std::string("ch3d9s71"), transportPaylod->getSessionID());
        }

        // http://xmpp.org/extensions/xep-0261.html#example-9
        void testParse_Xep0261_Example9() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                "        action='transport-info'\n"
                "        initiator='romeo@montague.lit/orchard'\n"
                "        sid='a73sjjvkla37jfea'>\n"
                "  <content creator='initiator' name='ex'>\n"
                "    <transport xmlns='urn:xmpp:jingle:transports:ibb:1'\n"
                "               block-size='2048'\n"
                "               sid='bt8a71h6'/>\n"
                "  </content>\n"
                "</jingle>\n"
            ));
            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::TransportInfo, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), jingle->getInitiator());
            CPPUNIT_ASSERT_EQUAL(std::string("a73sjjvkla37jfea"), jingle->getSessionID());

            std::vector<JingleContentPayload::ref> payloads = jingle->getContents();
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), payloads.size());
            JingleContentPayload::ref payload = payloads[0];
            CPPUNIT_ASSERT_EQUAL(JingleContentPayload::InitiatorCreator, payload->getCreator());
            CPPUNIT_ASSERT_EQUAL(std::string("ex"), payload->getName());

            JingleIBBTransportPayload::ref transportPaylod = payload->getTransport<JingleIBBTransportPayload>();
            CPPUNIT_ASSERT(transportPaylod);
            CPPUNIT_ASSERT_EQUAL(2048U, *transportPaylod->getBlockSize());
            CPPUNIT_ASSERT_EQUAL(std::string("bt8a71h6"), transportPaylod->getSessionID());
        }

        // http://xmpp.org/extensions/xep-0261.html#example-13
        void testParse_Xep0261_Example13() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                "          action='session-terminate'\n"
                "          initiator='romeo@montague.lit/orchard'\n"
                "          sid='a73sjjvkla37jfea'>\n"
                "    <reason><success/></reason>\n"
                "  </jingle>\n"
            ));

            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::SessionTerminate, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), jingle->getInitiator());
            CPPUNIT_ASSERT_EQUAL(std::string("a73sjjvkla37jfea"), jingle->getSessionID());
            CPPUNIT_ASSERT_EQUAL(JinglePayload::Reason::Success, jingle->getReason().get_value_or(JinglePayload::Reason()).type);

        }

        // Jingle File Transfer Examples

        // http://xmpp.org/extensions/xep-0234.html#example-1
        void testParse_Xep0234_Example1() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                    " action='session-initiate'\n"
                    " initiator='romeo@montague.lit/orchard'\n"
                    " sid='851ba2'>\n"
                    "<content creator='initiator' name='a-file-offer'>\n"
                        "<description xmlns='urn:xmpp:jingle:apps:file-transfer:4'>\n"
                            "<file>\n"
                                "<date>1969-07-21T02:56:15Z</date>\n"
                                "<desc>This is a test. If this were a real file...</desc>\n"
                                "<media-type>text/plain</media-type>\n"
                                "<name>test.txt</name>\n"
                                "<range/>\n"
                                "<size>1022</size>\n"
                                "<hash xmlns='urn:xmpp:hashes:1' algo='sha-1'>VS2nSZMIUsaa5dIUHTdmsQ==</hash>\n"
                            "</file>\n"
                        "</description>\n"
                        "<transport xmlns='urn:xmpp:jingle:transports:s5b:1'\n"
                            " mode='tcp'\n"
                            " sid='vj3hs98y'>\n"
                            "<candidate cid='hft54dqy'\n"
                                " host='192.168.4.1'\n"
                                " jid='romeo@montague.lit/orchard'\n"
                                " port='5086'\n"
                                " priority='8257636'\n"
                                " type='direct'/>\n"
                            "<candidate cid='hutr46fe'\n"
                                " host='24.24.24.1'\n"
                                " jid='romeo@montague.lit/orchard'\n"
                                " port='5087'\n"
                                " priority='8258636'\n"
                                " type='direct'/>\n"
                        "</transport>\n"
                    "</content>\n"
                "</jingle>\n"
            ));

            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::SessionInitiate, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), jingle->getInitiator());
            CPPUNIT_ASSERT_EQUAL(std::string("851ba2"), jingle->getSessionID());

            std::vector<JingleContentPayload::ref> contents = jingle->getContents();
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), contents.size());

            JingleFileTransferDescription::ref description = contents[0]->getDescription<JingleFileTransferDescription>();

            CPPUNIT_ASSERT(description);
            JingleFileTransferFileInfo fileInfo = description->getFileInfo();
            CPPUNIT_ASSERT_EQUAL(std::string("test.txt"), fileInfo.getName());
            CPPUNIT_ASSERT_EQUAL(std::string("sha-1"), fileInfo.getHashes().begin()->first);
            CPPUNIT_ASSERT_EQUAL(std::string("VS2nSZMIUsaa5dIUHTdmsQ=="), Base64::encode(fileInfo.getHashes().begin()->second));
            CPPUNIT_ASSERT(1022 == fileInfo.getSize());
            CPPUNIT_ASSERT_EQUAL(std::string("This is a test. If this were a real file..."), fileInfo.getDescription());
            CPPUNIT_ASSERT_EQUAL(true, fileInfo.getSupportsRangeRequests());
            CPPUNIT_ASSERT(stringToDateTime("1969-07-21T02:56:15Z") == fileInfo.getDate());
        }

        // http://xmpp.org/extensions/xep-0234.html#example-3
        void testParse_Xep0234_Example3() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                    " action='session-accept'\n"
                    " initiator='romeo@montague.lit/orchard'\n"
                    " sid='851ba2'>\n"
                    "<content creator='initiator' name='a-file-offer'>\n"
                        "<description xmlns='urn:xmpp:jingle:apps:file-transfer:4'>\n"
                            "<file>\n"
                                "<date>1969-07-21T02:56:15Z</date>\n"
                                "<desc>This is a test. If this were a real file...</desc>\n"
                                "<media-type>text/plain</media-type>\n"
                                "<name>test.txt</name>\n"
                                "<range/>\n"
                                "<size>1022</size>\n"
                                "<hash xmlns='urn:xmpp:hashes:1' algo='sha-1'>VS2nSZMIUsaa5dIUHTdmsQ==</hash>\n"
                            "</file>\n"
                        "</description>\n"
                "    <transport xmlns='urn:xmpp:jingle:transports:s5b:1'\n"
                "               mode='tcp'\n"
                "               sid='vj3hs98y'>\n"
                "      <candidate cid='ht567dq'\n"
                "                 host='192.169.1.10'\n"
                "                 jid='juliet@capulet.lit/balcony'\n"
                "                 port='6539'\n"
                "                 priority='8257636'\n"
                "                 type='direct'/>\n"
                "      <candidate cid='hr65dqyd'\n"
                "                 host='134.102.201.180'\n"
                "                 jid='juliet@capulet.lit/balcony'\n"
                "                 port='16453'\n"
                "                 priority='7929856'\n"
                "                 type='assisted'/>\n"
                "      <candidate cid='grt654q2'\n"
                "                 host='2001:638:708:30c9:219:d1ff:fea4:a17d'\n"
                "                 jid='juliet@capulet.lit/balcony'\n"
                "                 port='6539'\n"
                "                 priority='8257606'\n"
                "                 type='direct'/>\n"
                "    </transport>\n"
                "  </content>\n"
                "</jingle>\n"
            ));

            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::SessionAccept, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), jingle->getInitiator());
            CPPUNIT_ASSERT_EQUAL(std::string("851ba2"), jingle->getSessionID());

            std::vector<JingleContentPayload::ref> contents = jingle->getContents();
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), contents.size());

            JingleFileTransferDescription::ref description = contents[0]->getDescription<JingleFileTransferDescription>();

            CPPUNIT_ASSERT(description);

            JingleFileTransferFileInfo fileInfo = description->getFileInfo();
            CPPUNIT_ASSERT_EQUAL(std::string("test.txt"), fileInfo.getName());
            CPPUNIT_ASSERT_EQUAL(std::string("sha-1"), fileInfo.getHashes().begin()->first);
            CPPUNIT_ASSERT_EQUAL(std::string("VS2nSZMIUsaa5dIUHTdmsQ=="), Base64::encode(fileInfo.getHashes().begin()->second));
            CPPUNIT_ASSERT(1022 == fileInfo.getSize());
            CPPUNIT_ASSERT_EQUAL(std::string("This is a test. If this were a real file..."), fileInfo.getDescription());
            CPPUNIT_ASSERT_EQUAL(true, fileInfo.getSupportsRangeRequests());
            CPPUNIT_ASSERT(stringToDateTime("1969-07-21T02:56:15Z") == fileInfo.getDate());
        }

        // http://xmpp.org/extensions/xep-0234.html#example-5
        void testParse_Xep0234_Example5() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                "        action='transport-info'\n"
                "        initiator='romeo@montague.lit/orchard'\n"
                "        sid='a73sjjvkla37jfea'>\n"
                "  <content creator='initiator' name='ex'>\n"
                "    <transport xmlns='urn:xmpp:jingle:transports:s5b:1'\n"
                "               sid='vj3hs98y'>\n"
                "      <candidate-used cid='hr65dqyd'/>\n"
                "    </transport>\n"
                "  </content>\n"
                "</jingle>\n"
            ));

            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::TransportInfo, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), jingle->getInitiator());
            CPPUNIT_ASSERT_EQUAL(std::string("a73sjjvkla37jfea"), jingle->getSessionID());

            std::vector<JingleContentPayload::ref> contents = jingle->getContents();
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), contents.size());

            JingleS5BTransportPayload::ref transport = contents[0]->getTransport<JingleS5BTransportPayload>();
            CPPUNIT_ASSERT(transport);

            CPPUNIT_ASSERT_EQUAL(std::string("vj3hs98y"), transport->getSessionID());
            CPPUNIT_ASSERT_EQUAL(std::string("hr65dqyd"), transport->getCandidateUsed());
        }

        // http://xmpp.org/extensions/xep-0234.html#example-8
        void testParse_Xep0234_Example8() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                "        action='session-info'\n"
                "        initiator='romeo@montague.lit/orchard'\n"
                "        sid='a73sjjvkla37jfea'>\n"
                "    <checksum xmlns='urn:xmpp:jingle:apps:file-transfer:4'>\n"
                "      <file>\n"
                "          <hash xmlns='urn:xmpp:hashes:0' algo='sha-1'>VS2nSZMIUsaa5dIUHTdmsQ==</hash>\n"
                "      </file>\n"
                "    </checksum>\n"
                "</jingle>\n"
            ));
            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::SessionInfo, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), jingle->getInitiator());
            CPPUNIT_ASSERT_EQUAL(std::string("a73sjjvkla37jfea"), jingle->getSessionID());

            JingleFileTransferHash::ref hash = jingle->getPayload<JingleFileTransferHash>();
            CPPUNIT_ASSERT(hash);
            CPPUNIT_ASSERT_EQUAL(std::string("VS2nSZMIUsaa5dIUHTdmsQ=="), Base64::encode(hash->getFileInfo().getHash("sha-1").get()));
        }

        // http://xmpp.org/extensions/xep-0234.html#example-10
        void testParse_Xep0234_Example10() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                "        action='session-initiate'\n"
                "        initiator='romeo@montague.lit/orchard'\n"
                "        sid='uj3b2'>\n"
                "  <content creator='initiator' name='a-file-request'>\n"
                "    <description xmlns='urn:xmpp:jingle:apps:file-transfer:4'>\n"
                "      <file>\n"
                "        <hash xmlns='urn:xmpp:hashes:1' algo='sha-1'>VS2nSZMIUsaa5dIUHTdmsQ==</hash>\n"
                "        <range offset='270336'/>\n"
                "      </file>\n"
                "    </description>\n"
                "    <transport xmlns='urn:xmpp:jingle:transports:s5b:1'\n"
                "               mode='tcp'\n"
                "               sid='xig361fj'>\n"
                "      <candidate cid='ht567dq'\n"
                "                 host='192.169.1.10'\n"
                "                 jid='juliet@capulet.lit/balcony'\n"
                "                 port='6539'\n"
                "                 priority='8257636'\n"
                "                 type='direct'/>\n"
                "      <candidate cid='hr65dqyd'\n"
                "                 host='134.102.201.180'\n"
                "                 jid='juliet@capulet.lit/balcony'\n"
                "                 port='16453'\n"
                "                 priority='7929856'\n"
                "                 type='assisted'/>\n"
                "      <candidate cid='grt654q2'\n"
                "                 host='2001:638:708:30c9:219:d1ff:fea4:a17d'\n"
                "                 jid='juliet@capulet.lit/balcony'\n"
                "                 port='6539'\n"
                "                 priority='8257606'\n"
                "                 type='direct'/>\n"
                "    </transport>\n"
                "  </content>\n"
                "</jingle>\n"
            ));

            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::SessionInitiate, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), jingle->getInitiator());
            CPPUNIT_ASSERT_EQUAL(std::string("uj3b2"), jingle->getSessionID());

            JingleContentPayload::ref content = jingle->getPayload<JingleContentPayload>();
            CPPUNIT_ASSERT(content);

            JingleFileTransferFileInfo file = content->getDescription<JingleFileTransferDescription>()->getFileInfo();
            CPPUNIT_ASSERT_EQUAL(std::string("sha-1"), file.getHashes().begin()->first);
            CPPUNIT_ASSERT_EQUAL(std::string("VS2nSZMIUsaa5dIUHTdmsQ=="), Base64::encode(file.getHashes().begin()->second));
            CPPUNIT_ASSERT_EQUAL(static_cast<boost::uintmax_t>(270336), file.getRangeOffset());
            CPPUNIT_ASSERT_EQUAL(true, file.getSupportsRangeRequests());
        }

        // http://xmpp.org/extensions/xep-0260.html#example-1
        void testParse_Xep0260_Example1() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                "        action='session-initiate'\n"
                "        initiator='romeo@montague.lit/orchard'\n"
                "        sid='a73sjjvkla37jfea'>\n"
                "  <content creator='initiator' name='ex'>\n"
                "    <description xmlns='urn:xmpp:example'/>\n"
                "    <transport xmlns='urn:xmpp:jingle:transports:s5b:1'\n"
                "               mode='tcp'\n"
                "               sid='vj3hs98y'>\n"
                "      <candidate cid='hft54dqy'\n"
                "                 host='192.168.4.1'\n"
                "                 jid='romeo@montague.lit/orchard'\n"
                "                 port='5086'\n"
                "                 priority='8257636'\n"
                "                 type='direct'/>\n"
                "      <candidate cid='hutr46fe'\n"
                "                 host='24.24.24.1'\n"
                "                 jid='romeo@montague.lit/orchard'\n"
                "                 port='5087'\n"
                "                 priority='8258636'\n"
                "                 type='direct'/>\n"
                "    </transport>\n"
                "  </content>\n"
                "</jingle>\n"
            ));
            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::SessionInitiate, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), jingle->getInitiator());
            CPPUNIT_ASSERT_EQUAL(std::string("a73sjjvkla37jfea"), jingle->getSessionID());

            JingleContentPayload::ref content = jingle->getPayload<JingleContentPayload>();
            CPPUNIT_ASSERT(content);

            JingleS5BTransportPayload::ref s5bPayload = content->getTransport<JingleS5BTransportPayload>();
            CPPUNIT_ASSERT(s5bPayload);

            CPPUNIT_ASSERT_EQUAL(std::string("vj3hs98y"), s5bPayload->getSessionID());
            CPPUNIT_ASSERT_EQUAL(JingleS5BTransportPayload::TCPMode, s5bPayload->getMode());
            CPPUNIT_ASSERT_EQUAL(false, s5bPayload->hasCandidateError());
            CPPUNIT_ASSERT_EQUAL(false, s5bPayload->hasProxyError());
            CPPUNIT_ASSERT_EQUAL(std::string(), s5bPayload->getActivated());
            CPPUNIT_ASSERT_EQUAL(std::string(), s5bPayload->getCandidateUsed());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), s5bPayload->getCandidates().size());

            JingleS5BTransportPayload::Candidate candidate;
            candidate = s5bPayload->getCandidates()[0];
            CPPUNIT_ASSERT_EQUAL(std::string("hft54dqy"), candidate.cid);
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), candidate.jid);
            CPPUNIT_ASSERT(HostAddressPort(HostAddress::fromString("192.168.4.1").get(), 5086) == candidate.hostPort);
            CPPUNIT_ASSERT_EQUAL(8257636, candidate.priority);
            CPPUNIT_ASSERT_EQUAL(JingleS5BTransportPayload::Candidate::DirectType, candidate.type);

            candidate = s5bPayload->getCandidates()[1];
            CPPUNIT_ASSERT_EQUAL(std::string("hutr46fe"), candidate.cid);
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), candidate.jid);
            CPPUNIT_ASSERT(HostAddressPort(HostAddress::fromString("24.24.24.1").get(), 5087) == candidate.hostPort);
            CPPUNIT_ASSERT_EQUAL(8258636, candidate.priority);
            CPPUNIT_ASSERT_EQUAL(JingleS5BTransportPayload::Candidate::DirectType, candidate.type);
        }

        // http://xmpp.org/extensions/xep-0260.html#example-3
        void testParse_Xep0260_Example3() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<jingle xmlns='urn:xmpp:jingle:1'\n"
                "        action='session-accept'\n"
                "        initiator='romeo@montague.lit/orchard'\n"
                "        sid='a73sjjvkla37jfea'>\n"
                "  <content creator='initiator' name='ex'>\n"
                "    <description xmlns='urn:xmpp:example'/>\n"
                "    <transport xmlns='urn:xmpp:jingle:transports:s5b:1'\n"
                "               dstaddr='1a12fb7bc625e55f3ed5b29a53dbe0e4aa7d80ba'\n"
                "               mode='tcp'\n"
                "               sid='vj3hs98y'>\n"
                "      <candidate cid='ht567dq'\n"
                "                 host='192.169.1.10'\n"
                "                 jid='juliet@capulet.lit/balcony'\n"
                "                 port='6539'\n"
                "                 priority='8257636'\n"
                "                 type='direct'/>\n"
                "      <candidate cid='hr65dqyd'\n"
                "                 host='134.102.201.180'\n"
                "                 jid='juliet@capulet.lit/balcony'\n"
                "                 port='16453'\n"
                "                 priority='7929856'\n"
                "                 type='assisted'/>\n"
                "      <candidate cid='grt654q2'\n"
                "                 host='2001:638:708:30c9:219:d1ff:fea4:a17d'\n"
                "                 jid='juliet@capulet.lit/balcony'\n"
                "                 port='6539'\n"
                "                 priority='8257606'\n"
                "                 type='direct'/>\n"
                "    </transport>\n"
                "  </content>\n"
                "</jingle>\n"
            ));

            JinglePayload::ref jingle = parser.getPayload<JinglePayload>();
            CPPUNIT_ASSERT(jingle);
            CPPUNIT_ASSERT_EQUAL(JinglePayload::SessionAccept, jingle->getAction());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), jingle->getInitiator());
            CPPUNIT_ASSERT_EQUAL(std::string("a73sjjvkla37jfea"), jingle->getSessionID());

            JingleContentPayload::ref content = jingle->getPayload<JingleContentPayload>();
            CPPUNIT_ASSERT(content);

            JingleS5BTransportPayload::ref s5bPayload = content->getTransport<JingleS5BTransportPayload>();
            CPPUNIT_ASSERT(s5bPayload);

            CPPUNIT_ASSERT_EQUAL(std::string("vj3hs98y"), s5bPayload->getSessionID());
            CPPUNIT_ASSERT_EQUAL(JingleS5BTransportPayload::TCPMode, s5bPayload->getMode());
            CPPUNIT_ASSERT_EQUAL(std::string("1a12fb7bc625e55f3ed5b29a53dbe0e4aa7d80ba"), s5bPayload->getDstAddr());
            CPPUNIT_ASSERT_EQUAL(false, s5bPayload->hasCandidateError());
            CPPUNIT_ASSERT_EQUAL(false, s5bPayload->hasProxyError());
            CPPUNIT_ASSERT_EQUAL(std::string(), s5bPayload->getActivated());
            CPPUNIT_ASSERT_EQUAL(std::string(), s5bPayload->getCandidateUsed());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), s5bPayload->getCandidates().size());

            JingleS5BTransportPayload::Candidate candidate;
            candidate = s5bPayload->getCandidates()[0];
            CPPUNIT_ASSERT_EQUAL(std::string("ht567dq"), candidate.cid);
            CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.lit/balcony"), candidate.jid);
            CPPUNIT_ASSERT(HostAddressPort(HostAddress::fromString("192.169.1.10").get(), 6539) == candidate.hostPort);
            CPPUNIT_ASSERT_EQUAL(8257636, candidate.priority);
            CPPUNIT_ASSERT_EQUAL(JingleS5BTransportPayload::Candidate::DirectType, candidate.type);

            candidate = s5bPayload->getCandidates()[1];
            CPPUNIT_ASSERT_EQUAL(std::string("hr65dqyd"), candidate.cid);
            CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.lit/balcony"), candidate.jid);
            CPPUNIT_ASSERT(HostAddressPort(HostAddress::fromString("134.102.201.180").get(), 16453) == candidate.hostPort);
            CPPUNIT_ASSERT_EQUAL(7929856, candidate.priority);
            CPPUNIT_ASSERT_EQUAL(JingleS5BTransportPayload::Candidate::AssistedType, candidate.type);

            candidate = s5bPayload->getCandidates()[2];
            CPPUNIT_ASSERT_EQUAL(std::string("grt654q2"), candidate.cid);
            CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.lit/balcony"), candidate.jid);
            CPPUNIT_ASSERT(HostAddressPort(HostAddress::fromString("2001:638:708:30c9:219:d1ff:fea4:a17d").get(), 6539) == candidate.hostPort);
            CPPUNIT_ASSERT_EQUAL(8257606, candidate.priority);
            CPPUNIT_ASSERT_EQUAL(JingleS5BTransportPayload::Candidate::DirectType, candidate.type);
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(JingleParserTest);
