#pragma once

#include "SwifTools/Idle/IdleQuerier.h"

namespace Swift {
	class WindowsIdleQuerier : public IdleQuerier {
		public:
			WindowsIdleQuerier();

			virtual int getIdleTimeSeconds();
	};
}
