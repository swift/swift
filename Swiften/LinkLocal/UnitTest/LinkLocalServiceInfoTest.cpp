/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>

using namespace Swift;

class LinkLocalServiceInfoTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LinkLocalServiceInfoTest);
		CPPUNIT_TEST(testGetTXTRecord);
		CPPUNIT_TEST(testCreateFromTXTRecord);
		CPPUNIT_TEST(testCreateFromTXTRecord_InvalidSize);
		CPPUNIT_TEST(testGetTXTRecordCreateFromTXTRecord_RoundTrip);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetTXTRecord() {
			LinkLocalServiceInfo info;
			info.setFirstName("Remko");
			info.setLastName("Tron\xc3\xe7on");
			info.setStatus(LinkLocalServiceInfo::Away);

			CPPUNIT_ASSERT_EQUAL(createByteArray("\x09txtvers=1\x09" + std::string("1st=Remko\x0dlast=Tron\xc3\xe7on\x0bstatus=away")), info.toTXTRecord());
		}

		void testCreateFromTXTRecord() {
			LinkLocalServiceInfo info = LinkLocalServiceInfo::createFromTXTRecord(createByteArray("\x09txtvers=1\x09" + std::string("1st=Remko\x0dlast=Tron\xc3\xe7on\x0bstatus=away")));

			CPPUNIT_ASSERT_EQUAL(std::string("Remko"), info.getFirstName());
			CPPUNIT_ASSERT_EQUAL(std::string("Tron\xc3\xe7on"), info.getLastName());
			CPPUNIT_ASSERT_EQUAL(LinkLocalServiceInfo::Away, info.getStatus());
		}

		void testCreateFromTXTRecord_InvalidSize() {
			LinkLocalServiceInfo info = LinkLocalServiceInfo::createFromTXTRecord(createByteArray("\x10last=a"));

			CPPUNIT_ASSERT_EQUAL(std::string("a"), info.getLastName());
		}

		void testGetTXTRecordCreateFromTXTRecord_RoundTrip() {
			LinkLocalServiceInfo info;
			info.setFirstName("Remko");
			info.setLastName("Tron\xc3\xe7on");
			info.setEMail("remko-email@swift.im");
			info.setJID(JID("remko-jid@swift.im"));
			info.setMessage("I'm busy");
			info.setNick("el-tramo");
			info.setStatus(LinkLocalServiceInfo::DND);
			info.setPort(1234);

			LinkLocalServiceInfo info2 = LinkLocalServiceInfo::createFromTXTRecord(info.toTXTRecord());
			CPPUNIT_ASSERT_EQUAL(info.getFirstName(), info2.getFirstName());
			CPPUNIT_ASSERT_EQUAL(info.getLastName(), info2.getLastName());
			CPPUNIT_ASSERT_EQUAL(info.getEMail(), info2.getEMail());
			CPPUNIT_ASSERT_EQUAL(info.getJID(), info2.getJID());
			CPPUNIT_ASSERT_EQUAL(info.getMessage(), info2.getMessage());
			CPPUNIT_ASSERT_EQUAL(info.getNick(), info2.getNick());
			CPPUNIT_ASSERT(info.getStatus() == info2.getStatus());
			CPPUNIT_ASSERT(info.getPort() == info2.getPort());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkLocalServiceInfoTest);
