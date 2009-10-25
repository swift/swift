#pragma once

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
class IdleDetector {
	public:
		virtual ~IdleDetector() {};
		virtual void forceNotIdle() = 0;
		boost::signal<void ()> onInputIdle;
		boost::signal<void ()> onInputNotIdle;
};
}


