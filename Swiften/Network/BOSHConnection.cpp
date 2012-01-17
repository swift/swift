/*
 * Copyright (c) 2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/BOSHConnection.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <string>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Parser/BOSHBodyExtractor.h>

namespace Swift {

BOSHConnection::BOSHConnection(const URL& boshURL, Connector::ref connector, XMLParserFactory* parserFactory)
	: boshURL_(boshURL),
	  connector_(connector),
	  parserFactory_(parserFactory),
	  sid_(),
	  waitingForStartResponse_(false),
	  pending_(false),
	  connectionReady_(false)
{
}

BOSHConnection::~BOSHConnection() {
	cancelConnector();
	if (connection_) {
		connection_->onDataRead.disconnect(boost::bind(&BOSHConnection::handleDataRead, shared_from_this(), _1));
		connection_->onDisconnected.disconnect(boost::bind(&BOSHConnection::handleDisconnected, shared_from_this(), _1));
	}
	disconnect();
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

void BOSHConnection::disconnect() {
	cancelConnector();
	if(connection_) {
		connection_->disconnect();
		sid_ = "";
	}
}

void BOSHConnection::restartStream() {
	write(createSafeByteArray(""), true, false);
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

	header	<< "POST /" << boshURL.getPath() << " HTTP/1.1\r\n"
			<< "Host: " << boshURL.getHost() << ":" << boshURL.getPort() << "\r\n"
		/*<< "Accept-Encoding: deflate\r\n"*/
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
	connection_->write(safeHeader);
	pending_ = true;

	SWIFT_LOG(debug) << "write data: " << safeByteArrayToString(safeHeader) << std::endl;
}

void BOSHConnection::handleConnectFinished(Connection::ref connection) {
	cancelConnector();
	connectionReady_ = connection;
	if (connectionReady_) {
		connection_ = connection;
		connection_->onDataRead.connect(boost::bind(&BOSHConnection::handleDataRead, shared_from_this(), _1));
		connection_->onDisconnected.connect(boost::bind(&BOSHConnection::handleDisconnected, shared_from_this(), _1));
	}
	onConnectFinished(!connectionReady_);
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
			/*<< " ack='0'" FIXME: support acks */
			<< " xml:lang='en'"
			<< " xmlns:xmpp='urn:xmpp:bosh'"
			<< " xmpp:version='1.0'"
			<< " xmlns='http://jabber.org/protocol/httpbind' />";

	std::string contentString = content.str();

	header	<< "POST /" << boshURL_.getPath() << " HTTP/1.1\r\n"
			<< "Host: " << boshURL_.getHost() << ":" << boshURL_.getPort() << "\r\n"
		/*<< "Accept-Encoding: deflate\r\n"*/
			<< "Content-Type: text/xml; charset=utf-8\r\n"
			<< "Content-Length: " << contentString.size() << "\r\n\r\n"
			<< contentString;
	
	waitingForStartResponse_ = true;
	SafeByteArray safeHeader = createSafeByteArray(header.str());
	onBOSHDataWritten(safeHeader);
	connection_->write(safeHeader);
	SWIFT_LOG(debug) << "write stream header: " << safeByteArrayToString(safeHeader) << std::endl;
}

void BOSHConnection::handleDataRead(boost::shared_ptr<SafeByteArray> data) {
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
			onSessionTerminated(errorType == BOSHError::NoError ? boost::shared_ptr<BOSHError>() : boost::make_shared<BOSHError>(errorType));
		}
		buffer_.clear();
		if (waitingForStartResponse_) {
			waitingForStartResponse_ = false;
			sid_ = parser.getBody()->attributes.getAttribute("sid");
			std::string requestsString = parser.getBody()->attributes.getAttribute("requests");
			int requests = 2;
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
	onDisconnected(error);
	sid_ = "";
	connectionReady_ = false;
}


bool BOSHConnection::isReadyToSend() {
	/* Without pipelining you need to not send more without first receiving the response */
	/* With pipelining you can. Assuming we can't, here */
	return connectionReady_ && !pending_ && !waitingForStartResponse_ && !sid_.empty();
}

}
