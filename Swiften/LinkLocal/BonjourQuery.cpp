#include "Swiften/LinkLocal/BonjourQuery.h"
#include "Swiften/LinkLocal/BonjourQuerier.h"

namespace Swift {

BonjourQuery::BonjourQuery(boost::shared_ptr<BonjourQuerier> q) : querier(q), sdRef(0) {
}

BonjourQuery::~BonjourQuery() {
	DNSServiceRefDeallocate(sdRef);
}

void BonjourQuery::processResult() {
	std::cout << "Process result" << std::endl;
	boost::lock_guard<boost::mutex> lock(sdRefMutex);
	std::cout << "DNSSDServiceProcessResult" << std::endl;
	DNSServiceProcessResult(sdRef);
}

int BonjourQuery::getSocketID() const {
	boost::lock_guard<boost::mutex> lock(sdRefMutex);
	return DNSServiceRefSockFD(sdRef);
}

void BonjourQuery::run() {
	querier->addRunningQuery(shared_from_this());
}

void BonjourQuery::finish() {
	querier->removeRunningQuery(shared_from_this());
}

}
