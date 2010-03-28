#include "Swiften/Network/MainBoostIOServiceThread.h"

#include "Swiften/Network/BoostIOServiceThread.h"

namespace Swift {

BoostIOServiceThread& MainBoostIOServiceThread::getInstance() {
	static BoostIOServiceThread instance;
	return instance;
}

}
