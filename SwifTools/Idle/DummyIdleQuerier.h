#pragma once

#include "SwifTools/Idle/IdleQuerier.h"

namespace Swift {
	class DummyIdleQuerier : public IdleQuerier {
		public:
			DummyIdleQuerier() {}

			virtual int getIdleTimeSeconds() {
				return 0;
			}
	};
}
