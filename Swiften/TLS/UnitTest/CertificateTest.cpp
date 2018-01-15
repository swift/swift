/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Platform.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/CertificateFactory.h>
#include <Swiften/TLS/PlatformTLSFactories.h>
#include <Swiften/TLS/SimpleCertificate.h>

using namespace Swift;

class CertificateTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(CertificateTest);
        CPPUNIT_TEST(testGetSHA1Fingerprint);
#ifdef HAVE_OPENSSL
        CPPUNIT_TEST(testParsingPEMCertificate);
#endif
        CPPUNIT_TEST(testParsingPEMPrivateKey);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testGetSHA1Fingerprint() {
            SimpleCertificate::ref testling = std::make_shared<SimpleCertificate>();
            testling->setDER(createByteArray("abcdefg"));

            CPPUNIT_ASSERT_EQUAL(std::string("2f:b5:e1:34:19:fc:89:24:68:65:e7:a3:24:f4:76:ec:62:4e:87:40"), Certificate::getSHA1Fingerprint(testling, std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create()).get()));
        }

        void testParsingPEMCertificate() {
            PlatformTLSFactories tlsFactories;

            auto PEMCertificate = R"(-----BEGIN CERTIFICATE-----
MIIEsjCCApoCCQCbkjlQfUqPtTANBgkqhkiG9w0BAQsFADAbMRkwFwYDVQQDDBBt
b250YWd1ZS5leGFtcGxlMB4XDTE4MDExODE2NTMxMloXDTQ1MDYwNDE2NTMxMlow
GzEZMBcGA1UEAwwQbW9udGFndWUuZXhhbXBsZTCCAiIwDQYJKoZIhvcNAQEBBQAD
ggIPADCCAgoCggIBALAx5xuEYOjDJ9QHMvwRuDzxbHl1zR97SJFPAkmD8xH0sC61
DNRyUvRq6UXb4znhqeqrNuZ9PV47GyK2Dpy/c/MY5NE3m/c+Z1tUnrcqyCkxITIn
jdSI/elc9yhtMXX6lRi68BdPJnj/9+6trW0cWHlKEgnaSTAgCVB+4Dg9mjTIroCa
HLoAKhr2zS7Ihs28aWb0cSvZ+qFgQhr7FGP2kfssTTQYyRPn9uHhtyWH6RqSv5x9
BBGZWN5GtjkJvupcYWLt8ftuQyFpwEeEz5cgtiyKgsfz9CYvS9/OcwdLQr4z5Zq6
eKxsog9GdwIi1Us4KR0X6tKu9ze42iEWF+foEWFP9/bjrVK/tt5lTSKnenk0nA8I
amkG0btNAGihDti3lv60bGpd3B2/uh4gzzT2buXdf0EaybCt52MIr8xkwMU0Tkri
RAYchdS8U8sekWG5Lg9m3L2BDa8/TKS/WflJhGcZeInGQslgIx7fCgO1M7Zr50pO
wSURPXpvqUkXNEBy639UQEUsnBhntEQwZWx/6x6Ma/U5a5dL6qbtEJjlwIvS+nl9
3w26g3DvWydNMCtZIVhgdrl+dZs+Uw5eA3QkHkDTSfYvQk7X5SYL0J5ZxwBvU9r1
ED054+TAEuX2euiRA37xLhxonj8BaKkPQGlAHCLZaZPmNJWkNxElJhMoCfqBAgMB
AAEwDQYJKoZIhvcNAQELBQADggIBAF+FNyW3nVeQeugBMksIhj7EMJl1AEKi0+78
ZPsYX3CDbc/8GRZoTg/EWSiwPCBYc9VsxuKtODEYABCZgk7LnSBYoEauJDKWqkgM
UOKPJI2hu7mIK7FJpjvEZe2MnRRA63oI/NVDJm8T2clrv/vPkY+ppsVl0toC0SpH
/3dF5c65vYI19rTJraRU6kIrrgxFBzxzpn07LGh2rrOCQfy2umTSRMwz3ORAFfmi
+Kek1Dt7c+JVJ0ivCwhhc8MKza3JS2MuDfVWGnXtDLb81Ai0t4tQfLKvZEcgW+lh
Drz9gv22buwncWL/IxtuhzyILtDSDKAYFbhfG6IAQut9BjMgpMnKrBCDlOLJl08K
tgj2h7vUKyNSt3ndcSAtXjr6FD7+xPExJuyn/MgLONGGAZoZHFB4QO90wQaXxMPh
7rnjUtzfLR8qkDmX8ZB4f4VOWpDWo4hBpgjTk0gYfzEkrh+zTqE9reh7CZ1WzwXO
KnIBU2dZOE+XsJe49lW106DLqGzKRuQMUAwFMb7C7Nlg9GKTnvi41o+g6YE+MgxR
uPKu891pCBtnDxZiWPT+7Fa/9UXxdIOTVIHW6utSWiWYbeNwXSmIm2ShfmNfWj9m
x1JgJrFB6daWGR9aDBeDVRhgL6Z35lH7xI62pLJ4o2d2Y/9dUWAJfz5O8opeCyrF
zqCzpwGL
-----END CERTIFICATE-----
)";

            auto certificateChain = tlsFactories.getCertificateFactory()->createCertificateChain(createByteArray(PEMCertificate));
            CPPUNIT_ASSERT_EQUAL(size_t(1), certificateChain.size());

            auto certificate = certificateChain[0];
            CPPUNIT_ASSERT_EQUAL(std::string("/CN=montague.example"), certificate->getSubjectName());
        }

        void testParsingPEMPrivateKey() {
            PlatformTLSFactories tlsFactories;

            auto PEMPrivateKey = R"(-----BEGIN PRIVATE KEY-----
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
5IMXuORWuoq0gXiCdEyko+saXsyWlqw=
-----END PRIVATE KEY-----
)";

            auto privateKey = tlsFactories.getCertificateFactory()->createPrivateKey(createSafeByteArray(PEMPrivateKey));

            CPPUNIT_ASSERT(privateKey != nullptr);
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(CertificateTest);
