#include "Swiften/Queries/IQHandler.h"
#include "Swiften/Queries/IQRouter.h"

namespace Swift {

IQHandler::IQHandler(IQRouter* router) : router_(router) {
	router_->addHandler(this);
}

IQHandler::~IQHandler() {
	router_->removeHandler(this);
}

}
