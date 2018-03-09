/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <boost/variant.hpp>

#include <gtest/gtest.h>

#include <Swiften/Base/Log.h>
#include <Swiften/TLS/CertificateFactory.h>
#include <Swiften/TLS/PlatformTLSFactories.h>
#include <Swiften/TLS/TLSContext.h>
#include <Swiften/TLS/TLSContextFactory.h>
#include <Swiften/TLS/TLSOptions.h>

using namespace Swift;
namespace {


std::map<std::string, std::string> certificatePEM = {
    {"montague.example",
R"(-----BEGIN CERTIFICATE-----
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
)"},
    {"capulet.example",
R"(-----BEGIN CERTIFICATE-----
MIIEsDCCApgCCQDUGdmqHfGngTANBgkqhkiG9w0BAQsFADAaMRgwFgYDVQQDDA9j
YXB1bGV0LmV4YW1wbGUwHhcNMTgwMTE4MTY1NjEyWhcNNDUwNjA0MTY1NjEyWjAa
MRgwFgYDVQQDDA9jYXB1bGV0LmV4YW1wbGUwggIiMA0GCSqGSIb3DQEBAQUAA4IC
DwAwggIKAoICAQCgA/CpV7UGqUdjhEVoMjFIZ6Ca/C7utrVPAqBvE14eiurIhQrQ
AmYeC9zA4/uXCdqkGq/a/RbF3OOCKLRUejCcynb0EnxiHxHa48ZsvLbKCK6guoXE
pWnaZsmRpvJrBB2z6ohmxRuaDuc5CJT+Oq8AFPp3StTAFDo3Cju3fsGZPkNpCGbj
ELwk4ok9INtEuTrMEmHZTD+VfjpXauUfN4ygKaPXJanCuxuifyin403BWgd8igkD
oSCWxoDFMy3HGbh/NU+nJPJ23FxVC39RdDG3elvjNFpSZnALbiMnyor7YjF5TN93
1ZWwn3VTnl7DnFquEbKbYyVFUzRII8Cd1TzKOL48rVLrCAqMXwm6YFtST6qPb9TZ
0SX8qZGCwBfHV5OeS0ZgiBMMlmUPYcw9MlyvZnYyDPCOoPWmhEqd2gQzn//7hzb5
mriCEyfcMzAqohylBNHXUVZTx5KcazJz6oOYdWEs1jfSXNKefgSWlgeoG2fgTXPN
1OkQVS+FOiI0VCAIwR+vxhG3hVTz3kzXWvEt7M51faaHWWlnSzOrbSuj3f0ibS5J
cj6ClyzOQRQDwzofyZ7oPWh6No/XkepVIn3HTTlnj1/8e6VsH+EBKSzoX2XvWPkO
GAZEGHKiKh944u6d6mW37BPD2oKyusP3uPL5j2Fdm+m0HkP3/7yw+5EFVQIDAQAB
MA0GCSqGSIb3DQEBCwUAA4ICAQCfCGK4iDo8H0i12wBC0+6GB9NBmgn8P09AUN0l
tQjZPaqZCtLtBBqOrAaZQBQMfI0QAm5/h/VkhMvz5HQjqmQhI2drSDOmarYzOGgY
yVItkEwtLEVhiGoeJ+L3Smosoqq6QFjcDzH7UzPTrJNspnY+09+m53pJggh41zzt
1TOU+QasaB5oGopFaFUUlwjHAc7FpZgEd+tV6j0cpdT3GabVkkoLg01Z+0mqkpLD
OjRBKJX8XvZ38VESsy3gWpcXnDq03n+8OgZo4R9SEcyyxjWbyb+qg2dzbQvRB2Va
QNoXp5EzemXvFSulhR+TfDk2K1h45BurikRQxDi8LpBTUsCMwiqXdem68HOlTwLi
/kMWbnVBcdurYcWVSwlJU4EJcTEdk51JStO1V0nAA0nCwn/iEhY8I6BitnrcCJ5e
4CGVWr+zAm8DBjaFMTzy46Q5NcT0hwnHGN6T6l4aMcRggIK9anRbXCn6dSzma1pd
R5N/Do00FTpyZGcUlVPnSlIfZVl7y/9XEO1n6xDJURrefL1JrM7UMyB17jA8HMq3
S05kF7XRpludRB4QkAJt5BNNv6BPP7HPIKyR/rq94ONvzVPAo7uASyFE2sMBsfwP
pXAI1LVolPCoUC13jEkKdmc8kMSxU+XtsvFryNhkfQtZfSg+nBRFYptFE7GrZ9WY
GMSL4g==
-----END CERTIFICATE-----
)"}};
std::map<std::string, std::string> privateKeyPEM = {
    {"montague.example",
R"(-----BEGIN PRIVATE KEY-----
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
5IMXuORWuoq0gXiCdEyko+saXsyWlqw=
-----END PRIVATE KEY-----
)"},{"capulet.example",
R"(-----BEGIN PRIVATE KEY-----
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
dwiNVl130YWaJLjiA9Poc2llKtypfQ8=
-----END PRIVATE KEY-----
)"}};

auto montagueEncryptedPEM = R"(-----BEGIN RSA PRIVATE KEY-----
Proc-Type: 4,ENCRYPTED
DEK-Info: AES-256-CBC,6B0A69362920334824667C1B6207279B

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
-----END RSA PRIVATE KEY-----
)";

auto dhParamsOpenSslDer1024 = R"(-----BEGIN DH PARAMETERS-----
MIGHAoGBANjw4f5+gu8b8X8O6ALyJA1tH9TQTWZEI3YjUKQ1m0dVBMJ6XDC7FLJn
gqE4hIGcm1FAWwIiuo0uMufqyVwFT2c+G8j4JHWK5z1tEP+GaqiO34N0cUo86qHp
THSkJN7LuHbYRqI9fHWDZocW/5yAsDq5RPUCjFZAoh1BWdfDFfrjAgEC
-----END DH PARAMETERS-----
)";
auto dhParamsOpenSslDer2048 = R"(-----BEGIN DH PARAMETERS-----
MIIBCAKCAQEA0Q6vD5qtrh3XEkVkbN29ord/k3sgo2Q3PiFmyFt89qqVbebEzYmt
t8DwaFGMcGlyKs4Lb1s7vocm9y3M9C0FZm85Muvv8WCbLZVZ+wfEtMibOjgRoDqt
p7Qqe7/iPgMVrSjWegVkg3V8K8dnPpohPClM0yOe4NpBjSVNgBVJRpEtH8gFiCor
H7hw63HpN/MgFdkjZNeCN+erv8p673xH8LrN98gQpkdQ9vCqYt1dHxF2XZcxBp8x
XganwPeGgQosofkA6nVB70hyjwjEyxnHJZIMlx6DPXWC7X6ed0SazgH0sQNdACvG
uU1zHCVIv6/f0adKrJg0s1jrM3qWZ6HmUwIBAg==
-----END DH PARAMETERS-----
)";

auto createTLSContext = [](TLSContext::Mode mode) {
    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    auto tlsContextFactory = tlsFactories->getTLSContextFactory();

    auto tlsContext = std::unique_ptr<TLSContext>(tlsContextFactory->createTLSContext({}, mode));
    return tlsContext;
};

// This connects a TLSContext to another TLSContext
class ClientServerConnector {
    public:
        ClientServerConnector(TLSContext* client, TLSContext* server) : clientContext_(client), serverContext_(server) {
            connections_.push_back(clientContext_->onDataForNetwork.connect([&](const SafeByteArray& data) {
                serverContext_->handleDataFromNetwork(data);
            }));
            connections_.push_back(serverContext_->onDataForNetwork.connect([&](const SafeByteArray& data) {
                clientContext_->handleDataFromNetwork(data);
            }));
        }

    private:
        TLSContext* clientContext_;
        TLSContext* serverContext_;
        std::vector<boost::signals2::connection> connections_;
};

struct TLSDataForNetwork {
    SafeByteArray data;
};

struct TLSDataForApplication {
    SafeByteArray data;
};

struct TLSFault {
    std::shared_ptr<Swift::TLSError> error;
};

struct TLSConnected {
    std::vector<Certificate::ref> chain;
};

struct TLSServerNameRequested {
    std::string name;
};

using TLSEvent = boost::variant<TLSDataForNetwork, TLSDataForApplication, TLSFault, TLSConnected, TLSServerNameRequested>;

class TLSEventToSafeByteArrayVisitor : public boost::static_visitor<SafeByteArray> {
    public:
        SafeByteArray operator()(const TLSDataForNetwork& tlsData) const {
            return tlsData.data;
        }

        SafeByteArray operator()(const TLSDataForApplication& tlsData) const {
            return tlsData.data;
        }

        SafeByteArray operator()(const TLSFault&) const {
            return createSafeByteArray("");
        }

        SafeByteArray operator()(const TLSConnected&) const {
            return createSafeByteArray("");
        }

        SafeByteArray operator()(const TLSServerNameRequested&) const {
            return createSafeByteArray("");
        }

};

class TLSEventToStringVisitor : public boost::static_visitor<std::string> {
    public:
        std::string operator()(const TLSDataForNetwork& event) const {
            return std::string("TLSDataForNetwork(") + "size: " + std::to_string(event.data.size()) + ")";
        }

        std::string operator()(const TLSDataForApplication& event) const {
            return std::string("TLSDataForApplication(") + "size: " + std::to_string(event.data.size()) + ")";
        }

        std::string operator()(const TLSFault&) const {
            return "TLSFault()";
        }

        std::string operator()(const TLSConnected& event) const {
            std::string certificates;
            for (auto cert : event.chain) {
                certificates += "\t" + cert->getSubjectName() + "\n";
            }
            return std::string("TLSConnected()") + "\n" + certificates;
        }

        std::string operator()(const TLSServerNameRequested& event) const {
            return std::string("TLSServerNameRequested(") + "name: " + event.name + ")";
        }
};

class TLSClientServerEventHistory {
    public:
        TLSClientServerEventHistory(TLSContext* client, TLSContext* server) {
            connectContext(std::string("client"), client);
            connectContext(std::string("server"), server);
        }

        __attribute__((unused))
        void print() {
            auto count = 0;
            std::cout << "\n";
            for (auto event : events) {
                if (event.first == "server") {
                    std::cout << std::string(80, ' ');
                }
                std::cout << count << ". ";
                std::cout << event.first << " : " << boost::apply_visitor(TLSEventToStringVisitor(), event.second) << std::endl;
                count++;
            }
        }

    private:
        void connectContext(const std::string& name, TLSContext* context) {
            connections_.push_back(context->onDataForNetwork.connect([=](const SafeByteArray& data) {
                events.push_back(std::pair<std::string, TLSEvent>(name, TLSDataForNetwork{data}));
            }));
            connections_.push_back(context->onDataForApplication.connect([=](const SafeByteArray& data) {
                events.push_back(std::pair<std::string, TLSEvent>(name, TLSDataForApplication{data}));
            }));
            connections_.push_back(context->onError.connect([=](std::shared_ptr<Swift::TLSError> error) {
                events.push_back(std::pair<std::string, TLSEvent>(name, TLSFault{error}));
            }));
            connections_.push_back(context->onConnected.connect([=](){
                events.push_back(std::pair<std::string, TLSEvent>(name, TLSConnected{context->getPeerCertificateChain()}));
            }));
        }

    public:
        std::vector<std::pair<std::string, TLSEvent>> events;

    private:
        std::vector<boost::signals2::connection> connections_;
};

}

TEST(ClientServerTest, testInitAndFreeContext) {
    auto tlsClientContext = createTLSContext(TLSContext::Mode::Client);
    auto tlsServerContext = createTLSContext(TLSContext::Mode::Server);
}

TEST(ClientServerTest, testServerSetPrivateKey) {
    auto tlsServerContext = createTLSContext(TLSContext::Mode::Server);
}

TEST(ClientServerTest, testServerSetCertificateChain) {
    auto tlsServerContext = createTLSContext(TLSContext::Mode::Server);
}

TEST(ClientServerTest, testClientServerBasicCommunicationWithCertificateMissing) {
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(privateKeyPEM["capulet.example"]));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(true, serverContext->setPrivateKey(privateKey));

    serverContext->accept();
    clientContext->connect();

    ASSERT_EQ("server", events.events[1].first);
    ASSERT_EQ("TLSFault()", boost::apply_visitor(TLSEventToStringVisitor(), events.events[1].second));
}

TEST(ClientServerTest, testClientServerBasicCommunicationWithPrivateKeyMissing) {
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["capulet.example"]))));

    serverContext->accept();
    clientContext->connect();

    ASSERT_EQ("server", events.events[1].first);
    ASSERT_EQ("TLSFault()", boost::apply_visitor(TLSEventToStringVisitor(), events.events[1].second));
}

TEST(ClientServerTest, testWrongPrivateKeyAfterCertificate) {
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["capulet.example"]))));

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(privateKeyPEM["montague.example"]));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(false, serverContext->setPrivateKey(privateKey));

    serverContext->accept();
    clientContext->connect();
}

TEST(ClientServerTest, testWrongCertificateAfterPrivateKey) {
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(privateKeyPEM["montague.example"]));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(true, serverContext->setPrivateKey(privateKey));

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["capulet.example"]))));

    serverContext->accept();
    clientContext->connect();

    ASSERT_EQ("server", events.events[1].first);
    ASSERT_EQ("TLSFault()", boost::apply_visitor(TLSEventToStringVisitor(), events.events[1].second));
}

TEST(ClientServerTest, testClientServerBasicCommunication) {
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["capulet.example"]))));

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(privateKeyPEM["capulet.example"]));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(true, serverContext->setPrivateKey(privateKey));

    serverContext->accept();
    clientContext->connect();

    clientContext->handleDataFromApplication(createSafeByteArray("This is a test message from the client."));
    serverContext->handleDataFromApplication(createSafeByteArray("This is a test message from the server."));

    ASSERT_EQ(safeByteArrayToString(createSafeByteArray("This is a test message from the client.")), safeByteArrayToString(boost::apply_visitor(TLSEventToSafeByteArrayVisitor(), std::find_if(events.events.begin(), events.events.end(), [](std::pair<std::string, TLSEvent>& event){
        return event.first == "server" && (event.second.type() == typeid(TLSDataForApplication));
    })->second)));
    ASSERT_EQ(safeByteArrayToString(createSafeByteArray("This is a test message from the server.")), safeByteArrayToString(boost::apply_visitor(TLSEventToSafeByteArrayVisitor(), std::find_if(events.events.begin(), events.events.end(), [](std::pair<std::string, TLSEvent>& event){
        return event.first == "client" && (event.second.type() == typeid(TLSDataForApplication));
    })->second)));
}

TEST(ClientServerTest, testClientServerBasicCommunicationEncryptedPrivateKeyRightPassword) {
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["montague.example"]))));

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(montagueEncryptedPEM), createSafeByteArray("test"));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(true, serverContext->setPrivateKey(privateKey));

    serverContext->accept();
    clientContext->connect();

    clientContext->handleDataFromApplication(createSafeByteArray("This is a test message from the client."));
    serverContext->handleDataFromApplication(createSafeByteArray("This is a test message from the server."));

    ASSERT_EQ(safeByteArrayToString(createSafeByteArray("This is a test message from the client.")), safeByteArrayToString(boost::apply_visitor(TLSEventToSafeByteArrayVisitor(), std::find_if(events.events.begin(), events.events.end(), [](std::pair<std::string, TLSEvent>& event){
        return event.first == "server" && (event.second.type() == typeid(TLSDataForApplication));
    })->second)));
    ASSERT_EQ(safeByteArrayToString(createSafeByteArray("This is a test message from the server.")), safeByteArrayToString(boost::apply_visitor(TLSEventToSafeByteArrayVisitor(), std::find_if(events.events.begin(), events.events.end(), [](std::pair<std::string, TLSEvent>& event){
        return event.first == "client" && (event.second.type() == typeid(TLSDataForApplication));
    })->second)));
}

TEST(ClientServerTest, testSettingPrivateKeyWithWrongPassword) {
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["montague.example"]))));

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(montagueEncryptedPEM), createSafeByteArray("foo"));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(false, serverContext->setPrivateKey(privateKey));
}

TEST(ClientServerTest, testSettingPrivateKeyWithoutRequiredPassword) {
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["montague.example"]))));

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(montagueEncryptedPEM));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(false, serverContext->setPrivateKey(privateKey));
}

TEST(ClientServerTest, testClientServerSNIRequestedHostAvailable) {
    auto tlsFactories = std::make_shared<PlatformTLSFactories>();
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    serverContext->onServerNameRequested.connect([&](const std::string& requestedName) {
        if (certificatePEM.find(requestedName) != certificatePEM.end() && privateKeyPEM.find(requestedName) != privateKeyPEM.end()) {
            auto certChain = tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM[requestedName]));
            ASSERT_EQ(true, serverContext->setCertificateChain(certChain));

            auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(privateKeyPEM[requestedName]));
            ASSERT_NE(nullptr, privateKey.get());
            ASSERT_EQ(true, serverContext->setPrivateKey(privateKey));
        }
    });

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["capulet.example"]))));

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(privateKeyPEM["capulet.example"]));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(true, serverContext->setPrivateKey(privateKey));

    serverContext->accept();
    clientContext->connect("montague.example");

    clientContext->handleDataFromApplication(createSafeByteArray("This is a test message from the client."));
    serverContext->handleDataFromApplication(createSafeByteArray("This is a test message from the server."));
    ASSERT_EQ("This is a test message from the client.", safeByteArrayToString(boost::apply_visitor(TLSEventToSafeByteArrayVisitor(), std::find_if(events.events.begin(), events.events.end(), [](std::pair<std::string, TLSEvent>& event){
        return event.first == "server" && (event.second.type() == typeid(TLSDataForApplication));
    })->second)));
    ASSERT_EQ("This is a test message from the server.", safeByteArrayToString(boost::apply_visitor(TLSEventToSafeByteArrayVisitor(), std::find_if(events.events.begin(), events.events.end(), [](std::pair<std::string, TLSEvent>& event){
        return event.first == "client" && (event.second.type() == typeid(TLSDataForApplication));
    })->second)));

    ASSERT_EQ("/CN=montague.example", boost::get<TLSConnected>(events.events[5].second).chain[0]->getSubjectName());
}

TEST(ClientServerTest, testClientServerSNIRequestedHostUnavailable) {
    auto tlsFactories = std::make_shared<PlatformTLSFactories>();
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    serverContext->onServerNameRequested.connect([&](const std::string&) {
        serverContext->setAbortTLSHandshake(true);
    });

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["capulet.example"]))));

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(privateKeyPEM["capulet.example"]));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(true, serverContext->setPrivateKey(privateKey));

    serverContext->accept();
    clientContext->connect("montague.example");

    ASSERT_EQ("server", events.events[1].first);
    ASSERT_EQ("TLSFault()", boost::apply_visitor(TLSEventToStringVisitor(), events.events[1].second));

    ASSERT_EQ("client", events.events[3].first);
    ASSERT_EQ("TLSFault()", boost::apply_visitor(TLSEventToStringVisitor(), events.events[3].second));
}

TEST(ClientServerTest, testClientServerEqualFinishedMessage) {
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["capulet.example"]))));

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(privateKeyPEM["capulet.example"]));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(true, serverContext->setPrivateKey(privateKey));

    serverContext->accept();
    clientContext->connect();

    ASSERT_EQ(serverContext->getPeerFinishMessage(), clientContext->getFinishMessage());
    ASSERT_EQ(clientContext->getPeerFinishMessage(), serverContext->getFinishMessage());
}

TEST(ClientServerTest, testClientServerBasicCommunicationWith2048BitDHParams) {
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["capulet.example"]))));

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(privateKeyPEM["capulet.example"]));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(true, serverContext->setPrivateKey(privateKey));

    ASSERT_EQ(true, serverContext->setDiffieHellmanParameters(tlsFactories->getTLSContextFactory()->convertDHParametersFromPEMToDER(dhParamsOpenSslDer2048)));

    serverContext->accept();
    clientContext->connect();

    clientContext->handleDataFromApplication(createSafeByteArray("This is a test message from the client."));
    serverContext->handleDataFromApplication(createSafeByteArray("This is a test message from the server."));

    ASSERT_EQ(safeByteArrayToString(createSafeByteArray("This is a test message from the client.")), safeByteArrayToString(boost::apply_visitor(TLSEventToSafeByteArrayVisitor(), std::find_if(events.events.begin(), events.events.end(), [](std::pair<std::string, TLSEvent>& event){
        return event.first == "server" && (event.second.type() == typeid(TLSDataForApplication));
    })->second)));
    ASSERT_EQ(safeByteArrayToString(createSafeByteArray("This is a test message from the server.")), safeByteArrayToString(boost::apply_visitor(TLSEventToSafeByteArrayVisitor(), std::find_if(events.events.begin(), events.events.end(), [](std::pair<std::string, TLSEvent>& event){
        return event.first == "client" && (event.second.type() == typeid(TLSDataForApplication));
    })->second)));
}

TEST(ClientServerTest, testClientServerBasicCommunicationWith1024BitDHParams) {
    auto clientContext = createTLSContext(TLSContext::Mode::Client);
    auto serverContext = createTLSContext(TLSContext::Mode::Server);

    TLSClientServerEventHistory events(clientContext.get(), serverContext.get());

    ClientServerConnector connector(clientContext.get(), serverContext.get());

    auto tlsFactories = std::make_shared<PlatformTLSFactories>();

    ASSERT_EQ(true, serverContext->setCertificateChain(tlsFactories->getCertificateFactory()->createCertificateChain(createByteArray(certificatePEM["capulet.example"]))));

    auto privateKey = tlsFactories->getCertificateFactory()->createPrivateKey(createSafeByteArray(privateKeyPEM["capulet.example"]));
    ASSERT_NE(nullptr, privateKey.get());
    ASSERT_EQ(true, serverContext->setPrivateKey(privateKey));

    ASSERT_EQ(true, serverContext->setDiffieHellmanParameters(tlsFactories->getTLSContextFactory()->convertDHParametersFromPEMToDER(dhParamsOpenSslDer1024)));

    serverContext->accept();
    clientContext->connect();

    clientContext->handleDataFromApplication(createSafeByteArray("This is a test message from the client."));
    serverContext->handleDataFromApplication(createSafeByteArray("This is a test message from the server."));

    ASSERT_EQ(safeByteArrayToString(createSafeByteArray("This is a test message from the client.")), safeByteArrayToString(boost::apply_visitor(TLSEventToSafeByteArrayVisitor(), std::find_if(events.events.begin(), events.events.end(), [](std::pair<std::string, TLSEvent>& event){
        return event.first == "server" && (event.second.type() == typeid(TLSDataForApplication));
    })->second)));
    ASSERT_EQ(safeByteArrayToString(createSafeByteArray("This is a test message from the server.")), safeByteArrayToString(boost::apply_visitor(TLSEventToSafeByteArrayVisitor(), std::find_if(events.events.begin(), events.events.end(), [](std::pair<std::string, TLSEvent>& event){
        return event.first == "client" && (event.second.type() == typeid(TLSDataForApplication));
    })->second)));
}
