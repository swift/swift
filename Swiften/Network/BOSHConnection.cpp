/*
 * Copyright (c) 2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2011-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/BOSHConnection.h>

#include <string>
#include <thread>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Parser/BOSHBodyExtractor.h>
#include <Swiften/StreamStack/DummyStreamLayer.h>
#include <Swiften/StreamStack/TLSLayer.h>
#include <Swiften/TLS/TLSContext.h>
#include <Swiften/TLS/TLSOptions.h>

namespace Swift {

BOSHConnection::BOSHConnection(const URL& boshURL, Connector::ref connector, XMLParserFactory* parserFactory, TLSContextFactory* tlsContextFactory, const TLSOptions& tlsOptions)
    : boshURL_(boshURL),
      connector_(connector),
      parserFactory_(parserFactory),
      sid_(),
      waitingForStartResponse_(false),
        rid_(~0ULL),
      pending_(false),
      connectionReady_(false)
{
    if (boshURL_.getScheme() == "https") {
        tlsLayer_ = std::make_shared<TLSLayer>(tlsContextFactory, tlsOptions);
        // The following dummyLayer_ is needed as the TLSLayer will pass the decrypted data to its parent layer.
        // The dummyLayer_ will serve as the parent layer.
        dummyLayer_ = std::make_shared<DummyStreamLayer>(tlsLayer_.get());
    }
}

BOSHConnection::~BOSHConnection() {
    cancelConnector();
    if (connection_) {
        connection_->onDataRead.disconnect(boost::bind(&BOSHConnection::handleDataRead, shared_from_this(), _1));
        connection_->onDisconnected.disconnect(boost::bind(&BOSHConnection::handleDisconnected, shared_from_this(), _1));
    }
    BOSHConnection::disconnect();
}

void BOSHConnection::connect() {
    connector_->onConnectFinished.connect(boost::bind(&BOSHConnection::handleConnectFinished, shared_from_this(), _1));
    connector_->start();
}

void BOSHConnection::cancelConnector() {
    if (connector_) {
        connector_->onConnectFinished.disconnect(boost::bind(&BOSHConnection::handleConnectFinished, shared_from_this(), _1));
        connector_->stop();
        connector_.reset();
    }
}

void BOSHConnection::handleTLSConnected() {
    SWIFT_LOG(debug) << std::endl;
    onConnectFinished(false);
}

void BOSHConnection::handleTLSApplicationDataRead(const SafeByteArray& data) {
    SWIFT_LOG(debug) << std::endl;
    handleDataRead(std::make_shared<SafeByteArray>(data));
}

void BOSHConnection::handleTLSNetowrkDataWriteRequest(const SafeByteArray& data) {
    SWIFT_LOG(debug) << std::endl;
    connection_->write(data);
}

void BOSHConnection::handleRawDataRead(std::shared_ptr<SafeByteArray> data) {
    SWIFT_LOG(debug) << std::endl;
    tlsLayer_->handleDataRead(*data.get());
}

void BOSHConnection::handleTLSError(std::shared_ptr<TLSError> /* error */) {

}

void BOSHConnection::writeData(const SafeByteArray& data) {
    if (tlsLayer_) {
        tlsLayer_->writeData(data);
    }
    else {
        connection_->write(data);
    }
}

void BOSHConnection::disconnect() {
    if (connection_) {
        connection_->disconnect();
        sid_ = "";
    }
    else {
        /* handleDisconnected takes care of the connector_ as well */
        handleDisconnected(boost::optional<Connection::Error>());
    }
}

void BOSHConnection::restartStream() {
    write(createSafeByteArray(""), true, false);
}

bool BOSHConnection::setClientCertificate(CertificateWithKey::ref cert) {
    if (tlsLayer_) {
        SWIFT_LOG(debug) << "set client certificate" << std::endl;
        return tlsLayer_->setClientCertificate(cert);
    }
    else {
        return false;
    }
}

Certificate::ref BOSHConnection::getPeerCertificate() const {
    Certificate::ref peerCertificate;
    if (tlsLayer_) {
        peerCertificate = tlsLayer_->getPeerCertificate();
    }
    return peerCertificate;
}

std::vector<Certificate::ref> BOSHConnection::getPeerCertificateChain() const {
    std::vector<Certificate::ref> peerCertificateChain;
    if (tlsLayer_) {
        peerCertificateChain = tlsLayer_->getPeerCertificateChain();
    }
    return peerCertificateChain;
}

CertificateVerificationError::ref BOSHConnection::getPeerCertificateVerificationError() const {
    CertificateVerificationError::ref verificationError;
    if (tlsLayer_) {
        verificationError = tlsLayer_->getPeerCertificateVerificationError();
    }
    return verificationError;
}

void BOSHConnection::terminateStream() {
    write(createSafeByteArray(""), false, true);
}


void BOSHConnection::write(const SafeByteArray& data) {
    write(data, false, false);
}

std::pair<SafeByteArray, size_t> BOSHConnection::createHTTPRequest(const SafeByteArray& data, bool streamRestart, bool terminate, unsigned long long rid, const std::string& sid, const URL& boshURL) {
    size_t size;
    std::stringstream content;
    SafeByteArray contentTail = createSafeByteArray("</body>");
    std::stringstream header;

    content << "<body rid='" << rid << "' sid='" << sid << "'";
    if (streamRestart) {
        content << " xmpp:restart='true' xmlns:xmpp='urn:xmpp:xbosh'";
    }
    if (terminate) {
        content << " type='terminate'";
    }
    content << " xmlns='http://jabber.org/protocol/httpbind'>";

    SafeByteArray safeContent = createSafeByteArray(content.str());
    safeContent.insert(safeContent.end(), data.begin(), data.end());
    safeContent.insert(safeContent.end(), contentTail.begin(), contentTail.end());

    size = safeContent.size();

    header    << "POST " << boshURL.getPath() << " HTTP/1.1\r\n"
            << "Host: " << boshURL.getHost();
    if (boshURL.getPort()) {
            header << ":" << *boshURL.getPort();
    }
    header    << "\r\n"
        // << "Accept-Encoding: deflate\r\n"
            << "Content-Type: text/xml; charset=utf-8\r\n"
            << "Content-Length: " << size << "\r\n\r\n";

    SafeByteArray safeHeader = createSafeByteArray(header.str());
    safeHeader.insert(safeHeader.end(), safeContent.begin(), safeContent.end());

    return std::pair<SafeByteArray, size_t>(safeHeader, size);
}

void BOSHConnection::write(const SafeByteArray& data, bool streamRestart, bool terminate) {
    assert(connectionReady_);
    assert(!sid_.empty());

    SafeByteArray safeHeader = createHTTPRequest(data, streamRestart, terminate, rid_, sid_, boshURL_).first;

    onBOSHDataWritten(safeHeader);
    writeData(safeHeader);
    pending_ = true;

    SWIFT_LOG(debug) << "write data: " << safeByteArrayToString(safeHeader) << std::endl;
}

void BOSHConnection::handleConnectFinished(Connection::ref connection) {
    cancelConnector();
    connectionReady_ = !!connection;
    if (connectionReady_) {
        connection_ = connection;
        if (tlsLayer_) {
            connection_->onDataRead.connect(boost::bind(&BOSHConnection::handleRawDataRead, shared_from_this(), _1));
            connection_->onDisconnected.connect(boost::bind(&BOSHConnection::handleDisconnected, shared_from_this(), _1));

            tlsLayer_->getContext()->onDataForNetwork.connect(boost::bind(&BOSHConnection::handleTLSNetowrkDataWriteRequest, shared_from_this(), _1));
            tlsLayer_->getContext()->onDataForApplication.connect(boost::bind(&BOSHConnection::handleTLSApplicationDataRead, shared_from_this(), _1));
            tlsLayer_->onConnected.connect(boost::bind(&BOSHConnection::handleTLSConnected, shared_from_this()));
            tlsLayer_->onError.connect(boost::bind(&BOSHConnection::handleTLSError, shared_from_this(), _1));
            tlsLayer_->connect();
        }
        else {
            connection_->onDataRead.connect(boost::bind(&BOSHConnection::handleDataRead, shared_from_this(), _1));
            connection_->onDisconnected.connect(boost::bind(&BOSHConnection::handleDisconnected, shared_from_this(), _1));
        }
    }

    if (!connectionReady_ || !tlsLayer_) {
        onConnectFinished(!connectionReady_);
    }
}

void BOSHConnection::startStream(const std::string& to, unsigned long long rid) {
    assert(connectionReady_);
    // Session Creation Request
    std::stringstream content;
    std::stringstream header;

    content << "<body content='text/xml; charset=utf-8'"
            << " hold='1'"
            << " to='" << to << "'"
            << " rid='" << rid << "'"
            << " ver='1.6'"
            << " wait='60'" /* FIXME: we probably want this configurable*/
            // << " ack='0'" FIXME: support acks
            << " xml:lang='en'"
            << " xmlns:xmpp='urn:xmpp:bosh'"
            << " xmpp:version='1.0'"
            << " xmlns='http://jabber.org/protocol/httpbind' />";

    std::string contentString = content.str();

    header    << "POST " << boshURL_.getPath() << " HTTP/1.1\r\n"
            << "Host: " << boshURL_.getHost();
    if (boshURL_.getPort()) {
        header << ":" << *boshURL_.getPort();
    }
    header << "\r\n"
         // << "Accept-Encoding: deflate\r\n"
            << "Content-Type: text/xml; charset=utf-8\r\n"
            << "Content-Length: " << contentString.size() << "\r\n\r\n"
            << contentString;

    waitingForStartResponse_ = true;
    SafeByteArray safeHeader = createSafeByteArray(header.str());
    onBOSHDataWritten(safeHeader);
    writeData(safeHeader);
    SWIFT_LOG(debug) << "write stream header: " << safeByteArrayToString(safeHeader) << std::endl;
}

void BOSHConnection::handleDataRead(std::shared_ptr<SafeByteArray> data) {
    onBOSHDataRead(*data);
    buffer_ = concat(buffer_, *data);
    std::string response = safeByteArrayToString(buffer_);
    if (response.find("\r\n\r\n") == std::string::npos) {
        onBOSHDataRead(createSafeByteArray("[[Previous read incomplete, pending]]"));
        return;
    }

    std::string httpCode = response.substr(response.find(" ") + 1, 3);
    if (httpCode != "200") {
        onHTTPError(httpCode);
        return;
    }

    BOSHBodyExtractor parser(parserFactory_, createByteArray(response.substr(response.find("\r\n\r\n") + 4)));
    if (parser.getBody()) {
        if (parser.getBody()->attributes.getAttribute("type") == "terminate") {
            BOSHError::Type errorType = parseTerminationCondition(parser.getBody()->attributes.getAttribute("condition"));
            onSessionTerminated(errorType == BOSHError::NoError ? std::shared_ptr<BOSHError>() : std::make_shared<BOSHError>(errorType));
            return;
        }
        buffer_.clear();
        if (waitingForStartResponse_) {
            waitingForStartResponse_ = false;
            sid_ = parser.getBody()->attributes.getAttribute("sid");
            std::string requestsString = parser.getBody()->attributes.getAttribute("requests");
            size_t requests = 2;
            if (!requestsString.empty()) {
                try {
                    requests = boost::lexical_cast<size_t>(requestsString);
                }
                catch (const boost::bad_lexical_cast&) {
                }
            }
            onSessionStarted(sid_, requests);
        }
        SafeByteArray payload = createSafeByteArray(parser.getBody()->content);
        /* Say we're good to go again, so don't add anything after here in the method */
        pending_ = false;
        onXMPPDataRead(payload);
    }

}

BOSHError::Type BOSHConnection::parseTerminationCondition(const std::string& text) {
    BOSHError::Type condition = BOSHError::UndefinedCondition;
    if (text == "bad-request") {
        condition = BOSHError::BadRequest;
    }
    else if (text == "host-gone") {
        condition = BOSHError::HostGone;
    }
    else if (text == "host-unknown") {
        condition = BOSHError::HostUnknown;
    }
    else if (text == "improper-addressing") {
        condition = BOSHError::ImproperAddressing;
    }
    else if (text == "internal-server-error") {
        condition = BOSHError::InternalServerError;
    }
    else if (text == "item-not-found") {
        condition = BOSHError::ItemNotFound;
    }
    else if (text == "other-request") {
        condition = BOSHError::OtherRequest;
    }
    else if (text == "policy-violation") {
        condition = BOSHError::PolicyViolation;
    }
    else if (text == "remote-connection-failed") {
        condition = BOSHError::RemoteConnectionFailed;
    }
    else if (text == "remote-stream-error") {
        condition = BOSHError::RemoteStreamError;
    }
    else if (text == "see-other-uri") {
        condition = BOSHError::SeeOtherURI;
    }
    else if (text == "system-shutdown") {
        condition = BOSHError::SystemShutdown;
    }
    else if (text == "") {
        condition = BOSHError::NoError;
    }
    return condition;
}

const std::string& BOSHConnection::getSID() {
    return sid_;
}

void BOSHConnection::setRID(unsigned long long rid) {
    rid_ = rid;
}

void BOSHConnection::setSID(const std::string& sid) {
    sid_ = sid;
}

void BOSHConnection::handleDisconnected(const boost::optional<Connection::Error>& error) {
    cancelConnector();
    onDisconnected(error ? true : false);
    sid_ = "";
    connectionReady_ = false;
}


bool BOSHConnection::isReadyToSend() {
    /* Without pipelining you need to not send more without first receiving the response */
    /* With pipelining you can. Assuming we can't, here */
    return connectionReady_ && !pending_ && !waitingForStartResponse_ && !sid_.empty();
}

}
