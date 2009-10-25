#pragma once

#include "Swift/Controllers/IdleDetector.h"

namespace Swift {
class QtIdleDetector : public IdleDetector {
	public:
		QtIdleDetector();
		virtual ~QtIdleDetector();
		virtual void forceNotIdle();
	  
		
};
}
