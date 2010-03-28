#include "Swiften/StreamStack/OpenSSLLayer.h"

namespace Swift {

OpenSSLLayer::OpenSSLLayer() {
	context_.onDataForNetwork.connect(onWriteData);
	context_.onDataForApplication.connect(onDataRead);
	context_.onConnected.connect(onConnected);
	context_.onError.connect(onError);
}

void OpenSSLLayer::connect() {
	context_.connect();
}

void OpenSSLLayer::writeData(const ByteArray& data) {
	context_.handleDataFromApplication(data);
}

void OpenSSLLayer::handleDataRead(const ByteArray& data) {
	context_.handleDataFromNetwork(data);
}

bool OpenSSLLayer::setClientCertificate(const PKCS12Certificate& certificate) {
	return context_.setClientCertificate(certificate);
}

}
