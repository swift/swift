/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <vector>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/Tristate.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Disco/CapsInfoGenerator.h>
#include <Swiften/Disco/CapsProvider.h>
#include <Swiften/Disco/EntityCapsManager.h>
#include <Swiften/Disco/FeatureOracle.h>
#include <Swiften/Elements/CapsInfo.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Roster/XMPPRosterImpl.h>

using namespace Swift;

class FeatureOracleTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(FeatureOracleTest);
        CPPUNIT_TEST(testMergeAvailableResourcesForFeatures);
        CPPUNIT_TEST(testMostAvailableFileTransferClient);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            crypto_ = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
            dummyStanzaChannel_ = new DummyStanzaChannel();
            xmppRosterImpl_ = new XMPPRosterImpl();
            dummyCapsProvider_ = new DummyCapsProvider();
            entityCapsManager_ = new EntityCapsManager(dummyCapsProvider_, dummyStanzaChannel_);
            presenceOracle_ = new PresenceOracle(dummyStanzaChannel_, xmppRosterImpl_);
            featureOracle_ = new FeatureOracle(entityCapsManager_, presenceOracle_);
        }

        void tearDown() {
            delete featureOracle_;
            delete presenceOracle_;
            delete entityCapsManager_;
            delete dummyCapsProvider_;
            delete xmppRosterImpl_;
            delete dummyStanzaChannel_;
        }

        void simulateIncomingPresence(const JID& from, Presence::Type type, StatusShow::Type status, const DiscoInfo::ref& disco, const std::vector<Payload::ref>& additionalPayloads = {}) {
             auto capsInfo = std::make_shared<CapsInfo>(CapsInfoGenerator("http://example.com", crypto_.get()).generateCapsInfo(*disco.get()));
             dummyCapsProvider_->caps[capsInfo->getVersion()] = disco;

             Presence::ref capsNotifyPresence = std::make_shared<Presence>();
             capsNotifyPresence->setType(type);
             capsNotifyPresence->setFrom(from);
             capsNotifyPresence->setShow(status);
             capsNotifyPresence->addPayload(capsInfo);

             capsNotifyPresence->addPayloads(additionalPayloads);

             xmppRosterImpl_->addContact(from, "Foo", {}, RosterItemPayload::Both);
             dummyStanzaChannel_->onPresenceReceived(capsNotifyPresence);
        }

        DiscoInfo::ref fileTransferSupportingDisco() {
            DiscoInfo::ref discoInfo = std::make_shared<DiscoInfo>();
            discoInfo->addFeature(DiscoInfo::JingleFeature);
            discoInfo->addFeature(DiscoInfo::JingleFTFeature);
            discoInfo->addFeature(DiscoInfo::JingleTransportsS5BFeature);
            discoInfo->addFeature(DiscoInfo::JingleTransportsIBBFeature);
            return discoInfo;
        }

        DiscoInfo::ref noFileTransferSupportingDisco() {
            DiscoInfo::ref discoInfo = std::make_shared<DiscoInfo>();
            discoInfo->addFeature(DiscoInfo::JingleFeature);
            return discoInfo;
        }

        void testMergeAvailableResourcesForFeatures() {
            CPPUNIT_ASSERT_EQUAL(No, featureOracle_->isFileTransferSupported(baseJID));

            simulateIncomingPresence(noFileTransferJID, Presence::Available, StatusShow::Online, noFileTransferSupportingDisco());

            CPPUNIT_ASSERT_EQUAL(size_t(1), presenceOracle_->getAllPresence(baseJID).size());
            CPPUNIT_ASSERT_EQUAL(No, featureOracle_->isFileTransferSupported(baseJID));

            simulateIncomingPresence(fileTransferJID, Presence::Available, StatusShow::Online, fileTransferSupportingDisco());

            CPPUNIT_ASSERT_EQUAL(size_t(2), presenceOracle_->getAllPresence(baseJID).size());
            CPPUNIT_ASSERT_EQUAL(Maybe, featureOracle_->isFileTransferSupported(baseJID));

            simulateIncomingPresence(noFileTransferJID, Presence::Unavailable, StatusShow::None, noFileTransferSupportingDisco());

            CPPUNIT_ASSERT_EQUAL(size_t(1), presenceOracle_->getAllPresence(baseJID).size());
            CPPUNIT_ASSERT_EQUAL(Yes, featureOracle_->isFileTransferSupported(baseJID));

            simulateIncomingPresence(fileTransferJID, Presence::Unavailable, StatusShow::None, fileTransferSupportingDisco());

            CPPUNIT_ASSERT_EQUAL(size_t(1), presenceOracle_->getAllPresence(baseJID).size());
            CPPUNIT_ASSERT_EQUAL(No, featureOracle_->isFileTransferSupported(baseJID));
        }

        void testMostAvailableFileTransferClient() {
            simulateIncomingPresence(fileTransferJID, Presence::Available, StatusShow::DND, fileTransferSupportingDisco());

            CPPUNIT_ASSERT_EQUAL(fileTransferJID, featureOracle_->getMostAvailableClientForFileTrasfer(baseJID));

            simulateIncomingPresence(noFileTransferJID, Presence::Available, StatusShow::Online, noFileTransferSupportingDisco());

            CPPUNIT_ASSERT_EQUAL(fileTransferJID, featureOracle_->getMostAvailableClientForFileTrasfer(baseJID));

            auto moreAvailableJID = baseJID.withResource("moreAvailableFt");
            simulateIncomingPresence(moreAvailableJID, Presence::Available, StatusShow::Online, fileTransferSupportingDisco());

            CPPUNIT_ASSERT_EQUAL(moreAvailableJID, featureOracle_->getMostAvailableClientForFileTrasfer(baseJID));
        }

    private:
        struct DummyCapsProvider : public CapsProvider {
            virtual DiscoInfo::ref getCaps(const std::string& hash) const {
                std::map<std::string, DiscoInfo::ref>::const_iterator i = caps.find(hash);
                if (i != caps.end()) {
                    return i->second;
                }
                return DiscoInfo::ref();
            }

            std::map<std::string, DiscoInfo::ref> caps;
        };

    private:
        JID baseJID = "test@example.com";
        JID fileTransferJID = baseJID.withResource("fileTransfer");
        JID noFileTransferJID = baseJID.withResource("noFileTransfer");

        std::shared_ptr<CryptoProvider> crypto_;
        DummyCapsProvider* dummyCapsProvider_;
        DummyStanzaChannel* dummyStanzaChannel_;
        EntityCapsManager* entityCapsManager_;
        FeatureOracle* featureOracle_;
        PresenceOracle* presenceOracle_;
        XMPPRosterImpl* xmppRosterImpl_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(FeatureOracleTest);
